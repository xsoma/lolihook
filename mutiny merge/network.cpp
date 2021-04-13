#include "merged_hooks.h"



void FlashWindow(HWND pWindow)
{
	FLASHWINFO fwInfo;
	fwInfo.cbSize = sizeof(FLASHWINFO);
	fwInfo.hwnd = pWindow;
	fwInfo.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
	fwInfo.uCount = 0;
	fwInfo.dwTimeout = 0;
	FlashWindowEx(&fwInfo);
}

void SetLocalPlayerReady()
{
	using SetLocalPlayerReadyFn = void(__stdcall*)(const char*);
	static auto oSetLocalPlayerReady = reinterpret_cast<SetLocalPlayerReadyFn>(util::FindSignature("client.dll", ("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"))); // @xref: "deffered"

	if (oSetLocalPlayerReady != nullptr)
		oSetLocalPlayerReady("");
}


int __fastcall hkSendMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t uMsgType, const void* pData, std::uint32_t uData)
{
	static auto oSendMessage = network_hook->get_func_address<decltype(&hkSendMessage)>(0);

	std::uint32_t uMessageType = uMsgType & 0x7FFFFFFF;
	void* pDataMutable = const_cast<void*>(pData);

	int iStatus = oSendMessage(thisptr, edx, uMsgType, pDataMutable, uData);

	if (iStatus != 0)
		return iStatus;

	return iStatus;
}




int __fastcall hkRetrieveMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t* puMsgType, void* pDest, std::uint32_t uDest, std::uint32_t* puMsgSize)
{
	static auto oRetrieveMessage = network_hook->get_func_address<decltype(&hkRetrieveMessage)>(2);
	int iStatus = oRetrieveMessage(thisptr, edx, puMsgType, pDest, uDest, puMsgSize);

	if (iStatus != 0)
		return iStatus;

	std::uint32_t uMessageType = *puMsgType & 0x7FFFFFFF;

	// check for k_EMsgGCCStrike15_v2_GCToClientSteamdatagramTicket message when we can accept the game
	if (g_cfg.misc.autoaccept && uMessageType == 9177)
	{
		SetLocalPlayerReady();
		Beep(500, 800);
		FlashWindow(INIT::Window);
	}

	return iStatus;
}
