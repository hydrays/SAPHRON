#pragma once

#include "../Simulation/SimInfo.h"
#include "../Particles/Particle.h"
#include "../Observers/Visitable.h"
#include "../Properties/EPTuple.h"
#include "../JSON/Serializable.h"
#include "../Utils/Rand.h"
#include "json/json.h"
#include "vecmap.h"
#include <memory>
#include <functional>
#include <armadillo>
#include <queue>

namespace SAPHRON
{
	typedef std::vector<int> CompositionList; 
	typedef std::vector<World*> WorldList;
	typedef std::vector<int> WorldIndexList;
	typedef std::vector<std::queue<Particle*>> StashList;
	
	// Public interface representing the "World" in which particles live. 
	// A World object is responsible for setting up the "box" and associated 
	// geometry, handling boundary conditions and updating negihbor lists on
	// particles. World implements ParticleObserver so it can "listen in" 
	// on it's particles if it needs it. Note: it is the World implementation's 
	// responsibility to register itself with particles if it needs it, and to 
	// remove this when a particle is removed.
	class World : public ParticleObserver, public Visitable, public Serializable
	{
	private:
		// Neighbor list cutoff radius.
		double _ncut, _ncutsq;

		// Box matrix.
		Matrix2D _H;

		// Is diagoanl?
		bool _diag;

		// Periodic boundaries.
		bool _periodx, _periody, _periodz;

		// Skin thickness (calculated).
		double _skin, _skinsq;

		// System properties.
		double _temperature; 

		Energy _energy;

		Pressure _pressure;

		// Chemical potential.
		std::vector<double> _chemp;

		// de Broglie wavelength.
		std::vector<double> _debroglie;

		std::vector<std::vector<int>> _nbrs;

		// Particle list.
		ParticleList _particles;

		// Primitive particle list.
		ParticleList _primitives;

		// Random number generator.
		Rand _rand;

		// Composition of the system.
		CompositionList _composition;

		// Particle stash.
		StashList _stash;
		
		// Seed.
		unsigned _seed;		

		// World ID.
		int _id; 

		// Global world ID.
		static int _nextID;

		// Visit children.
		void VisitChildren(Visitor& v) const
		{
			for(auto& particle : _particles)
				particle->AcceptVisitor(v);
		}

		void AddParticleComposition(Particle* particle);
		void RemoveParticleComposition(Particle* particle);
		void ModifyParticleComposition(const ParticleEvent& pEvent);
		void UpdateNeighborList(Particle* particle, bool clear);

		// Compute de Broglie wavelength for particle p.
		void ComputeWavelength(Particle* p)
		{
			int id = p->GetSpeciesID();

			if((int)_debroglie.size() - 1 < id)
				_debroglie.resize(id + 1, 1.0);
			
			// de Broglie is 1 in reduced units.
			auto& sim = SimInfo::Instance();
			if(sim.GetUnits() == real)
			{
				auto denom = sqrt(2.0*M_PI*p->GetMass()*sim.GetkBamu()*GetTemperature());
				_debroglie[id] = sim.GetPlanck()/denom;
			}

		}

		// Perform all the appropriate configuration for a new particle.
		inline void ConfigureParticle(Particle* particle, bool updatelist)
		{
			// Add this world as an observer.
			// Propogates to children.
			particle->AddObserver(this);

			// Associate this world with particle.
			// propogates to children.
			particle->SetWorld(this);

			// Add particle to the composition list.
			AddParticleComposition(particle);

			// Compute de Broglie wavelength.
			ComputeWavelength(particle);

			// Update particle neighbor list. This will clear old
			// and create new for particle and children.
			if(updatelist)
				this->UpdateNeighborList(particle);
		}

		// Methods for parallel neighbor list.
		void rect(int i0, int i1, int j0, int j1);
		void triangle(int n0, int n1);
		void AddNeighbor(Particle* pi, Particle*pj);


	protected:

		// World optional string ID.
		std::string _stringid;

	public:
		typedef ParticleList::iterator iterator;
		typedef ParticleList::const_iterator const_iterator;

