#pragma once

#include "Enemy.h"

namespace Utils
{
	auto GetObjectFromForm(RE::TESForm* a_form, const char* a_scriptName) -> Script::ObjectPtr;
	//auto GetObjectFromEffect(RE::ActiveEffect* a_effect, const char* a_scriptName) -> Script::ObjectPtr;

	RE::ObjectRefHandle GetRandomEnemy(std::unordered_map<RE::ObjectRefHandle, EnemyHandler::Enemy::EnemyState>& EnemyList);

	int GenerateRandomInt(int min, int max);
	float GenerateRandomFloat(float min, float max);
}
