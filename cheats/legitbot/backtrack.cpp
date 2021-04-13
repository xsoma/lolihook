#include "backtrack.h"
#include "legitbot.h"
#include <algorithm> 


backtrackData headPositions[64][12];
player_bt bt_data[64][12];
#define TICK_INTERVAL			(m_globals()->m_intervalpertick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

inline float distance_point_to_line(Vector Point, Vector LineOrigin, Vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x * Dir.x + Dir.y * Dir.y + Dir.z * Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}

inline Vector angle_vector(Vector meme)
{
	auto sy = sin(meme.y / 180.f * static_cast<float>(3.14159265358979323846f));
	auto cy = cos(meme.y / 180.f * static_cast<float>(3.14159265358979323846f));

	auto sp = sin(meme.x / 180.f * static_cast<float>(3.14159265358979323846f));
	auto cp = cos(meme.x / 180.f * static_cast<float>(3.14159265358979323846f));

	return Vector(cp * cy, cp * sy, -sp);
}

void NewBacktrack::LegitBacktrack(CUserCmd* cmd)
{
	int bestTargetIndex = -1;
	float bestFov = FLT_MAX;
	player_info_t info;

	auto local_id = m_engine()->GetLocalPlayer();
	if (!local_id) return;

	player_t* local = (player_t*)m_entitylist()->GetClientEntity(local_id);
	if (!local) return;

	for (int i = 1; i < m_engine()->GetMaxClients(); i++)
	{
		player_t* entity = (player_t*)m_entitylist()->GetClientEntity(i);

		if (!entity) continue;
		if (entity == local) continue;
		if (!m_engine()->GetPlayerInfo(i, &info)) continue;
		if (entity->IsDormant()) continue;
		if (entity->m_iTeamNum() == local->m_iTeamNum()) continue;
		if (!entity->is_alive()) continue;

		float simtime = entity->m_flSimulationTime();
		Vector hitboxPos = entity->GetHitboxPos(0);
		matrix3x4_t bones[128];
		entity->SetupBones(bones, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, m_globals()->m_curtime);

		for (int idx = 0; idx <= 128; ++idx)
			bt_data[i][cmd->m_command_number % 13].bone[idx] = bones[idx];

		headPositions[i][cmd->m_command_number % 13] = backtrackData{ simtime, hitboxPos };
		Vector ViewDir = angle_vector(cmd->m_viewangles + (local->m_aimPunchAngle() * 2.f));
		float FOVDistance = distance_point_to_line(hitboxPos, local->GetEyePos(), ViewDir);

		if (bestFov > FOVDistance)
		{
			bestFov = FOVDistance;
			bestTargetIndex = i;
		}
	}

	float bestTargetSimTime = 0;
	if (bestTargetIndex != -1)
	{
		float tempFloat = FLT_MAX;
		Vector ViewDir = angle_vector(cmd->m_viewangles + (local->m_aimPunchAngle() * 2.f));
		for (int t = 0; t < g_cfg.legitbot.weapon[hooks::legit_weapon].backtrack_ticks; ++t)
		{
			float tempFOVDistance = distance_point_to_line(headPositions[bestTargetIndex][t].hitboxPos, local->GetEyePos(), ViewDir);
			if (tempFloat > tempFOVDistance && headPositions[bestTargetIndex][t].simtime > local->m_flSimulationTime() - 1)
			{
				tempFloat = tempFOVDistance;
				bestTargetSimTime = headPositions[bestTargetIndex][t].simtime;
			}
		}
	}


	cmd->m_tickcount = TIME_TO_TICKS(bestTargetSimTime);
}