		// Initialize an orthorhombic world.
		World(double xl, double yl, double ncut, double skin, unsigned seed = 1) : 
		_ncut(ncut), _ncutsq(ncut*ncut), _H(arma::fill::zeros), _diag(true),
		_periodx(true), _periody(true), _skin(skin), _skinsq(skin*skin), 
		_temperature(0.0), _chemp(0), _debroglie(0), _nbrs(0), _particles(0), _primitives(0), 
		_rand(seed), _composition(0), _stash(0), _seed(seed), _id(_nextID++)
		{
			_stringid = "world" + std::to_string(_id);
			_H(0,0) = xl;
			_H(1,1) = yl;
			
			_composition.reserve(20);
			_stash.reserve(20);
			_chemp.reserve(20);
			_debroglie.reserve(20);
		}

		// Draw a random particle from the world.
		// Returns nullptr if nothing is drawn.
		Particle* DrawRandomParticle()
		{
			size_t n = _particles.size();
			if(n < 1) return nullptr;
			return _particles[_rand.int32() % n];
		}

		// Uniformly draws a particle from the world based on a list 
		// of species. This basically selects a random particle based 
		// on mole fraction.
		Particle* DrawParticleFromSpeciesList(const std::vector<int>& species)
		{
			// Compute total number of drawable species.
			auto tot = 0;
			for(auto& s : species)
				tot += _composition[s];

			// Pick a random number < tot.
			int x = _rand.int32() % tot;

			// Re-iterate through species and choose the appropriate one.
			auto count = 0, id = 0;
			for(auto& s : species)
			{
				count += _composition[s];
				if(count > x)
				{
					id = s;
					break;
				}
			}

			return DrawRandomParticleBySpecies(id);
		}

		// Draws a random particle by species from the world. 
		// Will return nullptr if species doesn't exist.
		Particle* DrawRandomParticleBySpecies(int species)
		{
			if((int)_composition.size() - 1 < species || _composition[species] < 1)
				return nullptr; 

			// Select random number betwene [0, count-1].
			int i = _rand.int32() % _composition[species];
			return SelectParticleBySpecies(species, i);
		}

		// Draw a random primitive from the world.
		// Returns nullptr if nothing is drawn.
		Particle* DrawRandomPrimitive()
		{
			size_t n = _primitives.size();
			if(n < 1) return nullptr;
			return _primitives[_rand.int32() % n];
		}

		// Uniformly draws a primitive from the world based on a list 
		// of species. This basically selects a random primitive particle based 
		// on mole fraction.
		Particle* DrawPrimitiveFromSpeciesList(const std::vector<int>& species)
		{
			// Compute total number of drawable species.
			auto tot = 0;
			for(auto& s : species)
				tot += _composition[s];

			// Pick a random number < tot.
			int x = _rand.int32() % tot;

			// Re-iterate through species and choose the appropriate one.
			auto count = 0, id = 0;
			for(auto& s : species)
			{
				count += _composition[s];
				if(count > x)
				{
					id = s;
					break;
				}
			}

			return DrawRandomPrimitiveBySpecies(id);
		}

		// Draws a random primitive by species from the world. 
		// Will return nullptr if species don't exist.
		Particle* DrawRandomPrimitiveBySpecies(int species)
		{
			if((int)_composition.size() - 1 < species || _composition[species] < 1)
				return nullptr;

			int i = _rand.int32() % _composition[species];
			return SelectPrimitiveBySpecies(species, i);
		}

		// Draw random particles from the world and inserts them into particles. 
		// NOTE: this method clears the list before adding new elements.
		void DrawRandomParticles(ParticleList& particles, 
								 unsigned int count = 1) 
		{
			particles.resize(count);
			unsigned int n = this->_particles.size();
			for(unsigned int i = 0; i < count; i ++)
				particles[i] = _particles[_rand.int32() % n];
		}

		// Update the neighbor list for all particles in the world.
		void UpdateNeighborList();

		// Update neighbor list for a particle.
		void UpdateNeighborList(Particle* particle);
		
		// Checks particles and updates neighborlist if necessary.
		void CheckNeighborListUpdate(const ParticleList& particles)
		{
			for(auto& particle : particles)
			{
				auto dist = particle->GetCheckpointDist();
				ApplyMinimumImage(&dist);
				if(fdot(dist,dist) > _skinsq/4.0)
				{	
					UpdateNeighborList();
					return;
				}
			}
		}

