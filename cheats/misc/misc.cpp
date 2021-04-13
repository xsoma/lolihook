// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "misc.h"
#include "fakelag.h"
#include "..\ragebot\aim.h"
#include "..\visuals\world_esp.h"
#include "prediction_system.h"
#include "logs.h"
#include "../../sdk/Memory.h"
#include "../visuals/hitchams.h"
#include "..\utils\draw_manager.h"

#if RELEASE
#if BETA
#define VERSION crypt_str("Kitsune [beta] | ")
#else
#define VERSION crypt_str("LoliHook | ")
#endif
#else
#define VERSION crypt_str("Kitsune [alpha] | ")
#endif
int pre_flags = 0;

void misc::edgebug(CUserCmd* pCmd) {

	if (g_ctx.local()->get_move_type() == MOVETYPE_LADDER || g_ctx.local()->get_move_type() == MOVETYPE_NOCLIP) return;

	if (engineprediction::get().backup_data.flags & FL_ONGROUND && !(g_ctx.local()->m_fFlags() & FL_ONGROUND))
		pCmd->m_buttons |= IN_DUCK;

}
bool jumpbugged = false;

void misc::jumpbug(CUserCmd* pCmd) {

	auto poop = g_ctx.local()->m_fFlags();
	bool unduck = false;
	static bool niggacheck = false;

	if ((g_ctx.local()->m_fFlags() & (1 << 0)) && !(engineprediction::get().backup_data.flags & (1 << 0))) {

		if (g_cfg.misc.bunnyhop) {
			g_cfg.misc.bunnyhop = false;
			niggacheck = true;
		}

		if (unduck) {
			pCmd->m_buttons &= ~IN_DUCK;
			pCmd->m_buttons |= IN_JUMP;
			unduck = false;
			jumpbugged = true;
		}

		pCmd->m_buttons |= IN_DUCK;
		pCmd->m_buttons &= ~IN_JUMP;
		unduck = true;
		jumpbugged = false;
	}
	else if (niggacheck) {
		g_cfg.misc.bunnyhop = true;
		niggacheck = false;
	}

}

const std::string t() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%X", &tstruct);

	return buf;
}
void misc::null_strafes(CUserCmd* pCmd)
{
	if (!g_ctx.local() ||
		g_ctx.local()->m_fFlags() & FL_ONGROUND)
		return;


	if (pCmd->m_mousedx > 0 && pCmd->m_buttons & IN_MOVERIGHT && pCmd->m_buttons & IN_MOVELEFT)
		pCmd->m_sidemove = -450.f;
	else if (pCmd->m_mousedx < 0 && pCmd->m_buttons & IN_MOVELEFT && pCmd->m_buttons & IN_MOVERIGHT)
		pCmd->m_sidemove = 450.f;
}
void misc::watermark()
{
	if (!g_cfg.menu.watermark)
		return;

	auto screen_x = 0;
	auto screen_y = 0;
	m_engine()->GetScreenSize(screen_x, screen_y); //-V807

	int fps = g_ctx.globals.framerate;
	auto get_fps = crypt_str("") + std::to_string(fps);
	if (fps < 10)
		get_fps = crypt_str("   ") + std::to_string(fps);
	else if (fps < 100)
		get_fps = crypt_str("  ") + std::to_string(fps);
	int ping = g_ctx.globals.ping;
	auto get_ping = crypt_str("") + std::to_string(ping);
	if (ping < 10)
		get_ping = crypt_str(" ") + std::to_string(ping);
	auto watermark = crypt_str("FPS: ") + get_fps + crypt_str("   |   PING: ") + get_ping + crypt_str("   |   ") + g_ctx.globals.time;

	render::get().rect_filled(screen_x - 255, 5, 250, 70, Color(30, 30, 30));
	render::get().rect(screen_x - 255, 5, 250, 70, Color(15, 15, 15));
	render::get().rect_filled(screen_x - 245, 15, 230, 50, Color(20, 20, 20));
	render::get().rect(screen_x - 245, 15, 230, 50, Color(15, 15, 15));
	render::get().gradient(screen_x - 244, 38, 228, 1, Color(141, 55, 221), Color(89, 35, 140), GRADIENT_HORIZONTAL);
	render::get().text(fonts[NAME], screen_x - 159, 28, Color(0, 0, 0), HFONT_CENTERED_Y, crypt_str("LOLIHOOK.TOP"));
	render::get().text(fonts[NAME], screen_x - 160, 27, Color(255, 255, 255), HFONT_CENTERED_Y, crypt_str("LOLIHOOK.TOP"));
	render::get().text(fonts[NAME], screen_x - 236, 50, Color(255, 255, 255, 220), HFONT_CENTERED_Y, watermark.c_str());
}

