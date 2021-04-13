#pragma once
#include <Windows.h>
#include <array>
#include "../includes.hpp"

inline HANDLE EngineHandle;
inline HANDLE ClientHandle;
inline HMODULE ThisDLLHandle;
inline HANDLE MatchmakingHandle;
inline HANDLE VPhysicsHandle;
inline HANDLE VSTDLIBHandle;
inline HANDLE SHADERAPIDX9Handle;
inline HANDLE DatacacheHandle;
inline HANDLE Tier0Handle;
inline HANDLE MaterialSystemHandle;
inline HANDLE VGUIMatSurfaceHandle;
inline HANDLE VGUI2Handle;
inline HANDLE StudioRenderHandle;
inline HANDLE FileSystemStdioHandle;
inline HANDLE ServerHandle;


enum StaticOffsetName
{
	INVALID_STATIC_OFFSET = -1,

	_IsPlayer,
	_StriderMuzzleEffect,
	_GunshipMuzzleEffect,
	_CS_Bloodspray,
	_Tesla,
	_ViewRender,
	_ParseEventDelta,
	_GetPlayerResource,
	_CalcPlayerView,
	_InvalidatePhysicsRecursive,
	_GetShotgunSpread,
	_NoClipEnabled,
	_CanUseFastPath,
	_SetAbsAngles,
	_SetAbsAnglesDirect,
	_AbsVelocityDirect,
	_SetAbsOrigin,
	_AbsOriginDirect,
	_OverridePostProcessingDisable,
	_LastOcclusionCheckTime,
	_InvalidateBoneCache,
	_EntityGlowIndex, // Outdated
	_UpdateClientSideAnimation,
	_LastOutgoingCommand,
	_DeltaTick,
	_pClientState,
	_FinishDrawing,
	_StartDrawing,
	_UTIL_TraceLineIgnoreTwoEntities,
	_UTIL_ClipTraceToPlayers,
	_UTIL_ClipTraceToPlayers2,
	_IsEntityBreakable,
	_IsBaseCombatWeaponIndex,
	_LobbyScreen_Scaleform,
	_ChangeTeammateColor,
	_IsReady,
	_AcceptMatch,
	_GetWeaponDataIndex,
	_LineGoesThroughSmoke,
	_RevealRanks,
	_Cone,
	_Spread,
	_UpdateAccuracyPenalty,
	_GameResources,
	_GlowObjectManager,
	_KeyValues_LoadFromBuffer,
	_ChangeClantag,
	_KeyValues_Constructor,
	_EyeAnglesVMT,
	_LocalAngles,
	//_DeadFlag,
	//_RenderAngles,
	_CalcAbsoluteVelocity,
	_CalcAbsolutePosition,
	_OldOrigin,
	_LocalOrigin,
	_WorldSpaceCenter,
	_MoveType,
	_MoveCollide,
	_m_iEFlags,
	_InThreadedBoneSetup,
	_m_BoneSetupLock,
	_m_bIsToolRecording,
	_m_EntClientFlags,
	_GetPredictable,
	_m_pIK_offset,
	_m_flLastBoneSetupTime,
	_LastBoneChangedTime_Offset,
	_m_iPrevBoneMask,
	_m_iAccumulatedBoneMask,
	_LastSetupBonesFrameCount,
	_GetSolid,
	_m_iMostRecentModelBoneCounter,
	_g_iModelBoneCounter,
	_PhysicsSolidMaskForEntityVMT,
	_SetDormant,
	_LockStudioHdr,
	_GetPunchAngleVMT,
	_SetPunchAngleVMT,
	_SpawnTime,
	_m_dwObserverTargetVMT,
	_ReevaluateAnimLod,
	_AllowBoneAccessForViewModels,
	_AllowBoneAccessForNormalModels,
	_IsViewModel,
	_BoneAccessor,
	_m_pStudioHdr,
	_MarkForThreadedBoneSetup,
	_GetClassnameVMT,
	_SequencesAvailableVMT,
	_SequencesAvailableCall,
	_CreateIK,
	_Teleported,
	_IKInit,
	_ShouldSkipAnimFrame,
	_StandardBlendingRules,
	_BuildTransformations,
	_m_pRagdoll,
	_UpdateIKLocks,
	_CalculateIKLocks,
	_ControlMouth,
	_Wrap_SolveDependencies,
	_UpdateTargets,
	_AttachmentHelper,
	_UnknownOffset1,
	_DoExtraBoneProcessing,
	_m_CachedBoneData,
	_SplitScreenPlayerSlot,
	_g_VecRenderOrigin,
	_CacheVehicleView,
	_Weapon_ShootPosition,
	_Weapon_ShootPosition_Base,
	_GetClientVehicle,
	_IsInAVehicle,
	_VehicleViewOrigin,
	_GetObserverModeVMT,
	_m_bUseAnimationEyeOffset,
	_BaseCalcView,
	_m_bThirdPerson,
	_GetAliveVMT,
	_LookupBone,
	_m_rgflCoordinateFrame,
	_GameRules,
	_C_CSPlayer_CalcView,
	_PlayFootstepSound,
	_Simulate,
	_PhysicsSimulate,
	_ShouldInterpolate,
	_LookupPoseParameter,
	_GetPoseParameterRange,
	_m_flEncodedController,
	_SetGroundEntity,
	_SurfaceFriction,
	_m_chTextureType,
	_m_flWaterJumpTime,
	_m_nWaterType,
	_m_hPlayerAnimState,
	_GetCompetitiveMatchID,
	_IsPaused,
	_host_tickcount,
	_predictionrandomseed,
	_predictionplayer,
	_MD5PseudoRandom,
	_IsPausedExtrapolateReturnAddress,
	_g_pClientLeafSystem,
	_StandardFilterRulesCallOne,
	_SetAbsVelocity,
	_UpdateClientSideAnimationFunction,
	_frametime1,
	_frametime2,
	_frametime3,
	_ClientSideAnimationList,
	_EnableInvalidateBoneCache,
	_RadarBase,
	_IsEntityBreakable_FirstCall_Arg1,
	_IsEntityBreakable_FirstCall_Arg2,
	_IsEntityBreakable_SecondCall_Arg1,
	_IsEntityBreakable_SecondCall_Arg2,
	_IsEntityBreakable_ActualCall,
	_WeaponScriptPointer,
	_WeaponScriptPointerCall,
	_GetWeaponSystem,
	_Input,
	_oSetupBones,
	_host_interval_per_tick,
	_GetSequenceName,
	_GetSequenceActivity,
	_GetSequenceActivityNameForModel,
	_ActivityListNameForIndex,
	_pSeqDesc,
	_svtable,
	_hoststate,
	_CL_Move,
	_WriteUserCmd,
	_pCommands,
	_SetupVelocityReturnAddress,
	_CL_FireEvents,
	_AbsRecomputationEnabled,
	_AbsQueriesValid,
	_PlayerResource,
	_ResetAnimationState,
	_ProcessOnDataChangedEvents,
	_pTempEnts,
	_pBeams,
	_Net_Time,
	_Receivetable_Decode,
	_SequenceDuration,
	_GetFirstSequenceAnimTag,
	_GameTypes,
	_SurpressLadderChecks,
	_s_nTraceFilterCount,
	_s_TraceFilter,
	_IsCarryingHostage,
	_EyeVectors,
	_CreateStuckTable,
	_rgv3tStuckTable,
	_LevelShutdownPreEntity,
	_LevelInitPreEntity,
	_RenderViewTrap1,
	_RenderViewTrap2,
	_CHLClientTrap2,
	_EngineClientTrap2,
	_LevelShutdownCHLClientTrap,
	_LevelShutdownEngineClientTrap,
	_LocalPlayerEntityTrap,
	_ModelRenderGlow,
	_ModelRenderGlowTrap,
	_ModelRenderGlowTrap2,
	_PredictionUpdateHLTVCall,
	_GetBonePosition,
	_GetNetChannelInfoCvarCheck,
	_IClientEntityList,
	_RenderBeams,
	_DirectXPrePointer,
	_TE_EffectDispatch,
	_MoveHelperClient,
	_FormatViewModelAttachment,
	_GroupStudioHdr,
	_Attachments,
	_GetAbsAnglesVMT,
	_ShouldCollide,
	_IsTransparent,
	_EstimateAbsVelocity,
	_GetBaseAnimating,
	_AnimOverlay,
	_GetAbsOriginVMT,
	_GetPlayerMaxSpeedVMT,
	_PlayClientJumpSound,
	_PlayClientUnknownSound,
	_DoAnimationEvent1, //this is actually the BaseAnimState vtable
	_DoAnimationEvent2,
	_ResetLatched,
	_GetDefaultFOV,
	_UpdateStepSound,
	_GetActiveCSWeapon,
	_IsTaunting,
	_IsInThirdPersonTaunt,
	_IsBotPlayerResourceOffset,
	_StepSoundTime,
	_m_StuckLast,
	_m_VPhysicsObject,
	_m_vecWaterJumpVel,
	_m_flSwimSoundTime,
	_m_SurfaceProps,
	_m_iSurfaceData,
	_m_flTimeNotOnLadder,
	_m_vecDuckingOrigin,
	_m_duckUntilOnGround,
	_m_bServerSideJumpAnimation,
	_m_bSlowMovement,
	_m_bHasWalkMovedSinceLastJump,
	_m_pPredictionPlayer,
	_m_pPlayerCommand,
	_m_pCurrentCommand,
	_FreeEntityBaselines,
	_GetWeaponMaxSpeedVMT,
	_GetWeaponMaxSpeed2VMT,
	_WeaponOnJumpVMT,
	_WeaponGetZoomLevelVMT,
	_WeaponGetNumZoomLevelsVMT,
	_m_bInReload,
	_ParallelProcess,
	_ParallelQueueVT1,
	_ParallelQueueVT2,
	_DoExecute,
	_AbortJob,
	_ShotgunSpread,
	_GetMaxHealthVMT,
	_TakeDamage,
	_SurvivalCalcView,
	_SurvivalModeOrigin,
	_UnknownSurvivalBool,
	_GetHudPlayer,
	_UselessCalcViewSurvivalBool,
	_ThirdPersonSwitchVMT,
	_CanWaterJumpVMT,
	_GetEncumberance,
	_RenderHandle,
	_TE_FireBullets,
	_UserCmd_GetChecksum,
	_CL_SendMove,
	_SplitScreenMgr,
	_CL_SendMove_DefaultMemory,
	_CCLCMsg_Move_vtable1,
	_CCLCMsg_Move_vtable2,
	_CCLCMsg_Move_UnknownCall,
	_CCLCMsg_Move_Deconstructor_relative,
	_UnknownAnimationFloat,
	_UnknownAnimationCall,
	_PlayerFilterRules,
	_GetViewOffsetVMT,
	_GetSolidFlagsVMT,
	_PhysicsCheckWaterTransition,
	_PhysicsPushEntity,
	_GetUnknownEntity,
	_DesiredCollisionGroup,
	_EyePositionVMT,
	_MaintainSequenceTransitionsReturnAddress,
	_AccumulateLayersVMT,
	_boneFlagsOffset,
	_ClientEntityListArray,
	_HasC4,
	_FlashbangTime,
	_CacheSequences,
	_SetupLean,
	_SetupAimMatrix,
	_SetupWeaponAction,
	_SetupMovement,
	_SetSequenceVMT,
	_WeaponHasBurstVMT,
	_m_Activity,
	_IsFullAutoVMT,
	_GetCycleTimeVMT,
	_GetMaxClip1VMT,
	_GetReserveAmmoCount,
	_m_bFireOnEmpty,
	_m_bIsCustomPlayer,
	_UpdateViewModelAddonsSub,
	_UpdateViewModelAddonsVT,
	_ViewModelLabelHandle,
	_ViewModelStatTrackHandle,
	_RemoveViewModelStickers,
	_RemoveViewModelArmModels,
	_RemoveEntity,
	_OnLatchInterpolatedVariables,
	_FrameAdvance,
	_HandleTaserAnimation,
	_GetViewModel,
	_IsBaseCombatWeapon,
	_WeaponGetZoomFOVVMT,
	_FileSystemStringVMT,
	_SetSky,
	_FindElement,
	_Hud,
	_ClearDeathNotices,
	_IsInIronsight,
	_SetupVelocity,
	_UpdatePartition,
	_CAM_ToThirdPerson,
	_CAM_ToFirstPerson,
	_IronSightController,
	_OnLand,
	_NetSetConVar_Constructor,
	_NetSetConVar_Init,
	_NetSetConVar_Destructor,
	_NetIsMultiplayer,
	_CNetMsg_Tick_Constructor,
	_CNetMsg_Tick_Destructor,
	_CNetMsg_Tick_Setup,
	_CNetMsg_Tick_MiscAdr,
	_NET_SendPacketSig,
	_ViewMatrixPtr,
	_GetWeaponMaxSpeed3VMT,
	_LookupSequence,
	_GetWeaponMoveAnimation,
	_GetSequenceLinearMotion,
	_UpdateLayerOrderPreset,
	_UnknownSetupMovementFloat,
	_GetLayerIdealWeightFromSeqCycle,
	_GetSequenceCycleRate,
	_GetLayerSequenceCycleRate,
	_GetAnySequenceAnimTag,
	_GetPoseParameter,
	_FindKey,
	_GetInt,
	_CActivityToSequenceMapping_Reinitialize,
	_GetActivityListVersion,
	_g_nActivityListVersion,
	_IndexModelSequences,
	_g_ActivityModifiersTable,
	_m_afButtonLast,
	_m_nButtons,
	_m_afButtonPressed,
	_m_afButtonReleased,
	_symbolsInitialized,
	_s_pSymbolTable,
	_AddString,
	_FindString,
	_FindFieldByName,
	_GetPredDescMap,
	_GetPredictedFrame,
	_g_Predictables,
	_touchStamp,
	_doLocalPlayerPrePrediction,
	_PhysicsCheckForEntityUntouch,
	_PhysicsTouchTriggers,
	_MoveToLastReceivedPosition,
	_UnknownEntityPredictionBool,
	_StorePredictionResults,
	_m_nFinalPredictedTick,
	_m_pFirstPredictedFrame,
	_VPhysicsCompensateForPredictionErrorsVMT,
	_ComputeFirstCommandToExecute,
	_physenv,
	_g_BoneAccessStack,
	_g_BoneAcessBase,
	_RestoreOriginalEntityState,
	_RunSimulation,
	_RestoreEntityToPredictedFrame,
	_ShiftIntermediateDataForward,
	_ShiftFirstPredictedIntermediateDataForward,
	_SaveData,
	_ListLeavesInBox_ReturnAddrBytes,
	_IsVoiceRecording_Ptr,
	_Interpolate,
	_InterpolationVarMap,
	_DelayUnscope,
	_SelectWeightedSequence,
	_InitPose,
	_AccumulatePose,
	_CalcAutoplaySequences,
	_CalcBoneAdj,
	_IKConstruct,
	_IKDestruct,
	_ParticleProp,
	_SUB_Remove,
	_Spawn,
	_Precache,
	MAX_STATIC_OFFSETS
};

