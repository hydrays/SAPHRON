#pragma once

#include "../ForceFields/ForceFieldManager.h"
#include "../Moves/MoveManager.h"
#include "../Worlds/WorldManager.h"
#include "../Utils/Rand.h"
#include "../Worlds/World.h"
#include "Simulation.h"
#include <cmath>
#include <exception>

namespace SAPHRON
{
	// Class for a standard simulation. Depending on the moves, this may 
	// represent the NVT, NPT, \muVT, Gibbs, etc.. ensemble. 
	class LangevinSimulation : public Simulation
	{
	private:

		// Pointer to world manager.
		WorldManager* _wmanager;

		// Pointer to force field manager.
		ForceFieldManager* _ffmanager;

		// Pointer to move manager.
		MoveManager* _mmanager;

		// Acceptance map.
		AcceptanceMap _accmap;

		inline void UpdateAcceptances()
		{
			for(auto& move : *_mmanager)
				_accmap[move->GetName()] = move->GetAcceptanceRatio();
		}

		void Iterate();

	protected:

		// Visit children.
		virtual void VisitChildren(Visitor& v) const override
		{
			_wmanager->AcceptVisitor(v);
			_mmanager->AcceptVisitor(v);
			_ffmanager->AcceptVisitor(v);
		}

	public:

		LangevinSimulation(WorldManager* wm, ForceFieldManager* ffm, MoveManager* mm) :
			_wmanager(wm), _ffmanager(ffm), _mmanager(mm), _accmap()
		{
			#ifdef MULTI_WALKER
			if(_comm.size() > 1)
			{
				if(_comm.rank() == 0)
					throw std::logic_error("Langevin simulations cannot be run in multi-walker mode.");
			}
			#endif

			// printf("inside standardsimulation->initial: 00\n");

			// Moves per iteration.
			int mpi = 0;

			// Evaluate energies of systems. 
			for(auto& world : *_wmanager)
			{
				// printf("inside standardsimulation->initial: 00-1\n");

				auto EP = _ffmanager->EvaluateEnergy(*world);

				// printf("inside standardsimulation->initial: 00-2\n");
				// printf("EP: %f %f\n", EP.energy.intervdw, EP.energy.constraint);

				world->SetEnergy(EP.energy);

				// printf("inside standardsimulation->initial: 00-3\n");
				world->SetPressure(EP.pressure);
				mpi += world->GetParticleCount();
			}

			// printf("inside standardsimulation->initial: 01\n");

			this->SetMovesPerIteration(mpi);
			UpdateAcceptances();
		}

		// Run the NVT ensemble for a specified number of iterations.
		virtual void Run(int iterations) override;

		// Get ratio of accepted moves.
		virtual AcceptanceMap GetAcceptanceRatio() const override
		{
			return _accmap;
		}

		virtual std::string GetName() const override { return "Langevin"; }
	};
}
