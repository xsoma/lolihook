#pragma once


#include "../includes.hpp"



using TE_FireBullets_PostDataUpdateFn = void(__thiscall*)(C_TEFireBullets*, DataUpdateType_t);
inline TE_FireBullets_PostDataUpdateFn oTE_FireBullets_PostDataUpdate;
void __stdcall TE_FireBullets_PostDataUpdate(DataUpdateType_t updateType);



using AddRenderableFn = int(__thiscall*)(void*, IClientRenderable * p_renderable, int, RenderableTranslucencyType_t, int, int);
inline AddRenderableFn oAddRenderable;

int __fastcall AddRenderable(void* ecx, void* edx, IClientRenderable* p_renderable, int unk1, RenderableTranslucencyType_t n_type, int unk2, int unk3);

using InPredictionFn = bool(__fastcall*)(void*, void*);
inline InPredictionFn oInPrediction;
bool __fastcall InPrediction(void* ecx, void* edx);


using IsPausedFn = bool(__stdcall*)();
inline IsPausedFn oIsPaused;
using PlayerFallingDamageFn = void(__thiscall*)(void* ecx);
inline PlayerFallingDamageFn oPlayerFallingDamage;

int		__fastcall	hkSendMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t uMsgType, const void* pData, std::uint32_t uData);
int		__fastcall	hkRetrieveMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t* puMsgType, void* pDest, std::uint32_t uDest, std::uint32_t* puMsgSize);





