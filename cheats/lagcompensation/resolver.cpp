// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "animation_system.h"
#include "..\ragebot\aim.h"
#include <algorithm>

void resolver::initialize(player_t* e, adjust_data* record, const float& pitch)
{
	player = e;
	player_record = record;

	original_pitch = math::normalize_pitch(pitch);
}

void resolver::reset()
{
	player = nullptr;
	player_record = nullptr;

	original_pitch = 0.0f;
}

float fast_float_normalize(float angle_1, float angle_2)
{
	float difference; // xmm1_4

	for (; angle_1 > 180.0f; angle_1 = angle_1 - 360.0f)
		;
	for (; angle_1 < -180.0f; angle_1 = angle_1 + 360.0f)
		;
	for (; angle_2 > 180.0f; angle_2 = angle_2 - 360.0f)
		;
	for (; angle_2 < -180.0f; angle_2 = angle_2 + 360.0f)
		;
	for (difference = (angle_2 - angle_1); difference > 180.0f; difference = (difference - 360.0f))
		;
	for (; difference < -180.0f; (difference = difference + 360.0f))
		;
	return difference;
}

void resolver::ResolveAngles()
{
	player_info_t player_info;
	resolver* record;

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;

	if (player_info.fakeplayer || !g_ctx.local()->is_alive() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum() || record->anim_layers[6].m_flPlaybackRate == 0.0f)
		return;

	// set move distance to layers.
	float move_distance = player->m_vecVelocity().Length2D() / (record->anim_layers[6].m_flPlaybackRate / m_globals()->m_intervalpertick);

	// get max delta.
	int max_delta = INT_MAX;

	// best delta.
	float best_delta = FLT_MAX;

	// best move.
	float best_move = FLT_MAX;

	// get this move.
	auto move_delta = (best_move / best_delta); //>= abs(player->m_vecVelocity().Length2D() < 0.1f) /*<= m_globals()->m_intervalpertick*/;

	// desync side to using from resolver.
	int desync_side;

	// set real time.
	float delta_time = m_globals()->m_realtime;

	// activity for layers.
	const auto activity = player->sequence_activity(record->anim_layers[3].m_nSequence);

	// set fix angles.
	if (player->m_fFlags() & FL_ONGROUND)
	{
		if (player->m_vecVelocity().Length2D())
		{
			// set layers to standing desync.
			if (record->anim_layers[3].m_flWeight == 0.0f && record->anim_layers[3].m_flCycle == 0.0f && record->anim_layers[6].m_flWeight == 0.0f && player->m_fFlags() & FL_ONGROUND)
			{
				// delta to difference from goal feet yaw im do like.
				auto delta_difference = fast_float_normalize(player->m_angEyeAngles().y, player->get_animation_state()->m_flGoalFeetYaw);

				desync_side = 58 * (delta_difference) > 0.0f ? 58 : -58; // 2 sides 1 and -1 im do using 58 to from the complate two side.
			}
			else if (!player->get_animation_state()->m_flTimeSinceStartedMoving < 0.22f && record->anim_layers[12].m_flWeight * 1000.0f && int(record->anim_layers[6].m_flWeight * 1000.0f == int(record->anim_layers[6].m_flWeight * 1000.0f)))
			{
				const auto eye = fabsf(record->anim_layers[6].m_flPlaybackRate - record->resolver_anim_layer[0][6].m_flPlaybackRate);
				const auto positive = fabsf(record->anim_layers[6].m_flPlaybackRate - record->resolver_anim_layer[2][6].m_flPlaybackRate);
				const auto negative = fabsf(record->anim_layers[6].m_flPlaybackRate - record->resolver_anim_layer[1][6].m_flPlaybackRate);

				// break animations.
				if (activity == 979)
				{
					// fix angles.
					if (eye < positive || negative <= positive || (positive * 1000.0f))
					{
						if (eye >= negative && negative > positive && !(negative * 1000.0f))
						{
							//if (freestand_side[player->EntIndex()] == 1)
							//{
							if (is_slow_walk(player))
							{
								if (desync_side == negative)
									(move_distance < move_delta); //&& fabs(delta_time != player->get_animation_state()->get_body_yaw_modifier());
							}
							//}
						}
						else
						{
							//if (freestand_side[player->EntIndex()] == -1)
							//{
							if (is_slow_walk(player))
							{
								if (desync_side == positive)
									(move_delta > move_distance); //&& fabs(delta_time != player->get_animation_state()->get_body_yaw_modifier());
							}
							//}
						}
					}
				}
			}
			/*else
				goto antifreestand_part;*/
		}
		/*else
		{
		antifreestand_part:
			if (m_globals()->m_curtime - delta_time > 2.0f)
			{
				auto first_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player, g_ctx.local());
				auto second_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player, g_ctx.local());
				auto center_visible = util::visible(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.zero), player, g_ctx.local());

				if (first_visible != second_visible && center_visible != first_visible)
				{
					desync_side = second_visible;
					delta_time = m_globals()->m_curtime;
				}
				else
				{
					auto fire_data_first = c_autowall::get().wall_penetration(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.first), player);
					auto fire_data_second = c_autowall::get().wall_penetration(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.second), player);
					auto fire_data_center = c_autowall::get().wall_penetration(g_ctx.globals.eye_pos, player->hitbox_position_matrix(HITBOX_HEAD, player_record->matrixes_data.zero), player);

					if (fabs(fire_data_first.damage - fire_data_second.damage) > 10); //&& (fire_data_center.visible - fabs(fire_data_second.damage) > 100); // distance.
						desync_side = fire_data_first.damage > fire_data_second.damage && fabs(fire_data_center.visible > fire_data_first.visible) > move_delta; //&& fire_data_first.visible > fire_data_center.visible;
				}
			}
		}*/
	}

	player->get_animation_state()->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 58.f * desync_side/* freestand_side[player->EntIndex()]*/);
}