		// Check if neighbor lists need updating based on particle.
		void CheckNeighborListUpdate(Particle* p)
		{
			auto dist = p->GetCheckpointDist();
			ApplyMinimumImage(&dist);
			if(fdot(dist,dist) > _skinsq/4.0)	
				UpdateNeighborList();
		}

		// Get a specific particle based on location.
		Particle* SelectParticle(int location)
		{
			return _particles[location];
		}

		// Get a specific particle based on location (const).
		const Particle* SelectParticle(int location) const
		{
			return _particles[location];
		}

		// Select the "ith" particle of species "species". 
		Particle* SelectParticleBySpecies(int species, int i)
		{
			auto it = std::find_if(_particles.begin(), _particles.end(),[=](Particle* p){
				return p->GetSpeciesID() == species;
			});

			while(--i >= 0 && it != _particles.end())
			{
				++it;
    			it =  std::find_if(it, _particles.end(),[=](Particle* p){
					return p->GetSpeciesID() == species;
				});
			}

			return *it;
		}

		// Select the "ith" particle of species "species" (const). 
		const Particle* SelectParticleBySpecies(int species, int i) const
		{
			return SelectParticleBySpecies(species, i);
		}

		// Select the "ith" primitive of species "species".
		Particle* SelectPrimitiveBySpecies(int species, int i)
		{
			auto it = std::find_if(_primitives.begin(), _primitives.end(), [=](Particle* p){
				return p->GetSpeciesID() == species;
			});

			while(--i >= 0 && it != _primitives.end())
			{
				++it;
				it = std::find_if(it, _primitives.end(), [=](Particle* p){
					return p->GetSpeciesID() == species;
				});
			}

			return *it;
		}

		// Select the "ith" primitive of species "species" (const).
		const Particle* SelectPrimitiveBySpecies(int species, int i) const
		{
			return SelectPrimitiveBySpecies(species, i);
		}

		// Select a primitive particle by location.
		Particle* SelectPrimitive(int location)
		{
			return _primitives[location];
		}

		// Select a primitive particle by location (const).
		const Particle* SelectPrimitive(int location) const
		{
			return _primitives[location];
		}
		
		// Add a particle. Option to update neighbor list or not.
		void AddParticle(Particle&& particle, bool updatelist = true)
		{
			_particles.push_back(std::move(&particle));
			// Do this after since we are moving.
			ConfigureParticle(_particles.back(), updatelist); 
		}

		// Add a particle.
		void AddParticle(Particle* particle, bool updatelist = true)
		{
			ConfigureParticle(particle, updatelist);
			_particles.push_back(particle);
		}

		// Remove a specific particle based on location.
		void RemoveParticle(int location)
		{
			Particle* p = _particles[location];
			RemoveParticle(p);
		}

		// Remove particle based on pointer.
		void RemoveParticle(Particle* particle) 
		{
			auto it = std::find(_particles.begin(), _particles.end(), particle);
			if(it != _particles.end())
			{
				particle->RemoveFromNeighbors();
				particle->ClearNeighborList();

				particle->RemoveObserver(this);
				particle->SetWorld(nullptr);
				RemoveParticleComposition(particle);

				_particles.erase(it);
			}
		}

		// Stash particle itself. Make sure 
		// it is heap allocated!
		void StashParticle(Particle* particle)
		{
			int id = particle->GetSpeciesID();

			// If particle has a world, remove it. 
			// Stashed particles cannot belong to a world.
			if(particle->GetWorld() != nullptr)
			{
				particle->GetWorld()->RemoveParticle(particle);
				particle->SetWorld(nullptr);
			}
			
			// If ID doesn't exist yet, create it and fill in
			// the middle. 
			if((int)_stash.size() - 1 < id)
				_stash.resize(id + 1);

			_stash[id].push(particle);
		}

		// Stash "n" copies of particle.
		void StashParticle(Particle* particle, int n)
		{
			int id = particle->GetSpeciesID();

			// If particle has a world, remove it. 
			// Stashed particles cannot belong to a world.
			if(particle->GetWorld() != nullptr)
			{
				particle->GetWorld()->RemoveParticle(particle);
				particle->SetWorld(nullptr);
			}
			
			// If ID doesn't exist yet, create it and fill in
			// the middle. 
			if((int)_stash.size() - 1 < id)
				_stash.resize(id + 1);

			for(int i = 0; i < n; ++i)
				_stash[id].push(particle->Clone());
		}

