// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "aim.h"
#include "..\misc\misc.h"
#include "..\misc\logs.h"
#include "..\autowall\autowall.h"
#include "..\misc\prediction_system.h"
#include "..\fakewalk\slowwalk.h"
#include "..\lagcompensation\local_animations.h"

void aim::run(CUserCmd* cmd)
{
	backup.clear();
	targets.clear();
	scanned_targets.clear();
	final_target.reset();
	should_stop = false;

	if (!g_cfg.ragebot.enable)
		return;

	automatic_revolver(cmd);
	prepare_targets();

	if (g_ctx.globals.weapon->is_non_aim())
		return;

	if (g_ctx.globals.current_weapon == -1)
		return;

	scan_targets();

	if (!should_stop && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autostop_modifiers[AUTOSTOP_PREDICTIVE])
	{
		for (auto& target : targets)
		{
			if (!target.last_record->valid())
				continue;

			scan_data last_data;

			target.last_record->adjust_player();
			scan(target.last_record, last_data, g_ctx.globals.eye_pos);

			if (!last_data.valid())
				continue;

			should_stop = true;
			break;
		}
	}

	//if (automatic_stop(cmd))
		//find_best_target();

	//if (scanned_targets.empty())
		//find_best_target();

//	if (final_target.data.valid())
		//fire(cmd);

	if (!automatic_stop(cmd))
		return;

	if (scanned_targets.empty())
		return;

	find_best_target();

	if (!final_target.data.valid())
		return;

	fire(cmd);
}

void aim::automatic_revolver(CUserCmd* cmd)
{
	if (!m_engine()->IsActiveApp())
		return;

	if (g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
		return;

	if (cmd->m_buttons & IN_ATTACK)
		return;

	cmd->m_buttons &= ~IN_ATTACK2;

	static auto r8cock_time = 0.0f;
	auto server_time = TICKS_TO_TIME(g_ctx.globals.backup_tickbase);

	if (g_ctx.globals.weapon->can_fire(false))
	{
		if (r8cock_time <= server_time)
		{
			if (g_ctx.globals.weapon->m_flNextSecondaryAttack() <= server_time)
				r8cock_time = server_time + TICKS_TO_TIME(13);
			else
				cmd->m_buttons |= IN_ATTACK2;
		}
		else
			cmd->m_buttons |= IN_ATTACK;
	}
	else
	{
		r8cock_time = server_time + TICKS_TO_TIME(13);
		cmd->m_buttons &= ~IN_ATTACK;
	}

	g_ctx.globals.revolver_working = true;
}

void aim::prepare_targets()
{
	for (auto i = 1; i < m_globals()->m_maxclients; i++)
	{
		if (g_cfg.player_list.white_list[i])
			continue;

		auto e = (player_t*)m_entitylist()->GetClientEntity(i);

		if (!e->valid(true, false))
			continue;

		auto records = &player_records[i]; //-V826

		if (records->empty())
			continue;

		targets.emplace_back(target(e, get_record(records, false), get_record(records, true)));
	}

	if (targets.size() >= 4)
	{
		auto first = rand() % targets.size();
		auto second = rand() % targets.size();
		auto third = rand() % targets.size();

		for (auto i = 0; i < targets.size(); ++i)
		{
			if (i == first || i == second || i == third)
				continue;

			targets.erase(targets.begin() + i);

			if (i > 0)
				--i;
		}
	}

	for (auto& target : targets)
		backup.emplace_back(adjust_data(target.e));
}

static bool compare_records(const optimized_adjust_data& first, const optimized_adjust_data& second)
{
	auto first_pitch = math::normalize_pitch(first.angles.x);
	auto second_pitch = math::normalize_pitch(second.angles.x);

	if (fabs(first_pitch - second_pitch) > 15.0f)
		return fabs(first_pitch) < fabs(second_pitch);
	else if (first.shot != second.shot)
		return first.shot;
	else if (first.duck_amount != second.duck_amount) //-V550
		return first.duck_amount < second.duck_amount;
	else if (first.origin != second.origin)
		return first.origin.DistTo(g_ctx.local()->GetAbsOrigin()) < second.origin.DistTo(g_ctx.local()->GetAbsOrigin());

	return first.simulation_time > second.simulation_time;
}

adjust_data* aim::get_record(std::deque <adjust_data>* records, bool history)
{
	if (history)
	{
		std::deque <optimized_adjust_data> optimized_records; //-V826

		for (auto i = 0; i < records->size(); ++i)
		{
			auto record = &records->at(i);
			optimized_adjust_data optimized_record;

			optimized_record.i = i;
			optimized_record.player = record->player;
			optimized_record.simulation_time = record->simulation_time;
			optimized_record.duck_amount = record->duck_amount;
			optimized_record.angles = record->angles;
			optimized_record.origin = record->origin;
			optimized_record.shot = record->shot;

			optimized_records.emplace_back(optimized_record);
		}

		if (optimized_records.size() < 2)
			return nullptr;

		std::sort(optimized_records.begin(), optimized_records.end(), compare_records);

		for (auto& optimized_record : optimized_records)
		{
			auto record = &records->at(optimized_record.i);

			if (!record->valid())
				continue;

			return record;
		}
	}
	else
	{
		for (auto i = 0; i < records->size(); ++i)
		{
			auto record = &records->at(i);

			if (!record->valid())
				continue;

			return record;
		}
	}

	return nullptr;
}

int aim::get_minimum_damage(bool visible, int health)
{
	auto minimum_damage = 0;
	//minimum_damage = min(weaponData->iDamage / 3, health / 4);
	auto weaponData = g_ctx.globals.weapon->get_csweapon_info();
	if (visible)
	{
		if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_visible_damage == 0)
			minimum_damage = min(weaponData->iDamage / 3, health / 4);
		else if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_visible_damage > 100)
			minimum_damage = health + g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_visible_damage - 100;
		else
			minimum_damage = math::clamp(g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_visible_damage, 1, health);
	}
	else
	{
		if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_damage == 0)
			minimum_damage = min(weaponData->iDamage / 3, health / 4);
		else if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_damage > 100)
			minimum_damage = health + g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_damage - 100;
		else
			minimum_damage = math::clamp(g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_damage, 1, health);
	}

	if (key_binds::get().get_key_bind_state(4 + g_ctx.globals.current_weapon))
	{
		if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_override_damage > 100)
			minimum_damage = health + g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_override_damage - 100;
		else
			minimum_damage = math::clamp(g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_override_damage, 1, health);
	}

	return minimum_damage;
}

void aim::scan_targets()
{
	if (targets.empty())
		return;

	for (auto& target : targets)
	{
		if (target.history_record->valid())
		{
			scan_data last_data;

			if (target.last_record->valid())
			{
				target.last_record->adjust_player();
				scan(target.last_record, last_data);
			}

			scan_data history_data;

			target.history_record->adjust_player();
			scan(target.history_record, history_data);

			if (last_data.valid() && last_data.damage > history_data.damage)
				scanned_targets.emplace_back(scanned_target(target.last_record, last_data));
			else if (history_data.valid())
				scanned_targets.emplace_back(scanned_target(target.history_record, history_data));
		}
		else
		{
			if (!target.last_record->valid())
				continue;

			scan_data last_data;

			target.last_record->adjust_player();
			scan(target.last_record, last_data);

			if (!last_data.valid())
				continue;

			scanned_targets.emplace_back(scanned_target(target.last_record, last_data));
		}
	}
}