bool resolver::is_slow_walk(player_t* entity)
{
	float velocity_2D[64], old_velocity_2D[64];

	if (entity->m_vecVelocity().Length2D() != velocity_2D[entity->EntIndex()] && entity->m_vecVelocity().Length2D() != NULL) {
		old_velocity_2D[entity->EntIndex()] = velocity_2D[entity->EntIndex()];
		velocity_2D[entity->EntIndex()] = entity->m_vecVelocity().Length2D();
	}

	if (velocity_2D[entity->EntIndex()] > 0.1)
	{
		int tick_counter[64];

		if (velocity_2D[entity->EntIndex()] == old_velocity_2D[entity->EntIndex()])
			++tick_counter[entity->EntIndex()];
		else
			tick_counter[entity->EntIndex()] = 0;

		while (tick_counter[entity->EntIndex()] > (1 / m_globals()->m_intervalpertick) * fabsf(0.1f))
			return true;

	}
	return false;
}

void resolver::AntiFreestand()
{
	/*if (!g_ctx.local()->is_alive())
		return;

	for (int i = 1; i < m_globals()->m_maxclients; ++i)
	{
		auto get_player = m_entitylist()->GetClientEntity(i);

		auto player = (player_t*)(get_player);

		if (!player || !player->is_alive() || player->IsDormant() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
			continue;
	}*/

	/*if (!g_ctx.local()->is_alive())
		return;

	//static int freestand_side[64];

	for (int i = 1; i < m_globals()->m_maxclients; ++i)
	{
		auto player1 = m_entitylist()->GetClientEntity(i);

		auto player = (player_t*)(player1);


		if (!player || !player->is_alive() || player->IsDormant() || player->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
			continue;

		bool Autowalled = false, HitSide1 = false, HitSide2 = false;
		auto idx = player->EntIndex();
		float angToLocal = math::calculate_angle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).y;
		Vector ViewPoint = g_ctx.local()->m_vecOrigin() + Vector(0, 0, 90);
		Vector2D Side1 = { (45 * sin(DEG2RAD(angToLocal))),(45 * cos(DEG2RAD(angToLocal))) };
		Vector2D Side2 = { (45 * sin(DEG2RAD(angToLocal + 180))) ,(45 * cos(DEG2RAD(angToLocal + 180))) };

		Vector2D Side3 = { (50 * sin(DEG2RAD(angToLocal))),(50 * cos(DEG2RAD(angToLocal))) };
		Vector2D Side4 = { (50 * sin(DEG2RAD(angToLocal + 180))) ,(50 * cos(DEG2RAD(angToLocal + 180))) };

		Vector Origin = player->m_vecOrigin();

		Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

		Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

		for (int side = 0; side < 2; side++)
		{
			Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 90 };
			Vector ViewPointAutowall = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

			if (c_autowall::get().can_hit(OriginAutowall, ViewPoint))
			{
				if (side == 0)
				{
					HitSide1 = true;
					freestand_side[idx] = -1;
				}
				else if (side == 1)
				{
					HitSide2 = true;
					freestand_side[idx] = 1;
				}

				Autowalled = true;
			}
			else
			{
				for (int sidealternative = 0; sidealternative < 2; sidealternative++)
				{
					Vector ViewPointAutowallalternative = { Origin.x + OriginLeftRight[sidealternative].x,  Origin.y - OriginLeftRight[sidealternative].y , Origin.z + 90 };

					if (c_autowall::get().can_hit(ViewPointAutowallalternative, ViewPointAutowall))
					{
						if (sidealternative == 0)
						{
							HitSide1 = true;
							freestand_side[idx] = -1;
						}
						else if (sidealternative == 1)
						{
							HitSide2 = true;
							freestand_side[idx] = 1;
						}

						Autowalled = true;
					}
				}
			}
		}
	}

	/*if (freestand_side[player->ent_index()] == 1)
		animstate->m_foot_yaw = math::normalize_yaw(player->m_eye_angles().y - 58.0f);
	else
		animstate->m_foot_yaw = math::normalize_yaw(player->m_eye_angles().y - 58.0f);*/
}

