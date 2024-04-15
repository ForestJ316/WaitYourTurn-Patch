ScriptName WYT_MCMScript Extends SKI_ConfigBase

int OID_EnemiesForModStartInterior
int OID_EnemiesForModStartExterior

int OID_MinimumAttackIntervalInterior
int OID_MaximumAttackIntervalInterior
int OID_MinimumAttackIntervalExterior
int OID_MaximumAttackIntervalExterior

int OID_MaximumAttackingEnemiesInterior
int OID_MaximumAttackingEnemiesExterior

int OID_ModEnabled
int OID_BlockWhileCircling
int OID_DraugrBlockEnable
int OID_FalmerBlockEnable
int OID_bRandomize

int OID_bAutoLoad
int OID_bLoad
int OID_bSave

int OID_bLOSExperimental
int OID_bResetSettings

int OID_DebugMode

int Header_CircleSettings
int Header_InteriorSettings
int Header_ExteriorSettings
int Header_ExperimentalSettings
int Header_ResetSettings

string INF_MinimumAttackIntervalInterior = "The minimum possible time window in which an enemy indoors will get to fight if not circling, inclusive. The actual time window is chosen randomly between this value and the max value."
string INF_MaximumAttackIntervalInterior = "The maximum possible time window in which an enemy indoors will get to fight if not circling, inclusive. The actual time window is chosen randomly between the min value and this value."
string INF_MinimumAttackIntervalExterior = "The minimum possible time window in which an enemy outdoors will get to fight if not circling, inclusive. The actual time window is chosen randomly between this value and the max value."
string INF_MaximumAttackIntervalExterior = "The maximum possible time window in which an enemy outdoors will get to fight if not circling, inclusive. The actual time window is chosen randomly between the min value and this value."
string INF_MaxUnlockedEnemiesInterior = "The maximum amount of enemies that can be attacking you simultaenously indoors. The actual number of enemies attacking will never exceed this value."
string INF_MaxUnlockedEnemiesExterior = "The maximum amount of enemies that can be attacking you simultaenously outdoors. The actual number of enemies attacking will never exceed this value."
string INF_ModEnabled = "Turns the mod function on or off. Make sure to TURN THE MOD OFF HERE BEFORE YOU UNINSTALL IT MID PLAYTHROUGH/SAVE."
string INF_AutoLoad = "If enabled, will automatically load the preset made from this configuration at the start of a new save. You must Save Preset with this option enabled for it to work. Requires PapyrusUtil to be installed."
string INF_bLoad = "Load a saved JSON preset. Requires PapyrusUtil."
string INF_bSave = "Save the current configuration of settings to an external JSON preset file. Requires PapyrusUtil"
string INF_BlockWhileCircling = "Determines if enemies that are circling will block your attacks, not just the enemies that are currently attacking you. Recommended to be turned on to avoid cheesing."
string INF_DraugrBlockEnable = "Enable Draugr who are carrying shields to block while circling you. Requires the Enemies Block While Circling option."
string INF_FalmerBlockEnable = "Enable Falmer to block while circling you. Requires the Enemies Block While Circling option."
string INF_EnemiesForModStartInterior = "Determines at what point the mod will start the circling system indoors. When this number of hostile NPCs are reached in combat, enemies will start circling."
string INF_EnemiesForModStartExterior = "Determines at what point the mod will start the circling system outdoors. When this number of hostile NPCs are reached in combat, enemies will start circling."
string INF_Randomize = "Randomizes the number of attackers, from 1 to max (set accordingly to indoors/outdoors setting). Changes every few seconds."
string INF_bResetSettings = "Reset the current configuration back to default settings. WARNING: YOU WILL LOSE YOUR CURRENT SETTINGS UNLESS YOU SAVED THEM AS A PRESET."
string INF_bLOSExperimental = "You can try to enable this setting if you are experiencing enemies warping/teleporting in combat. WARNING: MAY DECREASE PERFORMANCE IN COMBAT!"

bool bInitiated = False

GlobalVariable Property bBlockWhileCircling auto
GlobalVariable Property bModEnabled auto
GlobalVariable Property bDebugMode auto
GlobalVariable Property WYTRandom auto
GlobalVariable Property bLOSExperimental auto

int VAL_iEnemiesForModStartInterior
int VAL_iEnemiesForModStartExterior

float VAL_fWindowIntervalMinInterior
float VAL_fWindowIntervalMaxInterior
float VAL_fWindowIntervalMinExterior
float VAL_fWindowIntervalMaxExterior
int VAL_iMaxUnlockedEnemiesInterior
int VAL_iMaxUnlockedEnemiesExterior

bool VAL_bBlockWhileCircling
bool VAL_bDraugrBlockEnable
bool VAL_bFalmerBlockEnable
bool VAL_bModEnabled
bool VAL_bRandomize
;bool VAL_bDebug = False
bool bAutoLoad = False
bool VAL_bLOSExperimental

string sPresetFilepath = "../Wait Your Turn/Data/Preset"
string sCurrentSettingsFilepath = "../Wait Your Turn/Data/WYTPatch_SKSECurrentSettings"

