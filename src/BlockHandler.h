#pragma once

namespace EnemyHandler
{
	class BlockHandler
	{
	public:
		static void InitializeForms();

		void EnemyAddBlockOverride(RE::Character* a_enemy, bool& bIsBlocking);
		void EnemyRemoveBlockOverride(RE::Character* a_enemy, bool& bIsBlocking);

		void EnemyAddBlockSpell(RE::Character* a_enemy);
		void EnemyRemoveBlockSpell(RE::Character* a_enemy);

	private:
		static inline RE::TESIdleForm* regularBlockStart_idleForm;
		static inline RE::TESIdleForm* draugrBlockStart_idleForm;
		static inline RE::TESIdleForm* falmerBlockStart_idleForm;
		static inline RE::TESIdleForm* idleStopLoose_idleForm;

		static inline RE::TESFaction* draugrFaction;
		static inline RE::TESFaction* falmerFaction;

		static inline RE::SpellItem* block_spell;

		using Lock = std::mutex;
		using Locker = std::lock_guard<Lock>;
		mutable Lock lock;
	};
}
