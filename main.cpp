// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <ShlObj.h>
#include <ShlObj_core.h>
#include "includes.hpp"
#include "utils\ctx.hpp"
#include "utils\recv.h"
#include "utils\imports.h"
//#include "utils\anti_debug.h"
#include "nSkinz\SkinChanger.h"
#include "minhook/minhook.h"
#include "mutiny merge/console.hpp"
#include "mutiny merge/NetworkedVariables.h"
#include "mutiny merge/merged.h"
#include "supremacy merge/fff.h"

#include "inventory/kitparser.h"
#include "skinchanger/skins.h"
#include "fake-case-opening/csgo_fake_opening.hpp"
//насрал

class zInterfaceReg
{
private:
	using InstantiateInterfaceFn = void* (*)();
public:
	InstantiateInterfaceFn m_CreateFn;
	const char* m_pName;
	zInterfaceReg* m_pNext;
};
template<typename T>
static T* zget_interface(const char* mod_name, const char* interface_name, bool exact = false) {
	T* iface = nullptr;
	zInterfaceReg* register_list;
	int part_match_len = strlen(interface_name); //-V103

	DWORD interface_fn = reinterpret_cast<DWORD>(GetProcAddress(GetModuleHandleA(mod_name), ("CreateInterface")));

	if (!interface_fn) {
		return nullptr;
	}

	unsigned int jump_start = (unsigned int)(interface_fn)+4;
	unsigned int jump_target = jump_start + *(unsigned int*)(jump_start + 1) + 5;

	register_list = **reinterpret_cast<zInterfaceReg***>(jump_target + 6);

	for (zInterfaceReg* cur = register_list; cur; cur = cur->m_pNext) {
		if (exact == true) {
			if (strcmp(cur->m_pName, interface_name) == 0)
				iface = reinterpret_cast<T*>(cur->m_CreateFn());
		}
		else {
			if (!strncmp(cur->m_pName, interface_name, part_match_len) && std::atoi(cur->m_pName + part_match_len) > 0) //-V106
				iface = reinterpret_cast<T*>(cur->m_CreateFn());
		}
	}
	return iface;
}
//inline unsigned int get_virtual(void* _class, unsigned int index) { return static_cast<unsigned int>((*static_cast<int**>(_class))[index]); }

class CHLClient
{
	template < typename t = Address >
	t get_method(Address this_ptr, size_t index) {
		return (t)this_ptr.to< t* >()[index];
	}
public:
	ClientClass* GetAllClasses() {
		return get_method< ClientClass* (__thiscall*)(decltype(this))>(this, 8)(this);
	}
};
inline CHLClient* fm_client;


class Netvars_merge {
private:
	struct NetvarData {
		bool        m_datamap_var; // we can't do proxies on stuff from datamaps :).
		RecvProp* m_prop_ptr;
		size_t      m_offset;

		__forceinline NetvarData() : m_datamap_var{}, m_prop_ptr{}, m_offset{} { }
	};

	std::unordered_map< hash32_t, int > m_client_ids;

	// netvar container.
	std::unordered_map< hash32_t,		// hash key + tables
		std::unordered_map< hash32_t,	// hash key + props
		NetvarData						// prop offset / prop ptr
		> > m_offsets;

public:
	// ctor.
	Netvars_merge() : m_offsets{} {}

	void init() {
		ClientClass* list;



		// sanity check on client.
		if (!fm_client)
			return;

		// grab linked list.
		list = fm_client->GetAllClasses();
		if (!list)
			return;

		// iterate list of netvars.
		for (; list != nullptr; list = list->m_pNext)
			StoreDataTable(list->m_pRecvTable->netTableName, list->m_pRecvTable);

		// find all datamaps
		FindAndStoreDataMaps();
	}

	// dtor.
	~Netvars_merge() {
		m_offsets.clear();
	}

	// gather all classids dynamically on runtime.
	void SetupClassData() {
		ClientClass* list;

		// clear old shit.
		m_client_ids.clear();

		// sanity check on client.
		if (!m_client())
			return;

		// grab linked list.
		list = m_client()->GetAllClasses();
		if (!list)
			return;

		// iterate list.
		for (; list != nullptr; list = list->m_pNext)
			m_client_ids[FNV1a::get(list->m_pNetworkName)] = list->m_ClassID;
	}

	void StoreDataTable(const std::string& name, RecvTable* table, size_t offset = 0) {
		hash32_t	var, base{ FNV1a::get(name) };
		RecvProp* prop;
		RecvTable* child;

		// iterate props
		for (int i{}; i < table->propCount; ++i) {
			prop = &table->props[i];
			child = prop->dataTable;

			// we have a child table, that contains props.
			if (child && child->propCount > 0)
				StoreDataTable(name, child, prop->offset + offset);

			// hash var name.
			var = FNV1a::get(prop->name);

			// insert if not present.
			if (!m_offsets[base][var].m_offset) {
				m_offsets[base][var].m_datamap_var = false;
				m_offsets[base][var].m_prop_ptr = prop;
				m_offsets[base][var].m_offset = (size_t)(prop->offset + offset);
			}
		}
	}

	// iterate client module and find all datamaps.
	void FindAndStoreDataMaps() {
		pattern::patterns_t matches{};

		matches = pattern::FindAll(PE::GetModule(HASH("client.dll")), ("C7 05 ? ? ? ? ? ? ? ? C7 05 ? ? ? ? ? ? ? ? C3 CC"));
		if (matches.empty())
			return;

		for (auto& m : matches)
			StoreDataMap(m);
	}

	void StoreDataMap(Address ptr) {
		datamap_t* map;
		hash32_t            base, var;
		typedescription_t* entry;

		// get datamap and verify.
		map = ptr.at(2).sub(4).as< datamap_t* >();

		//dataNumFields - m_size
		//dataClassName - m_name

		if (!map || !map->dataNumFields || map->dataNumFields > 200 || !map->dataDesc || !map->dataClassName)
			return;

		// hash table name.
		base = FNV1a::get(map->dataClassName);

		for (int i{}; i < map->dataNumFields; ++i) {
			entry = &map->dataDesc[i];
			if (!entry->fieldName)
				continue;

			// hash var name.
			var = FNV1a::get(entry->fieldName);

			// if we dont have this var stored yet.
			if (!m_offsets[base][var].m_offset) {
				m_offsets[base][var].m_datamap_var = true;
				m_offsets[base][var].m_offset = (size_t)entry->fieldOffset[TD_OFFSET_NORMAL];
				m_offsets[base][var].m_prop_ptr = nullptr;
			}
		}
	}