Function GetVars()
	if JsonUtil.JsonExists(sCurrentSettingsFilepath)
		if JsonUtil.IsGood(sCurrentSettingsFilepath) && JsonUtil.GetErrors(sCurrentSettingsFilepath) == ""
			VAL_iEnemiesForModStartInterior = JsonUtil.GetIntValue(sCurrentSettingsFilepath, "iEnemiesForModStartInterior", 2)
			VAL_iEnemiesForModStartExterior = JsonUtil.GetIntValue(sCurrentSettingsFilepath, "iEnemiesForModStartExterior", 2)
			
			VAL_iMaxUnlockedEnemiesInterior = JsonUtil.GetIntValue(sCurrentSettingsFilepath, "iMaxUnlockedEnemiesInterior", 4)
			VAL_iMaxUnlockedEnemiesExterior = JsonUtil.GetIntValue(sCurrentSettingsFilepath, "iMaxUnlockedEnemiesExterior", 4)
			
			VAL_fWindowIntervalMinInterior = JsonUtil.GetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMinInterior", 5.0)
			VAL_fWindowIntervalMaxInterior = JsonUtil.GetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMaxInterior", VAL_fWindowIntervalMinInterior + 5)
			
			VAL_fWindowIntervalMinExterior = JsonUtil.GetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMinExterior", 5.0)
			VAL_fWindowIntervalMaxExterior = JsonUtil.GetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMaxExterior", VAL_fWindowIntervalMinInterior + 5)
			
			VAL_bModEnabled = JsonUtil.GetIntValue(sCurrentSettingsFilepath, "bModEnabled", 1) as bool
			VAL_bBlockWhileCircling = JsonUtil.GetIntValue(sCurrentSettingsFilepath, "bBlockWhileCircling", 1) as bool
			VAL_bDraugrBlockEnable = JsonUtil.GetIntValue(sCurrentSettingsFilepath, "bDraugrBlockEnable", 1) as bool
			VAL_bFalmerBlockEnable = JsonUtil.GetIntValue(sCurrentSettingsFilepath, "bFalmerBlockEnable", 1) as bool
			VAL_bRandomize = JsonUtil.GetIntValue(sCurrentSettingsFilepath, "WYTRandom", 1) as bool
			
			bAutoLoad = JsonUtil.GetIntValue(sCurrentSettingsFilepath, "bAutoLoad", 0) as bool
			VAL_bLOSExperimental = JsonUtil.GetIntValue(sCurrentSettingsFilepath, "bLOSExperimental", 0) as bool
		else
			Debug.MessageBox("Wait Your Turn: Failed To Get Current Settings." + "\nErrors: " + JsonUtil.GetErrors(sCurrentSettingsFilepath))
		endif
	else
		if bInitiated
			Debug.MessageBox("Wait Your Turn: Failed To Get Current Settings. This function requires PapyrusUtil to be installed!")
		endif
	endif
Endfunction

Function PassVars()
	JsonUtil.SetIntValue(sCurrentSettingsFilepath, "iEnemiesForModStartInterior", VAL_iEnemiesForModStartInterior)
	JsonUtil.SetIntValue(sCurrentSettingsFilepath, "iEnemiesForModStartExterior", VAL_iEnemiesForModStartExterior)
	
	JsonUtil.SetIntValue(sCurrentSettingsFilepath, "iMaxUnlockedEnemiesInterior", VAL_iMaxUnlockedEnemiesInterior)
	JsonUtil.SetIntValue(sCurrentSettingsFilepath, "iMaxUnlockedEnemiesExterior", VAL_iMaxUnlockedEnemiesExterior)
	
	JsonUtil.SetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMinInterior", VAL_fWindowIntervalMinInterior)
	JsonUtil.SetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMaxInterior", VAL_fWindowIntervalMaxInterior)
	
	JsonUtil.SetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMinExterior", VAL_fWindowIntervalMinExterior)
	JsonUtil.SetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMaxExterior", VAL_fWindowIntervalMaxExterior)
	
	JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bModEnabled", VAL_bModEnabled as int)
	JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bBlockWhileCircling", VAL_bBlockWhileCircling as int)
	JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bDraugrBlockEnable", VAL_bDraugrBlockEnable as int)
	JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bFalmerBlockEnable", VAL_bFalmerBlockEnable as int)
	JsonUtil.SetIntValue(sCurrentSettingsFilepath, "WYTRandom", VAL_bRandomize as int)
	
	JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bAutoLoad", bAutoLoad as int)
	JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bLOSExperimental", VAL_bLOSExperimental as int)

	JsonUtil.Save(sCurrentSettingsFilepath)
	
	bModEnabled.SetValue(VAL_bModEnabled as float)
	bBlockWhileCircling.SetValue(VAL_bBlockWhileCircling as float)
	WYTRandom.SetValue(VAL_bRandomize as float)
	bLOSExperimental.SetValue(VAL_bLOSExperimental as float)
	
	if !JsonUtil.JsonExists(sCurrentSettingsFilepath)
		Debug.MessageBox("Wait Your Turn: Failed To Set Current Settings. This function requires PapyrusUtil to be installed!")
	endif
