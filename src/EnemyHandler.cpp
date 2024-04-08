#include <Settings.h>
#include <Utils.h>

#include <EnemyHandler.h>
#include <unordered_map>

namespace EnemyHandler
{
	void CombatHandler::Initialize()
	{
		auto scriptEventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
		scriptEventSourceHolder->GetEventSource<RE::TESDeathEvent>()->AddEventSink(CombatHandler::GetSingleton());
		logger::info("Registered {}"sv, typeid(RE::TESDeathEvent).name());
	}

	void CombatHandler::UpdateSettings()
	{
		if (!Settings::bModEnabled)
			return;
		auto combatHandler = CombatHandler::GetSingleton();
		if (combatHandler->bInitiated)
		{
			combatHandler->SyncSettings();
			Callback callback;
			for (auto it = combatHandler->EnemyList.begin(); it != combatHandler->EnemyList.end(); ++it)
			{
				if (!it->second.bIsDeadOrInvalid && !it->second.bIsLocked)
				{
					it->second.bIsLocked = true;
					combatHandler->iCurrentUnlockedEnemies -= 1;
					if (BlockHandler::block_spell && !Settings::bBlockWhileCircling && it->first->HasSpell(BlockHandler::block_spell))
					{
						it->first->RemoveSpell(BlockHandler::block_spell);
					}
					callback.reset(new VMCallback(it->first, it->second, false, false, true));
					auto effectScriptObject = Utils::GetObjectFromEffect(it->second.effect, "WYT_EnemyUnlockScript");
					Script::DispatchMethodCall(effectScriptObject, "MCMChanged"sv, callback);
				}
			}
		}
	}

	void CombatHandler::SyncSettings()
	{
		auto player = RE::PlayerCharacter::GetSingleton();
		const auto bIsInterior = player->GetParentCell()->IsInteriorCell();
		if (bIsInterior)
		{
			iEnemiesForModStart = Settings::iEnemiesForModStartInterior;
			iMaxUnlockedEnemies = Settings::iMaxUnlockedEnemiesInterior;
			fWindowIntervalMin = Settings::fWindowIntervalMinInterior;
			fWindowIntervalMax = Settings::fWindowIntervalMaxInterior;
		}
		else
		{
			iEnemiesForModStart = Settings::iEnemiesForModStartExterior;
			iMaxUnlockedEnemies = Settings::iMaxUnlockedEnemiesExterior;
			fWindowIntervalMin = Settings::fWindowIntervalMinExterior;
			fWindowIntervalMax = Settings::fWindowIntervalMaxExterior;
		}
	}

	void CombatHandler::ResetVars()
	{
		iEnemyCount = 0;
		iCurrentUnlockedEnemies = 0;
		EnemyList.clear();
	}

