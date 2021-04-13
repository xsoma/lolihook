#include "hitchams.h"

void hit_chams::add_matrix(player_t* player, matrix3x4_t* bones)
{
    auto& hit = m_Hitmatrix.emplace_back();

    std::memcpy(hit.pBoneToWorld, bones, player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));
    hit.time = m_globals()->m_realtime + 2.5f;

    static int m_nSkin = util::find_in_datamap(player->GetPredDescMap(), crypt_str("m_nSkin"));
    static int m_nBody = util::find_in_datamap(player->GetPredDescMap(), crypt_str("m_nBody"));

    hit.info.origin = player->GetAbsOrigin();
    hit.info.angles = player->GetAbsAngles();

    auto renderable = player->GetClientRenderable();

    if (!renderable)
        return;

    auto model = player->GetModel();

    if (!model)
        return;

    auto hdr = *(studiohdr_t**)(player->m_pStudioHdr());

    if (!hdr)
        return;

    hit.state.m_pStudioHdr = hdr;
    hit.state.m_pStudioHWData = m_modelcache()->GetHardwareData(model->studio);
    hit.state.m_pRenderable = renderable;
    hit.state.m_drawFlags = 0;

    hit.info.pRenderable = renderable;
    hit.info.pModel = model;
    hit.info.pLightingOffset = nullptr;
    hit.info.pLightingOrigin = nullptr;
    hit.info.hitboxset = player->m_nHitboxSet();
    hit.info.skin = (int)(uintptr_t(player) + m_nSkin);
    hit.info.body = (int)(uintptr_t(player) + m_nBody);
    hit.info.entity_index = player->EntIndex();
    hit.info.instance = call_virtual<ModelInstanceHandle_t(__thiscall*)(void*) >(renderable, 30u)(renderable);
    hit.info.flags = 0x1;

    hit.info.pModelToWorld = &hit.model_to_world;
    hit.state.m_pModelToWorld = &hit.model_to_world;

    hit.model_to_world.AngleMatrix(hit.info.angles, hit.info.origin);
}

void hit_chams::draw_hit_matrix()
{
    if (!g_ctx.available())
        m_Hitmatrix.clear();

    if (m_Hitmatrix.empty())
        return;

    if (!m_modelrender())
        return;

    auto ctx = m_materialsystem()->GetRenderContext();

    if (!ctx)
        return;

    auto it = m_Hitmatrix.begin();
    while (it != m_Hitmatrix.end()) {
        if (!it->state.m_pModelToWorld || !it->state.m_pRenderable || !it->state.m_pStudioHdr || !it->state.m_pStudioHWData ||
            !it->info.pRenderable || !it->info.pModelToWorld || !it->info.pModel) {
            ++it;
            continue;
        }

        auto alpha = 1.0f;
        auto delta = m_globals()->m_realtime -it->time;
        if (delta > 0.0f) {
            alpha -= delta;
            if (delta > 1.0f) {
                it = m_Hitmatrix.erase(it);
                continue;
            }
        }

        auto material = m_materialsystem()->FindMaterial(crypt_str("dev/glow_armsrace.vmt"), nullptr);

        auto alpha_c = (float)g_cfg.player.lag_hitbox_color.a() / 255.0f;
        float normal_color[3] =
        {
            g_cfg.player.lag_hitbox_color[0] / 255.0f,
            g_cfg.player.lag_hitbox_color[1] / 255.0f,
            g_cfg.player.lag_hitbox_color[2] / 255.0f
        };

        m_renderview()->SetBlend(alpha_c * alpha);
        util::color_modulate(normal_color, material);

        material->IncrementReferenceCount();
        material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);

        m_modelrender()->ForcedMaterialOverride(material);
        m_modelrender()->DrawModelExecute(ctx, it->state, it->info, it->pBoneToWorld);
        m_modelrender()->ForcedMaterialOverride(nullptr);

        ++it;
    }
}