bool aim::automatic_stop(CUserCmd* cmd)
{
	if (!should_stop)
		return true;

	if (!g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autostop)
		return true;

	if (g_ctx.globals.slowwalking)
		return true;

	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
		return true;

	if (g_ctx.globals.weapon->is_empty())
		return true;

	if (!g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autostop_modifiers[AUTOSTOP_BETWEEN_SHOTS] && !g_ctx.globals.weapon->can_fire(false))
		return true;

	auto animlayer = g_ctx.local()->get_animlayers()[1];

	if (animlayer.m_nSequence)
	{
		auto activity = g_ctx.local()->sequence_activity(animlayer.m_nSequence);

		if (activity == ACT_CSGO_RELOAD && animlayer.m_flWeight > 0.0f)
			return true;
	}

	auto weapon_info = g_ctx.globals.weapon->get_csweapon_info();

	if (!weapon_info)
		return true;

	auto max_speed = 0.33f * (g_ctx.globals.scoped ? weapon_info->flMaxPlayerSpeedAlt : weapon_info->flMaxPlayerSpeed);

	if (engineprediction::get().backup_data.velocity.Length2D() < max_speed)
		slowwalk::get().create_move(cmd);
	else
	{
		Vector direction;
		Vector real_view;

		math::vector_angles(engineprediction::get().backup_data.velocity, direction);
		m_engine()->GetViewAngles(real_view);

		direction.y = real_view.y - direction.y;

		Vector forward;
		math::angle_vectors(direction, forward);

		static auto cl_forwardspeed = m_cvar()->FindVar(crypt_str("cl_forwardspeed"));
		static auto cl_sidespeed = m_cvar()->FindVar(crypt_str("cl_sidespeed"));

		auto negative_forward_speed = -cl_forwardspeed->GetFloat();
		auto negative_side_speed = -cl_sidespeed->GetFloat();

		auto negative_forward_direction = forward * negative_forward_speed;
		auto negative_side_direction = forward * negative_side_speed;

		cmd->m_forwardmove = negative_forward_direction.x;
		cmd->m_sidemove = negative_side_direction.y;

		if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autostop_modifiers[AUTOSTOP_FORCE_ACCURACY])
			return false;
	}

	return true;
}

bool aim::bounding_box_check(adjust_data* record)
{
	const auto collideable = record->player->GetCollideable();
	if (!collideable)
		return false;

	const auto bbmin = record->mins + record->origin;
	const auto bbmax = record->maxs + record->origin;

	Vector points[7];
	points[0] = record->player->hitbox_position_matrix(HITBOX_HEAD, record->matrixes_data.main);
	points[1] = (bbmin + bbmax) * 0.5f;
	points[2] = Vector((bbmax.x + bbmin.x) * 0.5f, (bbmax.y + bbmin.y) * 0.5f, bbmin.z);
	points[3] = bbmax;
	points[4] = Vector(bbmax.x, bbmin.y, bbmax.z);
	points[5] = Vector(bbmin.x, bbmin.y, bbmax.z);
	points[6] = Vector(bbmin.x, bbmax.y, bbmax.z);

	for (auto& point : points)
	{
		return autowall::get().wall_penetration(g_ctx.globals.eye_pos, point, record->player).valid;
	}

	return false;
};

static bool compare_points(const scan_point& first, const scan_point& second)
{
	return !first.center && first.hitbox == second.hitbox;
}

