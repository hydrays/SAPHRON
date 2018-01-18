#include "../src/ForceFields/LennardJonesFF.h"
#include "../src/ForceFields/GayBerneFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Simulation/StandardSimulation.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/TranslateMove.h"
#include "../src/Moves/RotateMove.h"
#include "../src/Particles/Particle.h"

#include "../src/Worlds/World.h"
#include "../src/Worlds/WorldManager.h"
#include "../src/Constraints/Constraint.h"
#include "../src/Observers/DLMFileObserver.h"
#include "TestAccumulator.h"
#include "json/json.h"
#include "gtest/gtest.h"
#include <fstream>

#ifdef MULTI_WALKER
#include <boost/mpi.hpp>
using namespace boost;
#endif

using namespace SAPHRON;

int main()
{
	printf("here[00]");

	#ifdef MULTI_WALKER
	mpi::environment env;
	mpi::communicator comm;
	#endif
	
	printf("here[0-0-0]");

	// Load file (assumes we are in build folder).
	std::ifstream t("../project/gb_no_skin.json");
	// std::ifstream t("../project/nist_lj_config1.json");
	std::stringstream buffer;
	buffer << t.rdbuf();

	// Read JSON.
	Json::Reader reader;
	Json::Value root;
	reader.parse(buffer, root);

	printf("here[0-0]");

	// Build world.
	WorldManager wm;
	World* w;
	w = World::Build(root["worlds"][0], root["blueprints"]);
	wm.AddWorld(w);

	printf("here[0-1]");
	// Gay Berne interaction.
	ForceFieldManager ffm;
	GayBerneFF ff(1.0, 1.0, 3.0, 3.0, 1.0, 1.0, 5.0, 1.0, {5.0}, 2.0, 1.0);
	ffm.AddNonBondedForceField("GB", "GB", ff);

	// // LJ interaction.
	// ForceFieldManager ffm;
	// LennardJonesFF ff(1.0, 1.0, {10.0});
	// ffm.AddNonBondedForceField("LJ", "LJ", ff);

	printf("here[0]");
	// // Get constraints.
	// std::vector<Constraint*> constraints;
	// Constraint::BuildConstraints(root["forcefields"]["constraints"], &ffm, &wm, constraints);

	// // Get moves.
	// MoveManager mm;
	// MoveList moves;
	// Move::BuildMoves(root["moves"], &mm, &wm, moves);

    // Initialize moves.                                                                                                                   
    TranslateMove move1(0.22);
    MoveManager mm;
    mm.AddMove(&move1);

    RotateMove move2(0.2);
    mm.AddMove(&move2);

	printf("here[1]");

	// Initialize observer.
	SimFlags flags;
	flags.world_energy = 1;
	flags.iteration = 1;
	flags.move_acceptances = 1;
	flags.world_pressure = 1;
	flags.world_on();
	flags.simulation_on();
	flags.pressure_on();
	flags.particle_on();
	flags.energy_on();

	// Initialize accumulator. 
	TestAccumulator accumulator(flags, 100, 20000);
	DLMFileObserver csv("test", flags, 100);
	printf("here[2]");

	// Initialize simulation. 
	StandardSimulation sim(&wm, &ffm, &mm);
	printf("here[3]");

	sim.AddObserver(&accumulator);
	sim.AddObserver(&csv);

	printf("here[4]");
	sim.Run(40000);

	// Conservation of energy and pressure.
	auto H = ffm.EvaluateEnergy(*w);
	
	delete w;
	//for(auto& m : mm)
	// 	delete m;

	// for(auto& c : constraints)
	// 	delete c;
}
