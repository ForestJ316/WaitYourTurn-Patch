#include <Utils.h>

namespace Utils
{
	/*
	auto GetObjectFromForm(RE::TESForm* a_form, const char* a_scriptName) -> Script::ObjectPtr
	{
		Script::ObjectPtr object = Script::GetObject(a_form, a_scriptName);
		if (!object)
		{
			logger::warn("Script {} is not attached to Form Type {}."sv, a_scriptName, a_form->GetFormID());
		}
		return object;
	}*/

	auto GetObjectFromEffect(RE::ActiveEffect* a_effect, const char* a_scriptName) -> Script::ObjectPtr
	{
		Script::ObjectPtr object = Script::GetObject(a_effect, a_scriptName);
		if (!object)
		{
			logger::warn("Script {} is not attached to Magic Effect {}."sv, a_scriptName, a_effect->GetBaseObject()->GetName());
		}
		return object;
	}

	RE::Character* GetRandomEnemy(std::unordered_map<RE::Character*, EnemyHandler::Enemy::EnemyState>& EnemyList)
	{
		std::vector<RE::Character*> charVector;
		for (auto i = EnemyList.begin(); i != EnemyList.end(); ++i)
		{
			if (!i->second.bIsDeadOrInvalid && i->second.bIsLocked)
			{
				charVector.emplace_back(i->first);
			}
		}
		// Get random valid enemy actor
		int vectorSize = (int)charVector.size();
		if (vectorSize > 1)
		{
			int randNum = GenerateRandomInt(1, vectorSize);
			return charVector.at(randNum-1);
		}
		else if (vectorSize == 1)
		{
			return charVector.front();
		}
		return nullptr;
	}

	int GenerateRandomInt(int min, int max)
	{
		std::random_device random_seed;
		std::minstd_rand gen_random(random_seed());
		std::uniform_int_distribution<> dist(min, max);
		return dist(gen_random);
	}

	float GenerateRandomFloat(float min, float max)
	{
		std::random_device random_seed;
		std::minstd_rand gen_random(random_seed());
		std::uniform_real_distribution<float> dist(min, max);
		return dist(gen_random);
	}
}