Endfunction

Event OnConfigInit()
	Pages = New String[1]
	Pages[0] = "Circling"
	bool bImported = ImportPreset()
	; If preset exists then use preset variables
	if bImported
		PassVars()
		GetVars()
	; Otherwise if settings exist then use those
	elseif JsonUtil.JsonExists(sCurrentSettingsFilepath)
		GetVars()
		PassVars()
	; Otherwise use default
	else
		ResetSettings()
		PassVars()
	endif
	bInitiated = True
	OnConfigClose()
EndEvent

Function PassVarsOnLoadGame()
	if bInitiated
		PassVars()
		OnConfigClose()
	endif
EndFunction

; New version for SKSE to update settings on existing saves
Int Function GetVersion()
	return 2
EndFunction

Event OnVersionUpdate(int newVersion)
	bInitiated = False
	OnConfigInit()
EndEvent

Event OnConfigOpen()
	if !bInitiated
		bInitiated = True
	endif
Endevent

Event OnConfigClose() native

Event OnPageReset(String Page)
	if Page == ""
		;X offset = 376 - (height / 2)
		;Y offset = 223 - (width / 2)
		LoadCustomContent("wyt/WaitYourTurnMCMLogo.dds",120,-33)
	else 
		UnloadCustomContent()
	endif
	
	GetVars()
	
	if Page == "Circling"
		SetCursorFillMode(TOP_TO_BOTTOM)
		SetCursorPosition(0)
		
		AddHeaderOption("General Settings")
		OID_ModEnabled = AddToggleOption("Mod Enabled",VAL_bModEnabled)
		
		; Grey out settings for Load Game also
		if VAL_bModEnabled
			Header_CircleSettings = AddHeaderOption("Circle Settings", OPTION_FLAG_NONE)
			OID_BlockWhileCircling = AddToggleOption("Enemies Block While Circling", VAL_bBlockWhileCircling, OPTION_FLAG_NONE)
			if VAL_bBlockWhileCircling
				OID_DraugrBlockEnable = AddToggleOption("Draugr with Shields Block", VAL_bDraugrBlockEnable, OPTION_FLAG_NONE)
				OID_FalmerBlockEnable = AddToggleOption("Falmer Block", VAL_bFalmerBlockEnable, OPTION_FLAG_NONE)
			else
				OID_DraugrBlockEnable = AddToggleOption("Draugr with Shields Block", VAL_bDraugrBlockEnable, OPTION_FLAG_DISABLED)
				OID_FalmerBlockEnable = AddToggleOption("Falmer Block", VAL_bFalmerBlockEnable, OPTION_FLAG_DISABLED)
			endif
			OID_bRandomize = AddToggleOption("Randomize Attackers", VAL_bRandomize, OPTION_FLAG_NONE)
			
			Header_InteriorSettings = AddHeaderOption("Interior Settings", OPTION_FLAG_NONE)
			OID_EnemiesForModStartInterior = AddSliderOption("Mod Activate Point Indoors", VAL_iEnemiesForModStartInterior,"{0} enemies", OPTION_FLAG_NONE)
			OID_MaximumAttackingEnemiesInterior = AddSliderOption("Max Attacking Enemies Indoors", VAL_iMaxUnlockedEnemiesInterior, "{0} enemies", OPTION_FLAG_NONE)
			OID_MinimumAttackIntervalInterior = AddSliderOption("Minimum Attack Interval Indoors", VAL_fWindowIntervalMinInterior, "{1} seconds", OPTION_FLAG_NONE)
			OID_MaximumAttackIntervalInterior = AddSliderOption("Maximum Attack Interval Indoors", VAL_fWindowIntervalMaxInterior, "{1} seconds", OPTION_FLAG_NONE)
			
			Header_ExteriorSettings = AddHeaderOption("Exterior Settings", OPTION_FLAG_NONE)
			OID_EnemiesForModStartExterior = AddSliderOption("Mod Activate Point Outdoors", VAL_iEnemiesForModStartExterior,"{0} enemies", OPTION_FLAG_NONE)
			OID_MaximumAttackingEnemiesExterior = AddSliderOption("Max Attacking Enemies Outdoors", VAL_iMaxUnlockedEnemiesExterior, "{0} enemies", OPTION_FLAG_NONE)
			OID_MinimumAttackIntervalExterior = AddSliderOption("Minimum Attack Interval Outdoors", VAL_fWindowIntervalMinExterior, "{1} seconds", OPTION_FLAG_NONE)
			OID_MaximumAttackIntervalExterior = AddSliderOption("Maximum Attack Interval Outdoors", VAL_fWindowIntervalMaxExterior, "{1} seconds", OPTION_FLAG_NONE)
		else
			Header_CircleSettings = AddHeaderOption("Circle Settings", OPTION_FLAG_DISABLED)
			OID_BlockWhileCircling = AddToggleOption("Enemies Block While Circling", VAL_bBlockWhileCircling, OPTION_FLAG_DISABLED)
			OID_DraugrBlockEnable = AddToggleOption("Draugr with Shields Block", VAL_bDraugrBlockEnable, OPTION_FLAG_DISABLED)
			OID_FalmerBlockEnable = AddToggleOption("Falmer Block", VAL_bFalmerBlockEnable, OPTION_FLAG_DISABLED)
			OID_bRandomize = AddToggleOption("Randomize Attackers", VAL_bRandomize, OPTION_FLAG_DISABLED)
			
			Header_InteriorSettings = AddHeaderOption("Interior Settings", OPTION_FLAG_DISABLED)
			OID_EnemiesForModStartInterior = AddSliderOption("Mod Activate Point Indoors", VAL_iEnemiesForModStartInterior,"{0} enemies", OPTION_FLAG_DISABLED)
			OID_MaximumAttackingEnemiesInterior = AddSliderOption("Max Attacking Enemies Indoors", VAL_iMaxUnlockedEnemiesInterior, "{0} enemies", OPTION_FLAG_DISABLED)
			OID_MinimumAttackIntervalInterior = AddSliderOption("Minimum Attack Interval Indoors", VAL_fWindowIntervalMinInterior, "{1} seconds", OPTION_FLAG_DISABLED)
			OID_MaximumAttackIntervalInterior = AddSliderOption("Maximum Attack Interval Indoors", VAL_fWindowIntervalMaxInterior, "{1} seconds", OPTION_FLAG_DISABLED)
			
			Header_ExteriorSettings = AddHeaderOption("Exterior Settings", OPTION_FLAG_DISABLED)
			OID_EnemiesForModStartExterior = AddSliderOption("Mod Activate Point Outdoors", VAL_iEnemiesForModStartExterior,"{0} enemies", OPTION_FLAG_DISABLED)
			OID_MaximumAttackingEnemiesExterior = AddSliderOption("Max Attacking Enemies Outdoors", VAL_iMaxUnlockedEnemiesExterior, "{0} enemies", OPTION_FLAG_DISABLED)
			OID_MinimumAttackIntervalExterior = AddSliderOption("Minimum Attack Interval Outdoors", VAL_fWindowIntervalMinExterior, "{1} seconds", OPTION_FLAG_DISABLED)
			OID_MaximumAttackIntervalExterior = AddSliderOption("Maximum Attack Interval Outdoors", VAL_fWindowIntervalMaxExterior, "{1} seconds", OPTION_FLAG_DISABLED)
		endif		
		
		SetCursorPosition(1)
		AddHeaderOption("Preset Save/Load (Requires PapyrusUtil)")
		OID_bAutoLoad = AddToggleOption("Preset Auto Load", bAutoLoad)
		OID_bSave = AddTextOption("Save Preset", "")
		OID_bLoad = AddTextOption("Load Preset", "")
		
		if VAL_bModEnabled
			Header_ExperimentalSettings = AddHeaderOption("Experimental Settings", OPTION_FLAG_NONE)
			OID_bLOSExperimental = AddToggleOption("Line of Sight Checking", VAL_bLOSExperimental, OPTION_FLAG_NONE)
			
			Header_ResetSettings = AddHeaderOption("Reset Settings", OPTION_FLAG_NONE)
			string sResetText = "<font color='#FF0000'>Default Settings</font>"
			OID_bResetSettings = AddTextOption(sResetText, "", OPTION_FLAG_NONE)
		else
			Header_ExperimentalSettings = AddHeaderOption("Experimental Settings", OPTION_FLAG_DISABLED)
			OID_bLOSExperimental = AddToggleOption("Line of Sight Checking", VAL_bLOSExperimental, OPTION_FLAG_DISABLED)
			
			Header_ResetSettings = AddHeaderOption("Reset Settings", OPTION_FLAG_DISABLED)
			string sResetText = "<font color='#FF0000'>Default Settings</font>"
			OID_bResetSettings = AddTextOption(sResetText, "", OPTION_FLAG_DISABLED)
		endif
		;AddHeaderOption("Dev Options")
		;OID_DebugMode = AddToggleOption("Debug Notifications", VAL_bDebug)
	endif