float resolver::resolve_pitch()
{
	return original_pitch;
}

float GRD_TO_BOG(float GRD) {
	return (M_PI / 180) * GRD;
}

float angle_mod(float flAngle)
{
	return((360.0f / 65536.0f) * ((int32_t)(flAngle * (65536.0f / 360.0f)) & 65535));
}

float approach_angle(float flTarget, float flValue, float flSpeed) {
	float flAdjustedSpeed = flSpeed;
	if (flAdjustedSpeed < 0.0f)
		flAdjustedSpeed *= -1.0f;

	float flAngleModTarget = angle_mod(flTarget);
	float flAngleModValue = angle_mod(flValue);

	float flDelta = (flAngleModTarget - flAngleModValue);
	if (flDelta >= -180.0f) {
		if (flDelta >= 180.0f)
			flDelta -= 360.0f;
	}
	else {
		if (flDelta <= -180.0f)
			flDelta += 360.0f;
	}

	float flReturn;

	if (flDelta <= flAdjustedSpeed)
	{
		if ((flAdjustedSpeed * -1.0f) <= flDelta)
			flReturn = flAngleModTarget;
		else
			flReturn = (flAngleModValue - flAdjustedSpeed);
	}
	else
		flReturn = (flAngleModValue + flAdjustedSpeed);

	return flReturn;
}