	CombatHandler::EventResult CombatHandler::ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>*)
	{
		if (Settings::bModEnabled)
		{
			auto a_enemy = a_event->actorDying->As<RE::Character>();
			auto it = EnemyList.find(a_enemy);
			if (it != EnemyList.end() && !EnemyList[a_enemy].bIsDeadOrInvalid)
				Enemy::EnemyDeadOrInvalid(a_enemy, EnemyList[a_enemy], iCurrentUnlockedEnemies, false);
		}
		return EventResult::kContinue;
	}

	void Hooks::OnEffectStart(RE::ActiveEffect* a_effect)
	{
		_OnEffectStart(a_effect);
		
		const RE::FormID effectID = a_effect->GetBaseObject()->GetLocalFormID();

		// Filter effect on
		// Check whether actor object exists (for loading screens etc.)
		if (effectID == 0x805 && a_effect->target)
		{
			auto a_enemy = skyrim_cast<RE::Character*>(a_effect->target);
			if (!a_enemy) return;

			auto combatHandler = CombatHandler::GetSingleton();
			if (!combatHandler->bInitiated)
			{
				combatHandler->bInitiated = true;
				combatHandler->ResetVars();
				combatHandler->SyncSettings();
			}
			// Add enemy to list
			combatHandler->AddEnemyToList(a_enemy, a_effect);
		}
		// Block effect on
		else if (effectID == 0x807 && a_effect->target)
		{
			auto a_enemy = skyrim_cast<RE::Character*>(a_effect->target);
			if (!a_enemy) return;

			auto combatHandler = CombatHandler::GetSingleton();
			auto& enemyList = combatHandler->EnemyList;
			auto it = enemyList.find(a_enemy);
			if (it != enemyList.end() && !enemyList[a_enemy].bIsDeadOrInvalid)
			{
				enemyList[a_enemy].bBlockSpellActive = true;
				if (!enemyList[a_enemy].bEnemyIsBlocking && enemyList[a_enemy].bIsLocked)
					combatHandler->EnemyAddBlockOverride(a_enemy, enemyList[a_enemy].bEnemyIsBlocking);
			}
		}
	}

	void Hooks::OnEffectFinish(RE::ActiveEffect* a_effect)
	{
		_OnEffectFinish(a_effect);

		const RE::FormID effectID = a_effect->GetBaseObject()->GetLocalFormID();
		// If player stopped combat then just reset vars
		// Will also trigger for load game
		if (effectID == 0x80A )
		{
			auto combatHandler = CombatHandler::GetSingleton();
			combatHandler->bInitiated = false;
			// Check to prevent ResetVars() getting called 2 times (other call on Filter Effect start)
			// if player decides to disable/enable mod in combat
			if (Settings::bModEnabled)
				combatHandler->ResetVars();
		}
		// Filter effect off
		// Check whether actor object exists (for loading screens etc.)
		// Toggle enemy dead/invalid variable
		else if (effectID == 0x805 && a_effect->target)
		{
			auto a_enemy = skyrim_cast<RE::Character*>(a_effect->target);
			if (!a_enemy) return;

			// Remove block spell
			if (BlockHandler::block_spell && Settings::bBlockWhileCircling)
				a_enemy->RemoveSpell(BlockHandler::block_spell);

			auto combatHandler = CombatHandler::GetSingleton();
			auto& enemyList = combatHandler->EnemyList;
			auto it = enemyList.find(a_enemy);
			if (it != enemyList.end())
				combatHandler->EnemyDeadOrInvalid(a_enemy, enemyList[a_enemy], combatHandler->iCurrentUnlockedEnemies, true);
		}
		// Block effect off
		else if (effectID == 0x807 && a_effect->target)
		{
			auto a_enemy = skyrim_cast<RE::Character*>(a_effect->target);
			if (!a_enemy) return;

			auto combatHandler = CombatHandler::GetSingleton();
			auto& enemyList = combatHandler->EnemyList;
			auto it = enemyList.find(a_enemy);
			if (it != enemyList.end())
			{
				enemyList[a_enemy].bBlockSpellActive = false;
				if (enemyList[a_enemy].bEnemyIsBlocking)
					combatHandler->EnemyRemoveBlockOverride(a_enemy, enemyList[a_enemy].bEnemyIsBlocking);
			}
		}
	}

	void Hooks::OnEffectFinishLoadGame(RE::ActiveEffect* a_effect, RE::BGSLoadFormBuffer* a_buf)
	{
		_OnEffectFinishLoadGame(a_effect, a_buf);
		
		if (!Settings::bModEnabled) return;

		const RE::FormID effectID = a_effect->GetBaseObject()->GetLocalFormID();
		// Check for Filter effect and re-initiate mod accordingly
		if (effectID == 0x805 && a_effect->target
			&& a_effect->conditionStatus.any(RE::ActiveEffect::ConditionStatus::kTrue))
		{
			auto a_enemy = skyrim_cast<RE::Character*>(a_effect->target);
			if (!a_enemy) return;

			auto combatHandler = CombatHandler::GetSingleton();
			if (!combatHandler->bInitiated)
			{
				combatHandler->bInitiated = true;
				combatHandler->SyncSettings();
			}
			// Add enemy to list
			combatHandler->AddEnemyToList(a_enemy, a_effect);
		}
	}

	void CombatHandler::AddEnemyToList(RE::Character* a_enemy, RE::ActiveEffect* a_effect)
	{
		// If new enemy then increment the count
		auto it = EnemyList.find(a_enemy);
		if (it == EnemyList.end())
			iEnemyCount += 1;
		// Case for: OnEffectFinish didn't fire, but OnEffectStart did
		else if (!EnemyList[a_enemy].bIsDeadOrInvalid && !EnemyList[a_enemy].bIsLocked)
			iCurrentUnlockedEnemies -= 1;

		// Add enemy to list / Reinitialize enemy re-entering vars
		// Args: bIsLocked, bIsDeadOrInvalid, bBlockSpellActive, bEnemyIsBlocking, magic effect
		EnemyList[a_enemy] = { true, false, false, false, a_effect };

		// Add block spell
		if (BlockHandler::block_spell && Settings::bBlockWhileCircling && !a_enemy->HasSpell(BlockHandler::block_spell))
			a_enemy->AddSpell(BlockHandler::block_spell);

		// Lock each new enemy first when mod is started
		// Also applies if old enemy re-entering
		if (iEnemyCount >= iEnemiesForModStart)
			Enemy::LockEnemy(a_enemy, EnemyList[a_enemy], iCurrentUnlockedEnemies, true);
	}

	void CombatHandler::RaiseEnemyUnlock()
	{
		Locker locker(lock);

		int i = iCurrentUnlockedEnemies;
		int iMaxUpdated = iMaxUnlockedEnemies;
		if (Settings::bRandomizeAttackers && iMaxUnlockedEnemies > 1)
			iMaxUpdated = Utils::GenerateRandomInt(1, iMaxUnlockedEnemies);
		// Slight optimization
		if (iMaxUpdated > iEnemyCount)
			iMaxUpdated = iEnemyCount;
		
		while (i < iMaxUpdated)
		{
			i += 1;
			auto enemyID = Utils::GetRandomEnemy(EnemyList);
			if (enemyID)
			{
				EnemyList[enemyID].bIsLocked = false;
				Enemy::UnlockEnemy(enemyID, EnemyList[enemyID], iCurrentUnlockedEnemies);
			}
			// No locked enemies are currently available, stop looping
			else if (enemyID == nullptr)
			{
				break;
			}
		}
	}

	// Native function
	void CombatHandler::UpdateEnemyRelock(RE::Character* a_enemy)
	{
		if (EnemyList.find(a_enemy) != EnemyList.end())
		{
			bool bIsModStart = EnemyList[a_enemy].bIsLocked ? true : false;
			Enemy::LockEnemy(a_enemy, EnemyList[a_enemy], iCurrentUnlockedEnemies, bIsModStart);
		}
	}

	// Native function
	bool CombatHandler::GetEnemyLockedState(RE::Character* a_enemy)
	{
		if (iEnemyCount >= iEnemiesForModStart && EnemyList[a_enemy].bIsLocked)
			return true;
		else
			return false;
	}
}