	// get client id.
	int GetClientID(hash32_t network_name) {
		return m_client_ids[network_name];
	}

	// get netvar offset.
	size_t get(hash32_t table, hash32_t prop) {
		return m_offsets[table][prop].m_offset;
	}

	// get netvar proxy.
	recvProxy GetProxy(hash32_t table, hash32_t prop, recvProxy proxy) {
		// todo - dex; should we return null here if m_is_valid_netvar is false? probably doesn't matter anyway.
		return m_offsets[table][prop].m_prop_ptr->proxy;
	}

	// set netvar proxy.
	void SetProxy(hash32_t table, hash32_t prop, void* proxy, recvProxy& original) {
		auto netvar_entry = m_offsets[table][prop];

		// we can't set a proxy on a datamap.
		if (netvar_entry.m_datamap_var)
			return;

		// save original.
		original = netvar_entry.m_prop_ptr->proxy;

		// redirect.
		netvar_entry.m_prop_ptr->proxy = (recvProxy)proxy;
	}
};

inline Netvars_merge g_netvars;

//#include "utils\sha-256.h"
//#include "utils\protect.h"

//using namespace jwt::params;

enum error_type
{
	ERROR_NONE,
	ERROR_DEBUG,
	ERROR_OPEN_KEY,
	ERROR_QUERY_DATA,
	ERROR_CONNECT,
	ERROR_1,
	ERROR_2,
	ERROR_3,
	ERROR_4,
	ERROR_5,
	ERROR_6,
	ERROR_7,
	ERROR_8,
	ERROR_9,
	ERROR_CHECK_HASH
};

PVOID base_address = nullptr;
//Anti_debugger anti_debugger;
volatile error_type error = ERROR_NONE;

LONG CALLBACK ExceptionHandler(EXCEPTION_POINTERS* ExceptionInfo);
__forceinline void crash(bool debug = false);
__forceinline void setup_render();
__forceinline void setup_netvars();
__forceinline void setup_skins();
__forceinline void setup_hooks();

bool checkFile(std::string file_name) {
	std::ifstream file;
	file.open(file_name);
	if (!file)
		return false;
	return true;
}

