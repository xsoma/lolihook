#include <ShlObj_core.h>
#include "walkbot.h"

Color colorDefault = Color(0, 255, 0);
Color colorclose = Color(255, 0, 0);
Color colorselected = Color(0, 0, 255);
Color colortarget = Color(255, 255, 0);

float distancemeters(Vector v1, Vector v2) {
	float dist = sqrt(v1.DistToSqr(v2));
	return dist * 0.01905f;
}
bool file_exists(LPCTSTR fname)
{
	return::GetFileAttributes(fname) != DWORD(-1);
}
void walkbot::move(CUserCmd* cmd) {
	if (!g_cfg.misc.walkbot[WALKBOT_ENABLE])
		return;

	if (dots.size() < 2) 
		return;

	if (!g_ctx.local() || !g_ctx.local()->is_alive())
		return;

	if (marker > dots.size() - 1) {
		if (g_cfg.misc.walkbot[WALKBOT_CYCLE]) {
			prishel = true;
			marker = dots.size() - 1;
		}
		else marker = 0;
	}

	Vector target = dots.at(marker);
	Vector lookat = math::calculate_angle(g_ctx.globals.eye_pos, target);
	lookat.Normalize();
	lookat.Clamp();
	lookat.x = 0;

	cmd->m_viewangles = lookat;
	cmd->m_forwardmove = 450;
	//m_cvar()->ConsolePrintf("set to vector(%s, %s, %s), cmd->viewangldâes = vector(%s, %s, %s) \n", std::to_string(lookat.x), std::to_string(lookat.y), std::to_string(lookat.z), std::to_string(cmd->m_viewangles.x), std::to_string(cmd->m_viewangles.y), std::to_string(cmd->m_viewangles.z));
	
	if (g_ctx.local()->m_vecVelocity().Length2D() < 25 && !(marker == 0 || marker == dots.size() - 1))
		cmd->m_buttons |= IN_JUMP;

	float dist3D = distancemeters(g_ctx.local()->m_vecOrigin(), target);
	if (dist3D <= 0.5f) {
		if (g_cfg.misc.walkbot[WALKBOT_SOUND]) m_engine()->ExecuteClientCmd(crypt_str("play buttons\\light_power_on_switch_01"));
		if (g_cfg.misc.walkbot[WALKBOT_CYCLE])
			prishel ? marker-- : marker++;
		else
			marker++;

		if (g_cfg.misc.walkbot[WALKBOT_CYCLE])
			if (marker == 0)
				prishel = false;
	}
}

