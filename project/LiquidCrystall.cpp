#include "../src/ForceFields/GayBerneFF.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "../src/Simulation/StandardSimulation.h"
#include "../src/Moves/MoveManager.h"
#include "../src/Moves/TranslateMove.h"
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

// Tests conservation of energy in a system with GB 
// particles and no skin thickness. (rcut = nlist).
TEST(GayBerneFF, NoSkinConservation)
{
	#ifdef MULTI_WALKER
	mpi::environment env;
	mpi::communicator comm;
	#endif
	
	// Load file (assumes we are in build folder).
	std::ifstream t("../project/gb_no_skin.json");
	std::stringstream buffer;
	buffer << t.rdbuf();

	// Read JSON.
	Json::Reader reader;
	Json::Value root;
	ASSERT_TRUE(reader.parse(buffer, root));

	// Build world.
	WorldManager wm;
	World* w;
	ASSERT_NO_THROW(w = World::Build(root["worlds"][0], root["blueprints"]));
	ASSERT_NE(nullptr, w);
	wm.AddWorld(w);

	ASSERT_EQ(338, w->GetParticleCount());
	ASSERT_EQ(5.0, w->GetNeighborRadius());
	ASSERT_EQ(0.0, w->GetSkinThickness());

	// Gay Berne interaction.
	ForceFieldManager ffm;
	GayBerneFF ff(1.0, 1.0, 3.0, 3.0, 1.0, 1.0, 5.0, 1.0, {5.0}, 2.0, 1.0);
	ffm.AddNonBondedForceField("GB", "GB", ff);

	// Get constraints.
	std::vector<Constraint*> constraints;
	ASSERT_NO_THROW(Constraint::BuildConstraints(root["forcefields"]["constraints"], &ffm, &wm, constraints));

	// Get moves.
	MoveManager mm;
	MoveList moves;
	ASSERT_NO_THROW(Move::BuildMoves(root["moves"], &mm, &wm, moves));

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

	// Initialize simulation. 
	StandardSimulation sim(&wm, &ffm, &mm);

	sim.AddObserver(&accumulator);
	sim.AddObserver(&csv);
	sim.Run(40000);

	// Conservation of energy and pressure.
	auto H = ffm.EvaluateEnergy(*w);
	ASSERT_NEAR(H.energy.total(), w->GetEnergy().total(), 1e-10);

	delete w;
	for(auto& m : mm)
		delete m;

	for(auto& c : constraints)
		delete c;
}