DWORD WINAPI main(PVOID base)
{

	// grab some modules.
	auto m_kernel32_dll = PE::GetModule(HASH("kernel32.dll"));
	auto m_user32_dll = PE::GetModule(HASH("user32.dll"));
	auto m_shell32_dll = PE::GetModule(HASH("shell32.dll"));
	auto m_shlwapi_dll = PE::GetModule(HASH("shlwapi.dll"));
	auto m_client_dll = PE::GetModule(HASH("client.dll"));
	auto m_engine_dll = PE::GetModule(HASH("engine.dll"));
	auto m_vstdlib_dll = PE::GetModule(HASH("vstdlib.dll"));
	auto m_tier0_dll = PE::GetModule(HASH("tier0.dll"));
	auto m_shaderapidx9_dll = PE::GetModule(HASH("shaderapidx9.dll"));

	// import winapi functions.
	g_winapi.WideCharToMultiByte = PE::GetExport(m_kernel32_dll, HASH("WideCharToMultiByte")).as< WinAPI::WideCharToMultiByte_t >();
	g_winapi.MultiByteToWideChar = PE::GetExport(m_kernel32_dll, HASH("MultiByteToWideChar")).as< WinAPI::MultiByteToWideChar_t >();
	g_winapi.GetTickCount = PE::GetExport(m_kernel32_dll, HASH("GetTickCount")).as< WinAPI::GetTickCount_t >();
	g_winapi.VirtualProtect = PE::GetExport(m_kernel32_dll, HASH("VirtualProtect")).as< WinAPI::VirtualProtect_t >();
	g_winapi.VirtualQuery = PE::GetExport(m_kernel32_dll, HASH("VirtualQuery")).as< WinAPI::VirtualQuery_t >();
	g_winapi.CreateDirectoryA = PE::GetExport(m_kernel32_dll, HASH("CreateDirectoryA")).as< WinAPI::CreateDirectoryA_t >();
	g_winapi.SetWindowLongA = PE::GetExport(m_user32_dll, HASH("SetWindowLongA")).as< WinAPI::SetWindowLongA_t >();
	g_winapi.CallWindowProcA = PE::GetExport(m_user32_dll, HASH("CallWindowProcA")).as< WinAPI::CallWindowProcA_t >();
	g_winapi.SHGetFolderPathA = PE::GetExport(m_shell32_dll, HASH("SHGetFolderPathA")).as< WinAPI::SHGetFolderPathA_t >();
	g_winapi.PathAppendA = PE::GetExport(m_shlwapi_dll, HASH("PathAppendA")).as< WinAPI::PathAppendA_t >();



	g_csgo.fRandomInt = PE::GetExport(m_vstdlib_dll, HASH("RandomInt")).as< RandomInt_t >();


	g_ctx.signatures =
	{
		crypt_str("A1 ? ? ? ? 50 8B 08 FF 51 0C"),
		crypt_str("B9 ?? ?? ?? ?? A1 ?? ?? ?? ?? FF 10 A1 ?? ?? ?? ?? B9"),
		crypt_str("0F 11 05 ?? ?? ?? ?? 83 C8 01"),
		crypt_str("8B 0D ?? ?? ?? ?? 8B 46 08 68"),
		crypt_str("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10"),
		crypt_str("8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7"),
		crypt_str("A1 ? ? ? ? 8B 0D ? ? ? ? 6A 00 68 ? ? ? ? C6"),
		crypt_str("80 3D ? ? ? ? ? 53 56 57 0F 85"),
		crypt_str("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C"),
		crypt_str("80 3D ? ? ? ? ? 74 06 B8"),
		crypt_str("55 8B EC 83 E4 F0 B8 D8"),
		crypt_str("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C"),
		crypt_str("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6"),
		crypt_str("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36"),
		crypt_str("56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 21"),
		crypt_str("55 8B EC 83 E4 F8 83 EC 5C 53 8B D9 56 57 83"),
		crypt_str("55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9"),
		crypt_str("57 8B F9 8B 07 8B 80 ? ? ? ? FF D0 84 C0 75 02"),
		crypt_str("55 8B EC 81 EC ? ? ? ? 53 8B D9 89 5D F8 80"),
		crypt_str("53 0F B7 1D ? ? ? ? 56"),
		crypt_str("8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6")
	};
	shonax_void();

	g_ctx.indexes =
	{
		5,
		33,
		339,
		218,
		219,
		34,
		157,
		75,
		460,
		482,
		452,
		483,
		284,
		223,
		246,
		27,
		17,
		123
	};

	while (!GetModuleHandle(crypt_str("serverbrowser.dll")))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	base_address = base;
	IdManager->GetClassIDALL();
	//shonax
	static TCHAR path[MAX_PATH];
	std::string folder;
	SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path);
	folder = std::string(path) + crypt_str("\\Lolihook\\");
	CreateDirectory(folder.c_str(), 0);
	folder += ("\\Inventory\\");
	CreateDirectory(folder.c_str(), 0);
	auto ff = folder + "inventory.hack";
	if (!checkFile(ff))
	{
		std::ofstream outfile(ff);
		outfile.close();
	}

	//shonax


	/*some steam interfaces*/
	typedef uint32_t SteamPipeHandle;
	typedef uint32_t SteamUserHandle;
	SteamUserHandle hSteamUser = ((SteamUserHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamUser"))();
	SteamPipeHandle hSteamPipe = ((SteamPipeHandle(__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamAPI_GetHSteamPipe"))();
	SteamClient = ((ISteamClient * (__cdecl*)(void))GetProcAddress(GetModuleHandle("steam_api.dll"), "SteamClient"))();
	SteamGameCoordinator = (ISteamGameCoordinator*)SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, "SteamGameCoordinator001");
	SteamUser = (ISteamUser*)SteamClient->GetISteamUser(hSteamUser, hSteamPipe, "SteamUser019");
	SteamFriends = SteamClient->GetISteamFriends(hSteamUser, hSteamPipe, "SteamFriends015");
	static auto SteamInventory = SteamClient->GetISteamInventory(hSteamUser, hSteamPipe, "STEAMINVENTORY_INTERFACE_V002");
	auto _ = SteamFriends->GetLargeFriendAvatar(SteamUser->GetSteamID());
	SteamUtils = SteamClient->GetISteamUtils(hSteamPipe, "SteamUtils009");




	g_csgo.studioHdr = pattern::find(m_client_dll, ("8B 86 ? ? ? ? 89 44 24 10 85 C0")).add(2).to< size_t >();
	g_csgo.LockStudioHdr = pattern::find(m_client_dll, ("55 8B EC 51 53 8B D9 56 57 8D B3"));
	g_csgo.AnimOverlay = pattern::find(m_client_dll, ("8B 80 ? ? ? ? 8D 34 C8")).add(0x2).to< size_t >();
	g_csgo.SetAbsOrigin = pattern::find(m_client_dll, ("55 8B EC 83 E4 F8 51 53 56 57 8B F1"));
	g_csgo.SetAbsAngles = pattern::find(m_client_dll, ("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));
	g_csgo.AngleMatrix = pattern::find(m_client_dll, ("E8 ? ? ? ? 8B 07 89 46 0C")).rel32(0x1).as< AngleMatrix_t >();
	g_csgo.PlayerAnimState = pattern::find(m_client_dll, ("8B 8E ? ? ? ? 85 C9 74 3E")).add(2).to< size_t >();




	kit_parser.setup();
	g_Skins.ParseSkins();
	g_pIClientLeafSystem = zget_interface<IClientLeafSystem>(("client.dll"), ("ClientLeafSystem"));

	setup_sounds();


	setup_skins();


	setup_netvars();


	setup_render();


	cfg_manager->setup();

	c_lua::get().initialize();


	key_binds::get().initialize_key_binds();

	setup_hooks();
	Netvars::Netvars();

	std::cout << crypt_str("Done!");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	return EXIT_SUCCESS;

}

std::filesystem::path GetWorkingPath()
{
	std::filesystem::path fsWorkingPath;

	// get path to user documents
	if (PWSTR pszPathToDocuments; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0UL, nullptr, &pszPathToDocuments)))
	{
		fsWorkingPath.assign(pszPathToDocuments);
		fsWorkingPath.append((".base"));
		CoTaskMemFree(pszPathToDocuments);
	}

	return fsWorkingPath;
}


DWORD WINAPI nn(LPVOID lpThreadParameter)
{
	static int generate_number = 0;

	while (true)
	{
		for (auto& p : std::filesystem::directory_iterator(GetWorkingPath() / ("custom image\\")))
			generate_number++;
		g_ctx.generate_last_number = generate_number;
		if (!g_ctx.generate_last_number)
			g_ctx.custom_reload_picture = false;
		generate_number = 0;
	}
}

DWORD WINAPI custom(LPVOID lpThreadParameter)
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
		if (!g_ctx.generate_last_number)
			g_ctx.custom_counter = 0;
		if (g_ctx.custom_counter != g_ctx.generate_last_number - 1)
			g_ctx.custom_counter++;
		else
			g_ctx.custom_counter = 0;
	}
}

HANDLE FindHandle(std::string name)
{
	return GetModuleHandle(name.c_str());
}

#include "mutiny merge/merged.h"

DWORD WINAPI mutiny_merge(LPVOID lpThreadParameter)
{
	shonax_console.initialize();

	//logger::add(LSUCCESS, "reached mutiny merge thread");


	if (!ClientHandle)
		ClientHandle = FindHandle("client.dll");


	if (!MatchmakingHandle)
		MatchmakingHandle = FindHandle("matchmaking.dll");


	if (!VPhysicsHandle)
		VPhysicsHandle = FindHandle("vphysics.dll");


	if (!VSTDLIBHandle)
		VSTDLIBHandle = FindHandle("vstdlib.dll");

	if (!DatacacheHandle)
		DatacacheHandle = FindHandle("datacache.dll");

	if (!Tier0Handle)
		Tier0Handle = FindHandle("tier0.dll");

	if (!MaterialSystemHandle)
		MaterialSystemHandle = FindHandle("materialsystem.dll");

	if (!StudioRenderHandle)

		StudioRenderHandle = FindHandle(("studiorender.dll"));

	if (!FileSystemStdioHandle)
		FileSystemStdioHandle = FindHandle(("filesystem_stdio.dll"));
	if (!ServerHandle)

		ServerHandle = FindHandle(("server.dll"));
	if (!EngineHandle)
		EngineHandle = FindHandle(("engine.dll"));

	if (!VGUIMatSurfaceHandle)
		VGUIMatSurfaceHandle = FindHandle(("vguimatsurface.dll"));


	logger::add(LSUCCESS, "so it will be hook some usefull functions and console will be closed at the end");

	logger::add(LWARN, "you already can use cheat!!!");


	shonax_mutiny.run();



	shonax_console.close();

	return true;

}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);

		g_ctx.test_hooks = true;

		auto path = GetWorkingPath();
		std::filesystem::create_directories(path);
		std::filesystem::create_directories(path / ("custom image\\"));
		//g_ctx.custom_saves = std::filesystem::directory_iterator(g_ctx.path / ("custom image\\"));
		CreateThread(nullptr, 0, main, hModule, 0, nullptr); //-V718 //-V513
		CreateThread(nullptr, 0, nn, hModule, 0, nullptr); //-V718 //-V513
		CreateThread(nullptr, 0, custom, hModule, 0, nullptr); //-V718 //-V513
		CreateThread(nullptr, 0, mutiny_merge, hModule, 0, nullptr); //-V718 //-V513
		csgo_fake_opening::my_module = hModule;
		CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(csgo_fake_opening::init), NULL, 0, NULL);

		return TRUE;
	}
}