class CModule;

class StaticOffset
{
public:
	StaticOffsetName GetName() const { return Name; }
	StaticOffsetName GetIndex() const { return Name; }
	uint32_t GetOffset() const;
	template <class T, typename D> T GetOffsetByType(D addfrom = 0) { return (T)(GetOffset() + (uint32_t)addfrom); }
	template <class T> T GetOffsetByType() { return (T)GetOffset(); }
	template <class T, typename D> T GetVFuncByType(D vTable) { return (*(T**)(void*)vTable)[GetOffset()]; }
	bool HasBeenFound() const { return IsFound; }
	bool IsHardCoded() const { return DoNotSigScan; }
	size_t GetSignatureLength() const { return SignatureLength; }
	char* GetSignature() const { return Signature; }
	HANDLE GetHandle() const { return Handle; }
	bool ShouldDereference() const { return DereferenceTimes > 0; }
	bool ShouldDivideAfterDereference() const { return DivideAfterDereference != 0; }
	bool ShouldAddAtEnd() const { return AddAtEnd != 0; }
	int GetDereferenceTimes() const { return DereferenceTimes; }
	int GetAddBeforeDereference() const { return AddBeforeDereference; }
	int GetDivideAfterDereference() const { return DivideAfterDereference; }
	int GetAddAtEnd() const { return AddAtEnd; }
	int GetSizeOfValue() const { return SizeOfValue; }
	uint32_t GetRawAddress() const { return RawAddress ^ GetDecryptionKey(); }
	bool ShouldUseRawHandle() const { return UsesRawHandle; }
	uint32_t GetDecryptionKey() const { return DecryptionKey; }
	void GenerateDecryptionKey() { DecryptionKey = (uint32_t)__rdtsc(); }

