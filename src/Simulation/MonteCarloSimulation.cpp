#include "MonteCarloSimulation.h"

namespace SAPHRON
{
	inline void MonteCarloSimulation::Iterate()
	{

		//printf("inside montecarlosimulation->iterate: 00\n");

		_mmanager->ResetMoveAcceptances();
		
		// Select random move and perform.
		for(int i = 0; i < GetMovesPerIteration(); ++i)
		{
			//printf("inside montecarlosimulation->iterate: 01\n");
			auto* move = _mmanager->SelectRandomMove();
			move->Perform(_wmanager, _ffmanager, MoveOverride::None);
		}
		
		UpdateAcceptances();
		this->IncrementIterations();

		#ifdef MULTI_WALKER
		if(_comm.rank() == 0)
		#endif
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
	}

	// Run the NVT ensemble for a specified number of iterations.
	void MonteCarloSimulation::Run(int iterations)
	{
		#ifdef MULTI_WALKER
		if(_comm.rank() == 0)
		#endif
		this->NotifyObservers(SimEvent(this, this->GetIteration()));
	
		for(int i = 0; i < iterations; ++i)
			Iterate();
	}
}