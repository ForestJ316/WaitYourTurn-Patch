#pragma once

struct Settings
{
	static void ReadMCMSettings();

	static void GetBoolSetting(json& jsonObject, const char* sectionName, const char* settingName, bool& settingValue);
	static void GetInt32Setting(json& jsonObject, const char* sectionName, const char* settingName, uint32_t& settingValue);
	static void GetFloatSetting(json& jsonObject, const char* sectionName, const char* settingName, float& settingValue);

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
};
