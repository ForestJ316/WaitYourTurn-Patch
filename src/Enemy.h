#pragma once

#include <deque>

namespace EnemyHandler
{
	class Enemy
	{
	public:
		using Callback = RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>;

		static void InitializeCirclePackage();

		struct EnemyState
		{
			bool bIsLocked;
			bool bIsDeadOrInvalid;
			bool bBlockSpellActive;
			bool bEnemyIsBlocking;
			RE::ActiveEffect* effect;
		};
		void LockEnemy(RE::Character* a_enemy, EnemyState& a_enemyState, int& a_currentUnlockedEnemies, bool bIsModStart);
		void LockCallback(RE::Character* a_enemy, EnemyState& a_enemyState);
		void UnlockEnemy(RE::Character* a_enemy, EnemyState& a_enemyState, int& a_currentUnlockedEnemies);
		void UnlockCallback(RE::Character* a_enemy, EnemyState& a_enemyState);
		void EnemyDeadOrInvalid(RE::Character* a_enemy, EnemyState& a_enemyState, int& a_currentUnlockedEnemies, bool bEffectFinished);
		void OnMCMChanged(RE::Character* a_enemy, RE::ActiveEffect* a_effect);

	private:
		static inline RE::TESPackage* package;

		struct EnemyInfo
		{
			RE::Character* a_enemy;
			EnemyState& a_enemyState;
			int& a_currentUnlockedEnemies;
			bool bIsModStart;
			bool bIsLockFunc;
		};

		using Lock = std::mutex;
		using Locker = std::lock_guard<Lock>;
		mutable Lock lock;
	};

	class VMCallback :
		public RE::BSScript::IStackCallbackFunctor,
		public Enemy
	{
	public:
		VMCallback(RE::Character* a_enemy, EnemyState& a_enemyState, bool bIsLockFunc, bool bEffectFinished, bool bMCMChanged = false) :
			callbackEnemy(a_enemy),
			callbackEnemyState(a_enemyState),
			bIsLockFunction(bIsLockFunc),
			callbackEffectFinished(bEffectFinished),
			callbackMCMChanged(bMCMChanged)
		{};
		~VMCallback() {};

	private:
		RE::Character* callbackEnemy;
		EnemyState& callbackEnemyState;
		bool bIsLockFunction;
		bool callbackEffectFinished;
		bool callbackMCMChanged;

		virtual void operator()([[maybe_unused]] RE::BSScript::Variable result) override
		{
			// If MCM changed then only do this
			if (callbackMCMChanged)
			{
				Enemy::OnMCMChanged(callbackEnemy, callbackEnemyState.effect);
				return;
			}
			if (callbackEffectFinished)
			{
				callbackEnemy->EvaluatePackage();
			}
			if (!callbackEnemyState.bIsDeadOrInvalid)
			{
				if (bIsLockFunction == true)
				{
					Enemy::LockCallback(callbackEnemy, callbackEnemyState);
				}
				else if (bIsLockFunction == false)
				{
					Enemy::UnlockCallback(callbackEnemy, callbackEnemyState);
				}
			}
		}
		virtual void SetObject([[maybe_unused]] const RE::BSTSmartPointer<RE::BSScript::Object>& a_object) override {}
	};
};