EndEvent

Event OnOptionSliderOpen(int option)
	if option == OID_MaximumAttackingEnemiesInterior
		SetSliderDialogStartValue(VAL_iMaxUnlockedEnemiesInterior)
		SetSliderDialogDefaultValue(4.0)
		SetSliderDialogRange(1.0, 40.0)
		SetSliderDialogInterval(1.0)
	elseif option == OID_MaximumAttackingEnemiesExterior
		SetSliderDialogStartValue(VAL_iMaxUnlockedEnemiesExterior)
		SetSliderDialogDefaultValue(4.0)
		SetSliderDialogRange(1.0, 40.0)
		SetSliderDialogInterval(1.0)
		
	elseif option == OID_EnemiesForModStartInterior
		SetSliderDialogStartValue(VAL_iEnemiesForModStartInterior)
		SetSliderDialogDefaultValue(2.0)
		SetSliderDialogRange(1.0, 40.0)
		SetSliderDialogInterval(1.0)
	elseif option == OID_EnemiesForModStartExterior
		SetSliderDialogStartValue(VAL_iEnemiesForModStartExterior)
		SetSliderDialogDefaultValue(2.0)
		SetSliderDialogRange(1.0, 40.0)
		SetSliderDialogInterval(1.0)
		
	elseif option == OID_MinimumAttackIntervalInterior
		SetSliderDialogStartValue(VAL_fWindowIntervalMinInterior)
		SetSliderDialogDefaultValue(5.0)
		SetSliderDialogRange(1.0, VAL_fWindowIntervalMaxInterior)
		SetSliderDialogInterval(1.0)
	elseif option == OID_MaximumAttackIntervalInterior
		SetSliderDialogStartValue(VAL_fWindowIntervalMaxInterior)
		SetSliderDialogDefaultValue(10.0)
		SetSliderDialogRange(VAL_fWindowIntervalMinInterior, 150)
		SetSliderDialogInterval(1.0)
		
	elseif option == OID_MinimumAttackIntervalExterior
		SetSliderDialogStartValue(VAL_fWindowIntervalMinExterior)
		SetSliderDialogDefaultValue(5.0)
		SetSliderDialogRange(1.0, VAL_fWindowIntervalMaxExterior)
		SetSliderDialogInterval(1.0)
	elseif option == OID_MaximumAttackIntervalExterior
		SetSliderDialogStartValue(VAL_fWindowIntervalMaxExterior)
		SetSliderDialogDefaultValue(10.0)
		SetSliderDialogRange(VAL_fWindowIntervalMinExterior, 150)
		SetSliderDialogInterval(1.0)
	endif
