#pragma once
class fDetours {
public:
	void init();
public:
	static void __cdecl CL_Move(float accumulated_extra_samples, bool bFinalTick);
	decltype(&CL_Move) oCL_Move;
};

inline fDetours g_detours;