void walkbot::update(bool showdebug) {

	if (key_binds::get().get_key_bind_state(28)) {
		button_clicked = false;
		button_down = true;
	}
	else if (!key_binds::get().get_key_bind_state(28) && button_down) {
		button_clicked = true;
		button_down = false;
	}
	else {
		button_clicked = false;
		button_down = false;
	}

	std::string oldmapname = mapname;
	mapname = m_engine()->GetLevelName();
	if (oldmapname != mapname) {
		//refresh(mapname, dots, g_cfg.esp.walksave);
		file("_def", file_load);
		return;
	}

	int oldteamnum = teamnum;
	teamnum = g_ctx.local()->m_iTeamNum();
	if (oldteamnum != teamnum) {
		//refresh(mapname, dots, g_cfg.esp.walksave);
		marker = 0;
		prishel = false;
		return;
	}

	if (!g_ctx.local()) return;

	int w, h, cX, cY;
	m_engine()->GetScreenSize(w, h);
	cX = w / 2;
	cY = h / 2;

	bool spoterased = false, spotfound = false;

	for (int i = 0; i < dots.size(); i++) {
		Vector spot = dots.at(i);
		Color c = colorDefault;

		float dist3D = distancemeters(g_ctx.local()->GetAbsOrigin(), spot);
		if (dist3D <= 1) c = colorclose;

		Vector pos2d;
		if (math::world_to_screen(spot, pos2d))
		{
			float dist2D = pos2d.DistTo(Vector(cX, cY, 0));
			if (dist2D <= 15 && !spotfound)
			{
				c = colorselected;
				spotfound = true;

				if (button_clicked && !spoterased)
				{
					dots.erase(dots.begin() + i);
					if (g_cfg.misc.walkbot[WALKBOT_SOUND] && g_cfg.misc.walkbot[WALKBOT_DEVELOPER_MODE])
						m_engine()->ExecuteClientCmd(crypt_str("play error"));

					spoterased = true;
					continue;
				}
			}
		}

		Ray_t ray; trace_t tr; CTraceFilter filter;
		ray.Init(g_ctx.globals.eye_pos, spot);
		filter.pSkip = g_ctx.local();
		m_trace()->TraceRay(ray, 0x46004003, &filter, &tr);

		bool isvisible = tr.IsVisible();
		if (isvisible)
		{
			if (i == marker)
				if (c == colorselected)
					m_debugoverlay()->AddSphereOverlay(spot, 7, 20, 20, c.r(), c.g(), c.b(), 255, 0.025f);
				else
					m_debugoverlay()->AddSphereOverlay(spot, 7, 20, 20, colortarget.r(), colortarget.g(), colortarget.b(), 255, 0.025f);
			else
				m_debugoverlay()->AddSphereOverlay(spot, 7, 20, 20, c.r(), c.g(), c.b(), 255, 0.025f);

			if (i - 1 >= 0)
				m_debugoverlay()->AddLineOverlay(dots.at(i - 1), spot, 255, 255, 255, 255, 0.025f);
		}
		else
		{
			char dist_to[32];
			sprintf_s(dist_to, "%.0fm", dist3D);

			if (i != marker)
				render::get().text(fonts[LOGS], pos2d.x, pos2d.y, Color(255, 255, 255, 255), HFONT_CENTERED_Y | HFONT_CENTERED_X, dist_to);
		}

		if (i == marker)
			render::get().text(fonts[LOGS], pos2d.x, pos2d.y, Color(255, 0, 0, 255), HFONT_CENTERED_Y | HFONT_CENTERED_X, crypt_str("> target <"));


		if (showdebug) {
			render::get().text(fonts[NAME], pos2d.x, pos2d.y - 5, Color(255, 255, 255, 220), HFONT_CENTERED_Y, crypt_str(std::string("index = " + std::to_string(i)).c_str()));
			render::get().text(fonts[NAME], pos2d.x, pos2d.y + 10, Color(255, 255, 255, 220), HFONT_CENTERED_Y, crypt_str(std::string("x = " + std::to_string(spot.x)).c_str()));
			render::get().text(fonts[NAME], pos2d.x, pos2d.y + 25, Color(255, 255, 255, 220), HFONT_CENTERED_Y, crypt_str(std::string("y = " + std::to_string(spot.y)).c_str()));
			render::get().text(fonts[NAME], pos2d.x, pos2d.y + 40, Color(255, 255, 255, 220), HFONT_CENTERED_Y, crypt_str(std::string("z = " + std::to_string(spot.z)).c_str()));
			render::get().text(fonts[NAME], 100, 100, Color(255, 255, 0, 220), HFONT_CENTERED_Y, crypt_str(std::string("velocity: " + std::to_string(g_ctx.local()->m_vecVelocity().Length2D())).c_str()));
		}
	}

	if (button_clicked && !spoterased)
	{
		Vector tracestart, traceend;
		Vector tuda;

		m_engine()->GetViewAngles(tuda);
		math::angle_vectors(tuda, traceend);

		tracestart = g_ctx.globals.eye_pos;
		traceend = tracestart + (traceend * 8192.0f);

		Ray_t ray; trace_t tr; CTraceFilter filter;
		ray.Init(tracestart, traceend);
		filter.pSkip = g_ctx.local();

		m_trace()->TraceRay(ray, 0x46004003, &filter, &tr);

		Vector spot = tr.endpos;
		dots.push_back(spot);

		if (g_cfg.misc.walkbot[WALKBOT_SOUND] && g_cfg.misc.walkbot[WALKBOT_DEVELOPER_MODE])
			m_engine()->ExecuteClientCmd(crypt_str("play resource\\warning"));
	}
}

