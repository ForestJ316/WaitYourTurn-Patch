ScriptName PF_WYT_CirclePackage_010022F2 Extends Package Hidden

ObjectReference Property PlayerRef Auto

; OnBegin
Function Fragment_8(Actor akActor)
	akActor.SetLookAt(PlayerRef, True)
EndFunction

; OnChange
Function Fragment_9(Actor akActor)
	akActor.ClearLookAt()
EndFunction

; OnEnd
Function Fragment_10(Actor akActor)
	akActor.ClearLookAt()
EndFunction