		// Get a particle from the stash. 
		Particle* UnstashParticle(int species)
		{
			assert(species < (int)_stash.size());

			auto* p = _stash[species].front();
			_stash[species].pop();

			// If it's the last one, fill 'er up.
			if(_stash[species].empty())
				for(int i = 0; i < 100; ++i)
					_stash[species].push(p->Clone());

			return p;
		}

		// Applies periodic boundaries to positions.
		inline void ApplyPeriodicBoundaries(Position* position) const 
		{
			if(_periodx)
				(*position)[0] -= _H(0,0)*ffloor((*position)[0]/_H(0,0));
			if(_periody)
				(*position)[1] -= _H(1,1)*ffloor((*position)[1]/_H(1,1));
		}
	
		// Applies minimum image convention to distances. 
		inline void ApplyMinimumImage(Position* position) const
		{
			if(_periodx)
			{
				if((*position)[0] > _H(0,0)/2.0)
					(*position)[0] -= _H(0,0);
				else if((*position)[0] < -_H(0,0)/2.0)
					(*position)[0] += _H(0,0);
			}

			if(_periody)
			{
				if((*position)[1] > _H(1,1)/2.0)
					(*position)[1] -= _H(1,1);
				else if((*position)[1] < -_H(1,1)/2.0)
					(*position)[1] += _H(1,1);
			}
		}

		// Packs a world with the given particle blueprints and 
		// compositions to with "count" total particles and a 
		// specified density.
		void PackWorld(const std::vector<Particle*>& particles,
					   const std::vector<double>& fractions, 
					   int count, double density);

		// Configure Particles in the lattice. For n particles and n fractions, 
		// the lattice will be initialized with the appropriate composition.  
		// If max is set, it will stop at that number.
		void PackWorld(const std::vector<Particle*>& particles,
					   const std::vector<double>& fractions, int max = 0);

		/***************************
		 *                         *		
		 *   Getters and setters   *
		 *                         *
		 ***************************/

		// Get number of high level particles in the world.
		int GetParticleCount() const
		{
			return (int)_particles.size();
		}

		// Get number of primitives in the world.
		int GetPrimitiveCount() const
		{
			return (int)_primitives.size();
		}

		// Get neighbor list cutoff radius.
		double GetNeighborRadius() const { return _ncut; }

		// Set neighbor list radius cutoff.
		void SetNeighborRadius(double ncut)
		{
			_ncutsq = ncut*ncut;
			_ncut = ncut;
		}

		// Get the effective skin thickness of the world.
		double GetSkinThickness() const { return _skin;	}

		// Set the effective skin thickness of the world.
		void SetSkinThickness(double skin)
		{
			_skin = skin;
			_skinsq = skin*skin;
		}

		// Get system composition.
		const CompositionList& GetComposition() const
		{
			return _composition;
		}
		
		// Get the system number density.
		double GetNumberDensity() const
		{
			return (double)GetParticleCount()/GetVolume();
		}

		// Get system density (mol/cm^3)
		double GetDensity() const 
		{
			auto& sim = SimInfo::Instance();
			return 1.0e24*(double)GetParticleCount()/(sim.GetNa()*GetVolume());
		}

		// Gets the optional string ID for a world.
		std::string GetStringID() const { return _stringid; }

		// Get unique world ID.
		int GetID() const { return _id; }

		// Sets an optional string ID for a world.
		void SetStringID(std::string stringid) { _stringid = stringid; }

		// Get seed.
		unsigned GetSeed() const { return _seed; }

		// Get world temperature.
		double GetTemperature() const { return _temperature; }

		// Sets world temperature.
		void SetTemperature(double temperature) { _temperature = temperature; }

		// Get world pressure.
		Pressure GetPressure() const 
		{
			auto p = _pressure;
			auto& sim = SimInfo::Instance();
			p.ideal = GetParticleCount()*sim.GetkB()*_temperature/GetVolume(); 
			p *= sim.GetPressureConv();
			return p; 
		}
		
		// Set world pressure. 
		void SetPressure(const Pressure& p) { _pressure = p; }

		// Increment world pressure. (aka p += dp).
		void IncrementPressure(const Pressure& dp) { _pressure += dp; }

		// Get world energy. 
		Energy GetEnergy() const { return _energy; }

