#include "merged.h"
#include "NetworkedVariables.h"
//#include <mutiny merge\console.hpp>
#include "console.hpp"
//#include <minhook\minhook.h>
#include "../minhook/minhook.h"
#include "merged_hooks.h"



template <typename T>
T* CaptureInterface(HANDLE modulehandle, const char* strInterface)
{
	typedef T* (*CreateInterfaceFn)(const char* szName, int iReturn);
	CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress((HMODULE)(modulehandle), ("CreateInterface"));
	return CreateInterface(strInterface, 0);
}


class IClientLeafSystem;
DWORD AdrOfIsPausedExtrapolate = NULL;

void mutiny_final::setup()
{

	network_hook =  new vmthook(reinterpret_cast<DWORD**>(SteamGameCoordinator));
	network_hook->hook_function(reinterpret_cast<uintptr_t>(hkSendMessage), 0);
	network_hook->hook_function(reinterpret_cast<uintptr_t>(hkRetrieveMessage), 2);



	StaticOffsets.AddAllOffsets();
	StaticOffsets.UpdateAllOffsets();
}

//small implements
bool __stdcall IsPaused()
{
	if (_ReturnAddress() == (void*)AdrOfIsPausedExtrapolate)
		return true;
	return oIsPaused();
}

void mutiny_final::hooks()
{
	MH_Initialize();

	IClientLeafSystem* ClientLeafSystem = CaptureInterface<IClientLeafSystem>(ClientHandle, ("ClientLeafSystem002"));
	AdrOfIsPausedExtrapolate = StaticOffsets.GetOffsetValue(_IsPausedExtrapolateReturnAddress);


	const auto FireBullets_target = reinterpret_cast<void*>(get_virtual(TE_FireBullets, 7));
	const auto ClientLeafSystem_target = reinterpret_cast<void*>(get_virtual(ClientLeafSystem, 7));
	const auto InPrediction_target = reinterpret_cast<void*>(get_virtual(m_prediction(), 14));
	const auto paused_target = reinterpret_cast<void*>(get_virtual(m_engine(), 90));




	MH_CreateHook(FireBullets_target, &TE_FireBullets_PostDataUpdate, reinterpret_cast<void**>(&oTE_FireBullets_PostDataUpdate));
	MH_CreateHook(ClientLeafSystem_target, &AddRenderable, reinterpret_cast<void**>(&oAddRenderable));
	MH_CreateHook(InPrediction_target, &InPrediction, reinterpret_cast<void**>(&oInPrediction));
	MH_CreateHook(paused_target, &IsPaused, reinterpret_cast<void**>(&oIsPaused));



	MH_EnableHook(MH_ALL_HOOKS);
}

void mutiny_final::run()
{
	//logger::add(LSUCCESS, "reached setup point");
	setup();
	//logger::add(LSUCCESS, "reached hook point");
	hooks();
	//logger::add(LSUCCESS, "end");
}