void walkbot::file(std::string addictive_name, int todo)
{
	std::string levelNameFixed;
	for (size_t i = 0; i < mapname.size(); ++i)
		if (mapname[i] != '\\') levelNameFixed += mapname[i];

	if (g_ctx.local()->m_iTeamNum() > 0)
		levelNameFixed += std::to_string(g_ctx.local()->m_iTeamNum());

	levelNameFixed += addictive_name;
	static char path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path))) {
		char szCmd[256];
		sprintf(szCmd, crypt_str("\\walkbot\\%s.ini"), levelNameFixed);
		folder = std::string(path) + crypt_str("\\walkbot\\");
		file = std::string(path) + szCmd;
	}

	CreateDirectoryA(folder.c_str(), NULL);

	switch (todo)
	{
		case file_overwrite:
			DeleteFileA(file.c_str());
			for (int i = 0; i < dots.size(); i++) {
				Vector spot = dots.at(i);
				WritePrivateProfileStringA(std::to_string(i).c_str(), crypt_str("x"), std::to_string(spot.x).c_str(), file.c_str());
				WritePrivateProfileStringA(std::to_string(i).c_str(), crypt_str("y"), std::to_string(spot.y).c_str(), file.c_str());
				WritePrivateProfileStringA(std::to_string(i).c_str(), crypt_str("z"), std::to_string(spot.z).c_str(), file.c_str());
			}

			break;

		case file_load:
			if (file_exists(file.c_str())) {
				char value[32] = { '\0' };
				dots.clear();
				int i = 0;
				bool limitreached = false;
				while (!limitreached) {
					GetPrivateProfileStringA(std::to_string(i).c_str(), crypt_str("x"), "", value, 32, file.c_str());
					float x = atof(value);								
					GetPrivateProfileStringA(std::to_string(i).c_str(), crypt_str("y"), "", value, 32, file.c_str());
					float y = atof(value);								
					GetPrivateProfileStringA(std::to_string(i).c_str(), crypt_str("z"), "", value, 32, file.c_str());
					float z = atof(value);											 

					if (x == 0 && y == 0 && z == 0) {
						limitreached = true;
						continue;
					}
					Vector spot = Vector(x, y, z);
					dots.push_back(spot);
					i++;
				}
				marker = 0;
				prishel = false;
			}
			break;
	}

}

void walkbot::refresh(std::string levelName, std::vector<Vector> spots, bool save)
{
	std::string levelNameFixed;
	for (size_t i = 0; i < mapname.size(); ++i)
		if (levelName[i] != '\\') levelNameFixed += levelName[i];

	if (g_ctx.local()->m_iTeamNum() > 0)
		levelNameFixed += std::to_string(g_ctx.local()->m_iTeamNum());

	static char path[MAX_PATH];
	std::string folder, file;
	if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, path))) {
		char szCmd[256];
		sprintf(szCmd, crypt_str("\\walkbot\\%s.ini"), levelNameFixed);
		folder = std::string(path) + crypt_str("\\walkbot\\");
		file = std::string(path) + szCmd;
	}

	CreateDirectoryA(folder.c_str(), NULL);

	if (save) {
		DeleteFileA(file.c_str());
		for (int i = 0; i < spots.size(); i++) {
			Vector spot = spots.at(i);
			WritePrivateProfileStringA(std::to_string(i).c_str(), crypt_str("x"), std::to_string(spot.x).c_str(), file.c_str());
			WritePrivateProfileStringA(std::to_string(i).c_str(), crypt_str("y"), std::to_string(spot.y).c_str(), file.c_str());
			WritePrivateProfileStringA(std::to_string(i).c_str(), crypt_str("z"), std::to_string(spot.z).c_str(), file.c_str());
		}
	}
	else {
		char value[32] = { '\0' };
		dots.clear();

		int i = 0;
		bool limitreached = false;

		while (!limitreached) {
			GetPrivateProfileStringA(std::to_string(i).c_str(), crypt_str("x"), "", value, 32, file.c_str());
			float x = atof(value);								
			GetPrivateProfileStringA(std::to_string(i).c_str(), crypt_str("y"), "", value, 32, file.c_str());
			float y = atof(value);								
			GetPrivateProfileStringA(std::to_string(i).c_str(), crypt_str("z"), "", value, 32, file.c_str());
			float z = atof(value);

			if (x == 0 && y == 0 && z == 0) {
				limitreached = true;
				continue;
			}
			Vector spot = Vector(x, y, z);
			dots.push_back(spot);
			i++;
		}
		//refresh(mapname, dots, false);
	}
}

void walkbot::clear_dots() {
	dots.clear();
	marker = 0;
	prishel = false;
}