void aim::scan(adjust_data* record, scan_data& data, const Vector& shoot_position)
{
	auto weapon = g_ctx.globals.weapon;

	if (!weapon)
		return;

	auto weapon_info = weapon->get_csweapon_info();

	if (!weapon_info)
		return;

	auto hitboxes = get_hitboxes(record);

	if (hitboxes.empty())
		return;

	auto force_safe_points = record->player->m_iHealth() <= weapon_info->iDamage || key_binds::get().get_key_bind_state(3) || g_cfg.player_list.force_safe_points[record->i] || g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].max_misses && g_ctx.globals.missed_shots[record->i] >= g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].max_misses_amount; //-V648
	auto best_damage = 0;
	auto best_body_damage = 0.f;
	auto predicted_body_damage = 0.f;
	auto prev_hitbox = -1;

	auto minimum_damage = get_minimum_damage(false, record->player->m_iHealth() + 3);
	auto minimum_visible_damage = get_minimum_damage(true, record->player->m_iHealth() + 3);

	std::vector <scan_point> points; //-V826
	//bool body_enabled = false;

	for (auto& hitbox : hitboxes)
	{
		//if (!body_enabled && hitbox >= 2 && hitbox <= 6)
		//	body_enabled = true;

		auto current_points = get_points(record, hitbox);

		for (auto& point : current_points)
		{
			if (!record->bot)
			{
				auto safe = 0.0f;

				if (record->matrixes_data.zero[0].GetOrigin() == record->matrixes_data.first[0].GetOrigin() || record->matrixes_data.zero[0].GetOrigin() == record->matrixes_data.second[0].GetOrigin() || record->matrixes_data.first[0].GetOrigin() == record->matrixes_data.second[0].GetOrigin())
					safe = 0.0f;
				else if (!hitbox_intersection(record->player, record->matrixes_data.zero, hitbox, shoot_position, point.point, &safe))
					safe = 0.0f;
				else if (!hitbox_intersection(record->player, record->matrixes_data.first, hitbox, shoot_position, point.point, &safe))
					safe = 0.0f;
				else if (!hitbox_intersection(record->player, record->matrixes_data.second, hitbox, shoot_position, point.point, &safe))
					safe = 0.0f;

				point.safe = safe;
			}
			else
				point.safe = 1.0f;

			if (!force_safe_points || point.safe)
				points.emplace_back(point);
		}
	}

	for (auto& point : points)
	{
		if (points.empty())
			return;

		if (point.hitbox == HITBOX_HEAD)
			continue;

		for (auto it = points.begin(); it != points.end(); ++it)
		{
			if (point.point == it->point)
				continue;

			auto first_angle = math::calculate_angle(shoot_position, point.point);
			auto second_angle = math::calculate_angle(shoot_position, it->point);

			auto distance = shoot_position.DistTo(point.point);
			auto fov = math::fast_sin(DEG2RAD(math::get_fov(first_angle, second_angle))) * distance;

			if (fov < 5.0f)
			{
				points.erase(it);
				break;
			}
		}
	}

	if (points.empty())
		return;

	scan_point best_point;
	scan_data best_data;

	std::sort(points.begin(), points.end(), compare_points);

	auto points_size = points.size();

	//auto body_hitboxes = true;

	if (points_size > 7 && !bounding_box_check(record))
		return;

	for (auto& point : points)
	{
		if (!point.safe)
		{
			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].safe_points_conditions[SAFEPOINTS_INAIR] && !(record->flags & FL_ONGROUND))
				continue;

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].safe_points_conditions[SAFEPOINTS_INCROUCH] && record->flags & FL_ONGROUND && record->flags & FL_DUCKING)
				continue;

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].safe_points_conditions[SAFEPOINTS_ONLIMBS] && (point.hitbox >= HITBOX_RIGHT_THIGH && point.hitbox < HITBOX_MAX))
				continue;

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].safe_points_conditions[SAFEPOINTS_VISIBLE] && util::visible(g_ctx.globals.eye_pos, point.point, record->player, g_ctx.local()))
				continue;
		}

		auto predicted_eye_pos = g_ctx.globals.eye_pos + engineprediction::get().backup_data.velocity * m_globals()->m_intervalpertick * 8;
		if (point.hitbox < HITBOX_PELVIS || point.hitbox > HITBOX_UPPER_CHEST)
		{
			//body_hitboxes = false;

			if (g_cfg.player_list.force_body_aim[record->i])
				break;

			if (key_binds::get().get_key_bind_state(22))
				break;

			//if (best_damage >= record->player->m_iHealth())
			//	break;

			//if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].prefer_body_aim && best_damage >= 1)
				//break;

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].bodyaimcond[BODYAIMCOND_PREFER] && best_damage >= 1 && record->flags & FL_ONGROUND && !record->shot)
				break;

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].bodyaimcond[BODYAIMCOND_DOUBLETAP] && misc::get().double_tap_enabled)
				break;

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].bodyaimcond[BODYAIMCOND_LETHAL] && best_damage >= record->player->m_iHealth())
				break;
		}

		if ((g_ctx.globals.eye_pos - final_target.data.point.point).Length() > weapon_info->flRange)
			continue;

		auto fire_data = autowall::get().wall_penetration(shoot_position, point.point, record->player);
		auto predicted_fire_data = autowall::get().wall_penetration(predicted_eye_pos, point.point, record->player);

		if (!fire_data.valid)
			continue;

		if (fire_data.damage < 1)
			continue;

		if (!fire_data.visible && !g_cfg.ragebot.autowall)
			continue;

		/*if (key_binds::get().get_key_bind_state(27) && point.hitbox == HITBOX_HEAD)
		{

			for (int i = 1; i <= 10; i++)
			{
				if (record->shot)
				{
					return;
				}

			}
		}*/

		auto current_minimum_damage = fire_data.visible ? minimum_visible_damage : minimum_damage;

		if (predicted_fire_data.valid && point.hitbox >= HITBOX_PELVIS && point.hitbox <= HITBOX_UPPER_CHEST)
		{
			if (predicted_fire_data.damage >= record->player->m_iHealth() + 5)
			{
				if (predicted_fire_data.damage >= predicted_body_damage)
					predicted_body_damage = predicted_fire_data.damage;
			}
		}

		if (predicted_body_damage >= current_minimum_damage && !point.safe && (point.hitbox < HITBOX_PELVIS || point.hitbox > HITBOX_UPPER_CHEST))
			continue;

		if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].prefer_safe_points) {
			if (point.safe && point.hitbox >= HITBOX_PELVIS && point.hitbox <= HITBOX_UPPER_CHEST && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].bodyaimcond[BODYAIMCOND_PREFER]) {
				if ((fire_data.damage > best_body_damage && prev_hitbox != point.hitbox) || fire_data.damage > best_body_damage + 20.f) {
					if (fire_data.damage >= current_minimum_damage)
					{
						best_body_damage = best_damage = fire_data.damage;
						best_data.point = point;
						best_data.visible = fire_data.visible;
						best_data.damage = fire_data.damage;
						best_data.hitbox = fire_data.hitbox;
						prev_hitbox = point.hitbox;
					}
				}
			}
			else if (point.safe && point.hitbox >= HITBOX_PELVIS && point.hitbox <= HITBOX_UPPER_CHEST && fire_data.damage >= record->player->m_iHealth() + 5) {
				if (fire_data.damage >= current_minimum_damage && ((fire_data.damage > best_body_damage && prev_hitbox != point.hitbox) || fire_data.damage > best_body_damage + 20.f)) {
					best_body_damage = best_damage = fire_data.damage;
					best_data.point = point;
					best_data.visible = fire_data.visible;
					best_data.damage = fire_data.damage;
					best_data.hitbox = fire_data.hitbox;
					prev_hitbox = point.hitbox;
				}
			}
			else if (point.safe && (fire_data.damage > best_damage && prev_hitbox != point.hitbox) || fire_data.damage > best_damage + 20.f) {
				if (fire_data.damage >= current_minimum_damage)
				{
					best_damage = fire_data.damage;
					best_data.point = point;
					best_data.visible = fire_data.visible;
					best_data.damage = fire_data.damage;
					best_data.hitbox = fire_data.hitbox;
					prev_hitbox = point.hitbox;
				}
			}
			else if (fire_data.damage >= current_minimum_damage && ((fire_data.damage > best_damage && prev_hitbox != point.hitbox) || fire_data.damage > best_damage + 20.f)) {
				best_damage = fire_data.damage;
				best_data.point = point;
				best_data.visible = fire_data.visible;
				best_data.damage = fire_data.damage;
				best_data.hitbox = fire_data.hitbox;
				prev_hitbox = point.hitbox;
			}
		}
		else if (point.hitbox >= HITBOX_PELVIS && point.hitbox <= HITBOX_UPPER_CHEST && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].bodyaimcond[BODYAIMCOND_PREFER]) {
			if ((fire_data.damage > best_body_damage && prev_hitbox != point.hitbox) || fire_data.damage > best_body_damage + 20.f) {
				if (fire_data.damage >= current_minimum_damage)
				{
					best_body_damage = best_damage = fire_data.damage;
					best_data.point = point;
					best_data.visible = fire_data.visible;
					best_data.damage = fire_data.damage;
					best_data.hitbox = fire_data.hitbox;
					prev_hitbox = point.hitbox;
				}
			}
		}
		else if (point.hitbox >= HITBOX_PELVIS && point.hitbox <= HITBOX_UPPER_CHEST && fire_data.damage >= record->player->m_iHealth() + 5) {
			if (fire_data.damage >= current_minimum_damage && ((fire_data.damage > best_body_damage && prev_hitbox != point.hitbox) || fire_data.damage > best_body_damage + 20.f)) {
				best_body_damage = best_damage = fire_data.damage;
				best_data.point = point;
				best_data.visible = fire_data.visible;
				best_data.damage = fire_data.damage;
				best_data.hitbox = fire_data.hitbox;
				prev_hitbox = point.hitbox;
			}
		}
		else if (fire_data.damage >= current_minimum_damage && ((fire_data.damage > best_damage && prev_hitbox != point.hitbox) || fire_data.damage > best_damage + 20.f)) {
			best_damage = fire_data.damage;
			best_data.point = point;
			best_data.visible = fire_data.visible;
			best_data.damage = fire_data.damage;
			best_data.hitbox = fire_data.hitbox;
			prev_hitbox = point.hitbox;
		}

		if (fire_data.damage >= best_damage && fire_data.damage >= current_minimum_damage)
		{
			if (!should_stop)
			{
				should_stop = true;

				if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autostop_modifiers[AUTOSTOP_LETHAL] && fire_data.damage < record->player->m_iHealth())
					should_stop = false;
				else if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autostop_modifiers[AUTOSTOP_VISIBLE] && !fire_data.visible)
					should_stop = false;
				else if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autostop_modifiers[AUTOSTOP_CENTER] && !point.center)
					should_stop = false;
			}
		}
	}

	data.point = best_data.point;
	data.visible = best_data.visible;
	data.damage = best_data.damage;
	data.hitbox = best_data.hitbox;
}

