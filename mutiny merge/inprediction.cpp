#include "merged_hooks.h"
#include "NetworkedVariables.h"
#include "merged_hooks.h"



bool __fastcall InPrediction(void* ecx, void* edx)
{
	if ((DWORD)_ReturnAddress() == StaticOffsets.GetOffsetValue(_MaintainSequenceTransitionsReturnAddress))
		return true;

	// call original
	return oInPrediction(ecx, edx);
}