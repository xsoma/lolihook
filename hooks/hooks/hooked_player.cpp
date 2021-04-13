// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\lagcompensation\local_animations.h"
#include "..\..\cheats\misc\prediction_system.h"

_declspec(noinline)bool hooks::setupbones_detour(void* ecx, matrix3x4_t* bone_world_out, int max_bones, int bone_mask, float current_time)
{
	auto result = true;

	static auto r_jiggle_bones = m_cvar()->FindVar(crypt_str("r_jiggle_bones"));
	auto r_jiggle_bones_backup = r_jiggle_bones->GetInt();

	r_jiggle_bones->SetValue(0);

	if (!ecx)
		result = ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);
	else if (!g_cfg.ragebot.enable && !g_cfg.legitbot.enabled)
		result = ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);
	else
	{
		auto player = (player_t*)((uintptr_t)ecx - 0x4);

		if (!player->valid(false, false))
			result = ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);
		else
		{
			auto animstate = player->get_animation_state();
			auto previous_weapon = animstate ? animstate->m_pLastBoneSetupWeapon : nullptr;

			if (previous_weapon)
				animstate->m_pLastBoneSetupWeapon = animstate->m_pActiveWeapon; //-V1004

			if (g_ctx.globals.setuping_bones)
				result = ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);
			else if (g_cfg.legitbot.enabled && player != g_ctx.local())
				result = ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);
			else if (!g_ctx.local()->is_alive())
				result = ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);
			else if (player == g_ctx.local())
				result = ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);
			else if (!player->m_CachedBoneData().Count()) //-V807
				result = ((SetupBonesFn)original_setupbones)(ecx, bone_world_out, max_bones, bone_mask, current_time);
			else if (bone_world_out && max_bones != -1)
				memcpy(bone_world_out, player->m_CachedBoneData().Base(), player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));

			if (previous_weapon)
				animstate->m_pLastBoneSetupWeapon = previous_weapon;
		}
	}

	r_jiggle_bones->SetValue(r_jiggle_bones_backup);
	return result;
}

bool __fastcall hooks::hooked_setupbones(void* ecx, void* edx, matrix3x4_t* bone_world_out, int max_bones, int bone_mask, float current_time)
{
	return setupbones_detour(ecx, bone_world_out, max_bones, bone_mask, current_time);
}

enum Effects_t : int {
	EF_BONEMERGE = 0x001,	// Performs bone merge on client side
	EF_BRIGHTLIGHT = 0x002,	// DLIGHT centered at entity origin
	EF_DIMLIGHT = 0x004,	// player flashlight
	EF_NOINTERP = 0x008,	// don't interpolate the next frame
	EF_NOSHADOW = 0x010,	// Don't cast no shadow
	EF_NODRAW = 0x020,	// don't draw entity
	EF_NORECEIVESHADOW = 0x040,	// Don't receive no shadow
	EF_BONEMERGE_FASTCULL = 0x080,	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
	EF_ITEM_BLINK = 0x100,	// blink an item so that the user notices it.
	EF_PARENT_ANIMATES = 0x200,	// always assume that the parent entity is animating
	EF_MAX_BITS = 10
};

_declspec(noinline)void hooks::standardblendingrules_detour(player_t* player, int i, CStudioHdr* hdr, Vector* pos, Quaternion* q, float curtime, int boneMask)
{
	if (!player || (player->EntIndex() - 1) > 63)
		return ((StandardBlendingRulesFn)original_standardblendingrules)(player, hdr, pos, q, curtime, boneMask);

	// disable interpolation.
	if (!(player->m_fEffects() & EF_NOINTERP))
		player->m_fEffects() |= EF_NOINTERP;

	((StandardBlendingRulesFn)original_standardblendingrules)(player, hdr, pos, q, curtime, boneMask);

	// restore interpolation.
	player->m_fEffects() &= ~EF_NOINTERP;

	/*auto backup_effects = player->m_fEffects();

	if (player == g_ctx.local())
		player->m_fEffects() |= 8;

	((StandardBlendingRulesFn)original_standardblendingrules)(player, hdr, pos, q, curtime, boneMask);

	if (player == g_ctx.local())
		player->m_fEffects() = backup_effects;*/
}

