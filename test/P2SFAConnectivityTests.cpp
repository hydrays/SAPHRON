#include "../src/Connectivities/P2SFAConnectivity.h"
#include "../src/Particles/Particle.h"
#include "../src/ForceFields/ForceFieldManager.h"
#include "gtest/gtest.h"

using namespace SAPHRON;

TEST(P2SFAConnectivity, DefaultBehavior)
{
	// Test pure connectivity on a site.
	P2SFAConnectivity connectivity(1.0, [](const Particle&, Director& dir){
		dir[0] = 1.0;
		dir[1] = 0.0;
		dir[2] = 0.0;
	});

	Particle site1({0, 0, 0}, {1.0, 0, 0}, "E1");
	site1.AddConnectivity(&connectivity);

	ForceFieldManager ffm;

	ASSERT_EQ(-1.0, ffm.EvaluateEnergy(site1).energy.connectivity);
}