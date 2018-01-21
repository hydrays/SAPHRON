#include "LangevinSimulation.h"

namespace SAPHRON
{
	inline void LangevinSimulation::Iterate()
	{

		//printf("inside standardsimulation->iterate: 00\n");

		// _mmanager->ResetMoveAcceptances();
		
		// Select random move and perform.
		// for(int i = 0; i < GetMovesPerIteration(); ++i)
		// {
			//printf("inside standardsimulation->iterate: 01\n");
			// auto* move = _mmanager->SelectRandomMove();
			// move->Perform(_wmanager, _ffmanager, MoveOverride::None);
		// }

		auto* move = _mmanager->SelectRandomMove();
		if ( move->GetName().compare("Rotate") != 0 )
		{
			printf("inside standardsimulation->iterate: only rotate is allowed!\n");
		}
		move->Integrator(_wmanager, _ffmanager);
		
		// UpdateAcceptances();
		this->IncrementIterations();

		#ifdef MULTI_WALKER
		if(_comm.rank() == 0)
		#endif
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
	}

	// Run the NVT ensemble for a specified number of iterations.
	void LangevinSimulation::Run(int iterations)
	{
		#ifdef MULTI_WALKER
		if(_comm.rank() == 0)
		#endif
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
	
		for(int i = 0; i < iterations; ++i)
			Iterate();
	}
}