Endevent

Event OnOptionSliderAccept(int option, float value)
	if option == OID_MaximumAttackingEnemiesInterior
		VAL_iMaxUnlockedEnemiesInterior = value as int 
		SetSliderOptionValue(option, value, "{0} enemies")
		JsonUtil.SetIntValue(sCurrentSettingsFilepath, "iMaxUnlockedEnemiesInterior", VAL_iMaxUnlockedEnemiesInterior)
	elseif option == OID_MaximumAttackingEnemiesExterior
		VAL_iMaxUnlockedEnemiesExterior = value as int 
		SetSliderOptionValue(option, value, "{0} enemies")
		JsonUtil.SetIntValue(sCurrentSettingsFilepath, "iMaxUnlockedEnemiesExterior", VAL_iMaxUnlockedEnemiesExterior)
		
	elseif option == OID_EnemiesForModStartInterior
		VAL_iEnemiesForModStartInterior = value as int
		SetSliderOptionValue(option, value, "{0} enemies")
		JsonUtil.SetIntValue(sCurrentSettingsFilepath, "iEnemiesForModStartInterior", VAL_iEnemiesForModStartInterior)
	elseif option == OID_EnemiesForModStartExterior
		VAL_iEnemiesForModStartExterior = value as int 
		SetSliderOptionValue(option, value, "{0} enemies")
		JsonUtil.SetIntValue(sCurrentSettingsFilepath, "iEnemiesForModStartExterior", VAL_iEnemiesForModStartExterior)
		
	elseif option == OID_MinimumAttackIntervalInterior
		VAL_fWindowIntervalMinInterior = value 
		SetSliderOptionValue(option, value, "{1} seconds")
		JsonUtil.SetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMinInterior", VAL_fWindowIntervalMinInterior)
	elseif option == OID_MaximumAttackIntervalInterior
		VAL_fWindowIntervalMaxInterior = value 
		SetSliderOptionValue(option, value, "{1} seconds")
		JsonUtil.SetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMaxInterior", VAL_fWindowIntervalMaxInterior)
		
	elseif option == OID_MinimumAttackIntervalExterior
		VAL_fWindowIntervalMinExterior = value 
		SetSliderOptionValue(option, value, "{1} seconds")
		JsonUtil.SetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMinExterior", VAL_fWindowIntervalMinExterior)
	elseif option == OID_MaximumAttackIntervalExterior
		VAL_fWindowIntervalMaxExterior = value 
		SetSliderOptionValue(option, value, "{1} seconds")
		JsonUtil.SetFloatValue(sCurrentSettingsFilepath, "fWindowIntervalMaxExterior", VAL_fWindowIntervalMaxExterior)
	endif
	JsonUtil.Save(sCurrentSettingsFilepath)
	
	if !JsonUtil.JsonExists(sCurrentSettingsFilepath)
		Debug.MessageBox("Wait Your Turn: Settings Application Failed. This function requires PapyrusUtil to be installed!")
	endif
EndEvent

