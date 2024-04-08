#include <Settings.h>
#include <Utils.h>

#include <Enemy.h>
#include <EnemyHandler.h>
#include <BlockHandler.h>

namespace EnemyHandler
{
	// Make Utils class later for this
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

	void Enemy::LockEnemy(RE::Character* a_enemy, EnemyState& a_enemyState, int& a_currentUnlockedEnemies, bool bIsModStart)
	{
		if (a_enemyState.bIsDeadOrInvalid)
			return;
		// Do Lock
		Locker locker(lock);
		if (package)
		{
			Callback callback;
			callback.reset(new VMCallback(a_enemy, a_enemyState, true, false));
			Script::DispatchStaticCall("ActorUtil"sv, "AddPackageOverride"sv, callback, std::move(a_enemy), std::move(package), 100, 1);
			if (!bIsModStart)
				a_currentUnlockedEnemies -= 1;
		}
	}

	void Enemy::LockCallback(RE::Character* a_enemy, EnemyState& a_enemyState)
	{	
		a_enemy->EvaluatePackage();
		a_enemyState.bIsLocked = true;
		auto combatHandler = EnemyHandler::CombatHandler::GetSingleton();
		if (a_enemyState.bBlockSpellActive == true)
		{
			combatHandler->EnemyAddBlockOverride(a_enemy, a_enemyState.bEnemyIsBlocking);
		}
		combatHandler->RaiseEnemyUnlock();
	}

	void Enemy::UnlockEnemy(RE::Character* a_enemy, EnemyState& a_enemyState, int& a_currentUnlockedEnemies)
	{
		if (a_enemyState.bIsDeadOrInvalid)
			return;
		// Do unlock
		Locker locker(lock);
		if (package)
		{
			Callback callback;
			callback.reset(new VMCallback(a_enemy, a_enemyState, false, false));
			Script::DispatchStaticCall("ActorUtil"sv, "RemovePackageOverride"sv, callback, std::move(a_enemy), std::move(package));
			a_currentUnlockedEnemies += 1;
		}
	}

	void Enemy::UnlockCallback(RE::Character* a_enemy, EnemyState& a_enemyState)
	{
		a_enemy->EvaluatePackage();
		a_enemyState.bIsLocked = false;
		if (a_enemyState.bBlockSpellActive == true && a_enemyState.bEnemyIsBlocking)
		{
			EnemyHandler::CombatHandler::GetSingleton()->EnemyRemoveBlockOverride(a_enemy, a_enemyState.bEnemyIsBlocking);
		}
		// Register unlocked enemy for relock
		Callback callback;
		auto effectScriptObject = Utils::GetObjectFromEffect(a_enemyState.effect, "WYT_EnemyUnlockScript");		
		float randomTime = Utils::GenerateRandomFloat(CombatHandler::fWindowIntervalMin, CombatHandler::fWindowIntervalMax);
		Script::DispatchMethodCall(effectScriptObject, "RegisterActor"sv, callback, std::move(a_enemy), std::move(randomTime));
	}

	// On dead event or if magic effect off
	void Enemy::EnemyDeadOrInvalid(RE::Character* a_enemy, EnemyState& a_enemyState, int& a_currentUnlockedEnemies, bool bEffectFinished)
	{
		a_enemyState.bIsDeadOrInvalid = true;
		if (!a_enemyState.bIsLocked)
		{
			a_enemyState.bIsLocked = true;
			a_currentUnlockedEnemies -= 1;
		}
		Callback callback;
		callback.reset(new VMCallback(a_enemy, a_enemyState, false, bEffectFinished));
		Script::DispatchStaticCall("ActorUtil"sv, "RemovePackageOverride"sv, callback, std::move(a_enemy), std::move(package));

		if (Settings::bModEnabled)
			EnemyHandler::CombatHandler::GetSingleton()->RaiseEnemyUnlock();
	}

	void Enemy::OnMCMChanged(RE::Character* a_enemy, RE::ActiveEffect* a_effect)
	{
		CombatHandler::GetSingleton()->AddEnemyToList(a_enemy, a_effect);
	}
}
