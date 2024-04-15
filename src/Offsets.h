#pragma once

static float* g_deltaTimeRealTime = (float*)RELOCATION_ID(523661, 410200).address();

typedef void(_fastcall* tSetLookAt)(RE::BSScript::Internal::VirtualMachine* a_vm, RE::VMStackID a_stackID, RE::TESObjectREFR* a_source, RE::TESObjectREFR* a_target, bool should_face);
static REL::Relocation<tSetLookAt> SetLookAt{ RELOCATION_ID(53930, 54754) };
