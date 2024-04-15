#include "Settings.h"
#include "Utils.h"
#include "EnemyHandler.h"

#include <fstream>

void Settings::InitializeMCMQuest()
{
	logger::info("Initializing MCM Quest Form...");

	RE::ScriptEventSourceHolder::GetSingleton()->GetEventSource<RE::TESLoadGameEvent>()->AddEventSink(Settings::GetSingleton());
	logger::info("Registered {}"sv, typeid(RE::TESLoadGameEvent).name());

	MCMQuestForm = RE::TESDataHandler::GetSingleton()->LookupForm(0x80B, "WaitYourTurn.esp");
	if (!MCMQuestForm)
		logger::critical("Failed to initialize MCM Quest");

	logger::info("...MCM Quest Form done initializing.");
}

Settings::EventResult Settings::ProcessEvent(const RE::TESLoadGameEvent*, RE::BSTEventSource<RE::TESLoadGameEvent>*)
{
	auto scriptObject = Utils::GetObjectFromForm(MCMQuestForm, "WYT_MCMScript");
	Callback callback;
	Script::DispatchMethodCall(scriptObject, "PassVarsOnLoadGame"sv, callback);
	return EventResult::kContinue;
}

void Settings::ReadMCMSettings()
{
	constexpr auto MCMPath = L"Data/SKSE/Plugins/Wait Your Turn/Data/WYTPatch_SKSECurrentSettings.json";

	const auto readMCM = [&](std::filesystem::path path){
		std::ifstream file(path.string().c_str());
		if (file.is_open())
		{
			json mcm = json::parse(file);

			// General Settings
			GetBoolSetting(mcm, "int", "bmodenabled", bModEnabled);
			// Circle Settings
			GetBoolSetting(mcm, "int", "bblockwhilecircling", bBlockWhileCircling);
			GetBoolSetting(mcm, "int", "bdraugrblockenabled", bDraugrBlockEnabled);
			GetBoolSetting(mcm, "int", "bfalmerblockenabled", bFalmerBlockEnabled);
			GetBoolSetting(mcm, "int", "wytrandom", bRandomizeAttackers);
			// Interior Settings
			GetInt32Setting(mcm, "int", "ienemiesformodstartinterior", iEnemiesForModStartInterior);
			GetInt32Setting(mcm, "int", "imaxunlockedenemiesinterior", iMaxUnlockedEnemiesInterior);
			GetFloatSetting(mcm, "float", "fwindowintervalmininterior", fWindowIntervalMinInterior);
			GetFloatSetting(mcm, "float", "fwindowintervalmaxinterior", fWindowIntervalMaxInterior);
			// Exterior Settings
			GetInt32Setting(mcm, "int", "ienemiesformodstartexterior", iEnemiesForModStartExterior);
			GetInt32Setting(mcm, "int", "imaxunlockedenemiesexterior", iMaxUnlockedEnemiesExterior);
			GetFloatSetting(mcm, "float", "fwindowintervalminexterior", fWindowIntervalMinExterior);
			GetFloatSetting(mcm, "float", "fwindowintervalmaxexterior", fWindowIntervalMaxExterior);
			// Experimental Settings
			GetBoolSetting(mcm, "int", "blosexperimental", bLOSExperimentalEnabled);
			GetFloatSetting(mcm, "float", "flosexperimentaldelay", fLOSExperimentalDelay);

			logger::info("...MCM read successful."sv);
			SettingsApplied = true;
		}
		else
		{
			logger::warn("...File Path: {} for Current Settings doesn't exist or hasn't been generated yet."sv, path.string());
		}
	};
	
	logger::info("Reading MCM settings..."sv);
	readMCM(MCMPath);

	EnemyHandler::CombatHandler::UpdateSettings();
}

void Settings::GetBoolSetting(json& jsonObject, const char* sectionName, const char* settingName, bool& settingValue)
{
	if (jsonObject.contains(sectionName) && jsonObject[sectionName].contains(settingName))
	{
		const int found = jsonObject[sectionName].at(settingName);
		if (found == 1 || found == 0)
			settingValue = found == 1 ? true : false;
	}
}

void Settings::GetInt32Setting(json& jsonObject, const char* sectionName, const char* settingName, uint32_t& settingValue)
{
	if (jsonObject.contains(sectionName) && jsonObject[sectionName].contains(settingName))
	{
		const uint32_t found = jsonObject[sectionName].at(settingName);
		if (found) 
			settingValue = found;
	}
}

void Settings::GetFloatSetting(json& jsonObject, const char* sectionName, const char* settingName, float& settingValue)
{
	if (jsonObject.contains(sectionName) && jsonObject[sectionName].contains(settingName))
	{
		const float found = jsonObject[sectionName].at(settingName);
		if (found)
			settingValue = found;
	}
}
