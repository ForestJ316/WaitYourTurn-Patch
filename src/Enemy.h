#pragma once

namespace EnemyHandler
{
	class Enemy
	{
	public:
		static void InitializeCirclePackage();

		struct EnemyState
		{
			bool bIsLocked;
			bool bIsDeadOrInvalid;
			bool bBlockSpellActive;
			bool bEnemyIsBlocking;
		};
		void LockEnemy(RE::ObjectRefHandle a_enemyHandle, EnemyState& a_enemyState, bool bIsModStart);
		void LockCallback(RE::ObjectRefHandle a_enemyHandle, EnemyState& a_enemyState);
		void UnlockEnemy(RE::ObjectRefHandle a_enemyHandle, EnemyState& a_enemyState, bool bIsUnlockLOS);
		void UnlockCallback(RE::ObjectRefHandle a_enemyHandle, EnemyState& a_enemyState, bool bIsUnlockLOS);
		void EnemyDeadOrInvalid(RE::ObjectRefHandle a_enemyHandle, EnemyState& a_enemyState, bool bEffectFinished);		

	protected:
		static inline RE::TESPackage* package;

		static inline int iCurrentUnlockedEnemies = 0;

	private:
		using Lock = std::mutex;
		using Locker = std::lock_guard<Lock>;
		mutable Lock lock;
	};

	class VMCallback :
		public RE::BSScript::IStackCallbackFunctor,
		public Enemy
	{
	public:
		VMCallback(RE::ObjectRefHandle a_enemyHandle, EnemyState& a_enemyState, bool bIsLockFunc, bool bIsUnlockLOS = false, bool bEffectFinished = false) :
			callbackEnemyHandle(a_enemyHandle),
			callbackEnemyState(a_enemyState),
			bIsLockFunction(bIsLockFunc),
			callbackIsUnlockLOS(bIsUnlockLOS),
			callbackEffectFinished(bEffectFinished)
		{};
		~VMCallback() {};

	private:
		RE::ObjectRefHandle callbackEnemyHandle;
		EnemyState& callbackEnemyState;
		bool bIsLockFunction;
		bool callbackIsUnlockLOS;
		bool callbackEffectFinished;

		virtual void operator()([[maybe_unused]] RE::BSScript::Variable result) override
		{
			if (callbackEffectFinished)
			{
				if (callbackEnemyHandle.get().get() && callbackEnemyHandle.get().get()->As<RE::Character>())
				{
					auto a_enemyCharacter = callbackEnemyHandle.get().get()->As<RE::Character>();
					a_enemyCharacter->EvaluatePackage();
				}
			}
			if (!callbackEnemyState.bIsDeadOrInvalid)
			{
				if (bIsLockFunction == true)
					Enemy::LockCallback(callbackEnemyHandle, callbackEnemyState);
				else if (bIsLockFunction == false)
					Enemy::UnlockCallback(callbackEnemyHandle, callbackEnemyState, callbackIsUnlockLOS);
			}
		}
		virtual void SetObject([[maybe_unused]] const RE::BSTSmartPointer<RE::BSScript::Object>& a_object) override {}
	};
};
