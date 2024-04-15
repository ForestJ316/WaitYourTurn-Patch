#include "Settings.h"
#include "Utils.h"
#include "Offsets.h"

#include "EnemyHandler.h"

namespace EnemyHandler
{
	void CombatHandler::Initialize()
	{
		logger::info("Initializing EnemyHandler...");

		RE::ScriptEventSourceHolder::GetSingleton()->GetEventSource<RE::TESDeathEvent>()->AddEventSink(CombatHandler::GetSingleton());
		logger::info("Registered {}"sv, typeid(RE::TESDeathEvent).name());

		auto dataHandler = RE::TESDataHandler::GetSingleton();
		FilterEffectID = dataHandler->LookupForm(0x805, "WaitYourTurn.esp")->GetFormID();
		if (!FilterEffectID)
			logger::critical("Failed to initialize Filter Enemy Effect");
		BlockEffectID = dataHandler->LookupForm(0x807, "WaitYourTurn.esp")->GetFormID();
		if (!BlockEffectID)
			logger::critical("Failed to initialize Enemy Block Effect");
		PlayerCombatEffectID = dataHandler->LookupForm(0x80A, "WaitYourTurn.esp")->GetFormID();
		if (!PlayerCombatEffectID)
			logger::critical("Failed to initialize Player Combat Effect");

		logger::info("...EnemyHandler initialized.");
	}

