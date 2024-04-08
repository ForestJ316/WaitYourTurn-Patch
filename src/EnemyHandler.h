#pragma once

#include <Enemy.h>
#include <BlockHandler.h>
#include <unordered_map>
#include <shared_mutex>

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
		
		void AddEnemyToList(RE::Character* a_enemy, RE::ActiveEffect* a_effect);
		void RaiseEnemyUnlock();
		void UpdateEnemyRelock(RE::Character* a_enemy); // Native function
		bool GetEnemyLockedState(RE::Character* a_enemy); // Native function

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

		bool bInitiated = false;
		int iEnemyCount = 0;
		int iCurrentUnlockedEnemies = 0;
		std::unordered_map<RE::Character*, EnemyState> EnemyList;
		
		int iEnemiesForModStart = 0;
		int iMaxUnlockedEnemies = 0;
		static inline float fWindowIntervalMin = 0.f;
		static inline float fWindowIntervalMax = 0.f;
	};

	class Hooks
	{
	public:
		static void Install()
		{
			_OnEffectStart = REL::Relocation<uintptr_t>(RE::VTABLE_ScriptEffect[0]).write_vfunc(0x14, OnEffectStart); // Start
			_OnEffectFinish = REL::Relocation<uintptr_t>(RE::VTABLE_ScriptEffect[0]).write_vfunc(0x15, OnEffectFinish); // Finish
			_OnEffectFinishLoadGame = REL::Relocation<uintptr_t>(RE::VTABLE_ScriptEffect[0]).write_vfunc(0xA, OnEffectFinishLoadGame); // Finish
			logger::info("Installed virtual function hooks.");
		}

	private:
		static void OnEffectStart(RE::ActiveEffect* a_effect);
		static inline REL::Relocation<decltype(OnEffectStart)> _OnEffectStart;

		static void OnEffectFinish(RE::ActiveEffect* a_effect);
		static inline REL::Relocation<decltype(OnEffectFinish)> _OnEffectFinish;

		static void OnEffectFinishLoadGame(RE::ActiveEffect* a_effect, RE::BGSLoadFormBuffer* a_buf);
		static inline REL::Relocation<decltype(OnEffectFinishLoadGame)> _OnEffectFinishLoadGame;
	};
}