/*
float calculate_lerp() noexcept
{
	static ConVar* cl_interp = m_cvar()->FindVar("cl_interp");
	static ConVar* cl_updaterate = m_cvar()->FindVar("cl_updaterate");
	static ConVar* cl_interp_ratio = m_cvar()->FindVar("cl_interp_ratio");
	static ConVar* sv_maxupdaterate = m_cvar()->FindVar("sv_maxupdaterate");
	static ConVar* sv_minupdaterate = m_cvar()->FindVar("sv_minupdaterate");
	static ConVar* sv_client_min_interp_ratio = m_cvar()->FindVar("sv_client_min_interp_ratio");
	static ConVar* sv_client_max_interp_ratio = m_cvar()->FindVar("sv_client_max_interp_ratio");
	auto Interp = cl_interp->GetFloat();
	auto UpdateRate = cl_updaterate->GetFloat();
	auto InterpRatio = static_cast<float>(cl_interp_ratio->GetInt());
	auto MaxUpdateRate = static_cast<float>(sv_maxupdaterate->GetInt());
	auto MinUpdateRate = static_cast<float>(sv_minupdaterate->GetInt());
	auto ClientMinInterpRatio = sv_client_min_interp_ratio->GetFloat();
	auto ClientMaxInterpRatio = sv_client_max_interp_ratio->GetFloat();
	if (ClientMinInterpRatio > InterpRatio)
		InterpRatio = ClientMinInterpRatio;
	if (InterpRatio > ClientMaxInterpRatio)
		InterpRatio = ClientMaxInterpRatio;
	if (MaxUpdateRate <= UpdateRate)
		UpdateRate = MaxUpdateRate;
	if (MinUpdateRate > UpdateRate)
		UpdateRate = MinUpdateRate;
	auto v20 = InterpRatio / UpdateRate;
	if (v20 <= Interp)
		return Interp;
	else
		return v20;
}

static __forceinline entity_t* GetEntityByIndex(int index) noexcept {
	return static_cast<entity_t*>(m_entitylist()->GetClientEntity(index));
}
static __forceinline player_t* GetPlayerByIndex(int i) noexcept
{
	return static_cast<player_t*>(GetEntityByIndex(i));
}
void CBacktrack::Run(CUserCmd* pCmd) {
	if (!m_engine()->IsInGame() || !g_ctx.local() || !g_ctx.local()->is_alive()) {
		data.clear();
		return;
	}

	auto pWeapon = g_ctx.local()->m_hActiveWeapon();
	if (!pWeapon || !pWeapon->IsGun()) {
		data.clear();
		return;
	}

	auto weaponData = pWeapon->get_csweapon_info();

	static ConVar* sv_maxunlag = m_cvar()->FindVar("sv_maxunlag");
	static ConVar* sv_minupdaterate = m_cvar()->FindVar("sv_minupdaterate");
	static ConVar* sv_maxupdaterate = m_cvar()->FindVar("sv_maxupdaterate");

	static ConVar* sv_client_min_interp_ratio = m_cvar()->FindVar("sv_client_min_interp_ratio");
	static ConVar* sv_client_max_interp_ratio = m_cvar()->FindVar("sv_client_max_interp_ratio");

	static ConVar* cl_interp_ratio = m_cvar()->FindVar("cl_interp_ratio");
	static ConVar* cl_interp = m_cvar()->FindVar("cl_interp");
	static ConVar* cl_updaterate = m_cvar()->FindVar("cl_updaterate");

	float updaterate = cl_updaterate->GetFloat();

	float minupdaterate = sv_minupdaterate->GetFloat();
	float maxupdaterate = sv_maxupdaterate->GetFloat();

	float min_interp = sv_client_min_interp_ratio->GetFloat();
	float max_interp = sv_client_max_interp_ratio->GetFloat();

	float flLerpAmount = cl_interp->GetFloat();
	float flLerpRatio = cl_interp_ratio->GetFloat();
	flLerpRatio = std::clamp(flLerpRatio, min_interp, max_interp);
	if (flLerpRatio == 0.0f)
		flLerpRatio = 1.0f;

	float updateRate = std::clamp(updaterate, minupdaterate, maxupdaterate);
	lerp_time = std::fmaxf(flLerpAmount, flLerpRatio / updateRate);
	latency = m_engine()->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) + m_engine()->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
	correct_time = latency + lerp_time;

	// setup records
	player_t* player;
	for (int i = 1; i <= m_globals()->m_maxclients; ++i) {
		player = GetPlayerByIndex(i);
		if (player->IsNotTarget()) {
			if (data.count(i) > 0)
				data.erase(i);
			continue;
		}

	/*	if (!g_Aimbot.settings.deathmatch && player->m_iTeamNum() == local_player->m_iTeamNum()) {
			if (data.count(i) > 0)
				data.erase(i);
			continue;
		}tutaj

		auto& cur_data = data[i];
		if (!cur_data.empty()) {
			auto& front = cur_data.front();
			if (front.simTime == player->m_flSimulationTime())
				continue;

			while (!cur_data.empty()) {
				auto& back = cur_data.back();
				float deltaTime = correct_time - (m_globals()->m_curtime - back.simTime);
				if (std::fabsf(deltaTime) <= 0.2f)
					break;

				cur_data.pop_back();
			}
		}

		auto model = player->GetModel();
		if (!model)
			continue;

		auto hdr = m_modelinfo()->GetStudioModel(model);
		if (!hdr)
			continue;

		auto hitbox_set = hdr->pHitboxSet(player->m_nHitboxSet());
		auto hitbox_head = hitbox_set->GetHitbox(HITBOX_HEAD);
		auto hitbox_center = (hitbox_head->bbmin + hitbox_head->bbmax) * 0.5f;

		backtrack_data bd;
		bd.simTime = player->m_flSimulationTime();

		*(Vector*)((uintptr_t)player + 0xA0) = player->m_vecOrigin();
		*(int*)((uintptr_t)player + 0xA68) = 0;
		*(int*)((uintptr_t)player + 0xA30) = 0;
		player->invalidate_bone_cache();
		player->SetupBones(bd.boneMatrix, 128, BONE_USED_BY_ANYTHING, m_globals()->m_curtime);

		math::vector_transform(hitbox_center, bd.boneMatrix[hitbox_head->bone], bd.hitboxPos);

		data[i].push_front(bd);
	}

	Vector localEyePos = g_ctx.local()->GetEyePos();
	Vector angles;
	int tick_count = -1;
	float best_fov = 180.0f;
	for (auto& node : data) {
		auto& cur_data = node.second;
		if (cur_data.empty())
			continue;

		for (auto& bd : cur_data) {
			float deltaTime = correct_time - (m_globals()->m_curtime - bd.simTime);
			if (std::fabsf(deltaTime) > g_cfg.legitbot.weapon[g_ctx.globals.current_weapon].backtrack_ticks)
				continue;

			math::vector_angles(bd.hitboxPos - localEyePos, angles);
			math::clamp_angles(angles);
			float fov = legit_bot::get().static_fov_to(pCmd->m_viewangles, angles);
			if (best_fov > fov) {
				best_fov = fov;
				tick_count = TIME_TO_TICKS(bd.simTime + lerp_time);
			}
		}
	}

	if (tick_count != -1) {
		pCmd->m_tickcount = tick_count;
	}
}*/