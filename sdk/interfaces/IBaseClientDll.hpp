#pragma once

#include "IAppSystem.hpp"
#include "..\misc\GlobalVars.hpp"
#include "..\misc\ClientClass.hpp"
#include "..\math\Vector.hpp"

enum ClientFrameStage_t
{
    FRAME_UNDEFINED = -1,
    FRAME_START,
    FRAME_NET_UPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_START,
    FRAME_NET_UPDATE_POSTDATAUPDATE_END,
    FRAME_NET_UPDATE_END,
    FRAME_RENDER_START,
    FRAME_RENDER_END
};

enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,
	//	DATA_UPDATE_ENTERED_PVS,
	DATA_UPDATE_DATATABLE_CHANGED
	//	DATA_UPDATE_LEFT_PVS,
	//DATA_UPDATE_DESTROYED,
};

// Used by RenderView
enum RenderViewInfo_t
{
    RENDERVIEW_UNSPECIFIED = 0,
    RENDERVIEW_DRAWVIEWMODEL = (1 << 0),
    RENDERVIEW_DRAWHUD = (1 << 1),
    RENDERVIEW_SUPPRESSMONITORRENDERING = (1 << 2),
};

// Used by firebullets hook, move somewhere else if u want to cleanup
class C_TEFireBullets
{
public:
    char pad[12];
    int		m_iPlayer; //12
    int _m_iItemDefinitionIndex;
    Vector	_m_vecOrigin;
    Vector	m_vecAngles;
    int		_m_iWeaponID;
    int		m_iMode;
    int		m_iSeed;
    float	m_flSpread;
};


inline C_TEFireBullets* TE_FireBullets;

class CEffectData
{
public:
	Vector m_vOrigin; //0x0
	Vector m_vStart; //0xC
	Vector m_vNormal; //0x18
	Vector m_vAngles; //0x24
	int		flags; //0x30
	int /*ClientEntityHandle_t*/ m_hEntity; //0x34
	int Pad; //0x38
	float	m_flScale; //0x3C
	float	m_flMagnitude; //0x40
	float	m_flRadius; //0x44
	int		m_nAttachmentIndex; //0x48
	short	m_nSurfaceProp; //0x4C
	short Pad2; //0x4E
	// Some TF2 specific things
	int		m_nMaterial; //0x50
	int		m_nDamageType; //0x54
	int		m_nHitBox; //0x58

	int		m_nOtherEntIndex; //0x5C

	unsigned char	m_nColor; //0x60
	bool m_bPositionsAreRelativeToEntity; //0x61
	char Pad3[2]; //0x62

	// Don't mess with stuff below here. DispatchEffect handles all of this.
public:
	CEffectData()
	{
		m_vOrigin.Init();
		m_vStart.Init();
		m_vNormal.Init();
		m_vAngles.Init();

		flags = 0;
		m_hEntity = -1;
		m_flScale = 1.f;
		m_nAttachmentIndex = 0;
		m_nSurfaceProp = 0;

		m_flMagnitude = 0.0f;
		m_flRadius = 0.0f;

		m_nMaterial = 0;
		m_nDamageType = 0;
		m_nHitBox = 0;

		m_nColor = 0;

		m_nOtherEntIndex = 0;
	}

	int GetEffectNameIndex() { return m_iEffectName; }

	void* GetRenderable() const;
	void* GetEntity() const;
	int entindex() const;

	//private:

	int m_iEffectName;	//0x64 // Entry in the EffectDispatch network string table. The is automatically handled by DispatchEffect().
};

class C_TEEffectDispatch
{
public:
	char pad[12];
	CEffectData m_EffectData;
	//C_TE_BloodStream
	// particle effect sort origin
	//char pad[9];
	//char			r, g, b, a;
	//char pad[12];
	//Vector origin; //12

	//char			m_nAmount;
	//char pad[4];
	//Vector m_vecOrigin;
	//Vector m_vecDirection;
};
inline C_TEEffectDispatch* TE_EffectDispatch;

class C_TEImpact : public C_TEEffectDispatch
{
public:
	C_TEEffectDispatch base;
	Vector			m_vecOrigin;
	Vector			m_vecNormal;
	int				m_iType;
	unsigned char	m_ucFlags;
};

