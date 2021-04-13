// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "..\hooks.hpp"
#include "..\..\cheats\ragebot\antiaim.h"
#include "..\..\cheats\visuals\other_esp.h"
#include "..\..\cheats\misc\fakelag.h"
#include "..\..\cheats\misc\prediction_system.h"
#include "..\..\cheats\ragebot\aim.h"
#include "..\..\cheats\legitbot\legitbot.h"
#include "..\..\cheats\misc\bunnyhop.h"
#include "..\..\cheats\misc\airstrafe.h"
#include "..\..\cheats\misc\spammers.h"
#include "..\..\cheats\fakewalk\slowwalk.h"
#include "..\..\cheats\misc\misc.h"
#include "..\..\cheats\misc\logs.h"
#include "..\..\cheats\visuals\GrenadePrediction.h"
#include "..\..\cheats\ragebot\knifebot.h"
#include "..\..\cheats\ragebot\zeusbot.h"
#include "..\..\cheats\lagcompensation\local_animations.h"
#include "..\..\cheats\lagcompensation\animation_system.h"
#include "../../cheats/legitbot/backtrack.h"
#include "..\..\cheats\misc\walkbot.h"

using CreateMove_t = bool(__thiscall*)(IClientMode*, float, CUserCmd*);

float final_feet_yaw[65];
float old_feet_yaw[65];
float eye_lby_delta[65];