	void SetFound(bool found) { IsFound = found; }
	void SetOffset(uint32_t offset) { Offset = offset; }
	void SetRawAddress(uint32_t adr) { RawAddress = adr; }

	void ClearIdentifiableData()
	{
		const uint32_t _Filler = (uint32_t)__rdtsc();
		Name = INVALID_STATIC_OFFSET;
		SignatureLength = 0;

#if defined MUTINY_FRAMEWORK && !defined (DEVELOPER)
		if (!IsHardCoded() && Signature != nullptr)
		{
			memset(Signature, _Filler, strlen(Signature) + 1);
			delete[] Signature;
		}
#endif
		Signature = nullptr;
		DereferenceTimes = _Filler;
		DivideAfterDereference = _Filler;
		AddAtEnd = _Filler;
		AddBeforeDereference = _Filler;
		SizeOfValue = _Filler;
		RawAddress = _Filler;
		Handle = (HANDLE)_Filler;
		UsesRawHandle = false;
		IsFound = false;
	}
	StaticOffset::StaticOffset()
	{
		Name = INVALID_STATIC_OFFSET;
		IsFound = false;
		DecryptionKey = UINT32_MAX;
	};
	StaticOffset::StaticOffset(StaticOffsetName name, size_t signaturelength, char* signature, HANDLE dllhandle, int dereferencetimes = 0, int addbeforedereference = 0, int divideafterdereference = 0, int addatend = 0, int sizeofvalue = 4, uint32_t decryptionkey = UINT32_MAX)
	{
		Name = name;
		Offset = 0;
		IsFound = false;
		DoNotSigScan = false;
		UsesRawHandle = false;
		SignatureLength = signaturelength;
		Signature = signature;
		Handle = dllhandle;
		DereferenceTimes = dereferencetimes;
		AddBeforeDereference = addbeforedereference;
		DivideAfterDereference = divideafterdereference;
		AddAtEnd = addatend;
		SizeOfValue = sizeofvalue;
		RawAddress = 0;
		DecryptionKey = decryptionkey;
		if (decryptionkey == UINT32_MAX)
			GenerateDecryptionKey();
	}
	StaticOffset::StaticOffset(StaticOffsetName name, uint32_t offset, uint32_t decryptionkey = UINT32_MAX)
	{
		Name = name;
		Offset = offset;
		IsFound = true;
		DoNotSigScan = true;
		UsesRawHandle = false;
		SignatureLength = 0;
		Signature = 0;
		DecryptionKey = decryptionkey;
		if (decryptionkey == UINT32_MAX)
			GenerateDecryptionKey();
	}
	StaticOffset::StaticOffset(StaticOffsetName name, uint32_t rawadr, HANDLE handle, int dereferencetimes, int addbeforedereference, int divideafterdereference, int addatend, int sizeofvalue, uint32_t decryptionkey = UINT32_MAX)
	{
		Name = name;
		Offset = 0;
		IsFound = true;
		DoNotSigScan = false;
		UsesRawHandle = true;
		RawAddress = rawadr;
		Handle = handle;
		DereferenceTimes = dereferencetimes;
		AddBeforeDereference = addbeforedereference;
		DivideAfterDereference = divideafterdereference;
		AddAtEnd = addatend;
		SizeOfValue = sizeofvalue;
		DecryptionKey = decryptionkey;
		if (decryptionkey == UINT32_MAX)
			GenerateDecryptionKey();
	}
private:
	StaticOffsetName Name;
public:
	uint32_t Offset;
	uint32_t DecryptionKey;
private:
	bool IsFound;
	bool DoNotSigScan;
	bool UsesRawHandle;
	size_t SignatureLength;
	char* Signature;
	HANDLE Handle;

