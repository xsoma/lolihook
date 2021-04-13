#include "merged_hooks.h"


int __fastcall AddRenderable(void* ecx, void* edx, IClientRenderable* p_renderable, int unk1, RenderableTranslucencyType_t n_type, int unk2, int unk3)
{
	if (!p_renderable)
		return oAddRenderable(ecx, p_renderable, unk1, n_type, unk2, unk3);

	auto i_client_unknown = p_renderable->GetIClientUnknown();
	if (!i_client_unknown)
		return oAddRenderable(ecx, p_renderable, unk1, n_type, unk2, unk3);

	auto c_base_entity = i_client_unknown->GetBaseEntity();
	if (!c_base_entity)
		return oAddRenderable(ecx, p_renderable, unk1, n_type, unk2, unk3);

	auto p_entity = (player_t*)c_base_entity;
	if (!p_entity)
		return oAddRenderable(ecx, p_renderable, unk1, n_type, unk2, unk3);

	if (!p_entity->is_player())
		return oAddRenderable(ecx, p_renderable, unk1, n_type, unk2, unk3);

	n_type = RENDERABLE_IS_TRANSLUCENT;
	return oAddRenderable(ecx, p_renderable, unk1, n_type, unk2, unk3);
}



