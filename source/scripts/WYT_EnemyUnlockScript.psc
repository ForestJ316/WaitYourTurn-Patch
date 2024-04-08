ScriptName WYT_EnemyUnlockScript Extends ActiveMagicEffect

Function RelockEnemy(Actor akEnemy) global native
bool Function IsEnemyLocked(Actor akEnemy) global native

GlobalVariable Property fLOS_Experimental Auto
Package Property CirclePackage Auto
Actor Property PlayerRef Auto

Actor ThisEnemy

Event OnEffectStart(Actor akTarget, Actor akCaster)
	ThisEnemy = akTarget
	bool bLOSCheck = fLOS_Experimental.GetValue() as bool
	if bLOSCheck
		RegisterForLOS(akTarget, PlayerRef)
	endif
EndEvent

Event OnEffectFinish(Actor akTarget, Actor akCaster)
	ThisEnemy = None
EndEvent

Event OnGainLOS(Actor akSource, ObjectReference akTarget)
	bool bLOSCheck = fLOS_Experimental.GetValue() as bool
	; Check for case where feature got turned off in combat
	if !bLOSCheck
		UnregisterForLOS(akSource, PlayerRef)
	else
		bool bIsEnemyLocked = IsEnemyLocked(akSource)
		if bIsEnemyLocked
			akSource.SetLookAt(PlayerRef, True)
		endif
	endif
EndEvent

Function RegisterActor(Actor akEnemy, float a_delta)
	RegisterForSingleUpdate(a_delta)
EndFunction

Event OnUpdate()
	RelockEnemy(ThisEnemy)
EndEvent

; If MCM changed then unregister all
Function MCMChanged()
	UnregisterForUpdate()
EndFunction
