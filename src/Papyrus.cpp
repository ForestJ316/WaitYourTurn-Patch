#include <Papyrus.h>
#include <Settings.h>
#include <EnemyHandler.h>


namespace Papyrus
{
	void OnConfigClose(RE::TESQuest*)
	{
		Settings::ReadMCMSettings();
	}

	void RelockEnemy(RE::StaticFunctionTag*, RE::Character* akEnemy)
	{
		if (Settings::bModEnabled)
			EnemyHandler::CombatHandler::GetSingleton()->UpdateEnemyRelock(akEnemy);
	}

	bool IsEnemyLocked(RE::StaticFunctionTag*, RE::Character* akEnemy)
	{
		return EnemyHandler::CombatHandler::GetSingleton()->GetEnemyLockedState(akEnemy);
	}

	bool PapyrusNativeFunctions(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("RelockEnemy", "WYT_EnemyUnlockScript", RelockEnemy, true);
		a_vm->RegisterFunction("IsEnemyLocked", "WYT_EnemyUnlockScript", IsEnemyLocked, true);
		a_vm->RegisterFunction("OnConfigClose", "WYT_MCMScript", OnConfigClose);
		logger::info("Registered Papyrus native functions.");
		return true;
	}

	void RegisterPapyrus()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(PapyrusNativeFunctions);
	}
}