bool __stdcall hooks::hooked_createmove(float smt, CUserCmd* m_pcmd)
{
	static auto original_fn = g_ctx.test_hooks ? oCreateMove_t : clientmode_hook->get_func_address <CreateMove_t>(24);
	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

	g_ctx.globals.in_createmove = false;

	if (!m_pcmd)
		return original_fn(m_clientmode(), smt, m_pcmd);

	if (!m_pcmd->m_command_number)
		return original_fn(m_clientmode(), smt, m_pcmd);

	auto original_result = original_fn(m_clientmode(), smt, m_pcmd);

	if (original_result)
	{
		m_prediction()->SetLocalViewAngles(m_pcmd->m_viewangles);
		m_engine()->SetViewAngles(m_pcmd->m_viewangles);
	}

	if (!g_ctx.available())
		return original_fn(m_clientmode(), smt, m_pcmd);

	misc::get().rank_reveal();
	spammers::get().clan_tag();

	if (!g_ctx.local()->is_alive()) //-V807
		return original_fn(m_clientmode(), smt, m_pcmd);

	uintptr_t* frame_ptr;
	__asm mov frame_ptr, ebp;

	g_ctx.globals.weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!g_ctx.globals.weapon)
		return original_fn(m_clientmode(), smt, m_pcmd);

	g_ctx.globals.in_createmove = true;
	g_ctx.set_command(m_pcmd);

	if (menu_open && g_ctx.globals.focused_on_input)
	{
		m_pcmd->m_buttons = 0;
		m_pcmd->m_forwardmove = 0.0f;
		m_pcmd->m_sidemove = 0.0f;
		m_pcmd->m_upmove = 0.0f;
	}

	static auto should_recharge = false;

	if (should_recharge)
	{
		++g_ctx.globals.ticks_allowed;

		m_pcmd->m_tickcount = INT_MAX;
		m_pcmd->m_forwardmove = 0.0f;
		m_pcmd->m_sidemove = 0.0f;
		m_pcmd->m_upmove = 0.0f;
		m_pcmd->m_buttons &= ~IN_ATTACK;
		m_pcmd->m_buttons &= ~IN_ATTACK2;

		if (g_ctx.globals.ticks_allowed >= 16)
		{
			should_recharge = false;
			*(bool*)(*frame_ptr - 0x1C) = true; 
		}
		else
			*(bool*)(*frame_ptr - 0x1C) = false;

		return false;
	}

	if (g_ctx.globals.ticks_allowed < 16 && (misc::get().double_tap_enabled && misc::get().double_tap_key || misc::get().hide_shots_enabled && misc::get().hide_shots_key))
		should_recharge = true;

	// start backup tickbase from next time shift and ticks allowed for enemy and double tapping $$$ 
	g_ctx.globals.backup_tickbase = g_ctx.local()->m_nTickBase();

	g_ctx.globals.fixed_tickbase = g_ctx.local()->m_nTickBase() - g_ctx.globals.next_tickbase_shift;

	if (menu_open)
	{
		m_pcmd->m_buttons &= ~IN_ATTACK;
		m_pcmd->m_buttons &= ~IN_ATTACK2;
	}

	if (m_pcmd->m_buttons & IN_ATTACK2 && g_cfg.ragebot.enable && g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
		m_pcmd->m_buttons &= ~IN_ATTACK2;

	if (g_cfg.ragebot.enable && !g_ctx.globals.weapon->can_fire(true))
	{
		if (m_pcmd->m_buttons & IN_ATTACK && !g_ctx.globals.weapon->is_non_aim() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
			m_pcmd->m_buttons &= ~IN_ATTACK;
		else if ((m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2) && (g_ctx.globals.weapon->is_knife() || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER) && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_HEALTHSHOT)
		{
			if (m_pcmd->m_buttons & IN_ATTACK)
				m_pcmd->m_buttons &= ~IN_ATTACK;

			if (m_pcmd->m_buttons & IN_ATTACK2)
				m_pcmd->m_buttons &= ~IN_ATTACK2;
		}
	}

	if (m_pcmd->m_buttons & IN_FORWARD && m_pcmd->m_buttons & IN_BACK)
	{
		m_pcmd->m_buttons &= ~IN_FORWARD;
		m_pcmd->m_buttons &= ~IN_BACK;
	}

	if (m_pcmd->m_buttons & IN_MOVELEFT && m_pcmd->m_buttons & IN_MOVERIGHT)
	{
		m_pcmd->m_buttons &= ~IN_MOVELEFT;
		m_pcmd->m_buttons &= ~IN_MOVERIGHT;
	}

	g_ctx.send_packet = true;
	g_ctx.globals.tickbase_shift = 0;
	g_ctx.globals.double_tap_fire = false;
	g_ctx.globals.force_send_packet = false;
	g_ctx.globals.exploits = misc::get().double_tap_key || misc::get().hide_shots_key;
	g_ctx.globals.current_weapon = g_ctx.globals.weapon->get_weapon_group(g_cfg.ragebot.enable);
	g_ctx.globals.slowwalking = false;
	g_ctx.globals.original_forwardmove = m_pcmd->m_forwardmove;
	g_ctx.globals.original_sidemove = m_pcmd->m_sidemove;

	antiaim::get().breaking_lby = false;

	walkbot::get().move(m_pcmd);

	auto wish_angle = m_pcmd->m_viewangles;

	if (g_cfg.legitbot.legit_resolver)
	{
		for (int i = 1; i <= m_globals()->m_maxclients; i++) {
			auto entity = reinterpret_cast<player_t*>(m_entitylist()->GetClientEntity(i));

			if (!entity
				|| !g_ctx.local()
				|| !entity->is_alive()
				|| entity == g_ctx.local()
				|| entity->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
				continue;

			player_info_t player_info;
			m_engine()->GetPlayerInfo(entity->EntIndex(), &player_info);

			if (player_info.fakeplayer) // skip bots
				continue;

			auto anim_state = entity->get_animation_state();

			if (!anim_state)
				return;

			// dont forget to include fakelag checks, 
			// some p2cs require up to 3 ticks in order to lby desync,
			// but you can do it with just 1

			// take their feetyaw and normaclampalize for extra safety
			final_feet_yaw[entity->EntIndex()] = std::clamp(math::normalize_yaw(anim_state->m_flGoalFeetYaw), -180.f, 180.f);

			// can be used in the future if u want
			old_feet_yaw[entity->EntIndex()] = std::clamp(math::normalize_yaw(old_feet_yaw[entity->EntIndex()]), -180.f, 180.f);

			// calculate their lby delta men!
			eye_lby_delta[entity->EntIndex()] = math::normalize_yaw(entity->m_angEyeAngles().y - entity->m_flLowerBodyYawTarget());

			// make sure their lby is broken and their standing. once again p2cs can do things differently,
			// they usually jitter their speed between 0.0f and 1.1f so make sure we are above that interval.
			// Other cheats that actually flick to break lby can be checked with 0.0f speed
			if (fabs(eye_lby_delta[entity->EntIndex()]) > 35.f && entity->m_vecVelocity().Length2D() <= 2.f /* && choking[entity->index()] */)
			{
				// now we go, clamped their feetyaw to max range using the opposite of their lby delta, 
				// pretty basic, but its really all you gotta do
				final_feet_yaw[entity->EntIndex()] += std::clamp(eye_lby_delta[entity->EntIndex()], -58.f, 58.f);

				// maybe we can save set this old_feet_yaw for future usage,
				// possibly saving their standing desync angle and using it while moving afterwards
				old_feet_yaw[entity->EntIndex()] = final_feet_yaw[entity->EntIndex()];
			}

			// finally set their feet_yaw
			anim_state->m_flGoalFeetYaw = final_feet_yaw[entity->EntIndex()];
		}

	}

	if (g_cfg.esp.thunder)
	{
		static clock_t start_t = clock();

		double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;

		static int choose;

		if (timeSoFar > 6.0)
		{
			switch (choose)
			{
			case 1:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder4.wav"); break;
			case 2:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder5.wav"); break;
			case 3:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder6.wav"); break;
			case 4:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder_distant_01.wav"); break;
			case 5:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder_distant_02.wav"); break;
			case 6:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder_distant_06.wav"); break;
			}
			start_t = clock();
		}
		else
		{
			int randomnum = 1 + (rand() % 6);

			if (randomnum == choose)
				choose = 1 + (rand() % 6); // could still be the same number but less likely
			else
				choose = randomnum;
		}
	}

	misc::get().fast_stop(m_pcmd);

	if (g_cfg.misc.bunnyhop)
		bunnyhop::get().create_move();

	misc::get().SlideWalk(m_pcmd);

	misc::get().NoDuck(m_pcmd);

	misc::get().AutoCrouch(m_pcmd);

	GrenadePrediction::get().Tick(m_pcmd->m_buttons);

	if (g_cfg.misc.crouch_in_air && !(g_ctx.local()->m_fFlags() & FL_ONGROUND))
		m_pcmd->m_buttons |= IN_DUCK;

	engineprediction::get().prediction_data.reset(); //-V807
	engineprediction::get().setup();

	engineprediction::get().predict(m_pcmd);
	local_animations::get().update_prediction_animations();

	g_ctx.globals.eye_pos = g_ctx.local()->get_shoot_position();

	if (g_cfg.misc.airstrafe)
		airstrafe::get().create_move(m_pcmd);
	
	if (g_cfg.misc.check_for_null_strafes)
		misc::get().null_strafes(m_pcmd);

	if (key_binds::get().get_key_bind_state(25)) //-V807
		misc::get().blockbot(m_pcmd);

	if (key_binds::get().get_key_bind_state(19) && engineprediction::get().backup_data.flags & FL_ONGROUND && !(g_ctx.local()->m_fFlags() & FL_ONGROUND)) //-V807
		m_pcmd->m_buttons |= IN_JUMP;

	if (key_binds::get().get_key_bind_state(23)) //-V807
		misc::get().edgebug(m_pcmd);

	if (key_binds::get().get_key_bind_state(24)) //-V807
		misc::get().jumpbug(m_pcmd);

	if (key_binds::get().get_key_bind_state(21))
		slowwalk::get().create_move(m_pcmd);



	if (g_cfg.ragebot.enable && !g_ctx.globals.weapon->is_non_aim() && engineprediction::get().backup_data.flags & FL_ONGROUND && g_ctx.local()->m_fFlags() & FL_ONGROUND)
		slowwalk::get().create_move(m_pcmd, 0.95f + 0.003125f * (16 - m_clientstate()->iChokedCommands));

	if (!should_recharge)
		fakelag::get().Createmove();

	g_ctx.globals.aimbot_working = false;
	g_ctx.globals.revolver_working = false;

	auto backup_velocity = g_ctx.local()->m_vecVelocity();
	auto backup_abs_velocity = g_ctx.local()->m_vecAbsVelocity();

	g_ctx.local()->m_vecVelocity() = engineprediction::get().backup_data.velocity;
	g_ctx.local()->m_vecAbsVelocity() = engineprediction::get().backup_data.velocity;

	g_ctx.globals.weapon->update_accuracy_penality();

	g_ctx.local()->m_vecVelocity() = backup_velocity;
	g_ctx.local()->m_vecAbsVelocity() = backup_abs_velocity;

	g_ctx.globals.inaccuracy = g_ctx.globals.weapon->get_inaccuracy();
	g_ctx.globals.spread = g_ctx.globals.weapon->get_spread();

	aim::get().run(m_pcmd);
	legit_bot::get().createmove(m_pcmd);
	if (g_cfg.legitbot.backtrackl) {
		NewBacktrack::Get().LegitBacktrack(m_pcmd);
	}

	zeusbot::get().run(m_pcmd);
	knifebot::get().run(m_pcmd);

	misc::get().automatic_peek(m_pcmd, wish_angle.y);

	antiaim::get().desync_angle = 0.0f;
	antiaim::get().create_move(m_pcmd);

	if (m_gamerules()->m_bIsValveDS() && m_clientstate()->iChokedCommands >= 6)
	{
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}
	else if (!m_gamerules()->m_bIsValveDS() && m_clientstate()->iChokedCommands >= 16)
	{
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}

	if (g_ctx.globals.should_send_packet)
	{
		g_ctx.globals.force_send_packet = true;
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}

	if (g_ctx.globals.should_choke_packet && !(m_pcmd->m_buttons & IN_ATTACK))
	{
		g_ctx.globals.should_choke_packet = false;
		g_ctx.globals.should_send_packet = true;
		g_ctx.send_packet = false;
	}

	if (!g_ctx.globals.weapon->is_non_aim())
	{
		auto double_tap_aim_check = false;

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.double_tap_aim_check)
		{
			double_tap_aim_check = true;
			g_ctx.globals.double_tap_aim_check = false;
		}

		auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
		{
			static auto weapon_recoil_scale = m_cvar()->FindVar(crypt_str("weapon_recoil_scale"));

			if (g_cfg.ragebot.enable)
				m_pcmd->m_viewangles -= g_ctx.local()->m_aimPunchAngle() * weapon_recoil_scale->GetFloat();

			if (!g_ctx.globals.fakeducking)
			{
				g_ctx.globals.force_send_packet = true;
				g_ctx.globals.should_choke_packet = true;
				g_ctx.send_packet = true;
				fakelag::get().started_peeking = false;
			}

			aim::get().last_shoot_position = g_ctx.globals.eye_pos;

			if (!double_tap_aim_check)
				g_ctx.globals.double_tap_aim = false;
		}
	}
	else if (!g_ctx.globals.fakeducking && g_ctx.globals.weapon->is_knife() && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2))
	{
		g_ctx.globals.force_send_packet = true;
		g_ctx.globals.should_choke_packet = true;
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}

	if (g_ctx.globals.fakeducking)
		g_ctx.globals.force_send_packet = g_ctx.send_packet;

	for (auto& backup : aim::get().backup)
		backup.adjust_player();

	auto backup_ticks_allowed = g_ctx.globals.ticks_allowed;

	if (misc::get().double_tap(m_pcmd))
		misc::get().hide_shots(m_pcmd, false);
	else
	{
		g_ctx.globals.ticks_allowed = backup_ticks_allowed;
		misc::get().hide_shots(m_pcmd, true);
	}

	if (!g_ctx.globals.weapon->is_non_aim())
	{
		auto double_tap_aim_check = false;

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.double_tap_aim_check)
		{
			double_tap_aim_check = true;
			g_ctx.globals.double_tap_aim_check = false;
		}

		auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);

		if (!double_tap_aim_check && m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
			g_ctx.globals.double_tap_aim = false;
	}

	if (m_globals()->m_tickcount - g_ctx.globals.last_aimbot_shot > 16)
	{
		g_ctx.globals.double_tap_aim = false;
		g_ctx.globals.double_tap_aim_check = false;
	}

	engineprediction::get().finish();

	if (g_ctx.globals.loaded_script)
		for (auto current : c_lua::get().hooks.getHooks(crypt_str("on_createmove")))
			current.func();

	if (g_cfg.misc.anti_untrusted)
		math::normalize_angles(m_pcmd->m_viewangles);
	else
	{
		m_pcmd->m_viewangles.y = math::normalize_yaw(m_pcmd->m_viewangles.y);
		m_pcmd->m_viewangles.z = 0.0f;
	}

	util::movement_fix(wish_angle, m_pcmd);
	
	if (should_recharge)
		g_ctx.send_packet = true;

	static auto previous_ticks_allowed = g_ctx.globals.ticks_allowed;

	if (g_ctx.send_packet && m_clientstate()->pNetChannel)
	{
		auto choked_packets = m_clientstate()->pNetChannel->m_nChokedPackets;

		if (choked_packets >= 0)
		{
			auto ticks_allowed = g_ctx.globals.ticks_allowed;
			auto command_number = m_pcmd->m_command_number - choked_packets;

			do
			{
				auto command = &m_input()->m_pCommands[m_pcmd->m_command_number - MULTIPLAYER_BACKUP * (command_number / MULTIPLAYER_BACKUP) - choked_packets];

				if (!command || command->m_tickcount > m_globals()->m_tickcount + 72)
				{
					if (--ticks_allowed < 0)
						ticks_allowed = 0;

					g_ctx.globals.ticks_allowed = ticks_allowed;
				}

				++command_number;
				--choked_packets;
			} while (choked_packets >= 0);
		}
	}

	if (g_ctx.globals.ticks_allowed > 17)
		g_ctx.globals.ticks_allowed = math::clamp(g_ctx.globals.ticks_allowed - 1, 0, 17);

	if (previous_ticks_allowed && !g_ctx.globals.ticks_allowed)
		g_ctx.globals.ticks_choke = 16;

	previous_ticks_allowed = g_ctx.globals.ticks_allowed;

	if (g_ctx.globals.ticks_choke)
	{
		g_ctx.send_packet = g_ctx.globals.force_send_packet;
		--g_ctx.globals.ticks_choke;
	}

	auto& correct = g_ctx.globals.data.emplace_front();

	correct.command_number = m_pcmd->m_command_number;
	correct.choked_commands = m_clientstate()->iChokedCommands + 1;
	correct.tickcount = m_globals()->m_tickcount;

	if (g_ctx.send_packet)
		g_ctx.globals.choked_number.clear();
	else
		g_ctx.globals.choked_number.emplace_back(correct.command_number);

	while (g_ctx.globals.data.size() > (int)(2.0f / m_globals()->m_intervalpertick))
		g_ctx.globals.data.pop_back();

	auto& out = g_ctx.globals.commands.emplace_back();

	out.is_outgoing = g_ctx.send_packet;
	out.is_used = false;
	out.command_number = m_pcmd->m_command_number;
	out.previous_command_number = 0;

	while (g_ctx.globals.commands.size() > (int)(1.0f / m_globals()->m_intervalpertick))
		g_ctx.globals.commands.pop_front();

	if (!g_ctx.send_packet && !m_gamerules()->m_bIsValveDS())
	{
		auto net_channel = m_clientstate()->pNetChannel;

		if (net_channel->m_nChokedPackets > 0)
		{
			const auto backup_choke = net_channel->m_nChokedPackets;
			net_channel->m_nChokedPackets = 0;

			net_channel->send_datagram();
			--net_channel->m_nOutSequenceNr;

			net_channel->m_nChokedPackets = backup_choke;
		}
	}

	if (g_ctx.send_packet && !g_ctx.globals.should_send_packet && (!g_ctx.globals.should_choke_packet || (!misc::get().hide_shots_enabled && !g_ctx.globals.double_tap_fire)))
	{
		local_animations::get().local_data.fake_angles = m_pcmd->m_viewangles;
		local_animations::get().local_data.real_angles = local_animations::get().local_data.stored_real_angles;
	}

	if (!antiaim::get().breaking_lby)
		local_animations::get().local_data.stored_real_angles = m_pcmd->m_viewangles;

	if (g_ctx.send_packet && g_ctx.globals.should_send_packet)
		g_ctx.globals.should_send_packet = false;

	if (g_cfg.misc.buybot_enable && g_ctx.globals.should_buy)
	{
		--g_ctx.globals.should_buy;

		if (!g_ctx.globals.should_buy)
		{
			std::string buy;

			switch (g_cfg.misc.buybot1)
			{
			case 1:
				buy += crypt_str("buy g3sg1; ");
				break;
			case 2:
				buy += crypt_str("buy awp; ");
				break;
			case 3:
				buy += crypt_str("buy ssg08; ");
				break;
			}

			switch (g_cfg.misc.buybot2)
			{
			case 1:
				buy += crypt_str("buy elite; ");
				break;
			case 2:
				buy += crypt_str("buy deagle; buy revolver; ");
				break;
			}

			if (g_cfg.misc.buybot3[BUY_ARMOR])
				buy += crypt_str("buy vesthelm; buy vest; ");

			if (g_cfg.misc.buybot3[BUY_TASER])
				buy += crypt_str("buy taser; ");

			if (g_cfg.misc.buybot3[BUY_GRENADES])
				buy += crypt_str("buy molotov; buy hegrenade; buy smokegrenade; buy flashbang; buy flashbang; buy decoy; ");

			if (g_cfg.misc.buybot3[BUY_DEFUSER])
				buy += crypt_str("buy defuser; ");

			m_engine()->ExecuteClientCmd(buy.c_str());
		}
	}

	g_ctx.globals.in_createmove = false;

	*(bool*)(*frame_ptr - 0x1C) = g_ctx.send_packet;
	return false;
}


