#include "../../includes.hpp"
#include "../skinchanger/skins.h"
#include "../inventory/items.h"


enum ESequence
{
	SEQUENCE_DEFAULT_DRAW = 0,
	SEQUENCE_DEFAULT_IDLE1 = 1,
	SEQUENCE_DEFAULT_IDLE2 = 2,
	SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
	SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
	SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
	SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
	SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
	SEQUENCE_DEFAULT_LOOKAT01 = 12,

	SEQUENCE_BUTTERFLY_DRAW = 0,
	SEQUENCE_BUTTERFLY_DRAW2 = 1,
	SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
	SEQUENCE_BUTTERFLY_LOOKAT02 = 14,
	SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

	SEQUENCE_FALCHION_IDLE1 = 1,
	SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
	SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
	SEQUENCE_FALCHION_LOOKAT01 = 12,
	SEQUENCE_FALCHION_LOOKAT02 = 13,

	SEQUENCE_DAGGERS_IDLE1 = 1,
	SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
	SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
	SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
	SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

	SEQUENCE_BOWIE_IDLE1 = 1,
};

int random(const int& min, const int& max)
{
	return rand() % (max - min + 1) + min;
}

int GetNewAnimation(std::string model, const int sequence)
{
	if (model == "models/weapons/v_knife_butterfly.mdl")
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT02);
		default:
			return sequence + 1;
		}
	}
	if (model == "models/weapons/v_knife_falchion_advanced.mdl")
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
		}
	}
	if (model == "models/weapons/v_knife_push.mdl")
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return random(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return random(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
		}
	}
	if (model == "models/weapons/v_knife_survival_bowie.mdl")
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
		}
	}
	if (model == "models/weapons/v_knife_ursus.mdl" || model == "models/weapons/v_knife_cord.mdl" || model == "models/weapons/v_knife_canis.mdl" || model == "models/weapons/v_knife_outdoor.mdl" || model == "models/weapons/v_knife_skeleton.mdl")
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return random(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
		default:
			return sequence + 1;
		}
	}
	if (model == "models/weapons/v_knife_stiletto.mdl")
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random(12, 13);
		}
	}
	if (model == "models/weapons/v_knife_widowmaker.mdl")
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_LOOKAT01:
			return random(14, 15);
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
			return sequence - 1;
		}
	}
	return sequence;
}

void Remapping(recvProxyData data, weapon_t* entity)
{
	if (!g_ctx.local() || !g_ctx.local()->is_alive())
		return;

	const auto owner = m_entitylist()->GetClientEntityFromHandle(entity->m_hOwner());
	if (owner != g_ctx.local())
		return;


	const auto view_model_weapon = static_cast<weapon_t*>(m_entitylist()->GetClientEntityFromHandle(entity->weapon()));
	if (!view_model_weapon)
		return;

	const auto entry = k_weapon_info.find(view_model_weapon->m_iItemDefinitionIndex());

	if (entry == k_weapon_info.end())
		return;

	if (&entry->second == nullptr)
		return;

	const auto weaponInfo = &entry->second;
	auto& sequence = data.value._int;
	sequence = GetNewAnimation(weaponInfo->model, sequence);
}


void shonax_hooks::RecvProxy(recvProxyData& pData, void* entity, void* output)
{
	const auto proxy_data = const_cast<recvProxyData&>(pData);
	const auto view_model = static_cast<weapon_t*>(entity);
	Remapping(proxy_data, view_model);
	if (sequence_hook)
		sequence_hook(pData, entity, output);
}