Event OnOptionSelect(int option)
	if option == OID_ModEnabled
		VAL_bModEnabled = !VAL_bModEnabled
		bModEnabled.SetValue(VAL_bModEnabled as float)
		SetToggleOptionValue(option, VAL_bModEnabled)
		JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bModEnabled", VAL_bModEnabled as int)
		; Grey out valid settings if mod turned off
		if !VAL_bModEnabled
			SetOptionFlags(Header_CircleSettings, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_BlockWhileCircling, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_DraugrBlockEnable, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_FalmerBlockEnable, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_bRandomize, OPTION_FLAG_DISABLED)
			
			SetOptionFlags(Header_InteriorSettings, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_EnemiesForModStartInterior, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_MaximumAttackingEnemiesInterior, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_MinimumAttackIntervalInterior, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_MaximumAttackIntervalInterior, OPTION_FLAG_DISABLED)
			
			SetOptionFlags(Header_ExteriorSettings, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_EnemiesForModStartExterior, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_MaximumAttackingEnemiesExterior, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_MinimumAttackIntervalExterior, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_MaximumAttackIntervalExterior, OPTION_FLAG_DISABLED)
			
			SetOptionFlags(Header_ExperimentalSettings, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_bLOSExperimental, OPTION_FLAG_DISABLED)
			
			SetOptionFlags(Header_ResetSettings, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_bResetSettings, OPTION_FLAG_DISABLED)
		else
			SetOptionFlags(Header_CircleSettings, OPTION_FLAG_NONE)
			SetOptionFlags(OID_BlockWhileCircling, OPTION_FLAG_NONE)
			SetOptionFlags(OID_DraugrBlockEnable, OPTION_FLAG_NONE)
			SetOptionFlags(OID_FalmerBlockEnable, OPTION_FLAG_NONE)
			SetOptionFlags(OID_bRandomize, OPTION_FLAG_NONE)
			
			SetOptionFlags(Header_InteriorSettings, OPTION_FLAG_NONE)
			SetOptionFlags(OID_EnemiesForModStartInterior, OPTION_FLAG_NONE)
			SetOptionFlags(OID_MaximumAttackingEnemiesInterior, OPTION_FLAG_NONE)
			SetOptionFlags(OID_MinimumAttackIntervalInterior, OPTION_FLAG_NONE)
			SetOptionFlags(OID_MaximumAttackIntervalInterior, OPTION_FLAG_NONE)
			
			SetOptionFlags(Header_ExteriorSettings, OPTION_FLAG_NONE)
			SetOptionFlags(OID_EnemiesForModStartExterior, OPTION_FLAG_NONE)
			SetOptionFlags(OID_MaximumAttackingEnemiesExterior, OPTION_FLAG_NONE)
			SetOptionFlags(OID_MinimumAttackIntervalExterior, OPTION_FLAG_NONE)
			SetOptionFlags(OID_MaximumAttackIntervalExterior, OPTION_FLAG_NONE)
			
			SetOptionFlags(Header_ExperimentalSettings, OPTION_FLAG_NONE)
			SetOptionFlags(OID_bLOSExperimental, OPTION_FLAG_NONE)
			
			SetOptionFlags(Header_ResetSettings, OPTION_FLAG_NONE)
			SetOptionFlags(OID_bResetSettings, OPTION_FLAG_NONE)
		endif
	elseif option == OID_BlockWhileCircling
		VAL_bBlockWhileCircling = !VAL_bBlockWhileCircling
		bBlockWhileCircling.SetValue(VAL_bBlockWhileCircling as float)
		SetToggleOptionValue(option, VAL_bBlockWhileCircling)
		JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bBlockWhileCircling", VAL_bBlockWhileCircling as int)
		; Grey out the Draugr and Falmer block options if Block While Circling turned off
		if !VAL_bBlockWhileCircling
			SetOptionFlags(OID_DraugrBlockEnable, OPTION_FLAG_DISABLED)
			SetOptionFlags(OID_FalmerBlockEnable, OPTION_FLAG_DISABLED)
		else
			SetOptionFlags(OID_DraugrBlockEnable, OPTION_FLAG_NONE)
			SetOptionFlags(OID_FalmerBlockEnable, OPTION_FLAG_NONE)
		endif
	elseif option == OID_DraugrBlockEnable
		VAL_bDraugrBlockEnable = !VAL_bDraugrBlockEnable
		SetToggleOptionValue(option, VAL_bDraugrBlockEnable)
		JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bDraugrBlockEnable", VAL_bDraugrBlockEnable as int)
	elseif option == OID_FalmerBlockEnable
		VAL_bFalmerBlockEnable = !VAL_bFalmerBlockEnable
		SetToggleOptionValue(option, VAL_bFalmerBlockEnable)
		JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bFalmerBlockEnable", VAL_bFalmerBlockEnable as int)
	elseif option == OID_bAutoLoad
		bAutoLoad = !bAutoLoad
		SetToggleOptionValue(option, bAutoLoad)
		JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bAutoLoad", bAutoLoad as int)
	elseif option == OID_bLoad 
		bool bConfirm = ShowMessage("Load saved preset?")
		if bConfirm 
			ImportPreset()
			PassVars()
		endif
	elseif option == OID_bSave
		bool bConfirm = ShowMessage("Save current configuration to an external preset file?")
		if bConfirm
			ExportPreset()
		endif
	elseif option == OID_bRandomize
		VAL_bRandomize = !VAL_bRandomize
		WYTRandom.SetValue(VAL_bRandomize as float)
		SetToggleOptionValue(option, VAL_bRandomize)
		JsonUtil.SetIntValue(sCurrentSettingsFilepath, "WYTRandom", VAL_bRandomize as int)
	elseif option == OID_bResetSettings
		bool bConfirm = ShowMessage("Reset the configuration to default settings?")
		if bConfirm
			ResetSettings()
		endif
	;elseif option == OID_DebugMode
		;VAL_bDebug == !VAL_bDebug
		;bDebugMode.SetValue(VAL_bDebug as float)
		;Debug.MessageBox("Notification set to " + bDebugMode.GetValue() as bool)
		;SetToggleOptionValue(option, VAL_bDebug)
	elseif option == OID_bLOSExperimental
		VAL_bLOSExperimental = !VAL_bLOSExperimental
		bLOSExperimental.SetValue(VAL_bLOSExperimental as float)
		SetToggleOptionValue(option, VAL_bLOSExperimental)
		JsonUtil.SetIntValue(sCurrentSettingsFilepath, "bLOSExperimental", VAL_bLOSExperimental as int)
	endif
	JsonUtil.Save(sCurrentSettingsFilepath)
	
	if !JsonUtil.JsonExists(sCurrentSettingsFilepath)
		Debug.MessageBox("Wait Your Turn: Settings Application Failed. This function requires PapyrusUtil to be installed!")
	endif
