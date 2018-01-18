#include <random>
#include "Move.h"
#include "MoveManager.h"
#include "json/json.h"
#include "schema.h"
#include "../Validator/ObjectRequirement.h"
#include "../Validator/ArrayRequirement.h"
#include "../Simulation/SimException.h"
#include "../Worlds/WorldManager.h"
//#include "FlipSpinMove.h"
#include "TranslateMove.h"
#include "TranslatePrimitiveMove.h"
//#include "DirectorRotateMove.h"
//#include "ParticleSwapMove.h"
//#include "RandomIdentityMove.h"
#include "RotateMove.h"
//#include "SpeciesSwapMove.h"
//#include "VolumeSwapMove.h"
//#include "VolumeScaleMove.h"
//#include "InsertParticleMove.h"
//#include "DeleteParticleMove.h"
//#include "AnnealChargeMove.h"
//#include "AcidTitrationMove.h"
//#include "AcidReactionMove.h"
//#include "WidomInsertionMove.h"

using namespace Json;

namespace SAPHRON
{
	Move* Move::BuildMove(const Json::Value &json, SAPHRON::MoveManager *mm, WorldManager* wm)
	{
		return BuildMove(json, mm, wm, "#/moves");
	}

	Move* Move::BuildMove(const Value &json, 
						  MoveManager *mm, 
						  WorldManager* wm, 
						  const std::string& path)
	{
		ObjectRequirement validator;
		Value schema;
		Reader reader;

		Move* move = nullptr;

		// Random device for seed generation. 
		std::random_device rd;
		auto maxi = std::numeric_limits<int>::max();
		auto seed = json.get("seed", rd() % maxi).asUInt();

		// Get move type. 
		std::string type = json.get("type", "none").asString();

		if(type == "AcidReaction")
		{
		}
		else if(type == "AcidTitrate")
		{
		}
		else if(type == "DirectorRotate")
		{
		}
		else if(type == "Rotate")
		{
			reader.parse(JsonSchema::RotateMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());

			double dmax = json["maxangle"].asDouble();

			move = new RotateMove(dmax, seed);
		}
		else if(type == "Translate")
		{
			reader.parse(JsonSchema::TranslateMove, schema);
			validator.Parse(schema, path);

			// Validate inputs.
			validator.Validate(json, path);
			if(validator.HasErrors())
				throw BuildException(validator.GetErrors());
		
			if(json["dx"].isObject())
			{
				std::map<std::string, double> dx; 
				for(auto& s : json["dx"].getMemberNames())
					dx[s] = json["dx"][s].asDouble();

				auto expl = json.get("explicit_draw", false).asBool();

				move = new TranslateMove(dx, expl, seed);
			}
			else
			{
				auto dx = json["dx"].asDouble();
				move = new TranslateMove(dx, seed);
			}
		}
		else if(type == "TranslatePrimitive")
		{
		}
		else
		{
			throw BuildException({path + ": Unknown move type specified."});
		}

		// Add to appropriate species pair.
		try{
				int weight = json.get("weight", 1).asUInt();
				mm->AddMove(move, weight);
			} catch(std::exception& e) {
				delete move;
				throw BuildException({
					e.what()
				});
		}

		return move;
	}

	void Move::BuildMoves(const Json::Value &json, 
						  SAPHRON::MoveManager *mm, 
						  WorldManager* wm, 
						  MoveList &mvlist)
	{
		ArrayRequirement validator;
		Value schema;
		Reader reader;

		reader.parse(JsonSchema::Moves, schema);
		validator.Parse(schema, "#/moves");

		// Validate high level schema.
		validator.Validate(json, "#/moves");
		if(validator.HasErrors())
			throw BuildException(validator.GetErrors());

		// Loop through moves.
		int i = 0;
		for(auto& m : json)
		{
			mvlist.push_back(BuildMove(m, mm, wm, "#/moves/" + std::to_string(i)));
			++i;
		}

	}
}