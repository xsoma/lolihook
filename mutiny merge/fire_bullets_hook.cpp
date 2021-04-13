#include "merged_hooks.h"
#include "../cheats/lagcompensation/animation_system.h"


void __stdcall FireBullets_PostDataUpdate(C_TEFireBullets* thisptr, DataUpdateType_t updateType)
{
	int iPlayer = thisptr->m_iPlayer + 1;
	if (iPlayer <= 64)
	{
		Vector	vecAngles = thisptr->m_vecAngles;
		Vector vecOrigin = thisptr->_m_vecOrigin;
		auto _playerRecord = &player_records[iPlayer];
		if (_playerRecord && !_playerRecord->empty())
		{
			adjust_data* previous_record = nullptr;
			if (_playerRecord->size() >= 2)
				previous_record = &_playerRecord->at(1);
			auto record = &_playerRecord->front();
			if (record->player && record->player != g_ctx.local())
			{
				auto weapon = record->player->m_hActiveWeapon().Get();
				record->m_flLastShotSimtime = weapon ? weapon->m_fLastShotTime() : record->player->m_flSimulationTime();
				record->m_angLastShotAngle = vecAngles;
			}
		}
	}
	oTE_FireBullets_PostDataUpdate(thisptr, updateType);
}


__declspec (naked) void __stdcall TE_FireBullets_PostDataUpdate(DataUpdateType_t updateType)
{
	__asm
	{
		push[esp + 4]
		push ecx
		call FireBullets_PostDataUpdate
		retn 4
	}
}