__forceinline void setup_render()
{
	static auto create_font = [](const char* name, int size, int weight, DWORD flags) -> vgui::HFont
	{
		g_ctx.last_font_name = name;

		auto font = m_surface()->FontCreate();
		m_surface()->SetFontGlyphSet(font, name, size, weight, 0, 0, flags);

		return font;
	};

	fonts[LOGS] = create_font(crypt_str("Lucida Console"), 10, FW_MEDIUM, FONTFLAG_DROPSHADOW);
	fonts[ESP] = create_font(crypt_str("Smallest Pixel-7"), 11, FW_MEDIUM, FONTFLAG_OUTLINE);
	fonts[NAME] = create_font(crypt_str("Verdana"), 12, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[SUBTABWEAPONS] = create_font(crypt_str("undefeated"), 13, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);

	fonts[KNIFES] = create_font(crypt_str("icomoon"), 13, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[GRENADES] = create_font(crypt_str("undefeated"), 20, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[INDICATORFONT] = create_font(crypt_str("Calibri"), 26, 100, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	fonts[DAMAGE_MARKER] = create_font(crypt_str("CrashNumberingGothic"), 15, FW_HEAVY, FONTFLAG_ANTIALIAS | FONTFLAG_OUTLINE);
	fonts[TAHOMA_NORMAL] = create_font(crypt_str("Tahoma"), 15, FW_HEAVY, FONTFLAG_NONE);
	fonts[VELOCITY] = create_font(crypt_str("Tahoma"), 25, FW_HEAVY, FONTFLAG_ANTIALIAS);

	g_ctx.last_font_name.clear();
}

__forceinline void setup_netvars()
{
	netvars::get().tables.clear();
	auto client = m_client()->GetAllClasses();

	if (!client)
		return;

	while (client)
	{
		auto recvTable = client->m_pRecvTable;

		if (recvTable)
			netvars::get().tables.emplace(std::string(client->m_pNetworkName), recvTable);

		client = client->m_pNext;
	}
}

__forceinline void setup_skins()
{
	auto items = std::ifstream(crypt_str("csgo/scripts/items/items_game_cdn.txt"));
	auto gameItems = std::string(std::istreambuf_iterator <char> { items }, std::istreambuf_iterator <char> { });

	if (!items.is_open())
		return;

	items.close();
	memory.initialize();

	for (auto i = 0; i <= memory.itemSchema()->paintKits.lastElement; i++)
	{
		auto paintKit = memory.itemSchema()->paintKits.memory[i].value;

		if (paintKit->id == 9001)
			continue;

		auto itemName = m_localize()->FindSafe(paintKit->itemName.buffer + 1);
		auto itemNameLength = WideCharToMultiByte(CP_UTF8, 0, itemName, -1, nullptr, 0, nullptr, nullptr);

		if (std::string name(itemNameLength, 0); WideCharToMultiByte(CP_UTF8, 0, itemName, -1, &name[0], itemNameLength, nullptr, nullptr))
		{
			if (paintKit->id < 10000)
			{
				if (auto pos = gameItems.find('_' + std::string{ paintKit->name.buffer } + '='); pos != std::string::npos && gameItems.substr(pos + paintKit->name.length).find('_' + std::string{ paintKit->name.buffer } + '=') == std::string::npos)
				{
					if (auto weaponName = gameItems.rfind(crypt_str("weapon_"), pos); weaponName != std::string::npos)
					{
						name.back() = ' ';
						name += '(' + gameItems.substr(weaponName + 7, pos - weaponName - 7) + ')';
					}
				}
				SkinChanger::skinKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
			else
			{
				std::string_view gloveName{ paintKit->name.buffer };
				name.back() = ' ';
				name += '(' + std::string{ gloveName.substr(0, gloveName.find('_')) } + ')';
				SkinChanger::gloveKits.emplace_back(paintKit->id, std::move(name), paintKit->name.buffer);
			}
		}
	}

	std::sort(SkinChanger::skinKits.begin(), SkinChanger::skinKits.end());
	std::sort(SkinChanger::gloveKits.begin(), SkinChanger::gloveKits.end());
}

__declspec(naked) void shonax_create_move(int sequence_number, float input_sample_frametime, bool active)
{
	__asm
	{
		push ebp
		mov  ebp, esp
		push ebx;
		push esp
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call hooks::hl_create_move
			pop  ebx
			pop  ebp
			retn 0Ch
	}
}

int __stdcall hGetUnverifiedFileHashes(void* _this, void* someclass, int nMaxFiles)
{
	return 0;
}

void __fastcall hooked_do_procedural_foot_plant(void* ecx,
	void* edx, void* boneToWorld, void* pLeftFootChain, void* pRightFootChain, void* pos)
{
	return;
}
typedef void(__thiscall* do_procedural_foot_plant_fn)(void*, void*, void*, void*, void*);
do_procedural_foot_plant_fn do_procedural_foot_plant_original;

vmthook* what_ever_want_die;
using GetBool_t = bool(__thiscall*)(void*);

bool NetShowFragmentsGetBool() {
	if (!m_engine()->IsInGame())
		return what_ever_want_die->get_func_address< GetBool_t >(13);

	static auto read_sub_channel_data_ret = pattern::find(PE::GetModule(HASH("engine.dll")), "85 C0 74 12 53 FF 75 0C 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 0C").as< uintptr_t*>();
	static auto check_receiving_list_ret = pattern::find(PE::GetModule(HASH("engine.dll")), "8B 1D ? ? ? ? 85 C0 74 16 FF B6").as< uintptr_t*>();

	static uint32_t last_fragment = 0;

	if (_ReturnAddress() == reinterpret_cast<void*>(read_sub_channel_data_ret) && last_fragment > 0)
	{
		const auto data = &reinterpret_cast<uint32_t*>(m_clientstate()->pNetChannel)[0x56];
		const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

		if (bytes_fragments == last_fragment)
		{
			auto& buffer = reinterpret_cast<uint32_t*>(data)[0x42];
			buffer = 0;
		}
	}

	if (_ReturnAddress() == check_receiving_list_ret)
	{
		const auto data = &reinterpret_cast<uint32_t*>(m_clientstate()->pNetChannel)[0x56];
		const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

		last_fragment = bytes_fragments;
	}

	return what_ever_want_die->get_func_address< GetBool_t >(13);
}



class CMatchSessionOnlineHost {
public:
};

class CMatchFramework {
public:
	enum indices : size_t {
		GETMATCHSESSION = 13,
	};
};

CMatchFramework* m_match_framework;
using GetMatchSession_t = CMatchSessionOnlineHost * (__thiscall*)(void*);

#include "../cheats/misc/logs.h"
VMT m_match_framework_hook;
CMatchSessionOnlineHost* GetMatchSession() {
	Stack       stack;
	Address     kv;
	std::string map;

	// ClientJob_EMsgGCCStrike15_v2_MatchmakingGC2ClientReserve::BYieldingRunJobFromMsg
	static Address BYieldingRunJobFromMsg{ pattern::find(PE::GetModule(HASH("client.dll")), ("8B D8 85 DB 0F 84 ? ? ? ? 8B 13 8B CB 68 ? ? ? ? 68")) };

	// NOTE;
	// string xref "game/mmqueue" above that string is a virtual call which is g_pMatchFramework->GetMatchSession( )
	// string xref "Matchmaking reservation confirmed: %llx/%s\n"

	if (stack.ReturnAddress() == BYieldingRunJobFromMsg) {
		// note - nitro:
		// deref ebp = old stack -> old stack - 0xA8 -> keyvalue ptr -> deref keyvalue ptr + 0x24 = mapname

		kv = stack.next().local(0xA8).get();

		if (kv && kv.at(0x24)) {
			map = { kv.at(0x24).as< char* >() };
			std::stringstream shonax;
			shonax << "map :" << map;
			eventlogs::get().add(shonax.str().c_str(), false); //-V106
		}
	}

	// call og.
	return m_match_framework_hook.GetOldMethod< GetMatchSession_t >(CMatchFramework::GETMATCHSESSION)(m_match_framework);
}


template< typename o = void*, typename i = void* >
o force_cast_p(i in) {
	union { i in; o out; }
	u = { in };
	return u.out;
};


template< typename o = void*, typename i = void* >
__forceinline o f_force_cast(i in) {
	union { i in; o out; }
	u = { in };
	return u.out;
};



VMT f_client;
VMT m_net_channel;
using LevelInitPostEntity_t = void(__thiscall*)(void*);
using LevelShutdown_t = void(__thiscall*)(void*);
using LevelInitPreEntity_t = void(__thiscall*)(void*, const char*);
using SendNetMsg_t = bool(__thiscall*)(void*, INetMessage&, bool, bool);
using ProcessPacket_t = void(__thiscall*)(void*, void*, bool);
class fooks {
public:
	void                     LevelInitPostEntity();
	void                     LevelInitPreEntity(const char* map);
	void                     ProcessPacket(void* packet, bool header);
	bool					 SendNetMsg(INetMessage& msg, bool bForceReliable, bool bVoice);
};
#include "cheats/misc/logs.h"
void fooks::LevelInitPreEntity(const char* map) {
	std::stringstream yah;
	yah << "welcome on " << map;
	eventlogs::get().add(yah.str(), false); //-V106
	f_client.GetOldMethod< LevelInitPreEntity_t >(5)(this, map);
}


void fooks::ProcessPacket(void* packet, bool header) {
	m_net_channel.GetOldMethod< ProcessPacket_t >(39)(this, packet, header);

	// get this from CL_FireEvents string "Failed to execute event for classId" in engine.dll
	for (CEventInfo* it{ m_clientstate()->m_events }; it != nullptr; it = it->m_next) {
		if (!it->m_class_id)
			continue;

		// set all delays to instant.
		it->m_fire_delay = 0.f;
	}

	// game events are actually fired in OnRenderStart which is WAY later after they are received
	// effective delay by lerp time, now we call them right after theyre received (all receive proxies are invoked without delay).
	m_engine()->FireEvents();
}

bool fooks::SendNetMsg(INetMessage& msg, bool bForceReliable, bool bVoice)
{
	if (!m_engine()->GetNetChannelInfo())
		return m_net_channel.GetOldMethod<SendNetMsg_t>(40)(this, msg, bForceReliable, bVoice);

	if (m_gamerules()->m_bIsValveDS() && msg.GetType() == 14) // Return and don't send messsage if its FileCRCCheck
		return false;

	if (msg.GetGroup() == 9) { // Fix lag when transmitting voice and fakelagging
		bVoice = true;
	}

	return m_net_channel.GetOldMethod<SendNetMsg_t>(40)(this, msg, bForceReliable, bVoice);
}




void fooks::LevelInitPostEntity() {


	// if the INetChannelInfo pointer has changed, store it for later.
	auto m_net = m_engine()->GetNetChannelInfo();

	if (m_net) {
		m_net_channel.reset();
		m_net_channel.init(m_net);
		m_net_channel.add(39 /*PROCESSPACKET*/, f_force_cast(&fooks::ProcessPacket));
		//m_net_channel.add(INetChannel::SENDDATAGRAM, util::force_cast(&Hooks::SendDatagram));
		m_net_channel.add(40 /*SENDNETMSG*/, f_force_cast(&fooks::SendNetMsg));
	}




	f_client.GetOldMethod< LevelInitPostEntity_t >(6)(this);
}

recvProxy m_Force_original;




void Force_proxy(recvProxyData& data, Address ptr, Address out) {
	// convert to ragdoll.
	Ragdoll* ragdoll = ptr.as< Ragdoll* >();

	// get ragdoll owner.
	player_t* player = ragdoll->GetPlayer();

	// we only want this happening to noobs we kill.
	if (g_cfg.misc.ragdolls && g_ctx.local() && player && player->m_iTeamNum() != g_ctx.local()->m_iTeamNum()) {
		// get m_vecForce.
		Vector vel = { data.value._x, data.value._y, data.value._z };

		// give some speed to all directions.
		vel *= 1000.f;
		vel.x *= -100 * g_cfg.misc.ragdol_force_x;
		vel.y *= -100 * g_cfg.misc.ragdol_force_y;

		// boost z up a bit.
		if (vel.z <= 1.f)
			vel.z = 2.f;

		vel.z *= -800000.f * g_cfg.misc.ragdol_force;
#undef max
		// don't want crazy values for this... probably unlikely though?
		math::clamp(vel.x, std::numeric_limits< float >::lowest(), std::numeric_limits< float >::max());
		math::clamp(vel.y, std::numeric_limits< float >::lowest(), std::numeric_limits< float >::max());
		math::clamp(vel.z, std::numeric_limits< float >::lowest(), std::numeric_limits< float >::max());

		// set new velocity.
		data.value._x = vel.x;
		data.value._y = vel.y;
		data.value._z = vel.z;
	}

	if (m_Force_original)
		m_Force_original(data, ptr, out);
}

class c_bphook
{
public:
	explicit c_bphook(void* ent)
	{
		base = reinterpret_cast<uintptr_t**>(ent);
	}

	template<typename function, typename original_function>
	function apply(uint32_t index, original_function func)
	{
		auto old_func = base[index];
		DWORD old;
		VirtualProtect(&base[index], sizeof uintptr_t, PAGE_EXECUTE_READWRITE, &old);
		base[index] = reinterpret_cast<uintptr_t*>(func);
		VirtualProtect(&base[index], sizeof uintptr_t, old, &old);
		return reinterpret_cast<function>(old_func);
	}
private:

	std::uintptr_t** base;
};

c_bphook* clientstateplus8;


using temp_entities_fn = bool(__thiscall*)(void*, void*);
temp_entities_fn orig_temp_entities;
bool __fastcall temp_entities(void* ECX, void* EDX, void* msg)
{
	if (!g_ctx.local() || !m_engine()->IsInGame())
		return orig_temp_entities(ECX, msg);

	const bool ret = orig_temp_entities(ECX, msg);

	// Filtering events
	if (!g_ctx.local()->is_alive())
		return ret;

	CEventInfo* ei = m_clientstate()->m_events;
	CEventInfo* next = nullptr;

	if (!ei)
		return ret;
	do
	{
		next = *reinterpret_cast<CEventInfo**>(reinterpret_cast<uintptr_t>(ei) + 0x38);

		uint16_t classID = ei->m_class_id - 1;

		auto m_pCreateEventFn = ei->m_client_class->m_pCreateEventFn;
		if (!m_pCreateEventFn)
			continue;

		IClientNetworkable* pCE = m_pCreateEventFn();
		if (!pCE)
			continue;

		if (classID == IdManager->ClassIdMap[FNV1A::HashConst("CTEFireBullets")])
		{
			// set fire_delay to zero to send out event so its not here later.
			ei->m_fire_delay = 0.f;
		}
		ei = next;
	} while (next != nullptr);

	return ret;
}

//#include "fake-case-opening/hooks_f.hpp"

void __fastcall hkPerformScreenOverlay(void* _this, void* edx, int x, int y, int w, int h);
decltype(&hkPerformScreenOverlay) ohkPerformScreenOverlay;
void __fastcall hkPerformScreenOverlay(void* _this, void* edx, int x, int y, int w, int h) {
	if (g_cfg.misc.block_adv)
		return;
	return ohkPerformScreenOverlay(_this, edx, x, y, w, h);
}
typedef bool(__thiscall* tDispatchUserMessage)(void*, int type, unsigned int a3, unsigned int length, const void* msg_data);
tDispatchUserMessage otDispatchUserMessage;
bool __fastcall hkDispatchUserMessage(void* ecx, void* edx, int type, unsigned int a3, unsigned int length, const void* msg_data)
{

	if (!m_gamerules()->m_bIsValveDS()) {
		if (g_cfg.misc.block_adv && (type == 7 || type == 8 || type == 5)) {
			return true;
		}
	}

	return otDispatchUserMessage(ecx, type, a3, length, msg_data);
}

using ProcessMovementFn = void(__thiscall*)(void* pGameMovement, player_t* basePlayer, CMoveData* moveData);
inline ProcessMovementFn oProcessMovement;
void __fastcall ProcessMovement(void* ecx, DWORD edx, player_t* basePlayer, CMoveData* moveData)
{
	//Fix prediction errors when jumping
	moveData->m_bGameCodeMovedPlayer = false;

	oProcessMovement(ecx, basePlayer, moveData);
}

__forceinline void setup_hooks()
{

	Interfaces interfaces{};

	// get interface pointers.
	fm_client = interfaces.get< CHLClient* >(HASH("VClient"));

	g_netvars.init();

	clientstateplus8 = new c_bphook(reinterpret_cast<void*>(*reinterpret_cast<uintptr_t**>(reinterpret_cast<uintptr_t>(m_clientstate()) + 0x8)));

	orig_temp_entities = clientstateplus8->apply<temp_entities_fn>(36, temp_entities);

	g_detours.init();

	g_csgo.mp_teammates_are_enemies = m_cvar()->FindVar(("mp_teammates_are_enemies"));

	static auto getforeignfallbackfontname = (DWORD)(util::FindSignature(crypt_str("vguimatsurface.dll"), g_ctx.signatures.at(9).c_str()));
	hooks::original_getforeignfallbackfontname = (DWORD)DetourFunction((PBYTE)getforeignfallbackfontname, (PBYTE)hooks::hooked_getforeignfallbackfontname); //-V206

	static auto setupbones = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(10).c_str()));
	hooks::original_setupbones = (DWORD)DetourFunction((PBYTE)setupbones, (PBYTE)hooks::hooked_setupbones); //-V206

	static auto doextrabonesprocessing = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(11).c_str()));
	hooks::original_doextrabonesprocessing = (DWORD)DetourFunction((PBYTE)doextrabonesprocessing, (PBYTE)hooks::hooked_doextrabonesprocessing); //-V206

	static auto standardblendingrules = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(12).c_str()));
	hooks::original_standardblendingrules = (DWORD)DetourFunction((PBYTE)standardblendingrules, (PBYTE)hooks::hooked_standardblendingrules); //-V206

	static auto updateclientsideanimation = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(13).c_str()));
	hooks::original_updateclientsideanimation = (DWORD)DetourFunction((PBYTE)updateclientsideanimation, (PBYTE)hooks::hooked_updateclientsideanimation); //-V206

	static DWORD* do_procedural_foot_plant = nullptr;
	if (!do_procedural_foot_plant) do_procedural_foot_plant = reinterpret_cast<DWORD*>(util::FindSignature("client.dll", "55 8B EC 83 E4 F0 83 EC 78 56 8B F1 57 8B 56 60"));
	if (do_procedural_foot_plant) do_procedural_foot_plant_original = reinterpret_cast<do_procedural_foot_plant_fn>(DetourFunction(reinterpret_cast<PBYTE>(do_procedural_foot_plant), reinterpret_cast<PBYTE>(hooked_do_procedural_foot_plant)));


	static auto physicssimulate = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(14).c_str()));
	hooks::original_physicssimulate = (DWORD)DetourFunction((PBYTE)physicssimulate, (PBYTE)hooks::hooked_physicssimulate);

	static auto modifyeyeposition = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(15).c_str()));
	hooks::original_modifyeyeposition = (DWORD)DetourFunction((PBYTE)modifyeyeposition, (PBYTE)hooks::hooked_modifyeyeposition);

	static auto calcviewmodelbob = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(16).c_str()));
	hooks::original_calcviewmodelbob = (DWORD)DetourFunction((PBYTE)calcviewmodelbob, (PBYTE)hooks::hooked_calcviewmodelbob);

	static auto shouldskipanimframe = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(17).c_str()));
	DetourFunction((PBYTE)shouldskipanimframe, (PBYTE)hooks::hooked_shouldskipanimframe);

	static auto checkfilecrcswithserver = (DWORD)(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(18).c_str()));
	DetourFunction((PBYTE)checkfilecrcswithserver, (PBYTE)hooks::hooked_checkfilecrcswithserver);

	static auto processinterpolatedlist = (DWORD)(util::FindSignature(crypt_str("client.dll"), g_ctx.signatures.at(19).c_str()));
	hooks::original_processinterpolatedlist = (DWORD)DetourFunction((byte*)processinterpolatedlist, (byte*)hooks::processinterpolatedlist); //-V206







	MH_Initialize();


	//for wmt
	hooks::client_hook = new vmthook(reinterpret_cast<DWORD**>(m_client()));
	hooks::clientstate_hook = new vmthook(reinterpret_cast<DWORD**>((CClientState*)(uint32_t(m_clientstate()) + 0x8)));
	hooks::panel_hook = new vmthook(reinterpret_cast<DWORD**>(m_panel()));
	hooks::clientmode_hook = new vmthook(reinterpret_cast<DWORD**>(m_clientmode()));
	hooks::inputinternal_hook = new vmthook(reinterpret_cast<DWORD**>(m_inputinternal()));
	hooks::engine_hook = new vmthook(reinterpret_cast<DWORD**>(m_engine()));
	hooks::renderview_hook = new vmthook(reinterpret_cast<DWORD**>(m_renderview()));
	hooks::materialsys_hook = new vmthook(reinterpret_cast<DWORD**>(m_materialsystem()));
	hooks::modelrender_hook = new vmthook(reinterpret_cast<DWORD**>(m_modelrender()));
	hooks::surface_hook = new vmthook(reinterpret_cast<DWORD**>(m_surface()));
	hooks::bspquery_hook = new vmthook(reinterpret_cast<DWORD**>(m_engine()->GetBSPTreeQuery()));
	hooks::prediction_hook = new vmthook(reinterpret_cast<DWORD**>(m_prediction()));
	hooks::trace_hook = new vmthook(reinterpret_cast<DWORD**>(m_trace()));
	hooks::filesystem_hook = new vmthook(reinterpret_cast<DWORD**>(util::FindSignature(crypt_str("engine.dll"), g_ctx.signatures.at(20).c_str()) + 0x2));
	hooks::directx_hook = new vmthook(reinterpret_cast<DWORD**>(m_device()));
	//for wmt

	//test

	//test

	hooks::inputinternal_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_setkeycodestate), 91); //-V107 //-V221
	hooks::inputinternal_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_setmousecodestate), 92); //-V107 //-V221

	hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_writeusercmddeltatobuffer), 24);

	hooks::clientstate_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_packetstart), 5);
	hooks::clientstate_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_packetend), 6);

	hooks::panel_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_painttraverse), 41);


	const auto create_move_target = reinterpret_cast<void*>(get_virtual(m_clientmode(), 24));
	const auto postscreeneffects_target = reinterpret_cast<void*>(get_virtual(m_clientmode(), 44));
	const auto overrideview_target = reinterpret_cast<void*>(get_virtual(m_clientmode(), 18));
	const auto drawfog_target = reinterpret_cast<void*>(get_virtual(m_clientmode(), 17));
	const auto fsn_target = reinterpret_cast<void*>(get_virtual(m_client(), 37));
	const auto sendmessage_target = reinterpret_cast<void*>(get_virtual(SteamGameCoordinator, 0));
	const auto retrievemessage_target = reinterpret_cast<void*>(get_virtual(SteamGameCoordinator, 2));
	const auto gameventmanag_target = reinterpret_cast<void*>(get_virtual(m_eventmanager(), 9));
	

	static auto CViewRender_PerformScreenOverlay = util::FindSignature("client.dll", "55 8b ec 51 a1 ? ? ? ? 53 56 8b d9");
	ohkPerformScreenOverlay = (decltype(&hkPerformScreenOverlay))DetourFunction(reinterpret_cast<byte*>(CViewRender_PerformScreenOverlay), reinterpret_cast<byte*>(hkPerformScreenOverlay));
	const auto dispatch_target = reinterpret_cast<void*>(get_virtual(m_client(), 38));
	const auto gamemovement_target = reinterpret_cast<void*>(get_virtual(m_gamemovement(), 1));

	//m_materialsystem()

	//if (!g_ctx.test_hooks) //chill bro look at BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) function where g_ctx.test_hooks = true
	//{
	//	hooks::client_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_fsn), 37);
	//	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_createmove), 24); 
	//	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_postscreeneffects), 44); 
	//	hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_overrideview), 18); 
		hooks::clientmode_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_drawfog), 17);
	//
	//}
	//else
	{
		MH_CreateHook(fsn_target, &hooks::hooked_fsn, reinterpret_cast<void**>(&hooks::oFrameStageNotify_t));
		MH_CreateHook(create_move_target, &hooks::hooked_createmove, reinterpret_cast<void**>(&hooks::oCreateMove_t));
		MH_CreateHook(postscreeneffects_target, &hooks::hooked_postscreeneffects, reinterpret_cast<void**>(&hooks::oDoPostScreenEffects_t));
		MH_CreateHook(overrideview_target, &hooks::hooked_overrideview, reinterpret_cast<void**>(&hooks::oOverrideView_t));
		//MH_CreateHook(drawfog_target, &hooks::hooked_drawfog, nullptr);
		MH_CreateHook(sendmessage_target, &shonax_hooks::GCSendMessageHook, reinterpret_cast<void**>(&shonax_hooks::oGCSendMessage));
		MH_CreateHook(retrievemessage_target, &shonax_hooks::GCRetrieveMessageHook, reinterpret_cast<void**>(&shonax_hooks::oGCRetrieveMessage));
		MH_CreateHook(gameventmanag_target, &shonax_hooks::FireEventClientSide, reinterpret_cast<void**>(&shonax_hooks::oFireEventClientSidefn));
		MH_CreateHook(dispatch_target, &hkDispatchUserMessage, reinterpret_cast<void**>(&otDispatchUserMessage));
		MH_CreateHook(gamemovement_target, &ProcessMovement, reinterpret_cast<void**>(&oProcessMovement));

	}


	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_isconnected), 27); //-V107 //-V221
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_getscreenaspectratio), 101); //-V107 //-V221
	hooks::engine_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_ishltv), 93); //-V107 //-V221


	hooks::renderview_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_sceneend), 9); //-V107 //-V221


	hooks::materialsys_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_beginframe), 42); //-V107 //-V221
	hooks::materialsys_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_getmaterial), 84); //-V107 //-V221



	hooks::modelrender_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_dme), 21); //-V107 //-V221


	hooks::surface_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_lockcursor), 67); //-V107 //-V221


	hooks::bspquery_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_listleavesinbox), 6); //-V107 //-V221


	hooks::prediction_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_runcommand), 19); //-V107 //-V221


	hooks::trace_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_clip_ray_collideable), 4); //-V107 //-V221
	hooks::trace_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_trace_ray), 5); //-V107 //-V221


	hooks::filesystem_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_loosefileallowed), 128); //-V107 //-V221
	hooks::filesystem_hook->hook_function(reinterpret_cast<uintptr_t>(hGetUnverifiedFileHashes), 101); //-V107 //-V221




	D3DDEVICE_CREATION_PARAMETERS creationParameters;
	while (FAILED(m_device()->GetCreationParameters(&creationParameters)))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	INIT::Window = creationParameters.hFocusWindow;



	//while (!(INIT::Window = FindWindowA(crypt_str("Valve001"), nullptr)))
	//	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	INIT::OldWindow = (WNDPROC)g_winapi.SetWindowLongA(INIT::Window, GWL_WNDPROC, (LONG_PTR)hooks::Hooked_WndProc);


	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::Hooked_EndScene_Reset), 16); //-V107 //-V221
	hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::hooked_present), 17); //-V107 //-V221
	//hooks::directx_hook->hook_function(reinterpret_cast<uintptr_t>(hooks::Hooked_EndScene), 42); //-V107 //-V221





	MH_EnableHook(MH_ALL_HOOKS);







	g_netvars.SetProxy(HASH("DT_BaseViewModel"), HASH("m_nSequence"), shonax_hooks::RecvProxy, shonax_hooks::sequence_hook);
	g_netvars.SetProxy(HASH("DT_BaseViewModel"), HASH("m_nModelIndex"), shonax_hooks::Viewmodel, shonax_hooks::viewmodel_hook);
	g_netvars.SetProxy(HASH("DT_CSRagdoll"), HASH("m_vecForce"), Force_proxy, m_Force_original);

	f_client.init(m_client());
	f_client.add(5, f_force_cast(&fooks::LevelInitPreEntity));
	f_client.add(6, f_force_cast(&fooks::LevelInitPostEntity));


	hooks::hooked_events.RegisterSelf();
}