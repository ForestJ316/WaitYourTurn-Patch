#pragma once

#include "Enemy.h"
#include "BlockHandler.h"

#include <unordered_map>

// Hashing function for RE::ObjectRefHandle
// Ty to Ersh1 for the implementation!
template <class T>
struct std::hash<RE::BSPointerHandle<T>>
{
	uint32_t operator()(const RE::BSPointerHandle<T>& a_handle) const
	{
		uint32_t nativeHandle = const_cast<RE::BSPointerHandle<T>*>(&a_handle)->native_handle();
		return nativeHandle;
	}
};

namespace EnemyHandler
{
	class CombatHandler :
		public RE::BSTEventSink<RE::TESDeathEvent>,
		public Enemy,
		public BlockHandler
	{
	private:
		using EventResult = RE::BSEventNotifyControl;

	public:
		static CombatHandler* GetSingleton()
		{
			static CombatHandler singleton;
			return std::addressof(singleton);
		}
		virtual EventResult ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>* a_eventSource) override;

		static void Initialize();
		static void UpdateSettings();
		void SyncSettings();
		void ResetVars();
		
		void AddEnemyToList(RE::Character* a_enemy, bool bIsLoadGame);
		void RaiseEnemyUnlock();
		void EraseEnemy(RE::ObjectRefHandle a_enemyHandle);

		void UpdateEnemyRelock(RE::ObjectRefHandle a_enemyHandle, bool bIsLOSRelock);

	private:
		friend class Hooks;
		friend class Enemy;
		
		CombatHandler() = default;
		CombatHandler(const CombatHandler&) = delete;
		CombatHandler(CombatHandler&&) = delete;

		~CombatHandler() override = default;

		CombatHandler& operator=(const CombatHandler&) = delete;
		CombatHandler& operator=(CombatHandler&&) = delete;

		using Lock = std::mutex;
		using Locker = std::lock_guard<Lock>;
		mutable Lock lock;

		static inline RE::FormID FilterEffectID;
		static inline RE::FormID BlockEffectID;
		static inline RE::FormID PlayerCombatEffectID;

		static inline bool bModStarted = false;
		bool bCombatInitiated = false;
		int iEnemyCount = 0;
		std::unordered_map<RE::ObjectRefHandle, EnemyState> EnemyList;
		
		int iEnemiesForModStart = 0;
		int iMaxUnlockedEnemies = 0;
		float fWindowIntervalMin = 0.f;
		float fWindowIntervalMax = 0.f;
	};

	class Hooks
	{
	public:
		static void Install()
		{
			_OnEffectStart = REL::Relocation<uintptr_t>(RE::VTABLE_ScriptEffect[0]).write_vfunc(0x14, OnEffectStart); // Start
			_OnEffectFinish = REL::Relocation<uintptr_t>(RE::VTABLE_ScriptEffect[0]).write_vfunc(0x15, OnEffectFinish); // Finish
			_OnEffectFinishLoadGame = REL::Relocation<uintptr_t>(RE::VTABLE_ScriptEffect[0]).write_vfunc(0xA, OnEffectFinishLoadGame); // Finish Load Game
			_Update = REL::Relocation<uintptr_t>(RE::VTABLE_Character[0]).write_vfunc(0xAD, Update); // Character Update
			logger::info("Installed virtual function hooks.");
		}

		struct UpdateState
		{
			// Unlock timer
			float timer = 0.f;
			float unlockDuration = 0.f;
			bool isLoadGame = false;
			// Experimental LOS check
			float LOStimer = 0.f;
			bool EnemyHasPackage = true;
		};
		static inline std::unordered_map<RE::ObjectRefHandle, UpdateState> PollingHandler;

	private:
		static void OnEffectStart(RE::ActiveEffect* a_effect);
		static inline REL::Relocation<decltype(OnEffectStart)> _OnEffectStart;

		static void OnEffectFinish(RE::ActiveEffect* a_effect);
		static inline REL::Relocation<decltype(OnEffectFinish)> _OnEffectFinish;

		static void OnEffectFinishLoadGame(RE::ActiveEffect* a_effect, RE::BGSLoadFormBuffer* a_buf);
		static inline REL::Relocation<decltype(OnEffectFinishLoadGame)> _OnEffectFinishLoadGame;

		static void Update(RE::Character* a_enemy, float a_delta);
		static inline REL::Relocation<decltype(Update)> _Update;
	};
}
