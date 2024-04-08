ScriptName PF_WYT_CirclePackage_010022F2 Extends Package Hidden

GlobalVariable Property fLOS_Experimental Auto
ObjectReference Property PlayerRef Auto

Function Fragment_8(Actor akActor)
	bool bLOSCheck = fLOS_Experimental.GetValue() as bool
	if bLOSCheck && akActor.HasLOS(PlayerRef)
		akActor.SetLookAt(PlayerRef, True)
	elseif !bLOSCheck
		akActor.SetLookAt(PlayerRef, True)
	endif
EndFunction

Function Fragment_10(Actor akActor)
	akActor.ClearLookAt()
EndFunction

Function Fragment_9(Actor akActor)
	akActor.ClearLookAt()
EndFunction
