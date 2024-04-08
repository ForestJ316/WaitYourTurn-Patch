#include <BlockHandler.h>
#include <Settings.h>

namespace EnemyHandler
{
	void BlockHandler::InitializeForms()
	{
		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		logger::info("Initializing BlockHandler Forms...");

		// BlockingStart Form
		regularBlockStart_idleForm = dataHandler->LookupForm<RE::TESIdleForm>(0x13217, "Skyrim.esm");
		if (!regularBlockStart_idleForm)
			logger::critical("Failed to initialize Form {} (BlockingStart)", regularBlockStart_idleForm->GetFormID());

		// DraugrBlockStart Form
		draugrBlockStart_idleForm = dataHandler->LookupForm<RE::TESIdleForm>(0x3533C, "Skyrim.esm");
		if (!draugrBlockStart_idleForm)
			logger::critical("Failed to initialize Form {} (DraugrBlockStart)", draugrBlockStart_idleForm->GetFormID());

		// FalmerBlockStart Form
		falmerBlockStart_idleForm = dataHandler->LookupForm<RE::TESIdleForm>(0x36A77, "Skyrim.esm");
		if (!falmerBlockStart_idleForm)
			logger::critical("Failed to initialize Form {} (FalmerBlockStart)", falmerBlockStart_idleForm->GetFormID());

		// IdleStop_Loose Form
		idleStopLoose_idleForm = dataHandler->LookupForm<RE::TESIdleForm>(0x10D9EE, "Skyrim.esm");
		if (!idleStopLoose_idleForm)
			logger::critical("Failed to initialize Form {} (IdleStop_Loose)", idleStopLoose_idleForm->GetFormID());

		// Draugr Faction Form
		draugrFaction = dataHandler->LookupForm<RE::TESFaction>(0x2430D, "Skyrim.esm");
		if (!draugrFaction)
			logger::critical("Failed to initialize Form {} (Draugr Faction)", draugrFaction->GetFormID());

		// Falmer Faction Form
		falmerFaction = dataHandler->LookupForm<RE::TESFaction>(0x2997E, "Skyrim.esm");
		if (!falmerFaction)
			logger::critical("Failed to initialize Form {} (Falmer Faction)", falmerFaction->GetFormID());

		block_spell = dataHandler->LookupForm<RE::SpellItem>(0x804, "WaitYourTurn.esp");
		if (!block_spell)
			logger::critical("Failed to initialize Block Spell");

		logger::info("...BlockHandler Forms finished initializing.");
	}

	void BlockHandler::EnemyAddBlockOverride(RE::Character* a_enemy, bool& bIsBlocking)
	{
		Locker locker(lock);

		auto aiProcess = a_enemy->GetActorRuntimeData().currentProcess;
		if (a_enemy->IsInFaction(draugrFaction) && draugrBlockStart_idleForm && Settings::bDraugrBlockEnabled)
		{
			aiProcess->PlayIdle(a_enemy, draugrBlockStart_idleForm, a_enemy);
			bIsBlocking = true;
		}
		else if (a_enemy->IsInFaction(falmerFaction) && falmerBlockStart_idleForm && Settings::bFalmerBlockEnabled)
		{
			aiProcess->PlayIdle(a_enemy, falmerBlockStart_idleForm, a_enemy);
			bIsBlocking = true;
		}
		else if (regularBlockStart_idleForm)
		{
			aiProcess->PlayIdle(a_enemy, regularBlockStart_idleForm, a_enemy);
			bIsBlocking = true;
		}
	}

	void BlockHandler::EnemyRemoveBlockOverride(RE::Character* a_enemy, bool& bIsBlocking)
	{
		Locker locker(lock);

		if (idleStopLoose_idleForm)
		{
			auto aiProcess = a_enemy->GetActorRuntimeData().currentProcess;
			aiProcess->PlayIdle(a_enemy, idleStopLoose_idleForm, a_enemy);
			bIsBlocking = false;
		}
	}
}
