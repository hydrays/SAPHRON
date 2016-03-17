#include "schema.h"

namespace SAPHRON
{
	//INSERT_DEF_HERE
	std::string SAPHRON::JsonSchema::DirectorRestrictionC = "{\"additionalProperties\": false, \"required\": [\"type\", \"coefficient\", \"director\", \"index\", \"limits\", \"world\"], \"type\": \"object\", \"properties\": {\"coefficient\": {\"type\": \"number\"}, \"index\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 2}, \"limits\": {\"minItems\": 2, \"items\": {\"type\": \"number\"}, \"type\": \"array\", \"maxItems\": 2}, \"director\": {\"additionalItems\": false, \"minItems\": 3, \"varname\": \"Director\", \"maxItems\": 3, \"items\": {\"type\": \"number\"}, \"type\": \"array\"}, \"world\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"DirectorRestriction\"], \"type\": \"string\"}}}";
	std::string SAPHRON::JsonSchema::Constraints = "{\"type\": \"array\"}";
	std::string SAPHRON::JsonSchema::P2SAConnectivity = "{\"additionalProperties\": false, \"required\": [\"type\", \"coefficient\", \"director\", \"selector\"], \"type\": \"object\", \"properties\": {\"coefficient\": {\"type\": \"number\"}, \"director\": {\"additionalItems\": false, \"minItems\": 3, \"varname\": \"Director\", \"maxItems\": 3, \"items\": {\"type\": \"number\"}, \"type\": \"array\"}, \"type\": {\"enum\": [\"P2SA\"], \"type\": \"string\"}, \"selector\": {\"varname\": \"Selector\"}}}";
	std::string SAPHRON::JsonSchema::Connectivities = "{\"type\": \"array\", \"items\": {\"oneOf\": [{\"additionalProperties\": false, \"varname\": \"P2SAConnectivity\", \"required\": [\"type\", \"coefficient\", \"director\", \"selector\"], \"type\": \"object\", \"properties\": {\"coefficient\": {\"type\": \"number\"}, \"director\": {\"additionalItems\": false, \"minItems\": 3, \"varname\": \"Director\", \"maxItems\": 3, \"items\": {\"type\": \"number\"}, \"type\": \"array\"}, \"type\": {\"enum\": [\"P2SA\"], \"type\": \"string\"}, \"selector\": {\"varname\": \"Selector\"}}}]}}";
	std::string SAPHRON::JsonSchema::ParticleDistanceOP = "{\"additionalProperties\": false, \"required\": [\"type\", \"group1\", \"group2\"], \"type\": \"object\", \"properties\": {\"group1\": {\"minItems\": 1, \"items\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": \"array\"}, \"type\": {\"enum\": [\"ParticleDistance\"], \"type\": \"string\"}, \"group2\": {\"minItems\": 1, \"items\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": \"array\"}}}";
	std::string SAPHRON::JsonSchema::WangLandauOP = "{\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"type\": {\"enum\": [\"WangLandau\"], \"type\": \"string\"}}}";
	std::string SAPHRON::JsonSchema::ChargeFractionOP = "{\"additionalProperties\": false, \"required\": [\"type\", \"group1\", \"Charge\"], \"type\": \"object\", \"properties\": {\"group1\": {\"items\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": \"array\"}, \"Charge\": {\"minimum\": 0.0, \"type\": \"number\", \"maximum\": 1.0}, \"type\": {\"enum\": [\"ChargeFraction\"], \"type\": \"string\"}}}";
	std::string SAPHRON::JsonSchema::RgOP = "{\"additionalProperties\": false, \"required\": [\"type\", \"group1\"], \"type\": \"object\", \"properties\": {\"group1\": {\"items\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": \"array\"}, \"type\": {\"enum\": [\"Rg\"], \"type\": \"string\"}}}";
	std::string SAPHRON::JsonSchema::ElasticCoeffOP = "{\"additionalProperties\": false, \"required\": [\"type\", \"mode\", \"xrange\", \"world\"], \"type\": \"object\", \"properties\": {\"xrange\": {\"minItems\": 2, \"items\": {\"type\": \"number\"}, \"type\": \"array\", \"maxItems\": 2}, \"type\": {\"enum\": [\"ElasticCoeff\"], \"type\": \"string\"}, \"mode\": {\"enum\": [\"splay\", \"twist\", \"bend\"], \"type\": \"string\"}, \"world\": {\"minimum\": 0, \"type\": \"integer\"}}}";
	std::string SAPHRON::JsonSchema::Histogram = "{\"additionalProperties\": false, \"required\": [\"min\", \"max\"], \"type\": \"object\", \"properties\": {\"min\": {\"type\": \"number\"}, \"bincount\": {\"minimum\": 1, \"type\": \"integer\"}, \"max\": {\"type\": \"number\"}, \"values\": {\"items\": {\"type\": \"number\"}, \"type\": \"array\"}, \"binwidth\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"counts\": {\"items\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": \"array\"}}}";
	std::string SAPHRON::JsonSchema::Simulation = "{\"required\": [\"simtype\", \"iterations\"], \"type\": \"object\", \"properties\": {\"units\": {\"enum\": [\"real\", \"reduced\"], \"type\": \"string\"}, \"simtype\": {\"enum\": [\"standard\", \"DOS\"], \"type\": \"string\"}, \"iterations\": {\"minimum\": 1, \"type\": \"integer\"}, \"mpi\": {\"minimum\": 1, \"type\": \"integer\"}}}";
	std::string SAPHRON::JsonSchema::DOSSimulation = "{\"additionalProperties\": false, \"type\": \"object\", \"properties\": {\"sync_frequency\": {\"minimum\": 0, \"type\": \"integer\"}, \"target_flatness\": {\"exclusiveMinimum\": true, \"exclusiveMaximum\": true, \"minimum\": 0, \"type\": \"number\", \"maximum\": 1}, \"reset_freq\": {\"minimum\": 0, \"type\": \"integer\"}, \"convergence_factor\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"equilibration\": {\"minimum\": 0, \"type\": \"integer\"}}}";
	std::string SAPHRON::JsonSchema::LebwohlLasherFF = "{\"additionalProperties\": false, \"required\": [\"type\", \"epsilon\", \"gamma\", \"species\"], \"type\": \"object\", \"properties\": {\"epsilon\": {\"type\": \"number\"}, \"type\": {\"enum\": [\"LebwohlLasher\"], \"type\": \"string\"}, \"species\": {\"minItems\": 2, \"items\": {\"type\": \"string\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 2}, \"gamma\": {\"type\": \"number\"}}}";
	std::string SAPHRON::JsonSchema::LennardJonesFF = "{\"additionalProperties\": false, \"required\": [\"type\", \"sigma\", \"epsilon\", \"species\", \"rcut\"], \"type\": \"object\", \"properties\": {\"sigma\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"type\": {\"enum\": [\"LennardJones\"], \"type\": \"string\"}, \"species\": {\"minItems\": 2, \"items\": {\"type\": \"string\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 2}, \"rcut\": {\"minItems\": 1, \"items\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"type\": \"array\"}, \"epsilon\": {\"minimum\": 0, \"type\": \"number\"}}}";
	std::string SAPHRON::JsonSchema::DSFFF = "{\"additionalProperties\": false, \"required\": [\"type\", \"alpha\", \"rcut\"], \"type\": \"object\", \"properties\": {\"alpha\": {\"minimum\": 0, \"type\": \"number\"}, \"type\": {\"enum\": [\"DSF\"], \"type\": \"string\"}, \"rcut\": {\"minItems\": 1, \"items\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"type\": \"array\"}}}";
	std::string SAPHRON::JsonSchema::LennardJonesTSFF = "{\"additionalProperties\": false, \"required\": [\"type\", \"sigma\", \"epsilon\", \"species\", \"rcut\"], \"type\": \"object\", \"properties\": {\"sigma\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"type\": {\"enum\": [\"LennardJonesTS\"], \"type\": \"string\"}, \"species\": {\"minItems\": 2, \"items\": {\"type\": \"string\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 2}, \"rcut\": {\"minItems\": 1, \"items\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"type\": \"array\"}, \"epsilon\": {\"minimum\": 0, \"type\": \"number\"}}}";
	std::string SAPHRON::JsonSchema::DebyeHuckelFF = "{\"additionalProperties\": false, \"required\": [\"type\", \"kappa\", \"rcut\"], \"type\": \"object\", \"properties\": {\"kappa\": {\"minimum\": 0, \"type\": \"number\"}, \"type\": {\"enum\": [\"DebyeHuckel\"], \"type\": \"string\"}, \"rcut\": {\"minItems\": 1, \"items\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"type\": \"array\"}}}";
	std::string SAPHRON::JsonSchema::ModLennardJonesTSFF = "{\"additionalProperties\": false, \"required\": [\"type\", \"sigma\", \"epsilon\", \"beta\", \"species\", \"rcut\"], \"type\": \"object\", \"properties\": {\"type\": {\"enum\": [\"ModLennardJonesTS\"], \"type\": \"string\"}, \"rcut\": {\"minItems\": 1, \"items\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"type\": \"array\"}, \"beta\": {\"type\": \"number\"}, \"epsilon\": {\"minimum\": 0, \"type\": \"number\"}, \"sigma\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"species\": {\"minItems\": 2, \"items\": {\"type\": \"string\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 2}}}";
	std::string SAPHRON::JsonSchema::ForceFields = "{\"additionalProperties\": false, \"type\": \"object\", \"properties\": {\"electrostatic\": {\"type\": \"object\"}, \"nonbonded\": {\"type\": \"array\"}, \"bonded\": {\"type\": \"array\"}, \"constraints\": {\"type\": \"array\"}}}";
	std::string SAPHRON::JsonSchema::HardSphereFF = "{\"additionalProperties\": false, \"required\": [\"type\", \"sigma\", \"species\"], \"type\": \"object\", \"properties\": {\"sigma\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"type\": {\"enum\": [\"HardSphere\"], \"type\": \"string\"}, \"species\": {\"minItems\": 2, \"items\": {\"type\": \"string\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 2}}}";
	std::string SAPHRON::JsonSchema::FENEFF = "{\"additionalProperties\": false, \"required\": [\"type\", \"epsilon\", \"sigma\", \"kspring\", \"rmax\", \"species\"], \"type\": \"object\", \"properties\": {\"epsilon\": {\"type\": \"number\"}, \"rmax\": {\"minimum\": 0, \"type\": \"number\"}, \"sigma\": {\"minimum\": 0, \"type\": \"number\"}, \"type\": {\"enum\": [\"FENE\"], \"type\": \"string\"}, \"species\": {\"minItems\": 2, \"items\": {\"type\": \"string\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 2}, \"kspring\": {\"minimum\": 0, \"type\": \"number\"}}}";
	std::string SAPHRON::JsonSchema::GayBerneFF = "{\"required\": [\"type\", \"diameters\", \"lengths\", \"eps0\", \"epsE\", \"epsS\", \"rcut\", \"species\"], \"type\": \"object\", \"properties\": {\"diameters\": {\"items\": [{\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}], \"type\": \"array\"}, \"lengths\": {\"items\": [{\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}], \"type\": \"array\"}, \"epsE\": {\"minimum\": 0, \"type\": \"number\"}, \"rcut\": {\"minItems\": 1, \"items\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"type\": \"array\"}, \"nu\": {\"type\": \"number\"}, \"mu\": {\"type\": \"number\"}, \"species\": {\"minItems\": 2, \"items\": {\"type\": \"string\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 2}, \"dw\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"type\": {\"enum\": [\"GayBerne\"], \"type\": \"string\"}, \"epsS\": {\"minimum\": 0, \"type\": \"number\"}, \"eps0\": {\"minimum\": 0, \"type\": \"number\"}}}";
	std::string SAPHRON::JsonSchema::HarmonicFF = "{\"additionalProperties\": false, \"required\": [\"type\", \"kspring\", \"ro\", \"species\"], \"type\": \"object\", \"properties\": {\"ro\": {\"minimum\": 0, \"type\": \"number\"}, \"type\": {\"enum\": [\"Harmonic\"], \"type\": \"string\"}, \"species\": {\"minItems\": 2, \"items\": {\"type\": \"string\"}, \"additionalItems\": false, \"type\": \"array\", \"maxItems\": 2}, \"kspring\": {\"minimum\": 0, \"type\": \"number\"}}}";
	std::string SAPHRON::JsonSchema::Worlds = "{\"minItems\": 1, \"type\": \"array\", \"items\": {\"additionalProperties\": false, \"varname\": \"SimpleWorld\", \"required\": [\"type\", \"dimensions\", \"nlist_cutoff\", \"skin_thickness\", \"components\"], \"type\": \"object\", \"properties\": {\"dimensions\": {\"additionalItems\": false, \"minItems\": 3, \"varname\": \"Position\", \"maxItems\": 3, \"items\": {\"type\": \"number\"}, \"type\": \"array\"}, \"periodic\": {\"additionalProperties\": false, \"type\": \"object\", \"properties\": {\"y\": {\"type\": \"boolean\"}, \"x\": {\"type\": \"boolean\"}, \"z\": {\"type\": \"boolean\"}}}, \"components\": {\"minItems\": 1, \"varname\": \"Components\", \"type\": \"array\", \"items\": {\"minItems\": 2, \"items\": [{\"type\": \"string\"}, {\"minimum\": 1, \"type\": \"integer\"}], \"type\": \"array\", \"maxItems\": 2}}, \"skin_thickness\": {\"minimum\": 0, \"type\": \"number\"}, \"particles\": {\"type\": \"array\"}, \"lattice\": {\"type\": \"object\", \"properties\": {\"composition\": {\"patternProperties\": {\"^[A-z][A-z0-9]+$\": {\"exclusiveMinimum\": true, \"minimum\": 0.0, \"type\": \"number\", \"maximum\": 1.0}}, \"type\": \"object\"}}}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"chemical_potential\": {\"patternProperties\": {\"^[A-z][A-z0-9]+$\": {\"type\": \"number\"}}, \"type\": \"object\"}, \"nlist_cutoff\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"pack\": {\"type\": \"object\", \"properties\": {\"count\": {\"minimum\": 1, \"type\": \"integer\"}, \"composition\": {\"patternProperties\": {\"^[A-z][A-z0-9]+$\": {\"exclusiveMinimum\": true, \"minimum\": 0.0, \"type\": \"number\", \"maximum\": 1.0}}, \"type\": \"object\"}, \"density\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}}}, \"type\": {\"enum\": [\"Simple\"], \"type\": \"string\"}, \"temperature\": {\"minimum\": 0, \"type\": \"number\"}}}}";
	std::string SAPHRON::JsonSchema::SimpleWorld = "{\"additionalProperties\": false, \"required\": [\"type\", \"dimensions\", \"nlist_cutoff\", \"skin_thickness\", \"components\"], \"type\": \"object\", \"properties\": {\"dimensions\": {\"additionalItems\": false, \"minItems\": 3, \"varname\": \"Position\", \"maxItems\": 3, \"items\": {\"type\": \"number\"}, \"type\": \"array\"}, \"periodic\": {\"additionalProperties\": false, \"type\": \"object\", \"properties\": {\"y\": {\"type\": \"boolean\"}, \"x\": {\"type\": \"boolean\"}, \"z\": {\"type\": \"boolean\"}}}, \"components\": {\"minItems\": 1, \"varname\": \"Components\", \"type\": \"array\", \"items\": {\"minItems\": 2, \"items\": [{\"type\": \"string\"}, {\"minimum\": 1, \"type\": \"integer\"}], \"type\": \"array\", \"maxItems\": 2}}, \"skin_thickness\": {\"minimum\": 0, \"type\": \"number\"}, \"particles\": {\"type\": \"array\"}, \"lattice\": {\"type\": \"object\", \"properties\": {\"composition\": {\"patternProperties\": {\"^[A-z][A-z0-9]+$\": {\"exclusiveMinimum\": true, \"minimum\": 0.0, \"type\": \"number\", \"maximum\": 1.0}}, \"type\": \"object\"}}}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"chemical_potential\": {\"patternProperties\": {\"^[A-z][A-z0-9]+$\": {\"type\": \"number\"}}, \"type\": \"object\"}, \"nlist_cutoff\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}, \"pack\": {\"type\": \"object\", \"properties\": {\"count\": {\"minimum\": 1, \"type\": \"integer\"}, \"composition\": {\"patternProperties\": {\"^[A-z][A-z0-9]+$\": {\"exclusiveMinimum\": true, \"minimum\": 0.0, \"type\": \"number\", \"maximum\": 1.0}}, \"type\": \"object\"}, \"density\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\"}}}, \"type\": {\"enum\": [\"Simple\"], \"type\": \"string\"}, \"temperature\": {\"minimum\": 0, \"type\": \"number\"}}}";
	std::string SAPHRON::JsonSchema::Components = "{\"minItems\": 1, \"type\": \"array\", \"items\": {\"minItems\": 2, \"items\": [{\"type\": \"string\"}, {\"minimum\": 1, \"type\": \"integer\"}], \"type\": \"array\", \"maxItems\": 2}}";
	std::string SAPHRON::JsonSchema::Particles = "{\"minItems\": 1, \"additionalItems\": false, \"type\": \"array\", \"items\": {\"additionalItems\": false, \"minItems\": 3, \"varname\": \"Site\", \"maxItems\": 5, \"items\": [{\"minimum\": 1, \"type\": \"integer\"}, {\"type\": \"string\"}, {\"additionalItems\": false, \"minItems\": 3, \"varname\": \"Position\", \"maxItems\": 3, \"items\": {\"type\": \"number\"}, \"type\": \"array\"}, {\"additionalItems\": false, \"minItems\": 3, \"varname\": \"Director\", \"maxItems\": 3, \"items\": {\"type\": \"number\"}, \"type\": \"array\"}, {\"type\": \"string\"}], \"type\": \"array\"}}";
	std::string SAPHRON::JsonSchema::Site = "{\"additionalItems\": false, \"minItems\": 3, \"maxItems\": 5, \"items\": [{\"minimum\": 1, \"type\": \"integer\"}, {\"type\": \"string\"}, {\"additionalItems\": false, \"minItems\": 3, \"varname\": \"Position\", \"maxItems\": 3, \"items\": {\"type\": \"number\"}, \"type\": \"array\"}, {\"additionalItems\": false, \"minItems\": 3, \"varname\": \"Director\", \"maxItems\": 3, \"items\": {\"type\": \"number\"}, \"type\": \"array\"}, {\"type\": \"string\"}], \"type\": \"array\"}";
	std::string SAPHRON::JsonSchema::Director = "{\"additionalItems\": false, \"minItems\": 3, \"maxItems\": 3, \"items\": {\"type\": \"number\"}, \"type\": \"array\"}";
	std::string SAPHRON::JsonSchema::Selector = "{}";
	std::string SAPHRON::JsonSchema::Blueprints = "{\"additionalProperties\": false, \"patternProperties\": {\"^[A-z][A-z0-9]+$\": {\"additionalProperties\": false, \"type\": \"object\", \"properties\": {\"charge\": {\"type\": \"number\"}, \"mass\": {\"minimum\": 0, \"type\": \"number\"}, \"children\": {\"minItems\": 1, \"items\": {\"required\": [\"species\"], \"type\": \"object\", \"properties\": {\"charge\": {\"type\": \"number\"}, \"mass\": {\"minimum\": 0, \"type\": \"number\"}, \"species\": {\"type\": \"string\"}}}, \"type\": \"array\"}, \"bonds\": {\"items\": {\"minItems\": 2, \"items\": {\"minimum\": 0, \"type\": \"number\"}, \"type\": \"array\", \"maxItems\": 2}, \"type\": \"array\"}}, \"minProperties\": 1}}, \"type\": \"object\", \"minProperties\": 1}";
	std::string SAPHRON::JsonSchema::Position = "{\"additionalItems\": false, \"minItems\": 3, \"maxItems\": 3, \"items\": {\"type\": \"number\"}, \"type\": \"array\"}";
	std::string SAPHRON::JsonSchema::Observers = "{\"type\": \"array\"}";
	std::string SAPHRON::JsonSchema::DLMFileObserver = "{\"additionalProperties\": false, \"required\": [\"type\", \"prefix\", \"frequency\", \"flags\"], \"type\": \"object\", \"properties\": {\"extension\": {\"type\": \"string\"}, \"fixedwmode\": {\"type\": \"boolean\"}, \"prefix\": {\"type\": \"string\"}, \"delimiter\": {\"type\": \"string\"}, \"frequency\": {\"minimum\": 1, \"type\": \"integer\"}, \"flags\": {\"type\": \"object\", \"properties\": {\"energy_connectivity\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"hist_bin_count\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"energy_interelect\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"energy_intravdw\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"particle_species_id\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"particle_parent_species\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"energy_components\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"pressure_pxx\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"pressure_pxy\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world_chem_pot\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"energy_constraint\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"hist_upper_outliers\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"energy_bonded\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"pressure_tensor\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"particle_charge\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"pressure_pzz\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"particle_id\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"hist_values\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"particle_parent_id\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"hist_interval\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world_energy\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"hist_counts\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"move_acceptances\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world_pressure\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"energy_intraelect\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"dos_op\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world_volume\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"hist_lower_outliers\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"particle_species\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world_density\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"dos_flatness\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"pressure_pxz\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"dos_factor\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"energy_intervdw\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"energy_tail\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"histogram\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world_temperature\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"pressure_pyz\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"pressure_pyy\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"particle\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"pressure_tail\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"iteration\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"simulation\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"particle_director\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"particle_position\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"world_composition\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}, \"pressure_ideal\": {\"minimum\": 0, \"type\": \"integer\", \"maximum\": 1}}}, \"colwidth\": {\"minimum\": 1, \"type\": \"integer\"}, \"type\": {\"enum\": [\"DLMFile\"], \"type\": \"string\"}}}";
	std::string SAPHRON::JsonSchema::XYZObserver = "{\"additionalProperties\": false, \"required\": [\"type\", \"prefix\", \"frequency\"], \"type\": \"object\", \"properties\": {\"prefix\": {\"type\": \"string\"}, \"frequency\": {\"minimum\": 1, \"type\": \"integer\"}, \"type\": {\"enum\": [\"XYZ\"], \"type\": \"string\"}}}";
	std::string SAPHRON::JsonSchema::JSONObserver = "{\"additionalProperties\": false, \"required\": [\"type\", \"prefix\", \"frequency\"], \"type\": \"object\", \"properties\": {\"prefix\": {\"type\": \"string\"}, \"frequency\": {\"minimum\": 1, \"type\": \"integer\"}, \"type\": {\"enum\": [\"JSON\"], \"type\": \"string\"}}}";
	std::string SAPHRON::JsonSchema::SpeciesSwapMove = "{\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"SpeciesSwap\"], \"type\": \"string\"}, \"species\": {\"uniqueItems\": true, \"minItems\": 2, \"type\": \"array\", \"maxItems\": 2, \"items\": {\"type\": \"string\"}}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}, \"deep_copy\": {\"type\": \"boolean\"}}}";
	std::string SAPHRON::JsonSchema::AnnealChargeMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"species\"], \"type\": \"object\", \"properties\": {\"explicit_draw\": {\"type\": \"boolean\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"AnnealCharge\"], \"type\": \"string\"}, \"species\": {\"minItems\": 1, \"items\": {\"type\": \"string\"}, \"type\": \"array\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}}}";
	std::string SAPHRON::JsonSchema::VolumeSwapMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"dv\"], \"type\": \"object\", \"properties\": {\"dv\": {\"minimum\": 0, \"type\": \"number\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"VolumeSwap\"], \"type\": \"string\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}}}";
	std::string SAPHRON::JsonSchema::Moves = "{\"type\": \"array\"}";
	std::string SAPHRON::JsonSchema::AcidTitrationMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"species\", \"mu\"], \"type\": \"object\", \"properties\": {\"weight\": {\"minimum\": 1, \"type\": \"integer\"}, \"mu\": {\"type\": \"number\"}, \"op_prefactor\": {\"type\": \"boolean\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"proton_charge\": {\"type\": \"number\"}, \"type\": {\"enum\": [\"AcidTitrate\"], \"type\": \"string\"}, \"species\": {\"minItems\": 1, \"items\": {\"type\": \"string\"}, \"type\": \"array\"}}}";
	std::string SAPHRON::JsonSchema::DeleteParticleMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"species\"], \"type\": \"object\", \"properties\": {\"weight\": {\"minimum\": 1, \"type\": \"integer\"}, \"op_prefactor\": {\"tyoe\": \"boolean\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"DeleteParticle\"], \"type\": \"string\"}, \"species\": {\"items\": {\"type\": \"string\"}, \"type\": \"array\", \"minimumItems\": 1}, \"multi_delete\": {\"type\": \"boolean\"}}}";
	std::string SAPHRON::JsonSchema::InsertParticleMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"stash_count\", \"species\"], \"type\": \"object\", \"properties\": {\"multi_insertion\": {\"type\": \"boolean\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}, \"stash_count\": {\"minimum\": 1, \"type\": \"integer\"}, \"op_prefactor\": {\"type\": \"boolean\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"InsertParticle\"], \"type\": \"string\"}, \"species\": {\"items\": {\"type\": \"string\"}, \"type\": \"array\", \"minimumItems\": 1}}}";
	std::string SAPHRON::JsonSchema::AcidReactionMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"products\", \"swap\", \"pKo\", \"stash_count\"], \"type\": \"object\", \"properties\": {\"reactants\": {\"minItems\": 1, \"items\": {\"type\": \"string\"}, \"type\": \"array\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}, \"stash_count\": {\"minimum\": 1, \"type\": \"integer\"}, \"op_prefactor\": {\"type\": \"boolean\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"products\": {\"minItems\": 1, \"items\": {\"type\": \"string\"}, \"type\": \"array\"}, \"swap\": {\"minItems\": 1, \"items\": {\"type\": \"string\"}, \"type\": \"array\"}, \"type\": {\"enum\": [\"AcidReaction\"], \"type\": \"string\"}, \"pKo\": {\"type\": \"number\"}}}";
	std::string SAPHRON::JsonSchema::RotateMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"maxangle\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"Rotate\"], \"type\": \"string\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}, \"maxangle\": {\"exclusiveMinimum\": true, \"minimum\": 0, \"type\": \"number\", \"maximum\": 6.283185307179586}}}";
	std::string SAPHRON::JsonSchema::FlipSpinMove = "{\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"FlipSpin\"], \"type\": \"string\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}}}";
	std::string SAPHRON::JsonSchema::WidomInsertionMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"species\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"WidomInsertion\"], \"type\": \"string\"}, \"species\": {\"items\": {\"type\": \"string\"}, \"type\": \"array\", \"minimumItems\": 1}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}}}";
	std::string SAPHRON::JsonSchema::TranslateMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"dx\"], \"type\": \"object\", \"properties\": {\"explicit_draw\": {\"type\": \"boolean\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"Translate\"], \"type\": \"string\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}, \"dx\": {\"oneOf\": [{\"minimum\": 0, \"type\": \"number\"}, {\"patternProperties\": {\"^[A-z][A-z0-9]+$\": {\"exclusiveMinimum\": true, \"minimum\": 0.0, \"type\": \"number\"}}, \"type\": \"object\", \"minProperties\": 1}]}}}";
	std::string SAPHRON::JsonSchema::TranslatePrimitiveMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"dx\"], \"type\": \"object\", \"properties\": {\"explicit_draw\": {\"type\": \"boolean\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"TranslatePrimitive\"], \"type\": \"string\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}, \"dx\": {\"oneOf\": [{\"minimum\": 0, \"type\": \"number\"}, {\"patternProperties\": {\"^[A-z][A-z0-9]+$\": {\"exclusiveMinimum\": true, \"minimum\": 0.0, \"type\": \"number\"}}, \"type\": \"object\", \"minProperties\": 1}]}}}";
	std::string SAPHRON::JsonSchema::ParticleSwapMove = "{\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"ParticleSwap\"], \"type\": \"string\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}}}";
	std::string SAPHRON::JsonSchema::RandomIdentityMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"identities\"], \"type\": \"object\", \"properties\": {\"identities\": {\"uniqueItems\": true, \"items\": {\"type\": \"string\"}, \"type\": \"array\", \"minIems\": 1}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"RandomIdentity\"], \"type\": \"string\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}}}";
	std::string SAPHRON::JsonSchema::DirectorRotateMove = "{\"additionalProperties\": false, \"required\": [\"type\"], \"type\": \"object\", \"properties\": {\"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"DirectorRotate\"], \"type\": \"string\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}}}";
	std::string SAPHRON::JsonSchema::VolumeScaleMove = "{\"additionalProperties\": false, \"required\": [\"type\", \"dv\", \"Pextern\"], \"type\": \"object\", \"properties\": {\"dv\": {\"minimum\": 0, \"type\": \"number\"}, \"Pextern\": {\"minimum\": 0, \"type\": \"number\"}, \"seed\": {\"minimum\": 0, \"type\": \"integer\"}, \"type\": {\"enum\": [\"VolumeScale\"], \"type\": \"string\"}, \"weight\": {\"minimum\": 1, \"type\": \"integer\"}}}";
	
}