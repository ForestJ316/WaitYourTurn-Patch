#include "Settings.h"
#include "Utils.h"

#include "Enemy.h"
#include "EnemyHandler.h"
#include "BlockHandler.h"

namespace EnemyHandler
{
	void Enemy::InitializeCirclePackage()
	{
		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		const auto package_form = dataHandler->LookupForm(0x800, "WaitYourTurn.esp"); // CirclePackage Form ID
		if (package_form->Is(RE::TESPackage::FORMTYPE))
			package = package_form->As<RE::TESPackage>();
		if (package)
		{
			logger::info("Initialized Package {} (WYT_CirclePackage)."sv, package->GetFormID());
		}
		else if (package_form)
		{
			logger::critical("Couldn't initialize Package (WYT_CirclePackage) from Form {}."sv, package_form->GetFormID());
		}
	}

	void Enemy::LockEnemy(RE::ObjectRefHandle a_enemyHandle, EnemyState& a_enemyState, bool bIsModStart)
	{
		if (a_enemyState.bIsDeadOrInvalid)
			return;
		// Do Lock
		Locker locker(lock);
		if (package)
		{
			if (a_enemyHandle.get().get() && a_enemyHandle.get().get()->As<RE::Character>())
			{
				a_enemyState.bIsLocked = true;
				if (!bIsModStart)
					iCurrentUnlockedEnemies -= 1;
				Hooks::PollingHandler[a_enemyHandle].timer = 0.f;
				Hooks::PollingHandler[a_enemyHandle].unlockDuration = 0.f;

				auto a_enemyCharacter = a_enemyHandle.get().get()->As<RE::Character>();
				Callback callback;
				callback.reset(new VMCallback(a_enemyHandle, a_enemyState, true));
				// If LOS Setting enabled then do appropriate checks
				if (Settings::bLOSExperimentalEnabled)
				{
					auto a_player = RE::PlayerCharacter::GetSingleton();
					bool arg2; // dummy arg
					if (a_enemyCharacter->HasLineOfSight(a_player, arg2))
					{
						Script::DispatchStaticCall("ActorUtil"sv, "AddPackageOverride"sv, callback, std::move(a_enemyCharacter), std::move(package), 100, 1);
						Hooks::PollingHandler[a_enemyHandle].EnemyHasPackage = true;
					}
					else
					{
						a_enemyState.bIsLocked = false;
						Hooks::PollingHandler[a_enemyHandle].EnemyHasPackage = false;
					}
				}
				else
				{
					Script::DispatchStaticCall("ActorUtil"sv, "AddPackageOverride"sv, callback, std::move(a_enemyCharacter), std::move(package), 100, 1);
				}
			}
			else
			{
				CombatHandler::GetSingleton()->EraseEnemy(a_enemyHandle);
			}
		}
	}

	void Enemy::LockCallback(RE::ObjectRefHandle a_enemyHandle, EnemyState& a_enemyState)
	{
		if (a_enemyHandle.get().get() && a_enemyHandle.get().get()->As<RE::Character>())
		{
			auto a_enemyCharacter = a_enemyHandle.get().get()->As<RE::Character>();
			a_enemyCharacter->EvaluatePackage();
			
			auto combatHandler = CombatHandler::GetSingleton();
			if (a_enemyState.bBlockSpellActive == true && a_enemyState.bIsLocked) // Race-condition check in case enemy unlocked before callback
			{
				combatHandler->EnemyAddBlockOverride(a_enemyCharacter, a_enemyState.bEnemyIsBlocking);
			}
			combatHandler->RaiseEnemyUnlock();
		}
		else
		{
			CombatHandler::GetSingleton()->EraseEnemy(a_enemyHandle);
		}
	}

	void Enemy::UnlockEnemy(RE::ObjectRefHandle a_enemyHandle, EnemyState& a_enemyState, bool bIsUnlockLOS)
	{
		if (a_enemyState.bIsDeadOrInvalid)
			return;
		// Do unlock
		if (bIsUnlockLOS)
			Locker locker(lock);
		if (package)
		{
			if (a_enemyHandle.get().get() && a_enemyHandle.get().get()->As<RE::Character>())
			{
				a_enemyState.bIsLocked = false;
				if (!bIsUnlockLOS)
					iCurrentUnlockedEnemies += 1;
				auto a_enemyCharacter = a_enemyHandle.get().get()->As<RE::Character>();
				Callback callback;
				callback.reset(new VMCallback(a_enemyHandle, a_enemyState, false, bIsUnlockLOS));
				Script::DispatchStaticCall("ActorUtil"sv, "RemovePackageOverride"sv, callback, std::move(a_enemyCharacter), std::move(package));
			}
			else
			{
				CombatHandler::GetSingleton()->EraseEnemy(a_enemyHandle);
			}
		}
	}
	void Enemy::UnlockCallback(RE::ObjectRefHandle a_enemyHandle, EnemyState& a_enemyState, bool bIsUnlockLOS)
	{
		if (a_enemyHandle.get().get() && a_enemyHandle.get().get()->As<RE::Character>())
		{
			auto a_enemyCharacter = a_enemyHandle.get().get()->As<RE::Character>();
			a_enemyCharacter->EvaluatePackage();

			auto combatHandler = CombatHandler::GetSingleton();
			if (a_enemyState.bBlockSpellActive == true && a_enemyState.bEnemyIsBlocking)
			{
				combatHandler->EnemyRemoveBlockOverride(a_enemyCharacter, a_enemyState.bEnemyIsBlocking);
			}
			// Register unlocked enemy for relock
			float randomTime = Utils::GenerateRandomFloat(combatHandler->fWindowIntervalMin, combatHandler->fWindowIntervalMax);
			if (!bIsUnlockLOS)
				Hooks::PollingHandler[a_enemyHandle].unlockDuration = randomTime;
			else
				Hooks::PollingHandler[a_enemyHandle].unlockDuration = 0.1f; // Just a quick relock so that enemy can be locked on entering LOS
		}
		else
		{
			CombatHandler::GetSingleton()->EraseEnemy(a_enemyHandle);
		}
	}

	// On Dead event or if Filter Effect finished
	void Enemy::EnemyDeadOrInvalid(RE::ObjectRefHandle a_enemyHandle, EnemyState& a_enemyState, bool bEffectFinished)
	{
		if (a_enemyHandle.get().get() && a_enemyHandle.get().get()->As<RE::Character>())
		{
			a_enemyState.bIsDeadOrInvalid = true;
			if (!a_enemyState.bIsLocked)
			{
				a_enemyState.bIsLocked = true;
				if (Hooks::PollingHandler[a_enemyHandle].EnemyHasPackage) // Check for LOS setting, default will always be true
					iCurrentUnlockedEnemies -= 1;
			}
			Hooks::PollingHandler.erase(a_enemyHandle); // Remove from Update list

			auto a_enemyCharacter = a_enemyHandle.get().get()->As<RE::Character>();
			Callback callback;
			callback.reset(new VMCallback(a_enemyHandle, a_enemyState, false, false, bEffectFinished));
			Script::DispatchStaticCall("ActorUtil"sv, "RemovePackageOverride"sv, callback, std::move(a_enemyCharacter), std::move(package));

			if (Settings::bModEnabled && CombatHandler::bModStarted)
				CombatHandler::GetSingleton()->RaiseEnemyUnlock();
		}
		else
		{
			CombatHandler::GetSingleton()->EraseEnemy(a_enemyHandle);
		}
	}
}