void misc::get_keys()
{
	if (!(g_cfg.esp.keybind_list && m_engine()->IsInGame()))
		return;
	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (g_cfg.ragebot.double_tap && g_cfg.ragebot.double_tap_key.key > KEY_NONE && g_cfg.ragebot.double_tap_key.key < KEY_MAX && misc::get().double_tap_key) // Here your config bool
	{
		switch (g_cfg.ragebot.double_tap_key.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Double tap"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Double tap"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(0))
	{
		switch (g_cfg.legitbot.autofire_key.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Trigger bot"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Trigger bot"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(19))
	{
		switch (g_cfg.misc.edge_jump.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Edge jumpp"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Edge jump"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (g_cfg.antiaim.hide_shots && g_cfg.antiaim.hide_shots_key.key > KEY_NONE && g_cfg.antiaim.hide_shots_key.key < KEY_MAX && misc::get().hide_shots_key) // Here your config bool
	{
		switch (g_cfg.antiaim.hide_shots_key.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Hide shots"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Hide shots"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(20)) // Here your config bool
	{
		switch (g_cfg.misc.fakeduck_key.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Fake duck"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Fake duck"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(3)) // Here your config bool
	{
		switch (g_cfg.ragebot.safe_point_key.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Safe points"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Safe points"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(18)) // Here your config bool
	{
		switch (g_cfg.misc.automatic_peek.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Auto peek"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Auto peek"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(17)) // Here your config bool
	{
		switch (g_cfg.misc.thirdperson_toggle.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Third person"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Third person"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(16)) // Here your config bool
	{
		switch (g_cfg.antiaim.flip_desync.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Desync invert"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Desync invert"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(21))
	{
		switch (g_cfg.misc.slowwalk_key.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Slow walk"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Slow walk"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(22))
	{
		switch (g_cfg.ragebot.body_aim_key.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Body aim"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Body aim"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(23))
	{
		switch (g_cfg.misc.edgebug.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Edge bug"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Edge bug"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(24))
	{
		switch (g_cfg.misc.jumpbug.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Jump bug"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Jump bug"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(25))
	{
		switch (g_cfg.misc.block_bot.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Block bot"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Block bot"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(26))
	{
		switch (g_cfg.antiaim.freestanding_key.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Freestanding"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Freestanding"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(27))
	{
		switch (g_cfg.ragebot.onshot.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Force head"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Force head"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (key_binds::get().get_key_bind_state(28))
	{
		switch (g_cfg.misc.walkbot_key.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Walk bot"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Walk bot"), crypt_str("[TOGGLE]")));
			break;
		}
	}

	if (g_ctx.globals.current_weapon != -1 && key_binds::get().get_key_bind_state(4 + g_ctx.globals.current_weapon) && !weapon->is_non_aim())
	{
		switch (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].damage_override_key.mode)
		{
		case HOLD:
			m_keybinds.push_back(m_keybind(crypt_str("Override"), crypt_str("[HOLD]")));
			break;
		case TOGGLE:
			m_keybinds.push_back(m_keybind(crypt_str("Override"), crypt_str("[TOGGLE]")));
			break;
		}
	}
}

void misc::keybind_list() {
	if (!(g_cfg.esp.keybind_list && m_engine()->IsInGame()))
		return;

	static int spec_x;
	static int spec_y;

	static int _drag_x = 300;
	static int _drag_y = 300;

	static bool _dragging = false;
	bool _click = false;

	if (GetAsyncKeyState(VK_LBUTTON))
		_click = true;

	Vector2D _mouse_pos = m_surface()->GetMousePosition();

	if (_dragging && !_click)
		_dragging = false;

	if (_dragging && _click)
	{
		g_cfg.esp.keybinds_pos_x = _mouse_pos.x - _drag_x;
		g_cfg.esp.keybinds_pos_y = _mouse_pos.y - _drag_y;
		spec_x = g_cfg.esp.keybinds_pos_x;
		spec_y = g_cfg.esp.keybinds_pos_y;
	}


	int spec_width = 180;
	int spec_height = 25;

	if ((m_surface()->MouseInRegion(g_cfg.esp.keybinds_pos_x, g_cfg.esp.keybinds_pos_y, spec_width, spec_height)))
	{
		_dragging = true;
		_drag_x = _mouse_pos.x - g_cfg.esp.keybinds_pos_x;
		_drag_y = _mouse_pos.y - g_cfg.esp.keybinds_pos_y;
	}

	int h = 0;
	int h2 = 0;
	for (auto& keybind : m_keybinds)
	{
		h2 += 12;

	}
	Color gradient1 = Color(141, 55, 221);
	Color gradient2 = Color(89, 35, 140);
	int a;
	int h4;
	if (h2 < 1)
	{
		a = 0;
		h4 = 0;
	}
	else
	{
		a = 245;
		h4 = 15;
	}
	render::get().rect_filled(g_cfg.esp.keybinds_pos_x, g_cfg.esp.keybinds_pos_y, spec_width, h2 + spec_height + h4, Color(30, 30, 30, 245));
	render::get().rect(g_cfg.esp.keybinds_pos_x, g_cfg.esp.keybinds_pos_y, spec_width, h2 + spec_height + h4, Color(15, 15, 15, 245));
	render::get().gradient(g_cfg.esp.keybinds_pos_x, g_cfg.esp.keybinds_pos_y + 24, spec_width, spec_height - 24, gradient1, gradient2, GRADIENT_HORIZONTAL);
	auto text_size = render::get().text_width(fonts[RADIX_FONT], "Keybinds");
	render::get().text(fonts[RADIX_FONT], g_cfg.esp.keybinds_pos_x + (spec_width - text_size) / 2, g_cfg.esp.keybinds_pos_y + 12, Color(255, 255, 255), HFONT_CENTERED_Y, "Keybinds");
	render::get().rect_filled(g_cfg.esp.keybinds_pos_x + 5, g_cfg.esp.keybinds_pos_y + 30, spec_width - 10, h2 + 5, Color(20, 20, 20, a));
	render::get().rect(g_cfg.esp.keybinds_pos_x + 5, g_cfg.esp.keybinds_pos_y + 30, spec_width - 10, h2 + 5, Color(15, 15, 15, a));
	for (auto& keybind : m_keybinds)
	{
		render::get().text(fonts[RADIX_FONT], g_cfg.esp.keybinds_pos_x + 10, g_cfg.esp.keybinds_pos_y + 38 + h, Color(255, 255, 255), HFONT_CENTERED_Y, keybind.m_name.c_str());
		auto h3 = render::get().text_width(fonts[RADIX_FONT], keybind.m_mode.c_str());
		render::get().text(fonts[RADIX_FONT], g_cfg.esp.keybinds_pos_x + spec_width - 10 - h3, g_cfg.esp.keybinds_pos_y + 38 + h, Color(255, 255, 255), HFONT_CENTERED_Y, keybind.m_mode.c_str());
		h += 12;

	}
	m_keybinds.clear();
}

void misc::NoDuck(CUserCmd* cmd)
{
	if (!g_cfg.misc.noduck)
		return;

	if (m_gamerules()->m_bIsValveDS())
		return;

	cmd->m_buttons |= IN_BULLRUSH;
}

void misc::ChangeRegion()
{
	switch (g_cfg.misc.region_changer) {
	case 0:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster waw");
		break;
	case 1:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster atl");
		break;
	case 2:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster bom");
		break;
	case 3:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster can");
		break;
	case 4:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster canm");
		break;
	case 5:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster cant");
		break;
	case 6:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster canu");
		break;
	case 7:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster dxb");
		break;
	case 8:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster eat");
		break;
	case 9:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster fra");
		break;
	case 10:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster gru");
		break;
	case 11:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster hkg");
		break;
	case 12:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster iad");
		break;
	case 13:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster jnb");
		break;
	case 14:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lax");
		break;
	case 15:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lhr");
		break;
	case 16:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lim");
		break;
	case 17:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster lux");
		break;
	case 18:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster maa");
		break;
	case 19:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster mad");
		break;
	case 20:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster man");
		break;
	case 21:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster okc");
		break;
	case 22:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster ord");
		break;
	case 23:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster par");
		break;
	case 24:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwg");
		break;
	case 25:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwj");
		break;
	case 26:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwu");
		break;
	case 27:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pww");
		break;
	case 28:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster pwz");
		break;
	case 29:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster scl");
		break;
	case 30:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sea");
		break;
	case 31:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sgp");
		break;
	case 32:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sha");
		break;
	case 33:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sham");
		break;
	case 34:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shat");
		break;
	case 35:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shau");
		break;
	case 36:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster shb");
		break;
	case 37:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sto");
		break;
	case 38:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster sto2");
		break;
	case 39:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster syd");
		break;
	case 40:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsn");
		break;
	case 41:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnm");
		break;
	case 42:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnt");
		break;
	case 43:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tsnu");
		break;
	case 44:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tyo");
		break;
	case 45:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster tyo1");
		break;
	case 46:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster vie");
		break;
	case 47:
		m_engine()->ExecuteClientCmd("sdr SDRClient_ForceRelayCluster ams");
		break;
	}
}

void misc::ChatSpamer()
{
	if (!g_cfg.misc.chat)
		return;

	static std::string chatspam[] = 
	{ 
		crypt_str("Lolihook.top - stay with us or lose the game."),
		crypt_str("Get good. Get Lolihook.top."),
		crypt_str("Lolihook.top - just the best."),
		crypt_str("Go to another level with Lolihook.top."),
	};

	static auto lastspammed = 0;

	if (GetTickCount() - lastspammed > 800)
	{
		lastspammed = GetTickCount();

		srand(m_globals()->m_tickcount);
		std::string msg = crypt_str("say ") + chatspam[rand() % 4];

		m_engine()->ExecuteClientCmd(msg.c_str());
	}
}

void misc::AutoCrouch(CUserCmd* cmd)
{
	if (fakelag::get().condition)
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (m_gamerules()->m_bIsValveDS())
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (!key_binds::get().get_key_bind_state(20))
	{
		g_ctx.globals.fakeducking = false;
		return;
	}

	if (!g_ctx.globals.fakeducking && m_clientstate()->iChokedCommands != 7)
		return;

	if (m_clientstate()->iChokedCommands >= 7)
		cmd->m_buttons |= IN_DUCK;
	else
		cmd->m_buttons &= ~IN_DUCK;

	g_ctx.globals.fakeducking = true;
}
void misc::FakePrime() noexcept
{

	static bool lastState = false;

	if (g_cfg.misc.fake_prime != lastState) {
		lastState = g_cfg.misc.fake_prime;



		if (DWORD oldProtect; VirtualProtect(memory.fakePrime, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) {
			constexpr uint8_t patch[]{ 0x74, 0xEB };
			*memory.fakePrime = patch[g_cfg.misc.fake_prime];
			VirtualProtect(memory.fakePrime, 1, oldProtect, nullptr);
		}
	}
}
void misc::blockbot(CUserCmd* cmd)
{
	if (!g_cfg.misc.blockbot_enabled ||!key_binds::get().get_key_bind_state(25))
		return;

	float flBestDistance = 250.0f;
	int iBestIndex = -1;

	for (auto i = 1; i < m_globals()->m_maxclients; i++) {
		auto entity = (player_t*)m_entitylist()->GetClientEntity(i);

		if (!entity || entity->IsDormant() || !entity->is_alive() ||
			entity == g_ctx.local())
			continue;

		float flDistance = g_ctx.local()->m_vecOrigin().DistTo(entity->m_vecOrigin());
		if (flDistance < flBestDistance)
		{
			flBestDistance = flDistance;
			iBestIndex = i;
		}
	}

	auto entity = (player_t*)m_entitylist()->GetClientEntity(iBestIndex);

	if (!entity)
		return;

	float flBestSpeed = g_cfg.misc.blockbot_type == 0 ? entity->m_vecVelocity().Length() : 450.0f;

	Vector angLocal;
	m_engine()->GetViewAngles(angLocal);

	Vector vecForward = entity->m_vecOrigin() - g_ctx.local()->m_vecOrigin();
	if (entity->hitbox_position(6).z < g_ctx.local()->m_vecOrigin().z && g_ctx.local()->m_vecOrigin().DistTo(entity->m_vecOrigin()) < 100.0f)
	{
		cmd->m_forwardmove = ((sin(DEG2RAD(angLocal.y)) * vecForward.y) + (cos(DEG2RAD(angLocal.y)) * vecForward.x)) * flBestSpeed;
		cmd->m_sidemove = ((cos(DEG2RAD(angLocal.y)) * -vecForward.y) + (sin(DEG2RAD(angLocal.y)) * vecForward.x)) * flBestSpeed;
	}
	else
	{
		auto yaw_delta = (atan2(vecForward.y, vecForward.x) * 180.0f / M_PI) - angLocal.y;
		if (yaw_delta > 180) { yaw_delta -= 360; }
		else if (yaw_delta < -180) { yaw_delta += 360; }
		if (yaw_delta > 0.25) { cmd->m_sidemove = -flBestSpeed; }
		else if (yaw_delta < -0.25) { cmd->m_sidemove = flBestSpeed; }
	}
}
void misc::SlideWalk(CUserCmd* cmd)
{
	if (!g_ctx.local()->is_alive()) //-V807
		return;

	if (g_ctx.local()->get_move_type() == MOVETYPE_LADDER)
		return;

	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
		return;

	if (antiaim::get().condition(cmd, true) && g_cfg.misc.slidewalk)
	{
		if (cmd->m_forwardmove > 0.0f)
		{
			cmd->m_buttons |= IN_BACK;
			cmd->m_buttons &= ~IN_FORWARD;
		}
		else if (cmd->m_forwardmove < 0.0f)
		{
			cmd->m_buttons |= IN_FORWARD;
			cmd->m_buttons &= ~IN_BACK;
		}

		if (cmd->m_sidemove > 0.0f)
		{
			cmd->m_buttons |= IN_MOVELEFT;
			cmd->m_buttons &= ~IN_MOVERIGHT;
		}
		else if (cmd->m_sidemove < 0.0f)
		{
			cmd->m_buttons |= IN_MOVERIGHT;
			cmd->m_buttons &= ~IN_MOVELEFT;
		}
	}
	else
	{
		auto buttons = cmd->m_buttons & ~(IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD);

		if (g_cfg.misc.slidewalk)
		{
			if (cmd->m_forwardmove <= 0.0f)
				buttons |= IN_BACK;
			else
				buttons |= IN_FORWARD;

			if (cmd->m_sidemove > 0.0f)
				goto LABEL_15;
			else if (cmd->m_sidemove >= 0.0f)
				goto LABEL_18;

			goto LABEL_17;
		}
		else
			goto LABEL_18;

		if (cmd->m_forwardmove <= 0.0f) //-V779
			buttons |= IN_FORWARD;
		else
			buttons |= IN_BACK;

		if (cmd->m_sidemove > 0.0f)
		{
		LABEL_17:
			buttons |= IN_MOVELEFT;
			goto LABEL_18;
		}

		if (cmd->m_sidemove < 0.0f)
			LABEL_15:

		buttons |= IN_MOVERIGHT;

	LABEL_18:
		cmd->m_buttons = buttons;
	}
}

void misc::automatic_peek(CUserCmd* cmd, float wish_yaw)
{
	if (!g_ctx.globals.weapon->is_non_aim() && key_binds::get().get_key_bind_state(18))
	{
		if (g_ctx.globals.start_position.IsZero())
		{
			g_ctx.globals.start_position = g_ctx.local()->GetAbsOrigin();

			if (!(engineprediction::get().backup_data.flags & FL_ONGROUND))
			{
				Ray_t ray;
				CTraceFilterWorldAndPropsOnly filter;
				CGameTrace trace;

				ray.Init(g_ctx.globals.start_position, g_ctx.globals.start_position - Vector(0.0f, 0.0f, 1000.0f));
				m_trace()->TraceRay(ray, MASK_SOLID, &filter, &trace);
				
				if (trace.fraction < 1.0f)
					g_ctx.globals.start_position = trace.endpos + Vector(0.0f, 0.0f, 2.0f);
			}
		}
		else
		{
			auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (cmd->m_buttons & IN_ATTACK || cmd->m_buttons & IN_ATTACK2);

			if (cmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || revolver_shoot)
				g_ctx.globals.fired_shot = true;

			if (g_ctx.globals.fired_shot)
			{
				auto current_position = g_ctx.local()->GetAbsOrigin();
				auto difference = current_position - g_ctx.globals.start_position;

				if (difference.Length2D() > 5.0f)
				{
					auto velocity = Vector(difference.x * cos(wish_yaw / 180.0f * M_PI) + difference.y * sin(wish_yaw / 180.0f * M_PI), difference.y * cos(wish_yaw / 180.0f * M_PI) - difference.x * sin(wish_yaw / 180.0f * M_PI), difference.z);

					cmd->m_forwardmove = -velocity.x * 20.0f;
					cmd->m_sidemove = velocity.y * 20.0f;
				}
				else
				{
					g_ctx.globals.fired_shot = false;
					g_ctx.globals.start_position.Zero();
				}
			}
		}
	}
	else
	{
		g_ctx.globals.fired_shot = false;
		g_ctx.globals.start_position.Zero();
	}
}

void misc::ViewModel()
{
	if (g_cfg.esp.viewmodel_fov)
	{
		auto viewFOV = (float)g_cfg.esp.viewmodel_fov + 68.0f;
		static auto viewFOVcvar = m_cvar()->FindVar(crypt_str("viewmodel_fov"));

		if (viewFOVcvar->GetFloat() != viewFOV) //-V550
		{
			*(float*)((DWORD)&viewFOVcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewFOVcvar->SetValue(viewFOV);
		}
	}
	
	if (g_cfg.esp.viewmodel_x)
	{
		auto viewX = (float)g_cfg.esp.viewmodel_x / 2.0f;
		static auto viewXcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_x")); //-V807

		if (viewXcvar->GetFloat() != viewX) //-V550
		{
			*(float*)((DWORD)&viewXcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewXcvar->SetValue(viewX);
		}
	}

	if (g_cfg.esp.viewmodel_y)
	{
		auto viewY = (float)g_cfg.esp.viewmodel_y / 2.0f;
		static auto viewYcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_y"));

		if (viewYcvar->GetFloat() != viewY) //-V550
		{
			*(float*)((DWORD)&viewYcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewYcvar->SetValue(viewY);
		}
	}

	if (g_cfg.esp.viewmodel_z)
	{
		auto viewZ = (float)g_cfg.esp.viewmodel_z / 2.0f;
		static auto viewZcvar = m_cvar()->FindVar(crypt_str("viewmodel_offset_z"));

		if (viewZcvar->GetFloat() != viewZ) //-V550
		{
			*(float*)((DWORD)&viewZcvar->m_fnChangeCallbacks + 0xC) = 0.0f;
			viewZcvar->SetValue(viewZ);
		}
	}
}

void misc::FullBright()
{		
	if (!g_cfg.player.enable)
		return;

	static auto mat_fullbright = m_cvar()->FindVar(crypt_str("mat_fullbright"));

	if (mat_fullbright->GetBool() != g_cfg.esp.bright)
		mat_fullbright->SetValue(g_cfg.esp.bright);
}

void misc::PovArrows(player_t* e, Color color)
{
	auto isOnScreen = [](Vector origin, Vector& screen) -> bool
	{
		if (!math::world_to_screen(origin, screen))
			return false;

		static int iScreenWidth, iScreenHeight;
		m_engine()->GetScreenSize(iScreenWidth, iScreenHeight);

		auto xOk = iScreenWidth > screen.x; 
		auto yOk = iScreenHeight > screen.y;

		return xOk && yOk;
	};

	Vector screenPos;

	if (isOnScreen(e->GetAbsOrigin(), screenPos))
		return;

	Vector viewAngles;
	m_engine()->GetViewAngles(viewAngles);

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto screenCenter = Vector2D(width * 0.5f, height * 0.5f);
	auto angleYawRad = DEG2RAD(viewAngles.y - math::calculate_angle(g_ctx.globals.eye_pos, e->GetAbsOrigin()).y - 90.0f);

	auto radius = g_cfg.player.distance;
	auto size = g_cfg.player.size;

	auto newPointX = screenCenter.x + ((((width - (size * 3)) * 0.5f) * (radius / 100.0f)) * cos(angleYawRad)) + (int)(6.0f * (((float)size - 4.0f) / 16.0f));
	auto newPointY = screenCenter.y + ((((height - (size * 3)) * 0.5f) * (radius / 100.0f)) * sin(angleYawRad));

	std::array <Vector2D, 3> points
	{
		Vector2D(newPointX - size, newPointY - size),
		Vector2D(newPointX + size, newPointY),
		Vector2D(newPointX - size, newPointY + size)
	};

	math::rotate_triangle(points, viewAngles.y - math::calculate_angle(g_ctx.globals.eye_pos, e->GetAbsOrigin()).y - 90.0f);
	render::get().triangle(points.at(0), points.at(1), points.at(2), color);
}

void misc::zeus_range()
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.taser_range)
		return;

	if (!m_input()->m_fCameraInThirdPerson)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() != WEAPON_TASER)
		return;

	auto weapon_info = weapon->get_csweapon_info();

	if (!weapon_info)
		return;

	float circle_range = weapon_info->flRange / 2;

	auto draw_pos = g_ctx.local()->get_shoot_position();
	draw_pos.z -= 32;
	render::get().Draw3DRainbowCircle(draw_pos, circle_range);

	draw_pos.z -= 4;
	render::get().Draw3DRainbowCircle(draw_pos, circle_range);

	draw_pos.z -= 26;
	render::get().Draw3DFilledCircle(draw_pos, circle_range, Color(255, 255, 255, 120));
}

void misc::NightmodeFix()
{
	static auto in_game = false;

	if (m_engine()->IsInGame() && !in_game)
	{
		in_game = true;

		g_ctx.globals.change_materials = true;
		worldesp::get().changed = true;

		static auto skybox = m_cvar()->FindVar(crypt_str("sv_skyname"));
		worldesp::get().backup_skybox = skybox->GetString();
		return;
	}
	else if (!m_engine()->IsInGame() && in_game)
		in_game = false;

	static auto player_enable = g_cfg.player.enable;

	if (player_enable != g_cfg.player.enable)
	{
		player_enable = g_cfg.player.enable;
		g_ctx.globals.change_materials = true;
		return;
	}

	static auto setting = g_cfg.esp.nightmode;

	if (setting != g_cfg.esp.nightmode)
	{
		setting = g_cfg.esp.nightmode;
		g_ctx.globals.change_materials = true;
		return;
	}

	static auto setting_world = g_cfg.esp.world_color;

	if (setting_world != g_cfg.esp.world_color)
	{
		setting_world = g_cfg.esp.world_color;
		g_ctx.globals.change_materials = true;
		return;
	}

	static auto setting_props = g_cfg.esp.props_color;

	if (setting_props != g_cfg.esp.props_color)
	{
		setting_props = g_cfg.esp.props_color;
		g_ctx.globals.change_materials = true;
	}
}

void misc::desync_arrows()
{
	if (!g_ctx.local()->is_alive())
		return;

	if (!g_cfg.ragebot.enable)
		return;

	if (!g_cfg.antiaim.enable)
		return;

	if ((g_cfg.antiaim.manual_back.key <= KEY_NONE || g_cfg.antiaim.manual_back.key >= KEY_MAX) && (g_cfg.antiaim.manual_left.key <= KEY_NONE || g_cfg.antiaim.manual_left.key >= KEY_MAX) && (g_cfg.antiaim.manual_right.key <= KEY_NONE || g_cfg.antiaim.manual_right.key >= KEY_MAX))
		antiaim::get().manual_side = SIDE_NONE;

	if (!g_cfg.antiaim.flip_indicator)
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	static auto alpha = 1.0f;
	static auto switch_alpha = false;

	if (alpha <= 0.0f || alpha >= 1.0f)
		switch_alpha = !switch_alpha;

	alpha += switch_alpha ? 2.0f * m_globals()->m_frametime : -2.0f * m_globals()->m_frametime;
	alpha = math::clamp(alpha, 0.0f, 1.0f);

	auto color = g_cfg.antiaim.flip_indicator_color;
	color.SetAlpha((int)(min(255.0f * alpha, color.a())));

	if (antiaim::get().manual_side == SIDE_BACK)
		render::get().triangle(Vector2D(width / 2, height / 2 + 80), Vector2D(width / 2 - 10, height / 2 + 60), Vector2D(width / 2 + 10, height / 2 + 60), color);
	else if (antiaim::get().manual_side == SIDE_LEFT)
		render::get().triangle(Vector2D(width / 2 - 55, height / 2 + 10), Vector2D(width / 2 - 75, height / 2), Vector2D(width / 2 - 55, height / 2 - 10), color);
	else if (antiaim::get().manual_side == SIDE_RIGHT)
		render::get().triangle(Vector2D(width / 2 + 55, height / 2 - 10), Vector2D(width / 2 + 75, height / 2), Vector2D(width / 2 + 55, height / 2 + 10), color);
}

void misc::aimbot_hitboxes()
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.player.lag_hitbox)
		return;

	auto player = (player_t*)m_entitylist()->GetClientEntity(aim::get().last_target_index);

	if (!player)
		return;

	auto model = player->GetModel(); //-V807

	if (!model)
		return;

	auto studio_model = m_modelinfo()->GetStudioModel(model);

	if (!studio_model)
		return;

	auto hitbox_set = studio_model->pHitboxSet(player->m_nHitboxSet());

	if (!hitbox_set)
		return;

	hit_chams::get().add_matrix(player, aim::get().last_target[aim::get().last_target_index].record.matrixes_data.main);

	/*
	for (auto i = 0; i < hitbox_set->numhitboxes; i++)
	{
		auto hitbox = hitbox_set->pHitbox(i);

		if (!hitbox)
			continue;

		if (hitbox->radius == -1.0f) //-V550
			continue;

		auto min = ZERO;
		auto max = ZERO;

		math::vector_transform(hitbox->bbmin, aim::get().last_target[aim::get().last_target_index].record.matrixes_data.main[hitbox->bone], min);
		math::vector_transform(hitbox->bbmax, aim::get().last_target[aim::get().last_target_index].record.matrixes_data.main[hitbox->bone], max);

		m_debugoverlay()->AddCapsuleOverlay(min, max, hitbox->radius, g_cfg.player.lag_hitbox_color.r(), g_cfg.player.lag_hitbox_color.g(), g_cfg.player.lag_hitbox_color.b(), g_cfg.player.lag_hitbox_color.a(), 4.0f, 0, 1);
	}
	*/
}

void misc::ragdolls()
{
	if (!g_cfg.misc.ragdolls)
		return;

	for (auto i = 1; i <= m_entitylist()->GetHighestEntityIndex(); ++i)
	{
		auto e = static_cast<entity_t*>(m_entitylist()->GetClientEntity(i));

		if (!e)
			continue;

		if (e->IsDormant())
			continue;

		auto client_class = e->GetClientClass();

		if (!client_class)
			continue;

		if (client_class->m_ClassID != IdManager->ClassIdMap[FNV1A::HashConst("CCSRagdoll")])
			continue;

		auto ragdoll = (ragdoll_t*)e;
		ragdoll->m_vecForce().z = 800000.0f;
	}
}

void misc::rank_reveal()
{
	if (!g_cfg.misc.rank_reveal)
		return;

	using RankReveal_t = bool(__cdecl*)(int*);
	static auto Fn = (RankReveal_t)(util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 51 A1 ? ? ? ? 85 C0 75 37")));

	int array[3] = 
	{
		0,
		0,
		0
	};

	Fn(array);
}

void misc::fast_stop(CUserCmd* m_pcmd)
{
	if (!g_cfg.misc.fast_stop)
		return;

	if (!(g_ctx.local()->m_fFlags() & FL_ONGROUND && engineprediction::get().backup_data.flags & FL_ONGROUND))
		return;

	auto pressed_move_key = m_pcmd->m_buttons & IN_FORWARD || m_pcmd->m_buttons & IN_MOVELEFT || m_pcmd->m_buttons & IN_BACK || m_pcmd->m_buttons & IN_MOVERIGHT || m_pcmd->m_buttons & IN_JUMP;

	if (pressed_move_key)
		return;

	if (!((antiaim::get().type == ANTIAIM_LEGIT ? g_cfg.antiaim.desync : g_cfg.antiaim.type[antiaim::get().type].desync) && (antiaim::get().type == ANTIAIM_LEGIT ? !g_cfg.antiaim.legit_lby_type : !g_cfg.antiaim.lby_type) && (!g_ctx.globals.weapon->is_grenade() || g_cfg.esp.on_click & !(m_pcmd->m_buttons & IN_ATTACK) && !(m_pcmd->m_buttons & IN_ATTACK2))) || antiaim::get().condition(m_pcmd)) //-V648
	{
		auto velocity = g_ctx.local()->m_vecVelocity();

		if (velocity.Length2D() > 20.0f)
		{
			Vector direction;
			Vector real_view;

			math::vector_angles(velocity, direction);
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

			m_pcmd->m_forwardmove = negative_forward_direction.x;
			m_pcmd->m_sidemove = negative_side_direction.y;
		}
	}
	else
	{
		auto velocity = g_ctx.local()->m_vecVelocity();

		if (velocity.Length2D() > 20.0f)
		{
			Vector direction;
			Vector real_view;

			math::vector_angles(velocity, direction);
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

			m_pcmd->m_forwardmove = negative_forward_direction.x;
			m_pcmd->m_sidemove = negative_side_direction.y;
		}
		else
		{
			auto speed = 1.01f;

			if (m_pcmd->m_buttons & IN_DUCK || g_ctx.globals.fakeducking)
				speed *= 2.94117647f;

			static auto switch_move = false;

			if (switch_move)
				m_pcmd->m_sidemove += speed;
			else
				m_pcmd->m_sidemove -= speed;

			switch_move = !switch_move;
		}
	}
}

void misc::spectators_list() {
	if (g_cfg.misc.spectators_list) {
		std::vector <std::string> spectators;
		static int spectator_x;
		static int spectator_y;
		int h = 0;
		int h2 = 0;
		int a;
		int h4;
		int spectator_width = 180;
		int spectator_height = 25;
		static int _drag_x = 300;
		static int _drag_y = 300;
		static bool _dragging = false;
		bool _click = false;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			_click = true;
		}
		Vector2D _mouse_pos = m_surface()->GetMousePosition();

		if (_dragging && !_click) {
			_dragging = false;
		}
		if (_dragging && _click)
		{
			g_cfg.misc.spectatorlist_pos_x = _mouse_pos.x - _drag_x;
			g_cfg.misc.spectatorlist_pos_y = _mouse_pos.y - _drag_y;
			spectator_x = g_cfg.misc.spectatorlist_pos_x;
			spectator_y = g_cfg.misc.spectatorlist_pos_y;
		}
		if ((m_surface()->MouseInRegion(g_cfg.misc.spectatorlist_pos_x, g_cfg.misc.spectatorlist_pos_y, spectator_width, spectator_height)))
		{
			_dragging = true;
			_drag_x = _mouse_pos.x - g_cfg.misc.spectatorlist_pos_x;
			_drag_y = _mouse_pos.y - g_cfg.misc.spectatorlist_pos_y;
		}
		for (auto i = 0; i < spectators.size(); i++)
		{
			h2 += 12;

		}
		Color gradient1 = Color(141, 55, 221);
		Color gradient2 = Color(89, 35, 140);
		if (h2 < 1)
		{
			a = 0;
			h4 = 0;
		}
		else
		{
			a = 245;
			h4 = 15;
		}

		for (int i = 1; i < m_globals()->m_maxclients; i++)
		{
			auto e = static_cast<player_t*>(m_entitylist()->GetClientEntity(i));

			if (!e)
				continue;

			if (e->is_alive())
				continue;

			if (e->IsDormant())
				continue;

			if (e->m_hObserverTarget().Get() != g_ctx.local())
				continue;

			player_info_t player_info;
			m_engine()->GetPlayerInfo(i, &player_info);

			spectators.push_back(player_info.szName);
		}

		render::get().rect_filled(g_cfg.misc.spectatorlist_pos_x, g_cfg.misc.spectatorlist_pos_y, spectator_width, h2 + spectator_height + h4, Color(30, 30, 30, 245));
		render::get().rect(g_cfg.misc.spectatorlist_pos_x, g_cfg.misc.spectatorlist_pos_y, spectator_width, h2 + spectator_height + h4, Color(15, 15, 15, 245));
		render::get().gradient(g_cfg.misc.spectatorlist_pos_x, g_cfg.misc.spectatorlist_pos_y + 24, spectator_width, spectator_height - 24, gradient1, gradient2, GRADIENT_HORIZONTAL);
		auto text_size = render::get().text_width(fonts[RADIX_FONT], crypt_str("Spectator List"));
		render::get().text(fonts[RADIX_FONT], g_cfg.misc.spectatorlist_pos_x + (spectator_width - text_size) / 2, g_cfg.misc.spectatorlist_pos_y + 12, Color(255, 255, 255), HFONT_CENTERED_Y, crypt_str("Spectator List"));
		render::get().rect_filled(g_cfg.misc.spectatorlist_pos_x + 5, g_cfg.misc.spectatorlist_pos_y + 30, spectator_width - 10, h2 + 5, Color(20, 20, 20, a));
		render::get().rect(g_cfg.misc.spectatorlist_pos_x + 5, g_cfg.misc.spectatorlist_pos_y + 30, spectator_width - 10, h2 + 5, Color(15, 15, 15, a));

		for (auto i = 0; i < spectators.size(); i++) {
			render::get().text(fonts[RADIX_FONT], g_cfg.misc.spectatorlist_pos_x + 10, g_cfg.misc.spectatorlist_pos_y + 38 + h, Color(255, 255, 255), HFONT_CENTERED_Y, crypt_str(spectators.at(i).c_str()));
			h += 12;
		}
		spectators.clear();
	}
}

bool misc::can_shift_shot(int ticks)
{
	if (!g_ctx.local() || !g_ctx.local()->m_hActiveWeapon())
		return false;

	auto tickbase = g_ctx.local()->m_nTickBase();
	auto curtime = m_globals()->m_intervalpertick * (tickbase - ticks);

	if (curtime < g_ctx.local()->m_flNextAttack())
		return false;

	if (curtime < g_ctx.local()->m_hActiveWeapon()->m_flNextPrimaryAttack())
		return false;

	return true;
}

bool misc::double_tap(CUserCmd* m_pcmd)
{
	double_tap_enabled = true;

	static auto recharge_double_tap = false;
	//static auto last_double_tap = 0;
	auto max_tickbase_shift = m_gamerules()->m_bIsValveDS() ? 6 : 16;
	auto tickbase_shift = min(g_ctx.globals.weapon->get_max_tickbase_shift(), max_tickbase_shift);

	if (recharge_double_tap)
	{
		recharge_double_tap = false;
		recharging_double_tap = true;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (recharging_double_tap)
	{
		//auto recharge_time = g_ctx.globals.weapon->can_double_tap() ? TIME_TO_TICKS(0.1f) : TIME_TO_TICKS(0.1f);

		if (!aim::get().should_stop && can_shift_shot(tickbase_shift))
		{
			recharging_double_tap = false;
			double_tap_key = true;
		}
	}

	if (!g_cfg.ragebot.enable)
	{
		double_tap_enabled = false;
		double_tap_key = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (!g_cfg.ragebot.double_tap)
	{
		double_tap_enabled = false;
		double_tap_key = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (g_cfg.ragebot.double_tap_key.key <= KEY_NONE || g_cfg.ragebot.double_tap_key.key >= KEY_MAX)
	{
		double_tap_enabled = false;
		double_tap_key = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (double_tap_key && g_cfg.ragebot.double_tap_key.key != g_cfg.antiaim.hide_shots_key.key)
		hide_shots_key = false;

	if (!double_tap_key)
	{
		double_tap_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (g_ctx.local()->m_bGunGameImmunity() || g_ctx.local()->m_fFlags() & FL_FROZEN)
	{
		double_tap_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (m_gamerules()->m_bIsValveDS())
	{
		double_tap_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (g_ctx.globals.fakeducking)
	{
		double_tap_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return false;
	}

	if (antiaim::get().freeze_check)
		return true;

	if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER && !g_ctx.globals.weapon->is_knife() && g_ctx.send_packet && m_pcmd->m_buttons & IN_ATTACK) //-V648
	{
		auto next_command_number = m_pcmd->m_command_number + 1;
		auto user_cmd = m_input()->GetUserCmd(next_command_number);

		memcpy(user_cmd, m_pcmd, sizeof(CUserCmd));
		user_cmd->m_command_number = next_command_number;

		g_ctx.globals.teleport_cmd = user_cmd;
		g_ctx.globals.should_teleport = true;
		util::copy_command(user_cmd, tickbase_shift);

		if (g_ctx.globals.aimbot_working)
		{
			g_ctx.globals.double_tap_aim = true;
			g_ctx.globals.double_tap_aim_check = true;
		}

		recharge_double_tap = true;
		double_tap_enabled = false;
		double_tap_key = false;

		//last_double_tap = g_ctx.globals.fixed_tickbase;
	}
	else if (!g_ctx.globals.weapon->is_grenade() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_TASER && !g_ctx.globals.weapon->is_knife() && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER)
		g_ctx.globals.tickbase_shift = tickbase_shift;

	return true;
}

void misc::hide_shots(CUserCmd* m_pcmd, bool should_work)
{
	hide_shots_enabled = true;

	if (!g_cfg.ragebot.enable)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;

		if (should_work)
		{
			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.next_tickbase_shift = 0;
		}

		return;
	}

	if (!g_cfg.antiaim.hide_shots)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;

		if (should_work)
		{
			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.next_tickbase_shift = 0;
		}

		return;
	}

	if (g_cfg.antiaim.hide_shots_key.key <= KEY_NONE || g_cfg.antiaim.hide_shots_key.key >= KEY_MAX)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;

		if (should_work)
		{
			g_ctx.globals.ticks_allowed = 0;
			g_ctx.globals.next_tickbase_shift = 0;
		}

		return;
	}

	if (!should_work && double_tap_key)
	{
		hide_shots_enabled = false;
		hide_shots_key = false;
		return;
	}

	if (!hide_shots_key)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	double_tap_key = false;

	if (g_ctx.local()->m_bGunGameImmunity() || g_ctx.local()->m_fFlags() & FL_FROZEN)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	if (g_ctx.globals.fakeducking)
	{
		hide_shots_enabled = false;
		g_ctx.globals.ticks_allowed = 0;
		g_ctx.globals.next_tickbase_shift = 0;
		return;
	}

	if (antiaim::get().freeze_check)
		return;

	g_ctx.globals.next_tickbase_shift = m_gamerules()->m_bIsValveDS() ? 6 : 9;

	auto revolver_shoot = g_ctx.globals.weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && !g_ctx.globals.revolver_working && (m_pcmd->m_buttons & IN_ATTACK || m_pcmd->m_buttons & IN_ATTACK2);
	auto weapon_shoot = m_pcmd->m_buttons & IN_ATTACK && g_ctx.globals.weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER || m_pcmd->m_buttons & IN_ATTACK2 && g_ctx.globals.weapon->is_knife() || revolver_shoot;

	if (g_ctx.send_packet && !g_ctx.globals.weapon->is_grenade() && weapon_shoot)
		g_ctx.globals.tickbase_shift = g_ctx.globals.next_tickbase_shift;
}