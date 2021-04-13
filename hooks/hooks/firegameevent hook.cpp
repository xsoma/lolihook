#include "../../includes.hpp"

#include "../../skinchanger/skins.h"

static std::string thats_correct;
void Skins::iconfix(std::string fix)
{
	thats_correct = fix;
}



bool __fastcall shonax_hooks::FireEventClientSide(void* ecx, void* edx, IGameEvent* pEvent)
{
	if (!pEvent)
		return oFireEventClientSidefn(ecx, pEvent);
	if (m_engine()->IsInGame() && m_engine()->IsConnected() && g_ctx.local())
	{
		if (g_ctx.local()->m_hActiveWeapon() && g_ctx.local()->m_hActiveWeapon().Get()->is_knife())
		{
			if (!strcmp(pEvent->GetName(), "player_death")) {
				int attacker_id = pEvent->GetInt("attacker");
				if (m_engine()->GetPlayerForUserID(attacker_id) == m_engine()->GetLocalPlayer())
					pEvent->SetString("weapon", thats_correct.c_str());
			}
		}
	}
	return oFireEventClientSidefn(ecx, pEvent);
}