std::vector <int> aim::get_hitboxes(adjust_data* record)
{
	std::vector <int> hitboxes; //-V827

	// prefer, high speed.
	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].headaimcond[HEADIMCOND_AIR] && record->player->GetMaxBodyRotation() < 33.f)
		hitboxes.emplace_back(HITBOX_HEAD);

	// prefer, on shot.
	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].headaimcond[HEADIMCOND_ONSHOT] && record->shot)
		hitboxes.emplace_back(HITBOX_HEAD);

	bool only{ false };

	// only, force.
	if (key_binds::get().get_key_bind_state(27))
	{
		only = true;
		hitboxes.emplace_back(HITBOX_HEAD);
	}

	// only, high speed.
	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].headaimonlycond[HEADIMCOND_ONLY_AIR] && record->player->GetMaxBodyRotation() < 33.f) {
		only = true;
		hitboxes.emplace_back(HITBOX_HEAD);
	}

	// only, on shot.
	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].headaimonlycond[HEADIMCOND_ONLY_ONSHOT] && record->shot) {
		only = true;
		hitboxes.emplace_back(HITBOX_HEAD);
	}

	// do not insert more hitboxes.
	if (only)
		return hitboxes;

	float flDesiredSpeed = .1f;

	// get weapon.
	weapon_t* weapon = record->player->m_hActiveWeapon().Get();
	if (weapon)
	{
		// get weapon info.
		weapon_info_t* weapon_info = weapon->get_csweapon_info();
		if (weapon_info)
		{
			// get the max possible speed whilest we are still accurate.
			float flMaxSpeed = record->player->m_bIsScoped() ? weapon_info->flMaxPlayerSpeedAlt : weapon_info->flMaxPlayerSpeed;
			flDesiredSpeed = (flMaxSpeed * 0.33000001);
		}
	}

	bool ignore_limbs = record->velocity.Length2D() > flDesiredSpeed && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].ignore_limbs;

	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitboxes.at(1))
		hitboxes.emplace_back(HITBOX_UPPER_CHEST);

	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitboxes.at(2))
		hitboxes.emplace_back(HITBOX_CHEST);

	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitboxes.at(3))
		hitboxes.emplace_back(HITBOX_LOWER_CHEST);

	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitboxes.at(4))
		hitboxes.emplace_back(HITBOX_STOMACH);

	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitboxes.at(5))
		hitboxes.emplace_back(HITBOX_PELVIS);

	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitboxes.at(0))
		hitboxes.emplace_back(HITBOX_HEAD);

	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitboxes.at(6) && !ignore_limbs)
	{
		hitboxes.emplace_back(HITBOX_RIGHT_UPPER_ARM);
		hitboxes.emplace_back(HITBOX_LEFT_UPPER_ARM);
	}

	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitboxes.at(7))
	{
		hitboxes.emplace_back(HITBOX_RIGHT_THIGH);
		hitboxes.emplace_back(HITBOX_LEFT_THIGH);

		hitboxes.emplace_back(HITBOX_RIGHT_CALF);
		hitboxes.emplace_back(HITBOX_LEFT_CALF);
	}

	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitboxes.at(8) && !ignore_limbs)
	{
		hitboxes.emplace_back(HITBOX_RIGHT_FOOT);
		hitboxes.emplace_back(HITBOX_LEFT_FOOT);
	}

	return hitboxes;
}

bool IsFakePlayer(int index) {
	player_info_t info;
	if (m_engine()->GetPlayerInfo(index, &info))
		return info.fakeplayer;

	return false;
}

std::vector <scan_point> aim::get_points(adjust_data* record, int hitbox, bool from_aim)
{
	std::vector <scan_point> points;
	auto model = record->player->GetModel();

	if (!model)
		return points;

	auto hdr = m_modelinfo()->GetStudioModel(model);

	if (!hdr)
		return points;

	auto set = hdr->pHitboxSet(record->player->m_nHitboxSet());

	if (!set)
		return points;

	auto bbox = set->pHitbox(hitbox);

	if (!bbox)
		return points;

	float scale = g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].multipoints_scale * 0.01f;

	// reduce scale.
	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].adaptive_ps)
	{
		if (record->player->GetMaxBodyRotation() < 33.f || record->shot || IsFakePlayer(record->player->EntIndex()))
		{
			scale *= .5f;
		}
	}

	if (!(record->flags) & FL_ONGROUND)
		scale = 0.7f;

	float bscale = g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].stomach_scale * 0.01f;

	if (bbox->radius <= 0.f)
	{
		matrix3x4_t matrix;
		auto rotation_matrix = math::angle_matrix(bbox->rotation);

		math::concat_transforms(record->matrixes_data.main[bbox->bone], rotation_matrix, matrix);

		Vector origin = matrix.GetOrigin();

		Vector center = (bbox->bbmin + bbox->bbmax) * 0.875f;

		if (hitbox == HITBOX_RIGHT_FOOT || hitbox == HITBOX_LEFT_FOOT)
		{
			float d1 = (bbox->bbmin.z - center.z) * 0.875f;

			if (hitbox == HITBOX_LEFT_FOOT)
				d1 *= -1.f;

			points.emplace_back(scan_point(Vector(center.x, center.y, center.z + d1), hitbox, false));

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].multipoints_hitboxes[8])
			{
				float d2 = (bbox->bbmin.x - center.x) * scale;
				float d3 = (bbox->bbmax.x - center.x) * scale;

				points.emplace_back(scan_point(Vector(center.x + d2, center.y, center.z), hitbox, false));
				points.emplace_back(scan_point(Vector(center.x + d3, center.y, center.z), hitbox, false));
			}
		}
	}
	else
	{
		float r = bbox->radius * scale;
		float br = bbox->radius * bscale;

		Vector center = (bbox->bbmin + bbox->bbmax) / 2.f;

		if (hitbox == HITBOX_HEAD)
		{
			points.emplace_back(scan_point(Vector(center), hitbox, true));

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].multipoints_hitboxes[0])
			{
				constexpr float rotation = 0.70710678f;

				points.emplace_back(scan_point(Vector(bbox->bbmax.x + (rotation * r), bbox->bbmax.y + (-rotation * r), bbox->bbmax.z), hitbox, false));

				Vector right{ bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z + r };

				points.emplace_back(scan_point(Vector(right), hitbox, false));

				Vector left{ bbox->bbmax.x, bbox->bbmax.y, bbox->bbmax.z - r };

				points.emplace_back(scan_point(Vector(left), hitbox, false));

				points.emplace_back(scan_point(Vector(bbox->bbmax.x, bbox->bbmax.y - r, bbox->bbmax.z), hitbox, false));

				auto state = record->player->get_animation_state();

				if (state && record->velocity.Length() <= 0.1f && record->angles.x <= state->m_flPitch)
				{
					points.emplace_back(scan_point(Vector(bbox->bbmax.x - r, bbox->bbmax.y, bbox->bbmax.z), hitbox, false));
				}
			}
		}
		else if (hitbox == HITBOX_STOMACH)
		{
			points.emplace_back(scan_point(Vector(center), hitbox, true));

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].multipoints_hitboxes[4])
			{
				points.emplace_back(scan_point(Vector(center.x, bbox->bbmax.y - br, center.z), hitbox, false));
			}
		}
		else if (hitbox == HITBOX_PELVIS || hitbox == HITBOX_UPPER_CHEST)
		{
			points.emplace_back(scan_point(Vector(center.x, bbox->bbmax.y - r, center.z), hitbox, false));
		}
		else if (hitbox == HITBOX_LOWER_CHEST || hitbox == HITBOX_CHEST)
		{
			points.emplace_back(scan_point(Vector(center), hitbox, true));

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].multipoints_hitboxes[1] || g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].multipoints_hitboxes[2])
			{
				points.emplace_back(scan_point(Vector(center.x, bbox->bbmax.y - r, center.z), hitbox, false));
			}
		}
		else if (hitbox == HITBOX_RIGHT_CALF || hitbox == HITBOX_LEFT_CALF)
		{
			points.emplace_back(scan_point(Vector(center), hitbox, true));

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].multipoints_hitboxes[9])
			{
				points.emplace_back(scan_point(Vector(bbox->bbmax.x - (bbox->radius / 2), bbox->bbmax.y, bbox->bbmax.z), hitbox, false));
			}
		}
		else if (hitbox == HITBOX_RIGHT_THIGH || hitbox == HITBOX_LEFT_THIGH)
		{
			points.emplace_back(scan_point(Vector(center), hitbox, true));
		}
		else if (hitbox == HITBOX_RIGHT_UPPER_ARM || hitbox == HITBOX_LEFT_UPPER_ARM)
		{
			points.emplace_back(scan_point(Vector(bbox->bbmax.x + bbox->radius, center.y, center.z), hitbox, false));
		}
	}

	for (auto& point : points)
		math::vector_transform(point.point, record->matrixes_data.main[bbox->bone], point.point);

	return points;
}