Endevent

Event OnOptionHighlight(int option)
	if option == OID_MaximumAttackingEnemiesInterior
		SetInfoText(INF_MaxUnlockedEnemiesInterior)
		
	elseif option == OID_MaximumAttackingEnemiesExterior
		SetInfoText(INF_MaxUnlockedEnemiesExterior)
		
	elseif option == OID_EnemiesForModStartInterior
		SetInfoText(INF_EnemiesForModStartInterior)
		
	elseif option == OID_EnemiesForModStartExterior
		SetInfoText(INF_EnemiesForModStartExterior)

	elseif option == OID_MinimumAttackIntervalInterior
		SetInfoText(INF_MinimumAttackIntervalInterior)
	
	elseif option == OID_MaximumAttackIntervalInterior
		SetInfoText(INF_MaximumAttackIntervalInterior)

	elseif option == OID_MinimumAttackIntervalExterior
		SetInfoText(INF_MinimumAttackIntervalExterior)
	
	elseif option == OID_MaximumAttackIntervalExterior
		SetInfoText(INF_MaximumAttackIntervalExterior)

	elseif option == OID_ModEnabled
		SetInfoText(INF_ModEnabled)

	elseif option == OID_BlockWhileCircling
		SetInfoText(INF_BlockWhileCircling)
		
	elseif option == OID_DraugrBlockEnable
		SetInfoText(INF_DraugrBlockEnable)
	
	elseif option == OID_FalmerBlockEnable
		SetInfoText(INF_FalmerBlockEnable)

	elseif option == OID_bAutoLoad
		SetInfoText(INF_AutoLoad)

	elseif option == OID_bLoad
		SetInfoText(INF_bLoad)

	elseif option == OID_bSave
		SetInfoText(INF_bSave)
		
	elseif option == OID_bRandomize
	     SetInfoText(INF_Randomize)
		 
	elseif option == OID_bResetSettings
		SetInfoText(INF_bResetSettings)
		
	elseif option == OID_bLOSExperimental
		SetInfoText(INF_bLOSExperimental)
	endif
EndEvent

Function ExportPreset()
	JsonUtil.SetIntValue(sPresetFilepath, "iEnemiesForModStartInterior", VAL_iEnemiesForModStartInterior)
	JsonUtil.SetIntValue(sPresetFilepath, "iEnemiesForModStartExterior", VAL_iEnemiesForModStartExterior)
	
	JsonUtil.SetIntValue(sPresetFilepath, "iMaxUnlockedEnemiesInterior", VAL_iMaxUnlockedEnemiesInterior)
	JsonUtil.SetIntValue(sPresetFilepath, "iMaxUnlockedEnemiesExterior", VAL_iMaxUnlockedEnemiesExterior)
	
	JsonUtil.SetFloatValue(sPresetFilepath, "fWindowIntervalMinInterior", VAL_fWindowIntervalMinInterior)
	JsonUtil.SetFloatValue(sPresetFilepath, "fWindowIntervalMaxInterior", VAL_fWindowIntervalMaxInterior)
	
	JsonUtil.SetFloatValue(sPresetFilepath, "fWindowIntervalMinExterior", VAL_fWindowIntervalMinExterior)
	JsonUtil.SetFloatValue(sPresetFilepath, "fWindowIntervalMaxExterior", VAL_fWindowIntervalMaxExterior)
	
	JsonUtil.SetIntValue(sPresetFilepath, "bModEnabled", VAL_bModEnabled as int)
	JsonUtil.SetIntValue(sPresetFilepath, "bBlockWhileCircling", VAL_bBlockWhileCircling as int)
	JsonUtil.SetIntValue(sPresetFilepath, "bDraugrBlockEnable", VAL_bDraugrBlockEnable as int)
	JsonUtil.SetIntValue(sPresetFilepath, "bFalmerBlockEnable", VAL_bFalmerBlockEnable as int)
	JsonUtil.SetIntValue(sPresetFilepath, "WYTRandom", VAL_bRandomize as int)
	
	JsonUtil.SetIntValue(sPresetFilepath, "bAutoLoad", bAutoLoad as int)
	JsonUtil.SetIntValue(sPresetFilepath, "bLOSExperimental", VAL_bLOSExperimental as int)
	
	JsonUtil.Save(sPresetFilepath)

	if JsonUtil.JsonExists(sPresetFilepath)
		Debug.MessageBox("Your configuration has been saved to the preset file.")
	else 
		Debug.MessageBox("Preset Save Failed. This function requires PapyrusUtil to be installed!")
	endif 
