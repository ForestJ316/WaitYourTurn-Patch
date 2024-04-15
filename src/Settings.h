#pragma once

class Settings :
	public RE::BSTEventSink<RE::TESLoadGameEvent>
{
private:
	using EventResult = RE::BSEventNotifyControl;

public:
	static Settings* GetSingleton()
	{
		static Settings singleton;
		return std::addressof(singleton);
	}
	static void InitializeMCMQuest();
	virtual EventResult ProcessEvent(const RE::TESLoadGameEvent* a_event, RE::BSTEventSource<RE::TESLoadGameEvent>* a_eventSource) override;

	static void ReadMCMSettings();
	static void GetBoolSetting(json& jsonObject, const char* sectionName, const char* settingName, bool& settingValue);
	static void GetInt32Setting(json& jsonObject, const char* sectionName, const char* settingName, uint32_t& settingValue);
	static void GetFloatSetting(json& jsonObject, const char* sectionName, const char* settingName, float& settingValue);

	static inline bool SettingsApplied = false;

	// General
	static inline bool bModEnabled = true;
	// Circle
	static inline bool bBlockWhileCircling = true;
	static inline bool bDraugrBlockEnabled = true;
	static inline bool bFalmerBlockEnabled = true;
	static inline bool bRandomizeAttackers = true;
	// Interior
	static inline uint32_t iEnemiesForModStartInterior = 2;
	static inline uint32_t iMaxUnlockedEnemiesInterior = 4;
	static inline float fWindowIntervalMinInterior = 5.0f;
	static inline float fWindowIntervalMaxInterior = 5.0f + fWindowIntervalMinInterior;
	// Exterior
	static inline uint32_t iEnemiesForModStartExterior = 2;
	static inline uint32_t iMaxUnlockedEnemiesExterior = 4;
	static inline float fWindowIntervalMinExterior = 5.0f;
	static inline float fWindowIntervalMaxExterior = 5.0f + fWindowIntervalMinExterior;
	// Experimental
	static inline bool bLOSExperimentalEnabled = false;
	static inline float fLOSExperimentalDelay = 0.5f;

private:
	static inline RE::TESForm* MCMQuestForm;

	Settings() = default;
	Settings(const Settings&) = delete;
	Settings(Settings&&) = delete;

	~Settings() override = default;

	Settings& operator=(const Settings&) = delete;
	Settings& operator=(Settings&&) = delete;
};