static bool compare_targets(const scanned_target& first, const scanned_target& second)
{
	if (g_cfg.player_list.high_priority[first.record->i] != g_cfg.player_list.high_priority[second.record->i])
		return g_cfg.player_list.high_priority[first.record->i];

	switch (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].selection_type)
	{
	case 1:
		return first.fov < second.fov;
	case 2:
		return first.distance < second.distance;
	case 3:
		return first.health < second.health;
	case 4:
		return first.data.damage > second.data.damage;
	}

	return false;
}

void aim::find_best_target()
{
	if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].selection_type)
		std::sort(scanned_targets.begin(), scanned_targets.end(), compare_targets);

	for (auto& target : scanned_targets)
	{
		if (target.fov > (float)g_cfg.ragebot.field_of_view)
			continue;

		final_target = target;
		final_target.record->adjust_player();
		break;
	}
}

void aim::fire(CUserCmd* cmd)
{
	if (!g_ctx.globals.weapon->can_fire(true))
		return;

	auto aim_angle = math::calculate_angle(g_ctx.globals.eye_pos, final_target.data.point.point).Clamp();

	if (!g_cfg.ragebot.silent_aim)
		m_engine()->SetViewAngles(aim_angle);

	if (!g_cfg.ragebot.autoshoot && !(cmd->m_buttons & IN_ATTACK))
		return;

	auto final_hitchance = 0;
	auto hitchance_amount = 0;

	/*if (g_ctx.globals.double_tap_aim && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].double_tap_hitchance)
		hitchance_amount = g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].double_tap_hitchance_amount;
	else if (!g_ctx.globals.double_tap_aim && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitchance)
		hitchance_amount = g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitchance_amount;

	if (hitchance_amount)
	{
		if (g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SSG08 && !(g_ctx.local()->m_fFlags() & FL_ONGROUND) && !(engineprediction::get().backup_data.flags & FL_ONGROUND) && fabs(engineprediction::get().backup_data.velocity.z) < 5.0f && engineprediction::get().backup_data.velocity.Length2D() < 5.0f) //-V807
			final_hitchance = 101;
		else
			final_hitchance = hitchance(aim_angle);

		if (final_hitchance < hitchance_amount)
		{
			auto is_zoomable_weapon = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SCAR20 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_G3SG1 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SSG08 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_AWP || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_AUG || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SG553;
			bool hit = (g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SSG08 && g_ctx.local()->m_hActiveWeapon() && g_ctx.globals.inaccuracy < 0.009f || hitchance(aim_angle));

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope_type == 0 && is_zoomable_weapon && !g_ctx.globals.weapon->m_zoomLevel())
				cmd->m_buttons |= IN_ATTACK2;

			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope_type == 1 && is_zoomable_weapon && !g_ctx.globals.weapon->m_zoomLevel() && !hit)
			{
				cmd->m_buttons |= IN_ATTACK2;
				return;
			}

			// if we can scope.
			bool can_scope = g_ctx.globals.weapon && g_ctx.globals.weapon->m_zoomLevel() == 0 && g_ctx.globals.weapon->IsZoomable(true);

			if (can_scope) {
				// always.
				if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope_type == 0) {
					cmd->m_buttons |= IN_ATTACK2;
					return;
				}

				// hitchance fail.
				else if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope_type == 1 && !hit) {
					cmd->m_buttons |= IN_ATTACK2;
					return;
				}
			}

			return;
		}
	}*/

	auto is_valid_hitchance = calculate_hitchance(final_hitchance, final_target.record->player);

	if (!is_valid_hitchance)
	{
		auto is_zoomable_weapon = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SCAR20 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_G3SG1 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SSG08 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_AWP || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_AUG || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SG553;
		bool hit = (g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SSG08 && g_ctx.local()->m_hActiveWeapon() && g_ctx.globals.inaccuracy < 0.009f || calculate_hitchance(final_hitchance, final_target.record->player));

		// if we can scope.
		bool can_scope = g_ctx.globals.weapon && g_ctx.globals.weapon->m_zoomLevel() == 0 && g_ctx.globals.weapon->IsZoomable(true);

		if (can_scope) {
			// always.
			if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope_type == 0) {
				cmd->m_buttons |= IN_ATTACK2;
				return;
			}

			// hitchance fail.
			else if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].autoscope_type == 1 && !hit) {
				cmd->m_buttons |= IN_ATTACK2;
				return;
			}
		}

		return;
	}

	auto backtrack_ticks = 0;
	auto net_channel_info = m_engine()->GetNetChannelInfo();

	if (net_channel_info)
	{
		auto original_tickbase = g_ctx.globals.backup_tickbase;
		auto max_tickbase_shift = m_gamerules()->m_bIsValveDS() ? 6 : 16;

		if (g_cfg.ragebot.double_tap && g_cfg.ragebot.double_tap_key.key > KEY_NONE && g_cfg.ragebot.double_tap_key.key < KEY_MAX && misc::get().double_tap_key)
		{
			if (!g_ctx.local()->m_bGunGameImmunity() && !(g_ctx.local()->m_fFlags() & FL_FROZEN) && !antiaim::get().freeze_check && misc::get().double_tap_enabled && !g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
			{
				original_tickbase += min(g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_shift_value, max_tickbase_shift);
			}
		}

		static auto sv_maxunlag = m_cvar()->FindVar(crypt_str("sv_maxunlag"));

		auto correct = math::clamp(net_channel_info->GetLatency(FLOW_OUTGOING) + net_channel_info->GetLatency(FLOW_INCOMING) + util::get_interpolation(), 0.0f, sv_maxunlag->GetFloat());
		auto delta_time = correct - (TICKS_TO_TIME(original_tickbase) - final_target.record->simulation_time);

		backtrack_ticks = TIME_TO_TICKS(fabs(delta_time));
	}

	static auto get_hitbox_name = [](int hitbox, bool shot_info = false) -> std::string
	{
		switch (hitbox)
		{
		case HITBOX_HEAD:
			return shot_info ? crypt_str("Head") : crypt_str("head");
		case HITBOX_LOWER_CHEST:
			return shot_info ? crypt_str("Lower chest") : crypt_str("lower chest");
		case HITBOX_CHEST:
			return shot_info ? crypt_str("Chest") : crypt_str("chest");
		case HITBOX_UPPER_CHEST:
			return shot_info ? crypt_str("Upper chest") : crypt_str("upper chest");
		case HITBOX_STOMACH:
			return shot_info ? crypt_str("Stomach") : crypt_str("stomach");
		case HITBOX_PELVIS:
			return shot_info ? crypt_str("Pelvis") : crypt_str("pelvis");
		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_RIGHT_FOREARM:
		case HITBOX_RIGHT_HAND:
			return shot_info ? crypt_str("Left arm") : crypt_str("left arm");
		case HITBOX_LEFT_UPPER_ARM:
		case HITBOX_LEFT_FOREARM:
		case HITBOX_LEFT_HAND:
			return shot_info ? crypt_str("Right arm") : crypt_str("right arm");
		case HITBOX_RIGHT_THIGH:
		case HITBOX_RIGHT_CALF:
			return shot_info ? crypt_str("Left leg") : crypt_str("left leg");
		case HITBOX_LEFT_THIGH:
		case HITBOX_LEFT_CALF:
			return shot_info ? crypt_str("Right leg") : crypt_str("right leg");
		case HITBOX_RIGHT_FOOT:
			return shot_info ? crypt_str("Left foot") : crypt_str("left foot");
		case HITBOX_LEFT_FOOT:
			return shot_info ? crypt_str("Right foot") : crypt_str("right foot");
		}
	};

	player_info_t player_info;
	m_engine()->GetPlayerInfo(final_target.record->i, &player_info);

	cmd->m_viewangles = aim_angle;
	cmd->m_buttons |= IN_ATTACK;
	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND && key_binds::get().get_key_bind_state(20)))
		g_ctx.send_packet = true;
	cmd->m_tickcount = TIME_TO_TICKS(final_target.record->simulation_time + util::get_interpolation());

	last_target_index = final_target.record->i;
	last_shoot_position = g_ctx.globals.eye_pos;
	last_target[last_target_index] = Last_target
	{
		*final_target.record, final_target.data, final_target.distance
	};

	auto shot = &g_ctx.shots.emplace_back();

	shot->last_target = last_target_index;
	shot->fire_tick = m_globals()->m_tickcount;
	shot->shot_info.target_name = player_info.szName;
	shot->shot_info.client_hitbox = get_hitbox_name(final_target.data.hitbox, true);
	shot->shot_info.client_damage = final_target.data.damage;
	shot->shot_info.hitchance = math::clamp(final_hitchance, 0, 100);
	shot->shot_info.backtrack_ticks = backtrack_ticks;
	shot->shot_info.aim_point = final_target.data.point.point;

	g_ctx.globals.aimbot_working = true;
	g_ctx.globals.revolver_working = false;
	g_ctx.globals.last_aimbot_shot = m_globals()->m_tickcount;
}