bool __stdcall hooks::hl_create_move(int sequence_number, float input_sample_frametime, bool active, bool& sendpacket)
{
	ocreate_move_t(m_client(), sequence_number, input_sample_frametime, active);
	auto m_pcmd = m_input()->GetUserCmd(sequence_number);
	auto verified_cmd = m_input()->GetVerifiedUserCmd(sequence_number);
	if (!m_pcmd || !m_pcmd->m_command_number || !m_enginetool()->IsInGame())
		return ocreate_move_t(m_client(), sequence_number, input_sample_frametime, active);





	g_ctx.local((player_t*)m_entitylist()->GetClientEntity(m_engine()->GetLocalPlayer()), true);

	g_ctx.globals.in_createmove = false;


    auto original_result = ocreate_move_t(m_client(), sequence_number, input_sample_frametime, active);

	if (original_result)
	{
		m_prediction()->SetLocalViewAngles(m_pcmd->m_viewangles);
		m_engine()->SetViewAngles(m_pcmd->m_viewangles);
	}

	if (!g_ctx.available())
		return ocreate_move_t(m_client(), sequence_number, input_sample_frametime, active);

	misc::get().rank_reveal();
	spammers::get().clan_tag();

	if (!g_ctx.local()->is_alive()) //-V807
		return ocreate_move_t(m_client(), sequence_number, input_sample_frametime, active);

	g_ctx.globals.weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!g_ctx.globals.weapon)
		return ocreate_move_t(m_client(), sequence_number, input_sample_frametime, active);

	g_ctx.globals.in_createmove = true;
	g_ctx.set_command(m_pcmd);

	if (menu_open && g_ctx.globals.focused_on_input)
	{
		m_pcmd->m_buttons = 0;
		m_pcmd->m_forwardmove = 0.0f;
		m_pcmd->m_sidemove = 0.0f;
		m_pcmd->m_upmove = 0.0f;
	}

	static auto should_recharge = false;

	if (should_recharge)
	{
		++g_ctx.globals.ticks_allowed;

		m_pcmd->m_tickcount = INT_MAX;
		m_pcmd->m_forwardmove = 0.0f;
		m_pcmd->m_sidemove = 0.0f;
		m_pcmd->m_upmove = 0.0f;
		m_pcmd->m_buttons &= ~IN_ATTACK;
		m_pcmd->m_buttons &= ~IN_ATTACK2;

		if (g_ctx.globals.ticks_allowed >= 16)
		{
			should_recharge = false;
			sendpacket = true;
		}
		else
			sendpacket = false;

		return false;
	}

	if (g_ctx.globals.ticks_allowed < 16 && (misc::get().double_tap_enabled && misc::get().double_tap_key || misc::get().hide_shots_enabled && misc::get().hide_shots_key))
		should_recharge = true;

	g_ctx.globals.backup_tickbase = g_ctx.local()->m_nTickBase();

	if (g_ctx.globals.next_tickbase_shift)
		g_ctx.globals.fixed_tickbase = g_ctx.local()->m_nTickBase() - g_ctx.globals.next_tickbase_shift;
	else
		g_ctx.globals.fixed_tickbase = g_ctx.globals.backup_tickbase;

	if (menu_open)
	{
		m_pcmd->m_buttons &= ~IN_ATTACK;
		m_pcmd->m_buttons &= ~IN_ATTACK2;
	}

	if (m_pcmd->m_buttons & IN_ATTACK2 && g_cfg.ragebot.enable && g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
		m_pcmd->m_buttons &= ~IN_ATTACK2;

	if (g_cfg.ragebot.enable && !g_ctx.globals.weapon->can_fire(true))
	{
		if (m_pcmd->m_buttons & IN_ATTACK && !g_ctx.globals.weapon->is_non_aim() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
			m_pcmd->m_buttons &= ~IN_ATTACK;
		else if ((m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2) && (g_ctx.globals.weapon->is_knife() || g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER) && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_HEALTHSHOT)
		{
			if (m_pcmd->m_buttons & IN_ATTACK)
				m_pcmd->m_buttons &= ~IN_ATTACK;

			if (m_pcmd->m_buttons & IN_ATTACK2)
				m_pcmd->m_buttons &= ~IN_ATTACK2;
		}
	}

	if (m_pcmd->m_buttons & IN_FORWARD && m_pcmd->m_buttons & IN_BACK)
	{
		m_pcmd->m_buttons &= ~IN_FORWARD;
		m_pcmd->m_buttons &= ~IN_BACK;
	}

	if (m_pcmd->m_buttons & IN_MOVELEFT && m_pcmd->m_buttons & IN_MOVERIGHT)
	{
		m_pcmd->m_buttons &= ~IN_MOVELEFT;
		m_pcmd->m_buttons &= ~IN_MOVERIGHT;
	}

	g_ctx.send_packet = true;
	g_ctx.globals.tickbase_shift = 0;
	g_ctx.globals.double_tap_fire = false;
	g_ctx.globals.force_send_packet = false;
	g_ctx.globals.exploits = misc::get().double_tap_key || misc::get().hide_shots_key;
	g_ctx.globals.current_weapon = g_ctx.globals.weapon->get_weapon_group(g_cfg.ragebot.enable);
	g_ctx.globals.slowwalking = false;
	g_ctx.globals.original_forwardmove = m_pcmd->m_forwardmove;
	g_ctx.globals.original_sidemove = m_pcmd->m_sidemove;

	antiaim::get().breaking_lby = false;


	auto wish_angle = m_pcmd->m_viewangles;

	

	if (g_cfg.esp.thunder)
	{
		static clock_t start_t = clock();

		double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;

		static int choose;

		if (timeSoFar > 6.0)
		{
			switch (choose)
			{
			case 1:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder4.wav"); break;
			case 2:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder5.wav"); break;
			case 3:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder6.wav"); break;
			case 4:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder_distant_01.wav"); break;
			case 5:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder_distant_02.wav"); break;
			case 6:m_engine()->ExecuteClientCmd("play ambient\\playonce\\weather\\thunder_distant_06.wav"); break;
			}
			start_t = clock();
		}
		else
		{
			int randomnum = 1 + (rand() % 6);

			if (randomnum == choose)
				choose = 1 + (rand() % 6); // could still be the same number but less likely
			else
				choose = randomnum;
		}
	}

	misc::get().fast_stop(m_pcmd);

	if (g_cfg.misc.bunnyhop)
		bunnyhop::get().create_move();

	misc::get().SlideWalk(m_pcmd);

	misc::get().NoDuck(m_pcmd);

	misc::get().AutoCrouch(m_pcmd);

	GrenadePrediction::get().Tick(m_pcmd->m_buttons);

	if (g_cfg.misc.crouch_in_air && !(g_ctx.local()->m_fFlags() & FL_ONGROUND))
		m_pcmd->m_buttons |= IN_DUCK;

	engineprediction::get().prediction_data.reset(); //-V807
	engineprediction::get().setup();

	engineprediction::get().predict(m_pcmd);
	local_animations::get().update_prediction_animations();

	g_ctx.globals.eye_pos = g_ctx.local()->get_shoot_position();

	if (g_cfg.misc.airstrafe)
		airstrafe::get().create_move(m_pcmd);

	if (g_cfg.misc.check_for_null_strafes)
		misc::get().null_strafes(m_pcmd);

	if (key_binds::get().get_key_bind_state(25)) //-V807
		misc::get().blockbot(m_pcmd);

	if (key_binds::get().get_key_bind_state(19) && engineprediction::get().backup_data.flags & FL_ONGROUND && !(g_ctx.local()->m_fFlags() & FL_ONGROUND)) //-V807
		m_pcmd->m_buttons |= IN_JUMP;

	if (key_binds::get().get_key_bind_state(23)) //-V807
		misc::get().edgebug(m_pcmd);

	if (key_binds::get().get_key_bind_state(24)) //-V807
		misc::get().jumpbug(m_pcmd);

	if (key_binds::get().get_key_bind_state(21))
		slowwalk::get().create_move(m_pcmd);



	if (g_cfg.ragebot.enable && !g_ctx.globals.weapon->is_non_aim() && engineprediction::get().backup_data.flags & FL_ONGROUND && g_ctx.local()->m_fFlags() & FL_ONGROUND)
		slowwalk::get().create_move(m_pcmd, 0.95f + 0.003125f * (16 - m_clientstate()->iChokedCommands));

	if (!should_recharge)
		fakelag::get().Createmove();

	g_ctx.globals.aimbot_working = false;
	g_ctx.globals.revolver_working = false;

	auto backup_velocity = g_ctx.local()->m_vecVelocity();
	auto backup_abs_velocity = g_ctx.local()->m_vecAbsVelocity();

	g_ctx.local()->m_vecVelocity() = engineprediction::get().backup_data.velocity;
	g_ctx.local()->m_vecAbsVelocity() = engineprediction::get().backup_data.velocity;

	g_ctx.globals.weapon->update_accuracy_penality();

	g_ctx.local()->m_vecVelocity() = backup_velocity;
	g_ctx.local()->m_vecAbsVelocity() = backup_abs_velocity;

	g_ctx.globals.inaccuracy = g_ctx.globals.weapon->get_inaccuracy();
	g_ctx.globals.spread = g_ctx.globals.weapon->get_spread();

	aim::get().run(m_pcmd);
	legit_bot::get().createmove(m_pcmd);
	if (g_cfg.legitbot.backtrackl) {
		NewBacktrack::Get().LegitBacktrack(m_pcmd);
	}

	zeusbot::get().run(m_pcmd);
	knifebot::get().run(m_pcmd);

	misc::get().automatic_peek(m_pcmd, wish_angle.y);

	antiaim::get().desync_angle = 0.0f;
	antiaim::get().create_move(m_pcmd);

	if (m_gamerules()->m_bIsValveDS() && m_clientstate()->iChokedCommands >= 6) //-V648
	{
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}
	else if (!m_gamerules()->m_bIsValveDS() && m_clientstate()->iChokedCommands >= 16) //-V648
	{
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}

	if (g_ctx.globals.should_send_packet)
	{
		g_ctx.globals.force_send_packet = true;
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}

	if (g_ctx.globals.should_choke_packet)
	{
		g_ctx.globals.should_choke_packet = false;
		g_ctx.globals.should_send_packet = true;
		g_ctx.send_packet = false;
	}

	if (!g_ctx.globals.weapon->is_non_aim())
	{
		auto double_tap_aim_check = false;

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.double_tap_aim_check)
		{
			double_tap_aim_check = true;
			g_ctx.globals.double_tap_aim_check = false;
		}

		auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
		{
			static auto weapon_recoil_scale = m_cvar()->FindVar(crypt_str("weapon_recoil_scale"));

			if (g_cfg.ragebot.enable)
				m_pcmd->m_viewangles -= g_ctx.local()->m_aimPunchAngle() * weapon_recoil_scale->GetFloat();

			if (!g_ctx.globals.fakeducking)
			{
				g_ctx.globals.force_send_packet = true;
				g_ctx.globals.should_choke_packet = true;
				g_ctx.send_packet = true;
				fakelag::get().started_peeking = false;
			}

			aim::get().last_shoot_position = g_ctx.globals.eye_pos;

			if (!double_tap_aim_check)
				g_ctx.globals.double_tap_aim = false;
		}
	}
	else if (!g_ctx.globals.fakeducking && g_ctx.globals.weapon->is_knife() && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2))
	{
		g_ctx.globals.force_send_packet = true;
		g_ctx.globals.should_choke_packet = true;
		g_ctx.send_packet = true;
		fakelag::get().started_peeking = false;
	}

	if (g_ctx.globals.fakeducking)
		g_ctx.globals.force_send_packet = g_ctx.send_packet;

	for (auto& backup : aim::get().backup)
		backup.adjust_player();

	auto backup_ticks_allowed = g_ctx.globals.ticks_allowed;

	if (misc::get().double_tap(m_pcmd))
		misc::get().hide_shots(m_pcmd, false);
	else
	{
		g_ctx.globals.ticks_allowed = backup_ticks_allowed;
		misc::get().hide_shots(m_pcmd, true);
	}

	if (!g_ctx.globals.weapon->is_non_aim())
	{
		auto double_tap_aim_check = false;

		if (m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.double_tap_aim_check)
		{
			double_tap_aim_check = true;
			g_ctx.globals.double_tap_aim_check = false;
		}

		auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);

		if (!double_tap_aim_check && m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
			g_ctx.globals.double_tap_aim = false;
	}

	if (m_globals()->m_tickcount - g_ctx.globals.last_aimbot_shot > 16) //-V807
	{
		g_ctx.globals.double_tap_aim = false;
		g_ctx.globals.double_tap_aim_check = false;
	}

	engineprediction::get().finish();

	if (g_ctx.globals.loaded_script)
		for (auto current : c_lua::get().hooks.getHooks(crypt_str("on_createmove")))
			current.func();

	if (g_cfg.misc.anti_untrusted)
		math::normalize_angles(m_pcmd->m_viewangles);
	else
	{
		m_pcmd->m_viewangles.y = math::normalize_yaw(m_pcmd->m_viewangles.y);
		m_pcmd->m_viewangles.z = 0.0f;
	}

	util::movement_fix(wish_angle, m_pcmd);

	if (should_recharge)
		g_ctx.send_packet = true;

	static auto previous_ticks_allowed = g_ctx.globals.ticks_allowed;

	if (g_ctx.send_packet && m_clientstate()->pNetChannel)
	{
		auto choked_packets = m_clientstate()->pNetChannel->m_nChokedPackets;

		if (choked_packets >= 0)
		{
			auto ticks_allowed = g_ctx.globals.ticks_allowed;
			auto command_number = m_pcmd->m_command_number - choked_packets;

			do
			{
				auto command = &m_input()->m_pCommands[m_pcmd->m_command_number - MULTIPLAYER_BACKUP * (command_number / MULTIPLAYER_BACKUP) - choked_packets];

				if (!command || command->m_tickcount > m_globals()->m_tickcount + 72)
				{
					if (--ticks_allowed < 0)
						ticks_allowed = 0;

					g_ctx.globals.ticks_allowed = ticks_allowed;
				}

				++command_number;
				--choked_packets;
			} while (choked_packets >= 0);
		}
	}

	if (g_ctx.globals.ticks_allowed > 17)
		g_ctx.globals.ticks_allowed = math::clamp(g_ctx.globals.ticks_allowed - 1, 0, 17);

	if (previous_ticks_allowed && !g_ctx.globals.ticks_allowed)
		g_ctx.globals.ticks_choke = 16;

	previous_ticks_allowed = g_ctx.globals.ticks_allowed;

	if (g_ctx.globals.ticks_choke)
	{
		g_ctx.send_packet = g_ctx.globals.force_send_packet;
		--g_ctx.globals.ticks_choke;
	}

	auto& correct = g_ctx.globals.data.emplace_front();

	correct.command_number = m_pcmd->m_command_number;
	correct.choked_commands = m_clientstate()->iChokedCommands + 1;
	correct.tickcount = m_globals()->m_tickcount;

	if (g_ctx.send_packet)
		g_ctx.globals.choked_number.clear();
	else
		g_ctx.globals.choked_number.emplace_back(correct.command_number);

	while (g_ctx.globals.data.size() > (int)(2.0f / m_globals()->m_intervalpertick))
		g_ctx.globals.data.pop_back();

	auto& out = g_ctx.globals.commands.emplace_back();

	out.is_outgoing = g_ctx.send_packet;
	out.is_used = false;
	out.command_number = m_pcmd->m_command_number;
	out.previous_command_number = 0;

	while (g_ctx.globals.commands.size() > (int)(1.0f / m_globals()->m_intervalpertick))
		g_ctx.globals.commands.pop_front();

	if (!g_ctx.send_packet && !m_gamerules()->m_bIsValveDS())
	{
		auto net_channel = m_clientstate()->pNetChannel;

		if (net_channel->m_nChokedPackets > 0 && !(net_channel->m_nChokedPackets % 4))
		{
			auto backup_choke = net_channel->m_nChokedPackets;
			net_channel->m_nChokedPackets = 0;

			net_channel->send_datagram();
			--net_channel->m_nOutSequenceNr;

			net_channel->m_nChokedPackets = backup_choke;
		}
	}

	if (g_ctx.send_packet && !g_ctx.globals.should_send_packet && (!g_ctx.globals.should_choke_packet || (!misc::get().hide_shots_enabled && !g_ctx.globals.double_tap_fire)))
	{
		local_animations::get().local_data.fake_angles = m_pcmd->m_viewangles; //-V807
		local_animations::get().local_data.real_angles = local_animations::get().local_data.stored_real_angles;
	}

	if (!antiaim::get().breaking_lby)
		local_animations::get().local_data.stored_real_angles = m_pcmd->m_viewangles;

	if (g_ctx.send_packet && g_ctx.globals.should_send_packet)
		g_ctx.globals.should_send_packet = false;

	if (g_cfg.misc.buybot_enable && g_ctx.globals.should_buy)
	{
		--g_ctx.globals.should_buy;

		if (!g_ctx.globals.should_buy)
		{
			std::string buy;

			switch (g_cfg.misc.buybot1)
			{
			case 1:
				buy += crypt_str("buy g3sg1; ");
				break;
			case 2:
				buy += crypt_str("buy awp; ");
				break;
			case 3:
				buy += crypt_str("buy ssg08; ");
				break;
			}

			switch (g_cfg.misc.buybot2)
			{
			case 1:
				buy += crypt_str("buy elite; ");
				break;
			case 2:
				buy += crypt_str("buy deagle; buy revolver; ");
				break;
			}

			if (g_cfg.misc.buybot3[BUY_ARMOR])
				buy += crypt_str("buy vesthelm; buy vest; ");

			if (g_cfg.misc.buybot3[BUY_TASER])
				buy += crypt_str("buy taser; ");

			if (g_cfg.misc.buybot3[BUY_GRENADES])
				buy += crypt_str("buy molotov; buy hegrenade; buy smokegrenade; buy flashbang; buy flashbang; buy decoy; ");

			if (g_cfg.misc.buybot3[BUY_DEFUSER])
				buy += crypt_str("buy defuser; ");

			m_engine()->ExecuteClientCmd(buy.c_str());
		}
	}

	g_ctx.globals.in_createmove = false;

	sendpacket = g_ctx.send_packet;








	verified_cmd->m_cmd = *m_pcmd;
	verified_cmd->m_crc = m_pcmd->GetChecksum();
	return false;
}