// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "spammers.h"

void spammers::clan_tag()
{
	auto apply = [](const char* tag) -> void
	{
		using Fn = int(__fastcall*)(const char*, const char*);
		static auto fn = reinterpret_cast<Fn>(util::FindSignature(crypt_str("engine.dll"), crypt_str("53 56 57 8B DA 8B F9 FF 15")));

		fn(tag, tag);
	};

	static auto removed = false;

	if (!g_cfg.misc.clantag_spammer && !removed)
	{
		removed = true;
		apply(crypt_str(""));
		return;
	}

	if (g_cfg.misc.clantag_spammer)
	{
		auto nci = m_engine()->GetNetChannelInfo();

		if (!nci)
			return;

		static auto time = -1;

		auto ticks = TIME_TO_TICKS(nci->GetAvgLatency(FLOW_OUTGOING)) + (float)m_globals()->m_tickcount; //-V807
		auto intervals = 0.27f / m_globals()->m_intervalpertick;

		auto main_time = (int)(ticks / intervals) % 31;

		if (main_time != time && !m_clientstate()->iChokedCommands)
		{
			auto tag = ("");

			switch (main_time)
			{
			case 0:
				tag = ("");
				break;
			case 1:
				tag = ("|_");
				break;
			case 2:
				tag = ("L0");
				break;
			case 3:
				tag = ("Lo|_");
				break;
			case 4:
				tag = ("Lol");
				break;
			case 5:
				tag = ("Lol|");
				break;
			case 6:
				tag = ("Loli|-|");
				break;
			case 7:
				tag = ("LoliH");
				break;
			case 8:
				tag = ("LoliH0");
				break;
			case 9:
				tag = ("LoliH00");
				break;
			case 10:
				tag = ("LoliHoo");
				break;
			case 11:
				tag = ("LoliHook");
				break;
			case 12:
				tag = ("LoliHook.T");
				break;
			case 13:
				tag = ("LoliHook.T0");
				break;
			case 14:
				tag = ("LoliHook.TOP");
				break;
			case 15:
				tag = ("LoliHook.top");
				break;
			case 16:
				tag = ("LoliHook.TOP");
				break;
			case 17:
				tag = ("LoliHook.top");
				break;
			case 18:
				tag = ("LoliHook.TOP");
				break;
			case 19:
				tag = ("LoliHook.TO<");
				break;
			case 20:
				tag = ("LoliHook.T<");
				break;
			case 21:
				tag = ("LoliHook.<");
				break;
			case 22:
				tag = ("LoliHook<");
				break;
			case 23:
				tag = ("LoliHoo<");
				break;
			case 24:
				tag = ("LoliHo<");
				break;
			case 25:
				tag = ("LoliH<");
				break;
			case 26:
				tag = ("Loli<");
				break;
			case 27:
				tag = ("Lol<");
				break;
			case 28:
				tag = ("Lo<");
				break;
			case 29:
				tag = ("L<");
				break;
			case 30:
				tag = ("<");
				break;
			}
			apply(tag);
			time = main_time;

			removed = false;
		}
	}
}