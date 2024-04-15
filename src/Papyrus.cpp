#include "Papyrus.h"
#include "Settings.h"
#include "EnemyHandler.h"


namespace Papyrus
{
	void OnConfigClose(RE::TESQuest*)
	{
		Settings::ReadMCMSettings();
	}

	bool PapyrusNativeFunctions(RE::BSScript::IVirtualMachine* a_vm)
	{
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
