#pragma once

namespace Papyrus
{
	void OnConfigClose(RE::TESQuest*);
	bool PapyrusNativeFunctions(RE::BSScript::IVirtualMachine* a_vm);
	void RegisterPapyrus();
}
