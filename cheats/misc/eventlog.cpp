#include "eventlog.h"
#include <algorithm>
#include <iostream>
#include <cstdarg>

#include "../includes.hpp"

enum ConsoleColor
{
	BLACK = 0,
	DARKBLUE = FOREGROUND_BLUE,
	DARKGREEN = FOREGROUND_GREEN,
	DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
	DARKRED = FOREGROUND_RED,
	DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
	DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
	DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	GRAY = FOREGROUND_INTENSITY,
	BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
	MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
};

Features::CEventLogger* Features::EventLogger = new Features::CEventLogger();

void Features::CEventLogger::AddLog(const char* str, ...)
{
	if (!str)
		return;

	if (logs.size() > 7)
		logs.erase(logs.begin());

	va_list va_args;
	char buffer[2048] = { 0 };
	va_start(va_args, str);
	_vsnprintf(buffer, sizeof(buffer), str, va_args);
	va_end(va_args);
	
	logs.push_back({ m_globals()->m_realtime, -10, 0, std::string(buffer) });
}




void Game_Msg(const char* msg, ...)
{
	if (msg == nullptr)
		return;

	char buffer[2048];
	va_list list;
	va_start(list, msg);
	vsprintf(buffer, msg, list);
	va_end(list);
	Features::EventLogger->AddLog(std::string(buffer).c_str());
	m_cvar()->ConsoleColorPrintf(Color(179, 0, 179), "[LoliHook] ");
	m_cvar()->ConsolePrintf(std::string(buffer + std::string(("\n"))).c_str());
}

void Features::CEventLogger::Draw_D3D9()
{
	for (int i = 0; i < logs.size(); i++)
	{
		auto& l = logs.at(i);

		if (l.alpha == 0 && m_globals()->m_realtime - l.time >= 5)
			logs.erase(logs.begin() + i);

		if (l.yOffset < 0 && l.alpha < 255 && m_globals()->m_realtime - l.time < 5)
		{
			l.yOffset += 1;
			l.alpha += 17;
			continue;
		}

		if (l.yOffset <= 0 && l.alpha <= 255 && l.alpha > 0 && m_globals()->m_realtime - l.time >= 5)
		{
			l.yOffset -= 1;
			l.alpha -= 17;
			continue;
		}
	}

	auto offset = 0;
	auto textSize = 13;

	for (auto& l : logs)
	{
		render::get().text(fonts[FONT_CRIUS], 5, 5 + offset + l.yOffset, Color(255, 255, 255), HFONT_CENTERED_NONE, l.text.c_str());
		offset += textSize + l.yOffset;
	}
}