void resolver::resolve() {
	if (!g_ctx.local())
		return;

	for (int i = 1; i <= m_globals()->m_maxclients; i++) {

		player_t* player = reinterpret_cast<player_t*>(m_entitylist()->GetClientEntity(i));

		if (!player || !player->is_alive() || player == g_ctx.local() || player->IsDormant())
			continue;

		if (player->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
			continue;

		/* run onshot calculation */
		static float last_shot_time[64];

		/*C_BaseCombatWeapon* enemy_weapon = player->GetActiveWeapon( );
		if ( enemy_weapon ) {
			if ( last_shot_time[ player->Networkable( )->EntIndex( ) ] < enemy_weapon->m_fLastShotTime( ) ) {
				Global::fired_shot[ player->Networkable( )->EntIndex( ) ] = true;
				last_shot_time[ player->Networkable( )->EntIndex( ) ] = enemy_weapon->m_fLastShotTime( ) + 2;
				std::cout << last_shot_time[ player->Networkable( )->EntIndex( ) ] << enemy_weapon->m_fLastShotTime( ) << std::endl;
			}
			else {
				Global::fired_shot[ player->Networkable( )->EntIndex( ) ] = false;
			}
		}
		else {
			Global::fired_shot[ player->Networkable( )->EntIndex( ) ] = false;
			last_shot_time[ player->Networkable( )->EntIndex( ) ] = 0.f;
		}*/

		c_baseplayeranimationstate* state = player->get_animation_state();
		if (!state)
			continue;

		info.use_freestand_angle[i] = false;
		//Global::resolve_mode[player->Networkable()->EntIndex()] = "";

		//if (Controls.resolver_active) {
			bruteforce_resolve(player);
		//}
	}
}

void resolver::anti_freestanding(player_t* player) {
	bool
		ran_autowall = false,
		can_hit_right = false,
		can_hit_left = false;

	if (!g_ctx.local())
		return;

	float ang_to_local = math::CalcAngle(g_ctx.local()->m_vecOrigin(), player->m_vecOrigin()).yaw;
	Vector view_point = g_ctx.local()->m_vecOrigin() + Vector(0, 0, 90);
	Vector origin = player->m_vecOrigin();

	Vector2D origin_left_right[] = {
		Vector2D((45 * sin(GRD_TO_BOG(ang_to_local))),(45 * cos(GRD_TO_BOG(ang_to_local)))),
		Vector2D((45 * sin(GRD_TO_BOG(ang_to_local + 180))) ,(45 * cos(GRD_TO_BOG(ang_to_local + 180))))
	};

	Vector2D origin_left_right_local[] = {
		Vector2D((50 * sin(GRD_TO_BOG(ang_to_local))),(50 * cos(GRD_TO_BOG(ang_to_local)))),
		Vector2D((50 * sin(GRD_TO_BOG(ang_to_local + 180))) ,(50 * cos(GRD_TO_BOG(ang_to_local + 180))))
	};

	for (int side = 0; side < 2; side++) {
		Vector origin_autowall = {
			origin.x + origin_left_right[side].x,
			origin.y - origin_left_right[side].y ,
			origin.z + 80
		};
		Vector origin_autowall_local = {
			view_point.x + origin_left_right_local[side].x,
			view_point.y - origin_left_right_local[side].y ,

			view_point.z
		};

		if (autowall::get().can_hit(origin_autowall, view_point)) {
			if (side == 0) {
				can_hit_right = true;
				info.freestand_angle[player->Networkable()->EntIndex()] = 58;
			}
			else if (side == 1) {
				can_hit_left = true;
				info.freestand_angle[player->Networkable()->EntIndex()] = -58;
			}
			ran_autowall = true;
		}
		else {
			for (int bside = 0; bside < 2; bside++) {
				Vector origin_autowall_sides = {
					origin.x + origin_left_right[bside].x,
					origin.y - origin_left_right[bside].y ,
					origin.z + 80
				};

				if (autowall::get().can_hit(origin_autowall_sides, origin_autowall_local)) {
					if (bside == 0) {
						if (bside == 0) {
							can_hit_right = true;
							info.freestand_angle[player->Networkable()->EntIndex()] = 58;
						}
						else if (bside == 1) {
							can_hit_left = true;
							info.freestand_angle[player->Networkable()->EntIndex()] = -58;
						}
						ran_autowall = true;
					}
				}
			}
		}
		if (ran_autowall) {
			if (can_hit_right && can_hit_left) {
				info.use_freestand_angle[player->Networkable()->EntIndex()] = false;
			}
			else {
				info.use_freestand_angle[player->Networkable()->EntIndex()] = true;
				info.last_freestanding_angle[player->Networkable()->EntIndex()] = info.freestand_angle[player->Networkable()->EntIndex()];
			}
		}
	}
	if (info.use_freestand_angle[player->Networkable()->EntIndex()])
		info.last_freestanding_angle[player->Networkable()->EntIndex()] += math::normalize_yaw(ang_to_local);
}
void resolver::bruteforce_resolve(player_t* player) {
	auto state = player->get_animation_state();
	if (!state)
		return;

	player->update_clientside_animation();

	if (player_record->shot && fabs(original_pitch) < 82.f)
		return;

	auto& resolver_backup_info = info.resolver_data[player->Networkable()->EntIndex()];

	/* normalize velocity, will go further later */
	Vector velocity = player->m_vecVelocity();
	float spd = velocity.LengthSqr();
	if (spd > std::powf(1.2f * 260.0f, 2.f)) {
		Vector velocity_normalized = velocity.Normalized();
		velocity = velocity_normalized * (1.2f * 260.0f);
	}

	float duck_amnt_anim = std::clamp(player->m_flDuckAmount() + state->m_fLandingDuckAdditiveSomething, 0.0f, 1.0f);
	float max_eye = resolver_backup_info * 6.0f;
	float real_duck;

	// clamp angles
	if ((duck_amnt_anim - state->m_fDuckAmount) <= max_eye) {
		if (-max_eye <= (duck_amnt_anim - state->m_fDuckAmount))
			real_duck = duck_amnt_anim;
		else
			real_duck = state->m_fDuckAmount - max_eye;
	}
	else {
		real_duck = state->m_fDuckAmount + max_eye;
	}

	/* return velocity smoothed to a usable number */
	static auto get_smoothed_velocity = [](float min_delta, Vector a, Vector b) {
		Vector delta = a - b;
		float delta_length = delta.Length();

		if (delta_length <= min_delta) {
			Vector result;
			if (-min_delta <= delta_length) {
				return a;
			}
			else {
				float iradius = 1.0f / (delta_length + FLT_EPSILON);
				return b - ((delta * iradius) * min_delta);
			}
		}
		else {
			float iradius = 1.0f / (delta_length + FLT_EPSILON);
			return b + ((delta * iradius) * min_delta);
		}
	};

	float duck_amnt = std::clamp(real_duck, 0.0f, 1.0f);

	/* minimum delta between pre-smoothed and new, smoothed velocity, as to limit errors*/
	Vector animation_velocity = get_smoothed_velocity(info.choked_time * 2000.0f, velocity, Vector(state->m_vVelocityX, state->m_vVelocityY, state->flUpVelocity));

	/* max run speed 250, bhop around 315, average*/
	float speed = std::fminf(animation_velocity.Length(), 260.0f);

	/* GET WEAPONS NIGGA */
	weapon_t* weapon = player->m_hActiveWeapon().Get();
	if (!weapon) return;

	weapon_info_t* WeaponData = weapon->get_csweapon_info();
	if (!WeaponData) return;

	float max_movement_speed = 260.0f;
	if (weapon) {
		/* a kind of clamp, prevents outliers*/
		max_movement_speed = std::fmaxf(WeaponData->flMaxPlayerSpeed, 0.001f);
	}

	/* CLASSIC MAGIC NUMBERS*/
	float running_speed = speed / (max_movement_speed * 0.520f);
	float ducking_speed = speed / (max_movement_speed * 0.340f);

	running_speed = std::clamp(running_speed, 0.0f, 1.0f);

	/* Unknowncheats magic code */
	float yaw_modifier = (((state->m_flStopToFullRunningFraction * -0.3f) - 0.2f) * running_speed) + 1.0f;
	if (duck_amnt > 0.0f) {
		float ducking_speed = std::clamp(ducking_speed, 0.0f, 1.0f);
		yaw_modifier += (duck_amnt * ducking_speed) * (0.5f - yaw_modifier);
	}

	/* max right and max left yaw*/
	float min_body_yaw = std::fabsf(state->m_min_yaw() * yaw_modifier);
	float max_body_yaw = std::fabsf(state->m_max_yaw() * yaw_modifier);

	/* gets delta of real yaw and our supposed "resolved" yaw */
	float eye_yaw = player->m_angEyeAngles().y;
	float eye_yaw_diff = std::remainderf(eye_yaw - info.fake_goal_feet_yaw, 360.f);

	/* Unknowncheats magic code, also in SetupVelocity */
	if (eye_yaw_diff <= max_body_yaw) {
		if (min_body_yaw > eye_yaw_diff)
			info.fake_goal_feet_yaw = fabs(min_body_yaw) + eye_yaw;
	}
	else
		info.fake_goal_feet_yaw = eye_yaw - fabs(max_body_yaw);

	info.fake_goal_feet_yaw = std::remainderf(info.fake_goal_feet_yaw, 360.f);

	if (speed > 0.1f || fabs(velocity.z) > 100.0f)
		info.fake_goal_feet_yaw = approach_angle(eye_yaw, info.fake_goal_feet_yaw, ((state->m_flStopToFullRunningFraction * 20.0f) + 30.0f) * info.choked_time);
	else
		info.fake_goal_feet_yaw = approach_angle(player->m_flLowerBodyYawTarget(), info.fake_goal_feet_yaw, info.fake_goal_feet_yaw * 100.0f);

	/* desync either left or right */
	float left = state->m_flEyeYaw + min_body_yaw;
	float right = state->m_flEyeYaw + max_body_yaw;

	float resolve_yaw = 0.f;

	switch (g_ctx.globals.missed_shots[player->Networkable()->EntIndex()] % 3) {
	case 0: /* brute calc'd real */
		player_record->resolveValue = info.fake_goal_feet_yaw - eye_yaw;
		resolve_yaw = info.fake_goal_feet_yaw;
		//Global::resolve_mode[player->Networkable()->EntIndex()] = _("r: smart 1");
		break;
	case 1: /* brute left */
		player_record->resolveValue = min_body_yaw;
		resolve_yaw = left;
		//Global::resolve_mode[player->Networkable()->EntIndex()] = _("r: smart 2");
		break;
	case 2: /* brute right side */
		player_record->resolveValue = max_body_yaw;
		resolve_yaw = right;
		//Global::resolve_mode[player->Networkable()->EntIndex()] = _("r: smart 3");
		break;
	}

	state->m_flGoalFeetYaw = resolve_yaw;
}

/*int last_ticks[65];
int resolver::GetChokedPackets(player_t* entity)
{
    auto ticks = TIME_TO_TICKS(entity->m_flSimulationTime() - entity->m_flOldSimulationTime());
    if (ticks == 0 && last_ticks[entity->EntIndex()] > 0)
    {
        return last_ticks[entity->EntIndex()] - 1;
    }
    else
    {
        last_ticks[entity->EntIndex()] = ticks;
        return ticks;
    }
}

bool resolver::has_fake(player_t* entity, adjust_data* record)
{
    //auto player_lag_record = &cheat::features::lagcomp.records[index];
    //auto player_lag_record = g_lagcompensation.GetValidRecords(entity);

    //if (player_lag_record->m_Tickrecords.size() < 2)
    //	return true;

    float simtime = entity->m_flSimulationTime();
    float oldsimtime = entity->m_flOldSimulationTime();
    float simdiff = simtime - oldsimtime;

    auto chokedpackets = TIME_TO_TICKS(max(0.f, simdiff)); // you could also include latency

    if (chokedpackets < 1) // player not desyncing
        return false;

    if (fabs(entity->m_flSimulationTime() - entity->m_flOldSimulationTime()) == m_globals()->m_intervalpertick)
        return false;

    if (!entity)
        return false;

    if (GetChokedPackets(entity) < 1)
        return false;

    return true;
}

int last_resolving_side[65];

bool IsFakePlaja(int index) {
    player_info_t info;
    if (m_engine()->GetPlayerInfo(index, &info))
        return info.fakeplayer;

    return false;
}

float NormalizeYaw(float angle) {
    if (!std::isfinite(angle))
        angle = 0.f;

    return std::remainderf(angle, 360.0f);
}

void resolver::ResolveAngles(player_t* player, adjust_data* record) {
    auto& pdata = inform[player->EntIndex() - 1];

    c_baseplayeranimationstate animstate_backup;

    animstate_backup = *player->get_animation_state();

    if (has_fake(player, record) == false)
        return;

    //store_delta(player, &pdata, record);

    *player->get_animation_state() = animstate_backup;

    if (!record->shot) {

        // we arrived here we can do the acutal resolve.
        //if (record->m_mode == Modes::RESOLVE_STAND)

       if (IsFakePlaja(record->player->EntIndex())) // fix resolving botz
            //ResolveStand(data, record);
        {
            auto speed = player->m_vecVelocity().Length2D();
            if (speed < 0.1f)
            {
                auto layers = &player->m_AnimOverlay()[6];
                if (layers->m_flCycle > 0.9f && layers->m_flWeight > 0.9f)
                {
                    auto lby_delta = player->m_flLowerBodyYawTarget() - player->m_angEyeAngles().y;
                    auto absed_lby_delta = std::abs(NormalizeYaw(lby_delta));
                    if (absed_lby_delta > 30.f)
                    {
                        if (lby_delta > 0.f)
                        {
                            player->get_animation_state()->m_flGoalFeetYaw = NormalizeYaw(player->m_angEyeAngles().y + 60.f);
                            last_resolving_side[player->EntIndex()] = 1;
                        }
                        else if (lby_delta < 0.f)
                        {
                            player->get_animation_state()->m_flGoalFeetYaw = NormalizeYaw(player->m_angEyeAngles().y - 60.f);
                            last_resolving_side[player->EntIndex()] = -1;
                        }
                    }
                }
                else
                {
                    if (g_ctx.globals.missed_shots[player->EntIndex()] == 0)
                        player->get_animation_state()->m_flGoalFeetYaw = NormalizeYaw(player->m_angEyeAngles().y + 60.f * last_resolving_side[player->EntIndex()]);
                    else
                    {
                        switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2)
                        {
                        case 0:
                            player->get_animation_state()->m_flGoalFeetYaw = NormalizeYaw(player->m_angEyeAngles().y - 60.f * last_resolving_side[player->EntIndex()]);
                            break;
                        case 1:
                            player->get_animation_state()->m_flGoalFeetYaw = NormalizeYaw(player->m_angEyeAngles().y + 60.f * last_resolving_side[player->EntIndex()]);
                            break;
                        }
                    }
                }
            }
            else
            {
                if (g_ctx.globals.missed_shots[player->EntIndex()] == 0)
                {
                    float delta1 = abs(record->layers[3].m_flPlaybackRate - pdata.resolver_anim_layers[0][3].m_flPlaybackRate);
                    float delta2 = abs(record->layers[3].m_flPlaybackRate - pdata.resolver_anim_layers[1][3].m_flPlaybackRate);
                    float delta3 = abs(record->layers[3].m_flPlaybackRate - pdata.resolver_anim_layers[2][3].m_flPlaybackRate);

                    if (delta1 < delta3 || delta2 <= delta3 || (int)(delta3 * 1000.f))
                    {
                        if (delta1 >= delta2 && delta3 > delta2 && !(int)(delta2 * 1000.f))
                        {
                            last_resolving_side[player->EntIndex()] = -1;
                            player->get_animation_state()->m_flGoalFeetYaw = NormalizeYaw(player->m_angEyeAngles().y - 60.f);
                        }
                    }
                    else
                    {
                        last_resolving_side[player->EntIndex()] = 1;
                        player->get_animation_state()->m_flGoalFeetYaw = NormalizeYaw(player->m_angEyeAngles().y + 60.f);
                    }
                }
                else
                {
                    switch (g_ctx.globals.missed_shots[player->EntIndex()] % 2)
                    {
                    case 0:
                        player->get_animation_state()->m_flGoalFeetYaw = NormalizeYaw(player->m_angEyeAngles().y - 60.f * last_resolving_side[player->EntIndex()]);
                        break;
                    case 1:
                        player->get_animation_state()->m_flGoalFeetYaw = NormalizeYaw(player->m_angEyeAngles().y + 60.f * last_resolving_side[player->EntIndex()]);
                        break;
                    }
                }
            }
        }
    }
}*/