Endfunction 

bool Function ImportPreset()
	bool bPresetExists = JsonUtil.JsonExists(sPresetFilepath)
	if bPresetExists
		if JsonUtil.IsGood(sPresetFilepath) && JsonUtil.GetErrors(sPresetFilepath) == ""
			bAutoLoad = JsonUtil.GetIntValue(sPresetFilepath, "bAutoLoad", 0) as bool
			if bInitiated || bAutoLoad
				;do stuff here
				VAL_iEnemiesForModStartInterior = JsonUtil.GetIntValue(sPresetFilepath, "iEnemiesForModStartInterior", 2)
				VAL_iEnemiesForModStartExterior = JsonUtil.GetIntValue(sPresetFilepath, "iEnemiesForModStartExterior", 2)
				
				VAL_iMaxUnlockedEnemiesInterior = JsonUtil.GetIntValue(sPresetFilepath, "iMaxUnlockedEnemiesInterior", 4)
				VAL_iMaxUnlockedEnemiesExterior = JsonUtil.GetIntValue(sPresetFilepath, "iMaxUnlockedEnemiesExterior", 4)
				
				VAL_fWindowIntervalMinInterior = JsonUtil.GetFloatValue(sPresetFilepath, "fWindowIntervalMinInterior", 5.0)
				VAL_fWindowIntervalMaxInterior = JsonUtil.GetFloatValue(sPresetFilepath, "fWindowIntervalMaxInterior", VAL_fWindowIntervalMinInterior + 5)
				
				VAL_fWindowIntervalMinExterior = JsonUtil.GetFloatValue(sPresetFilepath, "fWindowIntervalMinExterior", 5.0)
				VAL_fWindowIntervalMaxExterior = JsonUtil.GetFloatValue(sPresetFilepath, "fWindowIntervalMaxExterior", VAL_fWindowIntervalMinInterior + 5)
				
				VAL_bModEnabled = JsonUtil.GetIntValue(sPresetFilepath, "bModEnabled", 1) as bool
				VAL_bBlockWhileCircling = JsonUtil.GetIntValue(sPresetFilepath, "bBlockWhileCircling", 1) as bool
				VAL_bDraugrBlockEnable = JsonUtil.GetIntValue(sPresetFilepath, "bDraugrBlockEnable", 1) as bool
				VAL_bFalmerBlockEnable = JsonUtil.GetIntValue(sPresetFilepath, "bFalmerBlockEnable", 1) as bool
				VAL_bRandomize = JsonUtil.GetIntValue(sPresetFilepath, "WYTRandom", 1) as bool
				
				bAutoLoad = JsonUtil.GetIntValue(sPresetFilepath, "bAutoLoad", 0) as bool
				VAL_bLOSExperimental = JsonUtil.GetIntValue(sPresetFilepath, "bLOSExperimental", 0) as bool
				
				;more options in the future possibly
				if bInitiated && !bAutoLoad
					ForcePageReset()
					Debug.MessageBox("Wait Your Turn: Your configuration has been loaded from the preset file.")
				elseif bAutoLoad
					Debug.Notification("Wait Your Turn: Existing preset loaded automatically.")
				endif
				return True
			endif
				;load
		else
			Debug.MessageBox("Wait Your Turn: Preset load failed." + "\nErrors: " + JsonUtil.GetErrors(sPresetFilepath))
		endif
	else 
		if bInitiated
			Debug.MessageBox("Wait Your Turn: Preset load failed. This function requires PapyrusUtil to be installed!")
		endif
	endif
	return False
EndFunction

Function ResetSettings()
	VAL_iEnemiesForModStartInterior = 2
	VAL_iEnemiesForModStartExterior = 2
	
	VAL_iMaxUnlockedEnemiesInterior = 4
	VAL_iMaxUnlockedEnemiesExterior = 4
	
	VAL_fWindowIntervalMinInterior = 5.0
	VAL_fWindowIntervalMaxInterior = VAL_fWindowIntervalMinInterior + 5
	
	VAL_fWindowIntervalMinExterior = 5.0
	VAL_fWindowIntervalMaxExterior = VAL_fWindowIntervalMinInterior + 5
	
	VAL_bModEnabled = 1 as bool
	VAL_bBlockWhileCircling = 1 as bool
	VAL_bDraugrBlockEnable = 1 as bool
	VAL_bFalmerBlockEnable = 1 as bool
	VAL_bRandomize = 1 as bool
	
	bAutoLoad = 0 as bool
	VAL_bLOSExperimental = 0 as bool
	; If called from the MCM
	if bInitiated
		PassVars()
		ForcePageReset()
	endif
Endfunction
