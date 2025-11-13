#include "BlockHandler.h"
#include "Settings.h"

namespace EnemyHandler
{
	void BlockHandler::InitializeForms()
	{
		logger::info("Initializing BlockHandler Forms...");
		const auto dataHandler = RE::TESDataHandler::GetSingleton();
		if (dataHandler) {
			// BlockingStart Form
			const auto regularBlockStart_Form = dataHandler->LookupForm(0x13217, "Skyrim.esm");
			if (regularBlockStart_Form && regularBlockStart_Form->Is(RE::TESIdleForm::FORMTYPE)) {
				regularBlockStart_idleForm = regularBlockStart_Form->As<RE::TESIdleForm>();
			}
			else {
				logger::critical("Failed to initialize BlockingStart Form");
			}
			// DraugrBlockStart Form (Shields only)
			const auto draugrBlockStart_Form = dataHandler->LookupForm(0x3533C, "Skyrim.esm");
			if (draugrBlockStart_Form && draugrBlockStart_Form->Is(RE::TESIdleForm::FORMTYPE)) {
				draugrBlockStart_idleForm = draugrBlockStart_Form->As<RE::TESIdleForm>();
			}
			else {
				logger::critical("Failed to initialize DraugrBlockStart Form");
			}
			// FalmerBlockStart Form
			const auto falmerBlockStart_Form = dataHandler->LookupForm(0x36A77, "Skyrim.esm");
			if (falmerBlockStart_Form && falmerBlockStart_Form->Is(RE::TESIdleForm::FORMTYPE)) {
				falmerBlockStart_idleForm = falmerBlockStart_Form->As<RE::TESIdleForm>();
			}
			else {
				logger::critical("Failed to initialize FalmerBlockStart Form");
			}
			// IdleStop_Loose Form
			const auto idleStopLoose_Form = dataHandler->LookupForm(0x10D9EE, "Skyrim.esm");
			if (idleStopLoose_Form && idleStopLoose_Form->Is(RE::TESIdleForm::FORMTYPE)) {
				idleStopLoose_idleForm = idleStopLoose_Form->As<RE::TESIdleForm>();
			}
			else {
				logger::critical("Failed to initialize IdleStop_Loose Form");
			}
			// Draugr Faction Form
			const auto draugrFaction_Form = dataHandler->LookupForm(0x2430D, "Skyrim.esm");
			if (draugrFaction_Form && draugrFaction_Form->Is(RE::TESFaction::FORMTYPE)) {
				draugrFaction = draugrFaction_Form->As<RE::TESFaction>();
			}
			else {
				logger::critical("Failed to initialize Draugr Faction Form");
			}
			// Falmer Faction Form
			const auto falmerFaction_Form = dataHandler->LookupForm(0x2997E, "Skyrim.esm");
			if (falmerFaction_Form && falmerFaction_Form->Is(RE::TESFaction::FORMTYPE)) {
				falmerFaction = falmerFaction_Form->As<RE::TESFaction>();
			}
			else {
				logger::critical("Failed to initialize Falmer Faction Form");
			}
			// Block Spell Form
			const auto block_spell_Form = dataHandler->LookupForm(0x804, "WaitYourTurn.esp");
			if (block_spell_Form && block_spell_Form->Is(RE::SpellItem::FORMTYPE)) {
				block_spell = block_spell_Form->As<RE::SpellItem>();
			}
			else {
				logger::critical("Failed to initialize Block Spell");
			}
		}
		else {
			logger::critical("TESDataHandler not found.");
		}

		logger::info("...BlockHandler Forms finished initializing.");
	}

	void BlockHandler::EnemyAddBlockOverride(RE::Character* a_enemy, bool& bIsBlocking)
	{
		Locker locker(lock);

		auto aiProcess = a_enemy->GetActorRuntimeData().currentProcess;
		if (draugrBlockStart_idleForm && Settings::bDraugrBlockEnabled && a_enemy->IsInFaction(draugrFaction))
		{
			aiProcess->PlayIdle(a_enemy, draugrBlockStart_idleForm, a_enemy);
			bIsBlocking = true;
		}
		else if (falmerBlockStart_idleForm && Settings::bFalmerBlockEnabled && a_enemy->IsInFaction(falmerFaction))
		{
			aiProcess->PlayIdle(a_enemy, falmerBlockStart_idleForm, a_enemy);
			bIsBlocking = true;
		}
		else if (regularBlockStart_idleForm && !a_enemy->IsInFaction(draugrFaction) && !a_enemy->IsInFaction(falmerFaction))
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

	void BlockHandler::EnemyAddBlockSpell(RE::Character* a_enemy)
	{
		if (block_spell && Settings::bBlockWhileCircling && !a_enemy->HasSpell(block_spell))
			a_enemy->AddSpell(block_spell);
	}

	void BlockHandler::EnemyRemoveBlockSpell(RE::Character* a_enemy)
	{
		if (block_spell && Settings::bBlockWhileCircling && a_enemy->HasSpell(block_spell))
			a_enemy->RemoveSpell(block_spell);
	}
}