__forceinline void aim::build_seed_table()
{
	for (auto i = 0; i < 255; i++) {
		math::random_seed(i + 1);

		const auto pi_seed = math::random_float(0.f, 6.283186f);

		precomputed_seeds.emplace_back(math::random_float(0.f, 1.f),
			pi_seed);
	}
}

bool aim::weaponIsMaxAccuracy(float inaccuracy)
{
	if (m_cvar()->FindVar(crypt_str("weapon_accuracy_nospread"))->GetInt() > 0)
		return true;

	const auto round_acc = [](const float accuracy) { return floorf(accuracy * 1000.0f) / 1000.0f; };
	const auto crouched = g_ctx.local()->m_fFlags() & FL_DUCKING;
	const auto sniper = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_AWP || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SCAR20 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_G3SG1 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SSG08;
	if (g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
			return inaccuracy < (crouched ? .0020f : .0055f);
	}

	bool scoped = g_ctx.local()->m_bIsScoped();
	if (sniper)
	{
		if (crouched)
		{
			if (round_acc(inaccuracy) == round_acc(g_ctx.globals.weapon->get_csweapon_info()->flInaccuracyCrouchAlt))
				return true;
		}
		else
		{
			if (round_acc(inaccuracy) == round_acc(g_ctx.globals.weapon->get_csweapon_info()->flInaccuracyStandAlt))
				return true;
		}
	}
	else
	{
		if (crouched)
		{
			if (round_acc(inaccuracy) == round_acc(g_ctx.globals.weapon->get_csweapon_info()->flInaccuracyCrouch))
				return true;
		}
		else
		{
			if (round_acc(inaccuracy) == round_acc(g_ctx.globals.weapon->get_csweapon_info()->flInaccuracyStand))
				return true;
		}
	}

	return false;
}

__forceinline bool IsValidHitgroup(int i) {
	if ((i >= HITGROUP_HEAD && i <= HITGROUP_RIGHTLEG) || i == HITGROUP_GEAR)
		return true;
	
	return false;
}

bool aim::CanHit(Vector start, Vector end, player_t* e, int box, matrix3x4_t* bones)
{
	if (!e)
		return false;

	if (!bones)
		return false;

	const model_t* model = e->GetModel();
	if (!model)
		return false;

	studiohdr_t* hdr = m_modelinfo()->GetStudioModel(model);
	if (!hdr)
		return false;

	mstudiohitboxset_t* set = hdr->pHitboxSet(e->m_nHitboxSet());
	if (!set)
		return false;

	mstudiobbox_t* bbox = set->pHitbox(box);
	if (!bbox)
		return false;

	Vector min, max;
	const auto IsCapsule = false && bbox->radius != -1.f;

	if (IsCapsule)
	{
		math::vector_transform(bbox->bbmin, bones[bbox->bone], min);
		math::vector_transform(bbox->bbmax, bones[bbox->bone], max);
		const auto dist = math::segment_to_segment(start, end, min, max);

		if (dist < bbox->radius) {
			return true;
		}
	}
	else {
		CGameTrace tr;

		m_trace()->ClipRayToEntity(Ray_t(start, end), MASK_SHOT, e, &tr);

		if (tr.hit_entity == e && IsValidHitgroup(tr.hitgroup))
			return true;
	}

	return false;
}