	void CombatHandler::UpdateSettings()
	{
		if (!Settings::bModEnabled)
			return;
		auto combatHandler = CombatHandler::GetSingleton();
		if (combatHandler->bModStarted)
		{
			combatHandler->SyncSettings();
			for (auto it = combatHandler->EnemyList.begin(); it != combatHandler->EnemyList.end(); ++it)
			{
				if (!it->second.bIsDeadOrInvalid && !it->second.bIsLocked && Hooks::PollingHandler[it->first].EnemyHasPackage)
				{
					combatHandler->Enemy::iCurrentUnlockedEnemies -= 1;
					if (it->first.get().get() && it->first.get().get()->As<RE::Character>())
					{
						auto a_enemyCharacter = it->first.get().get()->As<RE::Character>();
						it->second.bIsLocked = true;
						combatHandler->BlockHandler::EnemyRemoveBlockSpell(a_enemyCharacter);
						combatHandler->AddEnemyToList(a_enemyCharacter, false);
					}
					else
					{
						combatHandler->EraseEnemy(it->first);
					}
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
		Enemy::iCurrentUnlockedEnemies = 0;
		EnemyList.clear();
		Hooks::PollingHandler.clear();
		Settings::SettingsApplied = false;
	}

	CombatHandler::EventResult CombatHandler::ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>*)
	{
		if (Settings::bModEnabled)
		{
			if (a_event && a_event->actorDying)
			{
				auto a_enemyHandle = a_event->actorDying->GetHandle();
				auto it = EnemyList.find(a_enemyHandle);
				if (it != EnemyList.end() && !EnemyList[a_enemyHandle].bIsDeadOrInvalid)
					Enemy::EnemyDeadOrInvalid(a_enemyHandle, EnemyList[a_enemyHandle], false);
			}
		}
		return EventResult::kContinue;
	}

	void Hooks::OnEffectStart(RE::ActiveEffect* a_effect)
	{
		_OnEffectStart(a_effect);
		
		const RE::FormID effectID = a_effect->GetBaseObject()->GetFormID();
		// Filter effect on
		// Check whether actor object exists (for loading screens etc.)
		if (effectID == CombatHandler::FilterEffectID && a_effect->target)
		{
			auto a_enemy = skyrim_cast<RE::Character*>(a_effect->target);
			if (!a_enemy || a_enemy->IsPlayerRef()) return;

			auto combatHandler = CombatHandler::GetSingleton();
			if (!combatHandler->bCombatInitiated)
			{
				combatHandler->bCombatInitiated = true;
				combatHandler->ResetVars();
				combatHandler->SyncSettings();
			}
			// Add enemy to list
			combatHandler->AddEnemyToList(a_enemy, false);
		}
		// Block effect on
		else if (effectID == CombatHandler::BlockEffectID && a_effect->target)
		{
			auto a_enemy = skyrim_cast<RE::Character*>(a_effect->target);
			if (!a_enemy || a_enemy->IsPlayerRef()) return;

			auto combatHandler = CombatHandler::GetSingleton();
			if (!combatHandler->bModStarted) return;

			auto enemyHandle = a_enemy->GetHandle();
			auto& enemyList = combatHandler->EnemyList;
			auto it = enemyList.find(enemyHandle);
			if (it != enemyList.end() && !enemyList[enemyHandle].bIsDeadOrInvalid)
			{
				enemyList[enemyHandle].bBlockSpellActive = true;
				if (!enemyList[enemyHandle].bEnemyIsBlocking && enemyList[enemyHandle].bIsLocked)
					combatHandler->EnemyAddBlockOverride(a_enemy, enemyList[enemyHandle].bEnemyIsBlocking);
			}
		}
	}

	void Hooks::OnEffectFinish(RE::ActiveEffect* a_effect)
	{
		_OnEffectFinish(a_effect);

		const RE::FormID effectID = a_effect->GetBaseObject()->GetFormID();
		// If player stopped combat then just reset vars. Will also trigger for load game
		if (effectID == CombatHandler::PlayerCombatEffectID)
		{
			auto combatHandler = CombatHandler::GetSingleton();
			combatHandler->bCombatInitiated = false;
			combatHandler->bModStarted = false;
			// Check to prevent ResetVars() getting called 2 times (other call on Filter Effect start)
			// if player decides to disable/enable mod in combat
			if (Settings::bModEnabled)
				combatHandler->ResetVars();
		}
		// Filter effect off
		// Toggle enemy dead/invalid variable
		else if (effectID == CombatHandler::FilterEffectID && a_effect->target)
		{
			auto a_enemy = skyrim_cast<RE::Character*>(a_effect->target);
			if (!a_enemy || a_enemy->IsPlayerRef()) return;

			auto combatHandler = CombatHandler::GetSingleton();
			// Remove block spell
			combatHandler->BlockHandler::EnemyRemoveBlockSpell(a_enemy);
			
			auto enemyHandle = a_enemy->GetHandle();
			auto& enemyList = combatHandler->EnemyList;
			auto it = enemyList.find(enemyHandle);
			if (it != enemyList.end() && !enemyList[enemyHandle].bIsDeadOrInvalid)
				combatHandler->EnemyDeadOrInvalid(enemyHandle, enemyList[enemyHandle], true);
		}
		// Block effect off
		else if (effectID == CombatHandler::BlockEffectID && a_effect->target)
		{
			auto a_enemy = skyrim_cast<RE::Character*>(a_effect->target);
			if (!a_enemy || a_enemy->IsPlayerRef()) return;

			auto combatHandler = CombatHandler::GetSingleton();
			if (!combatHandler->bModStarted) return;

			auto enemyHandle = a_enemy->GetHandle();
			auto& enemyList = combatHandler->EnemyList;
			auto it = enemyList.find(enemyHandle);
			if (it != enemyList.end())
			{
				enemyList[enemyHandle].bBlockSpellActive = false;
				if (enemyList[enemyHandle].bEnemyIsBlocking)
					combatHandler->EnemyRemoveBlockOverride(a_enemy, enemyList[enemyHandle].bEnemyIsBlocking);
			}
		}
	}

	void Hooks::OnEffectFinishLoadGame(RE::ActiveEffect* a_effect, RE::BGSLoadFormBuffer* a_buf)
	{
		_OnEffectFinishLoadGame(a_effect, a_buf);
		
		if (!Settings::bModEnabled) return;

		const RE::FormID effectID = a_effect->GetBaseObject()->GetFormID();
		// Check for Filter effect and re-initiate mod accordingly
		if (effectID == CombatHandler::FilterEffectID && a_effect->target
			&& a_effect->conditionStatus.any(RE::ActiveEffect::ConditionStatus::kTrue))
		{
			auto a_enemy = skyrim_cast<RE::Character*>(a_effect->target);
			if (!a_enemy || a_enemy->IsPlayerRef()) return;

			auto combatHandler = CombatHandler::GetSingleton();
			if (!combatHandler->bCombatInitiated)
			{
				combatHandler->bCombatInitiated = true;
				combatHandler->ResetVars();
				combatHandler->SyncSettings();
			}
			// Add enemy to list
			combatHandler->AddEnemyToList(a_enemy, true);
		}
	}

	void Hooks::Update(RE::Character* a_enemy, float a_delta)
	{
		_Update(a_enemy, a_delta);

		if (!CombatHandler::bModStarted || !Settings::bModEnabled) return;
		
		auto a_enemyHandle = a_enemy->GetHandle();
		if (PollingHandler.find(a_enemyHandle) == PollingHandler.end())
			return;

		// Check only Unlocked Enemies or for Effect FinishLoadGame delay
		if (PollingHandler[a_enemyHandle].unlockDuration > 0.f || (PollingHandler[a_enemyHandle].isLoadGame && Settings::SettingsApplied))
		{
			PollingHandler[a_enemyHandle].timer += *g_deltaTimeRealTime;
			// If time passed then try to relock enemy
			if (PollingHandler[a_enemyHandle].timer >= PollingHandler[a_enemyHandle].unlockDuration)
			{
				PollingHandler[a_enemyHandle].timer = 0.f;
				auto combatHandler = CombatHandler::GetSingleton();
				auto& enemyList = combatHandler->EnemyList;
				if (enemyList.find(a_enemyHandle) != enemyList.end())
				{
					// Add check for Effect FinishLoadGame and call lock manually on delay
					if (PollingHandler[a_enemyHandle].isLoadGame == true)
					{
						PollingHandler[a_enemyHandle].isLoadGame = false;
						combatHandler->Enemy::LockEnemy(a_enemyHandle, enemyList[a_enemyHandle], true);
					}
					// Enemy locks on MCM changed, so check works for it also
					else if (!enemyList[a_enemyHandle].bIsLocked)
					{
						bool bIsLOSRelock = PollingHandler[a_enemyHandle].unlockDuration < 1.f ? true : false;
						combatHandler->UpdateEnemyRelock(a_enemyHandle, bIsLOSRelock);
					}
				}
			}
		}
		// LOS check if setting turned on
		else if (Settings::bLOSExperimentalEnabled)
		{
			PollingHandler[a_enemyHandle].LOStimer += *g_deltaTimeRealTime;
			// Check every user-defined seconds. Default is 0.5s.
			if (PollingHandler[a_enemyHandle].LOStimer >= Settings::fLOSExperimentalDelay)
			{
				PollingHandler[a_enemyHandle].LOStimer = 0.f;
				auto a_player = RE::PlayerCharacter::GetSingleton();
				bool arg2; // Dummy arg for reference
				auto combatHandler = CombatHandler::GetSingleton();
				if (a_enemy->HasLineOfSight(a_player, arg2) && !PollingHandler[a_enemyHandle].EnemyHasPackage && !combatHandler->EnemyList[a_enemyHandle].bIsLocked)
				{
					combatHandler->Enemy::LockEnemy(a_enemyHandle, combatHandler->EnemyList[a_enemyHandle], true);
					PollingHandler[a_enemyHandle].EnemyHasPackage = true;
				}
				// Check in case Enemy went out of LOS with package active (interiors only)
				else if (a_player->GetParentCell()->IsInteriorCell() && !a_enemy->HasLineOfSight(a_player, arg2)
					&& PollingHandler[a_enemyHandle].EnemyHasPackage && combatHandler->EnemyList[a_enemyHandle].bIsLocked)
				{
					auto playerPos = a_player->GetPosition();
					auto enemyPos = a_enemy->GetPosition();
					if (playerPos.GetDistance(enemyPos) > 320) // Multiplicative of 64
					{
						combatHandler->Enemy::UnlockEnemy(a_enemyHandle, combatHandler->EnemyList[a_enemyHandle], true);
						PollingHandler[a_enemyHandle].EnemyHasPackage = false;
					}
				}
			}
		}
	}

	void CombatHandler::AddEnemyToList(RE::Character* a_enemy, bool bIsLoadGame)
	{
		auto a_enemyHandle = a_enemy->GetHandle();
		// If new enemy then increment the count
		if (EnemyList.find(a_enemyHandle) == EnemyList.end())
			iEnemyCount += 1;
		// Case for: OnEffectFinish didn't fire, but OnEffectStart did
		else if (!EnemyList[a_enemyHandle].bIsDeadOrInvalid && !EnemyList[a_enemyHandle].bIsLocked && Enemy::iCurrentUnlockedEnemies > 0)
			Enemy::iCurrentUnlockedEnemies -= 1;

		// Add enemy to list / Reinitialize enemy re-entering vars
		// Args: bIsLocked, bIsDeadOrInvalid, bBlockSpellActive, bEnemyIsBlocking
		EnemyList[a_enemyHandle] = { true, false, false, false };

		Hooks::PollingHandler[a_enemyHandle].timer = 0.f;
		Hooks::PollingHandler[a_enemyHandle].isLoadGame = bIsLoadGame;

		// Add block spell
		BlockHandler::EnemyAddBlockSpell(a_enemy);

		// Lock each new enemy first when mod is started
		// Also applies if old enemy re-entering
		if (iEnemyCount >= iEnemiesForModStart)
		{
			if (!bModStarted)
			{
				bModStarted = true;
				// Lock all previous enemies also when mod starts
				if (!bIsLoadGame)
				{
					for (auto it = EnemyList.begin(); it != EnemyList.end(); ++it)
					{
						Enemy::LockEnemy(it->first, it->second, true);
					}
				}
			}
			else if (!bIsLoadGame)
			{
				Enemy::LockEnemy(a_enemyHandle, EnemyList[a_enemyHandle], true);
			}
		}
	}

	void CombatHandler::RaiseEnemyUnlock()
	{
		Locker locker(lock);
		int i = Enemy::iCurrentUnlockedEnemies;
		int iMaxUpdated = iMaxUnlockedEnemies;
		if (Settings::bRandomizeAttackers && iMaxUnlockedEnemies > 1)
			iMaxUpdated = Utils::GenerateRandomInt(1, iMaxUnlockedEnemies);
		// Slight optimization
		if (iMaxUpdated > iEnemyCount)
			iMaxUpdated = iEnemyCount;
		
		while (i < iMaxUpdated)
		{
			i += 1;
			auto enemyHandle = Utils::GetRandomEnemy(EnemyList);
			if (enemyHandle)
			{
				EnemyList[enemyHandle].bIsLocked = false;
				Enemy::UnlockEnemy(enemyHandle, EnemyList[enemyHandle], false);
			}
			// No locked enemies are currently available, stop looping
			else
			{
				break;
			}
		}
	}

	void CombatHandler::EraseEnemy(RE::ObjectRefHandle a_enemyHandle)
	{
		EnemyList.erase(a_enemyHandle);
		Hooks::PollingHandler.erase(a_enemyHandle);
	}

	void CombatHandler::UpdateEnemyRelock(RE::ObjectRefHandle a_enemyHandle, bool bIsLOSRelock)
	{
		Enemy::LockEnemy(a_enemyHandle, EnemyList[a_enemyHandle], bIsLOSRelock);
	}
}