	int DereferenceTimes;
	int AddBeforeDereference;
	int DivideAfterDereference;
	int AddAtEnd;
	int SizeOfValue;
	uint32_t RawAddress;
};

class CStaticOffsets
{
public:
	enum API : char
	{
		API_VERSION = 4
	};

	enum CachedStaticOffsetsHandle
	{
		CACHE_CLIENTDLL = 0,
		CACHE_ENGINEDLL,
		CACHE_VGUIMATSURFACE,
		CACHE_SHADERAPIDX9,
		CACHE_SERVERDLL,
		CACHE_UNKNOWN
	};

	CStaticOffsets();
	void AddAllOffsets();
	void UpdateAllOffsets();
	void HideStaticOffsets();
	bool ReadOffsetsFromFile();
	void DumpOffsetsToFile();
	StaticOffset& GetOffset(StaticOffsetName name);
	uint32_t GetOffsetValue(StaticOffsetName name);
	template <class T, typename D>
	T GetOffsetValueByType(const StaticOffsetName name, D addfrom)
	{
		return (T)GetOffset(name).GetOffsetByType<T>(addfrom);
	}

	template <class T>
	T GetOffsetValueByType(const StaticOffsetName name)
	{
		return (T)GetOffset(name).GetOffsetByType<T>();
	}

	template <class T, typename D>
	T GetVFuncByType(const StaticOffsetName name, D vTable)
	{
		return (T)GetOffset(name).GetVFuncByType<T, D>(vTable);
	}

private:
	void AddOffset(const StaticOffset& fulloffset);
	CachedStaticOffsetsHandle HandleToCachedHandle(HANDLE handle);
	HANDLE HandleToCachedHandle(CachedStaticOffsetsHandle dll);
public:
	std::array<StaticOffset, MAX_STATIC_OFFSETS> Offsets;
private:
	bool m_bAlreadyAdded;
};


extern CStaticOffsets StaticOffsets;