bool aim::calculate_hitchance(int& final_hitchance, player_t* player)
{
	if (precomputed_seeds.empty())
		build_seed_table();

	float HITCHANCE_MAX = 100.f;
	constexpr int SEED_MAX = 255;

	float hitchance = g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitchance_amount;

	if (g_ctx.globals.double_tap_aim && g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].double_tap_hitchance)
		hitchance = g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].double_tap_hitchance_amount;

	if (m_cvar()->FindVar(crypt_str("weapon_accuracy_nospread"))->GetInt())
	{
		final_hitchance = 100;
		return true;
	}

	size_t total_hits{ }, needed_hits{ (size_t)std::ceil((SEED_MAX * (hitchance / HITCHANCE_MAX))) };

	Vector start{ g_ctx.globals.eye_pos }, end, fwd, right, up, dir, wep_spread;

	static auto weapon_recoil_scale = m_cvar()->FindVar(crypt_str("weapon_recoil_scale"));
	const auto cur_aim_angle = math::calculate_angle(g_ctx.globals.eye_pos, final_target.data.point.point) - (g_ctx.local()->m_aimPunchAngle() * weapon_recoil_scale->GetFloat());

	math::angle_vectors(cur_aim_angle, &fwd, &right, &up);

	math::fast_vec_normalize(fwd);
	math::fast_vec_normalize(right);
	math::fast_vec_normalize(up);

	if ((g_ctx.globals.eye_pos - cur_aim_angle).Length2D() > g_ctx.globals.weapon->get_csweapon_info()->flRange)
		return false;

	const auto id = g_ctx.globals.weapon->m_iItemDefinitionIndex();
	const auto round_acc = [](const float accuracy) { return roundf(accuracy * 1000.f) / 1000.f; };

	auto inaccuracy = g_ctx.globals.weapon->get_inaccuracy();
	auto spread = g_ctx.globals.weapon->get_spread();

	g_ctx.globals.weapon->update_accuracy_penality();

	for (int i = 0; i < SEED_MAX; i++) {
		wep_spread = CalculateSpread(i, inaccuracy, spread);

		dir = (fwd + (right * wep_spread.x) + (up * wep_spread.y)).Normalized();

		end = start + (dir * g_ctx.globals.weapon->get_csweapon_info()->flRange);

		if (id == WEAPON_SSG08 && !(g_ctx.local()->m_fFlags() & FL_ONGROUND))
		{
			if (inaccuracy < 0.009f)
				return true;
		}

		Vector viewForward = g_ctx.local()->get_shoot_position() + (viewForward * g_ctx.globals.weapon->get_csweapon_info()->flRange);

		CGameTrace Trace;

		m_trace()->ClipRayToEntity(Ray_t(g_ctx.local()->get_shoot_position(), viewForward), MASK_SHOT | CONTENTS_GRATE, player, &Trace);

		if (CanHit(start, end, player, final_target.data.hitbox, final_target.record->matrixes_data.main))
			++total_hits;

		if (total_hits >= needed_hits)
		{
			final_hitchance = (size_t)std::ceil(total_hits / 2.55f);
			return true;
		}

		if ((SEED_MAX - i + total_hits) < needed_hits)
			return false;

	}

	return false;
	/*const auto info = g_ctx.globals.weapon->get_csweapon_info();

	if (!info)
	{
		final_hitchance = 0;
		return true;
	}

	const auto hitchance_cfg = g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitchance_amount;

	const auto weapon_inaccuracy = g_ctx.globals.weapon->get_inaccuracy();

	if (weaponIsMaxAccuracy(weapon_inaccuracy))
		return true;

	static auto weapon_recoil_scale = m_cvar()->FindVar(crypt_str("weapon_recoil_scale"));
	const auto aim_angle = math::calculate_angle(g_ctx.globals.eye_pos, final_target.data.point.point) - (g_ctx.local()->m_aimPunchAngle() * weapon_recoil_scale->GetFloat());
	auto forward = ZERO;
	auto right = ZERO;
	auto up = ZERO;

	math::angle_vectors(aim_angle, &forward, &right, &up);

	math::fast_vec_normalize(forward);
	math::fast_vec_normalize(right);
	math::fast_vec_normalize(up);

	auto current = 0;

	Vector total_spread, spread_angle, end;
	float inaccuracy, spread_x, spread_y;
	std::tuple<float, float, float>* seed;

	for (auto i = 0u; i < 255; i++)
	{
		seed = &precomputed_seeds[i];

		inaccuracy = std::get<0>(*seed) * weapon_inaccuracy;
		spread_x = std::get<2>(*seed) * inaccuracy;
		spread_y = std::get<1>(*seed) * inaccuracy;
		total_spread = (forward + right * spread_x + up * spread_y);
		total_spread.Normalize();

		math::vector_angles(total_spread, spread_angle);

		math::angle_vectors(spread_angle, end);
		end.Normalize();
		end = g_ctx.globals.eye_pos + end * info->flRange;

		if (hitbox_intersection(final_target.record->player, final_target.record->matrixes_data.main, final_target.data.hitbox, g_ctx.globals.eye_pos, end))
			current++;

		const auto weapon_spread = g_ctx.globals.weapon->get_spread();
		const auto crouched = g_ctx.local()->m_fFlags() & FL_DUCKING;

		if ((static_cast<float>(current) / 255.f) * 100.f >= hitchance_cfg)
		{
			final_hitchance = (static_cast<float>(current) / 255.f) * 100.f;
			return true;
		}

		if (g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SSG08 && !(g_ctx.local()->m_fFlags() & FL_ONGROUND))
			if (weapon_inaccuracy < 0.009f)
				return true;

		if ((static_cast<float>(current + 255 - i) / 255.f) * 100.f < hitchance_cfg)
		{
			final_hitchance = (static_cast<float>(current + 255 - i) / 255.f) * 100.f;
			return false;
		}
	}

	final_hitchance = (static_cast<float>(current) / 255.f) * 100.f;
	return (static_cast<float>(current) / 255.f) * 100.f >= hitchance_cfg;*/
}