class IBaseClientDLL
{
public:
    virtual int              Connect(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
    virtual int              Disconnect(void) = 0;
    virtual int              Init(CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals) = 0;
    virtual void             PostInit() = 0;
    virtual void             Shutdown(void) = 0;
    virtual void             LevelInitPreEntity(char const* pMapName) = 0;
    virtual void             LevelInitPostEntity() = 0;
    virtual void             LevelShutdown(void) = 0;
    virtual ClientClass*     GetAllClasses(void) = 0;

	bool DispatchUserMessage(int messageType, int arg, int arg1, void* data)
	{
		using DispatchUserMessage_t = bool* (__thiscall*)(void*, int, int, int, void*);
		return call_virtual<DispatchUserMessage_t>(this, 38)(this, messageType, arg, arg1, data);
	}
};

class ServerClass
{
public:
	const char* m_pNetworkName;
	void** m_pTable;
	ServerClass* m_pNext;
	int							m_ClassID;
	int							m_InstanceBaselineIndex;
};
class CStandardSendProxies;

enum EncryptedMessageKeyType_t
{
	// Warning: do not renumber the key types as they get baked in demos
	kEncryptedMessageKeyType_None = 0,
	kEncryptedMessageKeyType_Private = 1,
	kEncryptedMessageKeyType_Public = 2,
};


typedef int QueryCvarCookie_t;
typedef enum
{
	eQueryCvarValueStatus_ValueIntact = 0,	// It got the value fine.
	eQueryCvarValueStatus_CvarNotFound = 1,
	eQueryCvarValueStatus_NotACvar = 2,		// There's a ConCommand, but it's not a ConVar.
	eQueryCvarValueStatus_CvarProtected = 3	// The cvar was marked with FCVAR_SERVER_CAN_NOT_QUERY, so the server is not allowed to have its value.
} EQueryCvarValueStatus;

//-----------------------------------------------------------------------------
// Purpose: These are the interfaces that the game .dll exposes to the engine
//-----------------------------------------------------------------------------
class IServerGameDLL
{
public:
	// Initialize the game (one-time call when the DLL is first loaded )
	// Return false if there is an error during startup.
	virtual bool			DLLInit(CreateInterfaceFn engineFactory,
		CreateInterfaceFn physicsFactory,
		CreateInterfaceFn fileSystemFactory,
		void* pGlobals) = 0;

	// This is called when a new game is started. (restart, map)
	virtual bool			GameInit(void) = 0;

	// Called any time a new level is started (after GameInit() also on level transitions within a game)
	virtual bool			LevelInit(char const* pMapName,
		char const* pMapEntities, char const* pOldLevel,
		char const* pLandmarkName, bool loadGame, bool background) = 0;

	// The server is about to activate
	virtual void			ServerActivate(void* pEdictList, int edictCount, int clientMax) = 0;

	// The server should run physics/think on all edicts
	virtual void			GameFrame(bool simulating) = 0;

	// Called once per simulation frame on the final tick
	virtual void			PreClientUpdate(bool simulating) = 0;

	// Called when a level is shutdown (including changing levels)
	virtual void			LevelShutdown(void) = 0;
	// This is called when a game ends (server disconnect, death, restart, load)
	// NOT on level transitions within a game
	virtual void			GameShutdown(void) = 0;

	// Called once during DLL shutdown
	virtual void			DLLShutdown(void) = 0;

	// Get the simulation interval (must be compiled with identical values into both client and game .dll for MOD!!!)
	// Right now this is only requested at server startup time so it can't be changed on the fly, etc.
	virtual float			GetTickInterval(void) const = 0;

	// Give the list of datatable classes to the engine.  The engine matches class names from here with
	//  edict_t::classname to figure out how to encode a class's data for networking
	virtual ServerClass* GetAllServerClasses(void) = 0;

	// Returns string describing current .dll.  e.g., TeamFortress 2, Half-Life 2.  
	//  Hey, it's more descriptive than just the name of the game directory
	virtual const char* GetGameDescription(void) = 0;

	// Let the game .dll allocate it's own network/shared string tables
	virtual void			CreateNetworkStringTables(void) = 0;
};

inline IServerGameDLL* ServerGameDLL;



#include <map>


using FNV1A_t = std::uint32_t;

/*
 * 32-BIT FNV1A HASH
 * much proper to use fnv1a instead crc32 with randomly access ~1kb
 * @credits: underscorediscovery
 */
namespace FNV1A
{
	/* fnv1a constants */
	constexpr std::uint32_t ullBasis = 0x811C9DC5;
	constexpr std::uint32_t ullPrime = 0x1000193;

	/* create compile time hash */
	constexpr FNV1A_t HashConst(const char* szString, const FNV1A_t uValue = ullBasis) noexcept
	{
		return !*szString ? uValue : HashConst(&szString[1], (uValue ^ FNV1A_t(szString[0])) * ullPrime);
	}

	/* create runtime hash */
	inline const FNV1A_t Hash(const char* szString)
	{
		FNV1A_t uHashed = ullBasis;

		for (std::size_t i = 0U; i < strlen(szString); ++i)
		{
			uHashed ^= szString[i];
			uHashed *= ullPrime;
		}

		return uHashed;
	}
}

class ClassIdManager
{

	std::uintptr_t uid;
public:
	void GetClassIDALL()
	{
		ClassIdMap.clear();
		ServerClass* serverclass = ServerGameDLL->GetAllServerClasses();
		unsigned int id = 0;
		while (serverclass)
		{
			std::stringstream shonax;
			shonax << serverclass->m_pNetworkName;
			const FNV1A_t uHash = FNV1A::Hash(shonax.str().c_str());
			ClassIdMap[uHash] = id;
			serverclass = serverclass->m_pNext, id++;
		}
	}
	std::map<FNV1A_t, std::uintptr_t> ClassIdMap;
};

inline ClassIdManager* IdManager = new ClassIdManager();