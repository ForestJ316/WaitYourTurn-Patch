#pragma once

namespace Papyrus
{
	void OnConfigClose(RE::TESQuest*);
	void RelockEnemy(RE::StaticFunctionTag*, RE::Character* akEnemy);
	bool IsEnemyLocked(RE::StaticFunctionTag*, RE::Character* akEnemy);
	bool PapyrusNativeFunctions(RE::BSScript::IVirtualMachine* a_vm);
	void RegisterPapyrus();	
}