int aim::hitchance(const Vector& aim_angle)
{
	auto final_hitchance = 0;
	auto weapon_info = g_ctx.globals.weapon->get_csweapon_info();

	if (!weapon_info)
		return final_hitchance;

	if ((g_ctx.globals.eye_pos - final_target.data.point.point).Length() > weapon_info->flRange)
		return final_hitchance;

	auto forward = ZERO;
	auto right = ZERO;
	auto up = ZERO;

	math::angle_vectors(aim_angle, &forward, &right, &up);

	math::fast_vec_normalize(forward);
	math::fast_vec_normalize(right);
	math::fast_vec_normalize(up);

	auto is_special_weapon = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_AWP || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_G3SG1 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SCAR20 || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SSG08;
	auto inaccuracy = weapon_info->flInaccuracyStand;

	if (g_ctx.local()->m_fFlags() & FL_DUCKING)
	{
		if (is_special_weapon)
			inaccuracy = weapon_info->flInaccuracyCrouchAlt;
		else
			inaccuracy = weapon_info->flInaccuracyCrouch;
	}
	else if (is_special_weapon)
		inaccuracy = weapon_info->flInaccuracyStandAlt;

	if (g_ctx.globals.inaccuracy - 0.000001f < inaccuracy)
		final_hitchance = 101;
	else
	{
		static auto setup_spread_values = true;
		static float spread_values[256][6];

		if (setup_spread_values)
		{
			setup_spread_values = false;

			for (auto i = 0; i < 256; ++i)
			{
				math::random_seed(i + 1);

				auto a = math::random_float(0.0f, 1.0f);
				auto b = math::random_float(0.0f, DirectX::XM_2PI);
				auto c = math::random_float(0.0f, 1.0f);
				auto d = math::random_float(0.0f, DirectX::XM_2PI);

				spread_values[i][0] = a;
				spread_values[i][1] = c;

				auto sin_b = 0.0f, cos_b = 0.0f;
				DirectX::XMScalarSinCos(&sin_b, &cos_b, b);

				auto sin_d = 0.0f, cos_d = 0.0f;
				DirectX::XMScalarSinCos(&sin_d, &cos_d, d);

				spread_values[i][2] = sin_b;
				spread_values[i][3] = cos_b;
				spread_values[i][4] = sin_d;
				spread_values[i][5] = cos_d;
			}
		}

		auto hits = 0;

		for (auto i = 0; i < 256; ++i)
		{
			auto inaccuracy = spread_values[i][0] * g_ctx.globals.inaccuracy;
			auto spread = spread_values[i][1] * g_ctx.globals.spread;

			auto spread_x = spread_values[i][3] * inaccuracy + spread_values[i][5] * spread;
			auto spread_y = spread_values[i][2] * inaccuracy + spread_values[i][4] * spread;

			auto direction = ZERO;

			direction.x = forward.x + right.x * spread_x + up.x * spread_y;
			direction.y = forward.y + right.y * spread_x + up.y * spread_y;
			direction.z = forward.z + right.z * spread_x + up.z * spread_y; //-V778

			auto end = g_ctx.globals.eye_pos + direction * weapon_info->flRange;

			if (hitbox_intersection(final_target.record->player, final_target.record->matrixes_data.main, final_target.data.hitbox, g_ctx.globals.eye_pos, end))
				++hits;
		}

		final_hitchance = (int)((float)hits / 2.56f);
	}

	if (g_ctx.globals.double_tap_aim)
		return final_hitchance;

	auto damage = 0;
	auto high_accuracy_weapon = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_AWP || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_SSG08;

	auto spread = g_ctx.globals.spread + g_ctx.globals.inaccuracy;

	for (auto i = 1; i <= 6; ++i)
	{
		for (auto j = 0; j < 8; ++j)
		{
			auto current_spread = spread * ((float)i / 6.0f);

			auto direction_cos = 0.0f;
			auto direction_sin = 0.0f;

			DirectX::XMScalarSinCos(&direction_cos, &direction_sin, (float)j / 8.0f * DirectX::XM_2PI);

			auto spread_x = direction_cos * current_spread;
			auto spread_y = direction_sin * current_spread;

			auto direction = ZERO;

			direction.x = forward.x + spread_x * right.x + spread_y * up.x;
			direction.y = forward.y + spread_x * right.y + spread_y * up.y;
			direction.z = forward.z + spread_x * right.z + spread_y * up.z;

			auto end = g_ctx.globals.eye_pos + direction * weapon_info->flRange;

			if (hitbox_intersection(final_target.record->player, final_target.record->matrixes_data.main, final_target.data.hitbox, g_ctx.globals.eye_pos, end))
			{
				auto fire_data = autowall::get().wall_penetration(g_ctx.globals.eye_pos, end, final_target.record->player);
				auto valid_hitbox = true;

				if (final_target.data.hitbox == HITBOX_HEAD && fire_data.hitbox != HITBOX_HEAD)
					valid_hitbox = false;

				if (fire_data.valid && fire_data.damage >= 1 && valid_hitbox)
					damage += high_accuracy_weapon ? fire_data.damage : 1;
			}
		}
	}

	if (high_accuracy_weapon)
		return (float)damage / 48.0f >= get_minimum_damage(final_target.data.visible, final_target.health) ? final_hitchance : 0;

	return (float)damage / 48.0f >= (float)g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitchance_amount * 0.01f ? final_hitchance : 0;
}

static int clip_ray_to_hitbox(const Ray_t& ray, mstudiobbox_t* hitbox, matrix3x4_t& matrix, trace_t& trace)
{
	static auto fn = util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 F3 0F 10 42"));

	trace.fraction = 1.0f;
	trace.startsolid = false;

	return reinterpret_cast <int(__fastcall*)(const Ray_t&, mstudiobbox_t*, matrix3x4_t&, trace_t&)> (fn)(ray, hitbox, matrix, trace);
}

/*bool aim::hitbox_intersection(player_t* e, matrix3x4_t* matrix, int hitbox, const Vector& start, const Vector& end)
{
	auto model = e->GetModel();

	if (!model)
		return false;

	auto studio_model = m_modelinfo()->GetStudioModel(model);

	if (!studio_model)
		return false;

	auto studio_set = studio_model->pHitboxSet(e->m_nHitboxSet());

	if (!studio_set)
		return false;

	auto studio_hitbox = studio_set->pHitbox(hitbox);

	if (!studio_hitbox)
		return false;

	Vector min, max;

	const auto is_capsule = studio_hitbox->radius != -1.f;

	if (is_capsule)
	{
		math::vector_transform(studio_hitbox->bbmin, matrix[studio_hitbox->bone], min);
		math::vector_transform(studio_hitbox->bbmax, matrix[studio_hitbox->bone], max);
		const auto dist = math::segment_to_segment(start, end, min, max);

		if (dist < studio_hitbox->radius)
			return true;
	}
	else
	{
		math::vector_transform(math::vector_rotate(studio_hitbox->bbmin, studio_hitbox->rotation), matrix[studio_hitbox->bone], min);
		math::vector_transform(math::vector_rotate(studio_hitbox->bbmax, studio_hitbox->rotation), matrix[studio_hitbox->bone], max);

		math::vector_i_transform(start, matrix[studio_hitbox->bone], min);
		math::vector_i_rotate(end, matrix[studio_hitbox->bone], max);

		if (math::intersect_line_with_bb(min, max, studio_hitbox->bbmin, studio_hitbox->bbmax))
			return true;
	}

	return false;
}*/

bool aim::hitbox_intersection(player_t* e, matrix3x4_t* matrix, int hitbox, const Vector& start, const Vector& end, float* safe)
{
	auto model = e->GetModel();

	if (!model)
		return false;

	auto studio_model = m_modelinfo()->GetStudioModel(model);

	if (!studio_model)
		return false;

	auto studio_set = studio_model->pHitboxSet(e->m_nHitboxSet());

	if (!studio_set)
		return false;

	auto studio_hitbox = studio_set->pHitbox(hitbox);

	if (!studio_hitbox)
		return false;

	trace_t trace;

	Ray_t ray;
	ray.Init(start, end);

	auto intersected = clip_ray_to_hitbox(ray, studio_hitbox, matrix[studio_hitbox->bone], trace) >= 0;

	if (!safe)
		return intersected;

	Vector min, max;

	math::vector_transform(studio_hitbox->bbmin, matrix[studio_hitbox->bone], min);
	math::vector_transform(studio_hitbox->bbmax, matrix[studio_hitbox->bone], max);

	auto center = (min + max) * 0.5f;
	auto distance = center.DistTo(end);

	if (distance > *safe)
		*safe = distance;

	return intersected;
}