		// Set world energy. 
		void SetEnergy(const Energy& e) { _energy = e; }

		// Increment world energy (e += de).
		void IncrementEnergy(const Energy& de) { _energy += de; }

		// Get the de Broglie wavelength for species.
		double GetWavelength(int species) const
		{
			if((int)_debroglie.size() - 1 < species)
			{
				std::cerr << "de Broglie wavelength for species " << species
						  << " not defined." << std::endl;
				exit(-1);
			}

			return _debroglie[species];
		}

		// Get species chemical potential. 
		// Returns 0 if species not found.
		double GetChemicalPotential(int species) const
		{
			if((int)_chemp.size() - 1 < species)
				return 0;

			return _chemp[species];
		}

		// Get species chemical potential.
		// Returns 0 if species not found.
		double GetChemicalPotential(const std::string& species) const
		{
			auto& spec = Particle::GetSpeciesList();
			auto it = std::find(spec.begin(), spec.end(), species);
			if(it == spec.end())
				return 0;

			int id = it - spec.begin();

			return GetChemicalPotential(id);
		}

		// Sets the chemical potential for species in the 
		// world.
		void SetChemicalPotential(int species, double chemp)
		{
			if((int)_chemp.size() - 1 < species)
				_chemp.resize(species + 1, 0);

			_chemp[species] = chemp;	
		}

		// Sets the chemical potential for species in the 
		// world.
		void SetChemicalPotential(std::string species, double chemp)
		{
			auto& spec = Particle::GetSpeciesList();
			auto it = std::find(spec.begin(), spec.end(), species);
			if(it == spec.end())
			{
				std::cerr << "Cannot set chemical potential for "
							 "nonexistent species \"" + species +  "\"." 
						  << std::endl;
			}
			
			int id = it - spec.begin();
			if((int)_chemp.size() - 1 < id)
				_chemp.resize(id + 1, 0);

			_chemp[id] = chemp;	
		}

		// Get H matrix.
		inline const Matrix2D& GetHMatrix() const { return _H; }

		// Get system volume.
		inline double GetVolume() const
		{
			return _H(0,0)*_H(1,1);
		}

		// Sets the volume of the world isotropically and if scale is true, 
		// scale the coordinates of the particles in the system. 
		// Energy recalculation after this procedure is recommended.
		// If scaling is not applied, periodic boundary conditions 
		// are applied to all particles. The neighbor list is auto regenerated.
		void SetVolume(double v, bool scale);

		// Gets/sets the periodicity of the x-coordinate.
		bool GetPeriodicX() const { return _periodx; }
		void SetPeriodicX(bool periodic) { _periodx = periodic; }

		// Gets/sets the periodicity of the y-coordinate.
		bool GetPeriodicY() const { return _periody; }
		void SetPeriodicY(bool periodic) { _periody = periodic; }

		// Iterators.
		iterator begin() { return _particles.begin(); }
		iterator end() { return _particles.end(); }
		const_iterator begin() const { return _particles.begin(); }
		const_iterator end() const { return _particles.end(); }

		/***********************************
		 *                                 *
		 * Other interface implementations *
		 *                                 *
		 ***********************************/

		// Particle observer to update world composition.
		virtual void ParticleUpdate(const ParticleEvent& pEvent) override
		{
			if(pEvent.species)
				ModifyParticleComposition(pEvent);

			if(pEvent.child_add)
				AddParticleComposition(pEvent.GetChild());
	
			if(pEvent.child_remove)
				RemoveParticleComposition(pEvent.GetChild());
		}

		// Accept a visitor.
		virtual void AcceptVisitor(Visitor &v) const override
		{
			VisitChildren(v);
		}

		// Serialize world.
		virtual void Serialize(Json::Value& json) const override;

		/**********************************
		 *                                *
		 *      Static world builders     *
		 *                                *
		 **********************************/

		// Builds a world from JSON value. Returns a pointer to world object, 
		// or throws a BuildException if validation fails. Object lifetime 
		// is caller's responsibility!
		static World* Build(const Json::Value& json,
							const Json::Value& blueprints);

		virtual ~World()
		{
			for(auto& p : _particles)
					delete p;

			for(auto& s : _stash)
				while(!s.empty())
				{
					delete s.front();
					s.pop();
				}

			_stash.clear();
			_particles.clear();
		}
	};
}
