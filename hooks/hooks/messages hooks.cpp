#include "../../includes.hpp"




#include "../protobuffs/Protobuffs.h"

unsigned long __fastcall shonax_hooks::GCRetrieveMessageHook(void* ecx, void*, uint32_t* punMsgType, void* pubDest, uint32_t cubDest, uint32_t* pcubMsgSize)
{
	//static auto oGCRetrieveMessage = gc.get_original<GCRetrieveMessage>(2);
	auto status = oGCRetrieveMessage(ecx, punMsgType, pubDest, cubDest, pcubMsgSize);
	if (status == k_EGCResultOK)
	{

		void* thisPtr = nullptr;
		__asm mov thisPtr, ebx;
		auto oldEBP = *reinterpret_cast<void**>((uint32_t)_AddressOfReturnAddress() - 4);

		uint32_t messageType = *punMsgType & 0x7FFFFFFF;
		write.ReceiveMessage(thisPtr, oldEBP, messageType, pubDest, cubDest, pcubMsgSize);
	}
	return status;
}

unsigned long __fastcall shonax_hooks::GCSendMessageHook(void* ecx, void*, uint32_t unMsgType, const void* pubData, uint32_t cubData)
{
	//static auto oGCSendMessage = gc.get_original<GCSendMessage>(0);
	bool sendMessage = write.PreSendMessage(unMsgType, const_cast<void*>(pubData), cubData);
	if (!sendMessage)
		return k_EGCResultOK;
	return oGCSendMessage(ecx, unMsgType, const_cast<void*>(pubData), cubData);
}