void __fastcall hooks::hooked_standardblendingrules(player_t* player, int i, CStudioHdr* hdr, Vector* pos, Quaternion* q, float curtime, int boneMask)
{
	return standardblendingrules_detour(player, i, hdr, pos, q, curtime, boneMask);
}

_declspec(noinline)void hooks::doextrabonesprocessing_detour(player_t* player, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_list, void* context)
{
	if (!player)
		return ((DoExtraBonesProcessingFn)original_doextrabonesprocessing)(player, hdr, pos, q, matrix, bone_list, context);

	if (player && player->is_player() && player->is_alive()) {
		int* pAnimLayersOwner = reinterpret_cast<int*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(player) + g_csgo.AnimOverlay) + 0x30);
		for (int i = 13; i; --i) {
			if (reinterpret_cast<player_t*>(pAnimLayersOwner) != player) {
				*pAnimLayersOwner = reinterpret_cast<uintptr_t>(player);
			}

			pAnimLayersOwner += 14;
		}
	}

	return;
}

void __fastcall hooks::hooked_doextrabonesprocessing(player_t* player, void* edx, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_list, void* context)
{
	return doextrabonesprocessing_detour(player, hdr, pos, q, matrix, bone_list, context);
}

_declspec(noinline)void hooks::updateclientsideanimation_detour(player_t* player)
{
	if (g_ctx.globals.updating_animation && m_clientstate()->iDeltaTick != -1)
		return ((UpdateClientSideAnimationFn)original_updateclientsideanimation)(player);

	if (player == g_ctx.local())
		return ((UpdateClientSideAnimationFn)original_updateclientsideanimation)(player);
	
	if (!g_cfg.ragebot.enable && !g_cfg.legitbot.enabled)
		return ((UpdateClientSideAnimationFn)original_updateclientsideanimation)(player);

	if (!player->valid(false, false))
		return ((UpdateClientSideAnimationFn)original_updateclientsideanimation)(player);
}

void __fastcall hooks::hooked_updateclientsideanimation(player_t* player, uint32_t i)
{
	return updateclientsideanimation_detour(player);
}

_declspec(noinline)void hooks::physicssimulate_detour(player_t* player)
{
	auto simulation_tick = *(int*)((uintptr_t)player + 0x2AC);

	if (player != g_ctx.local() || !g_ctx.local()->is_alive() || m_globals()->m_tickcount == simulation_tick)
	{
		((PhysicsSimulateFn)original_physicssimulate)(player);
		return;
	}

	engineprediction::get().restore_netvars();
	((PhysicsSimulateFn)original_physicssimulate)(player);
	engineprediction::get().store_netvars();
}

void __fastcall hooks::hooked_physicssimulate(player_t* player)
{
	return physicssimulate_detour(player);
}

_declspec(noinline)void hooks::modifyeyeposition_detour(c_baseplayeranimationstate* state, Vector& position)
{
	if (state && g_ctx.globals.in_createmove)
		return ((ModifyEyePositionFn)original_modifyeyeposition)(state, position);
}

void __fastcall hooks::hooked_modifyeyeposition(c_baseplayeranimationstate* state, void* edx, Vector& position)
{
	return modifyeyeposition_detour(state, position);
}

_declspec(noinline)void hooks::calcviewmodelbob_detour(player_t* player, Vector& position)
{
	if (!g_cfg.esp.removals[REMOVALS_LANDING_BOB] || player != g_ctx.local() || !g_ctx.local()->is_alive())
		return ((CalcViewmodelBobFn)original_calcviewmodelbob)(player, position);
}

void __fastcall hooks::hooked_calcviewmodelbob(player_t* player, void* edx, Vector& position)
{
	return calcviewmodelbob_detour(player, position);
}

bool __fastcall hooks::hooked_shouldskipanimframe()
{
	return false;
}

int hooks::processinterpolatedlist()
{
	static auto allow_extrapolation = *(bool**)(util::FindSignature(crypt_str("client.dll"), crypt_str("A2 ? ? ? ? 8B 45 E8")) + 0x1);

	if (allow_extrapolation)
		*allow_extrapolation = false;

	return ((ProcessInterpolatedListFn)original_processinterpolatedlist)();
}