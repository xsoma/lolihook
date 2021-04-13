/////////////////////////////////////////////////////
//                                                   
//    Generated with QuarcStudio. (ex. ImGui Editor) 
//    QuarcStudio is designed with love by pers0na2. 
//    All rights to QuarcStudio belong to pers0na2.  
//                                                   
/////////////////////////////////////////////////////


#include <string>

#include <d3d9.h>
#include <d3dx9.h>
#include "menu.h"
#include <ShlObj_core.h>
#include "../../inventory/inventorychanger.h"
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#include "..\cheats\misc\misc.h"
#include "../ImGui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#define IM_USE using namespace ImGui; 
#include "../ImGui/imgui_internal.h"
#include "../cheats/misc/walkbot.h"
#include "../../inventory/items.h"
#include "../../skinchanger/skins.h"

#include "../../protobuffs/Protobuffs.h"

#include <string>
#include <thread>
#include <mutex>
#include <algorithm>



#include "../nemesis/sffont.hpp"
#include "../nemesis/hashes.h"
#include "../nemesis/faprolight.hpp"
#include "nemesis.h"
#include "../nemesis/zcraw.h"
#include "../nemesis/Vector.hpp"
#include "../includes.hpp"
#include "../constchars.h"
#include <WinInet.h>
#include < urlmon.h >
#include "../cheats/misc/logs.h"

#include "../fake-case-opening/hooks_f.hpp"
#include "../fake-case-opening/vars_omg.h"
#include "../fake-case-opening/item_manager.hpp"
#include "../fake-case-opening/sdk.hpp"


#pragma comment(lib, "urlmon.lib")
#define ALPHA (ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar| ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)
#define NOALPHA (ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float)
ImVec2 p;
ImDrawList* draw;
IDirect3DTexture9* background = nullptr;
IDirect3DTexture9* all_skins_1[36];

IDirect3DTexture9** friends_images;
LPDIRECT3DTEXTURE9* custom_image;
LPDIRECT3DTEXTURE9 image;

std::filesystem::path* lul;
static float pfix = 226;

std::string zGetTimeString()
{
	static time_t current_time;
	struct tm* time_info;
	static char timeString[10];
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(timeString, sizeof(timeString), "%X", time_info);
	return timeString;
}

int zgetfps()
{
	return static_cast<int>(1.f / m_enginetool()->ClientFrameTime());
}
int zgettickrate()
{
	return static_cast<int>(1.f / m_globals()->m_intervalpertick);
}

std::once_flag init_lists;

struct menu_item_econ_item {
	econ_item_definition* item;
	std::string name;
};

float alphamodulate = 0.0f;
struct list_econ_item {
	std::vector<econ_item_definition*>* original_list;
	int current_item = 0;
	std::vector<char> filter_buffer;
	std::vector<menu_item_econ_item> items_filtered;

	void apply_filter_items() {
		static const auto V_UCS2ToUTF8 = reinterpret_cast<int(*)(const wchar_t* ucs2, char* utf8, int len)>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "V_UCS2ToUTF8")); /* nSkinz namazso*/

		items_filtered.clear();

		auto filter = std::string(filter_buffer.data());
		std::transform(filter.begin(), filter.end(), filter.begin(), ::tolower);

		std::vector<menu_item_econ_item> best_results, possible_results;

		for (auto& item : *original_list) {
			auto wide_name = item->get_weapon_localize_name();
			char name[256];
			V_UCS2ToUTF8(wide_name, name, sizeof(name));

			if (filter.empty())
				best_results.push_back({ item, name });
			else {
				auto str_name = std::string(name);
				std::transform(str_name.begin(), str_name.end(), str_name.begin(), ::tolower);
				auto pos = str_name.find(filter);
				if (pos == 0)
					best_results.push_back({ item, name });
				else if (pos != std::string::npos)
					possible_results.push_back({ item, name });
			}
		}

		items_filtered.reserve(best_results.size() + possible_results.size());
		items_filtered.insert(items_filtered.end(), best_results.begin(), best_results.end());
		items_filtered.insert(items_filtered.end(), possible_results.begin(), possible_results.end());

		if (current_item >= static_cast<int>(items_filtered.size()))
			current_item = 0;
	}


	void init(std::vector<econ_item_definition*>& list) {
		original_list = &list;
		filter_buffer.resize(256);
		apply_filter_items();
	}
};

static int selectedtab = 1;
int players_tab;
std::string preview = crypt_str("-");
auto selected_script_1 = 0;
auto loaded_editing_script_1 = false;
std::vector <std::string> files_1;
std::vector <std::string> scripts_1;
std::string editing_script_1;


std::string FindURl(std::string paintName)
{
	std::ifstream inFile;
	inFile.open(".\\csgo\\scripts\\items\\items_game_cdn.txt");
	std::string line;
	//int founds = 0;
	unsigned int curLine = 0;
	std::string search = paintName.append("=");
	//string weapon = "aug";
	while (getline(inFile, line)) {
		curLine++;
		if (line.find(search, 0) != std::string::npos)
		{
			return line.substr(line.find(search)).erase(0, search.length());
		}

	}
	return "";
}
//item-lists
list_econ_item crate_list;
list_econ_item key_list;

//crate-create-option
auto crate_as_unacknowledged = false;
auto add_key_to_crate = false;
auto crate_create_size = 1;
auto stage_tournament_crate = 0;
auto team0_tournament_crate = 0;
auto team1_tournament_crate = 0;
auto player_tournament_crate = 0;

//key-crate-option
auto key_as_unacknowledged = false;
auto key_create_size = 1;

//teams
std::vector<std::pair<int32_t, std::string>> csgo_teams_filtered;
std::vector<std::pair<int32_t, std::string>> csgo_players_filtered;

void update_players(int32_t tournament_id) {
	csgo_players_filtered.clear();
	csgo_players_filtered.push_back({ 0, "None" });

	if (tournament_id > 0) {
		auto t0 = csgo_teams_filtered[team0_tournament_crate].first;
		auto t1 = csgo_teams_filtered[team1_tournament_crate].first;
		std::vector<int32_t> players;
		for (auto& sticker : item_manager::stickers) {
			if (sticker->get_event_id() == tournament_id) {
				if (sticker->get_player_id() > 0 && (sticker->get_team_id() == t0 || sticker->get_team_id() == t1))
					if (std::find(players.begin(), players.end(), sticker->get_player_id()) == players.end())
						players.push_back(sticker->get_player_id());
			}
		}

		for (auto id : players)
			csgo_players_filtered.push_back({ id, item_manager::csgo_players[id] });
	}

	if (player_tournament_crate >= static_cast<int32_t>(csgo_players_filtered.size()))
		player_tournament_crate = 0;
}

void update_teams() {
	csgo_teams_filtered.clear();

	auto tournament_id_val = 0;

	if (crate_list.current_item < static_cast<int32_t>(crate_list.items_filtered.size())) {
		auto& crate_item = crate_list.items_filtered.at(crate_list.current_item);
		auto attributes_crate = crate_item.item->get_attributes();
		auto is_tournament_crate = true;
		auto supply_create_series = std::find(attributes_crate.begin(), attributes_crate.end(), 68);
		if (supply_create_series != attributes_crate.end()) {
			auto name = csgo_sdk::get_item_schema()->get_create_series_by_id(supply_create_series->value);
			if (name) {
				auto items = csgo_sdk::get_weapons_for_crate(name);
				if (items.front().sticker_id != 0)
					is_tournament_crate = false;
			}
		}

		if (is_tournament_crate) {
			auto tournament_id = std::find(attributes_crate.begin(), attributes_crate.end(), 137);
			if (tournament_id != attributes_crate.end()) {
				std::vector<int32_t> teams;
				for (auto& sticker : item_manager::stickers) {
					if (sticker->get_event_id() == tournament_id->value) {

						if (sticker->get_team_id() > 0)
							if (std::find(teams.begin(), teams.end(), sticker->get_team_id()) == teams.end())
								teams.push_back(sticker->get_team_id());
					}
				}

				for (auto id : teams)
					csgo_teams_filtered.push_back({ id, item_manager::csgo_teams[id - 1] });

				tournament_id_val = tournament_id->value;
			}
		}
	}

	if (team0_tournament_crate >= static_cast<int32_t>(csgo_teams_filtered.size()))
		team0_tournament_crate = 0;

	if (team1_tournament_crate >= static_cast<int32_t>(csgo_teams_filtered.size()))
		team1_tournament_crate = 0;

	update_players(tournament_id_val);
}

std::filesystem::path zGetWorkingPath()
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

void CopyConvert(const uint8_t* rgba, uint8_t* out, const size_t size)
{
	auto in = reinterpret_cast<const uint32_t*>(rgba);
	auto buf = reinterpret_cast<uint32_t*>(out);
	for (auto i = 0u; i < (size / 4); ++i)
	{
		const auto pixel = *in++;
		*buf++ = (pixel & 0xFF00FF00) | ((pixel & 0xFF0000) >> 16) | ((pixel & 0xFF) << 16);
	}
}
LPDIRECT3DTEXTURE9 steam_image()
{
	int iImage = SteamFriends->GetLargeFriendAvatar(SteamUser->GetSteamID());
	if (iImage == -1)
		return nullptr;
	uint32 uAvatarWidth, uAvatarHeight;
	if (!SteamUtils->GetImageSize(iImage, &uAvatarWidth, &uAvatarHeight))
		return nullptr;
	const int uImageSizeInBytes = uAvatarWidth * uAvatarHeight * 4;
	uint8* pAvatarRGBA = new uint8[uImageSizeInBytes];
	if (!SteamUtils->GetImageRGBA(iImage, pAvatarRGBA, uImageSizeInBytes))
	{
		delete[] pAvatarRGBA;
		return nullptr;
	}
	auto res = c_menu::get().device->CreateTexture(uAvatarWidth,
		uAvatarHeight,
		1,
		D3DUSAGE_DYNAMIC,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&image,
		nullptr);
	std::vector<uint8_t> texData;
	texData.resize(uAvatarWidth * uAvatarHeight * 4u);
	CopyConvert(pAvatarRGBA,
		texData.data(),
		uAvatarWidth * uAvatarHeight * 4u);
	D3DLOCKED_RECT rect;
	res = image->LockRect(0, &rect, nullptr, D3DLOCK_DISCARD);
	auto src = texData.data();
	auto dst = reinterpret_cast<uint8_t*>(rect.pBits);
	for (auto y = 0u; y < uAvatarHeight; ++y)
	{
		std::copy(src, src + (uAvatarWidth * 4), dst);

		src += uAvatarWidth * 4;
		dst += rect.Pitch;
	}
	res = image->UnlockRect(0);
	delete[] pAvatarRGBA;
	return image;


}

LPDIRECT3DTEXTURE9 steam_image_friends(CSteamID SteamId)
{
	LPDIRECT3DTEXTURE9 asdgsdgadsg;



	int iImage = SteamFriends->GetLargeFriendAvatar(SteamId);
	if (iImage == -1)
		return nullptr;
	uint32 uAvatarWidth, uAvatarHeight;
	if (!SteamUtils->GetImageSize(iImage, &uAvatarWidth, &uAvatarHeight))
		return nullptr;
	const int uImageSizeInBytes = uAvatarWidth * uAvatarHeight * 4;
	uint8* pAvatarRGBA = new uint8[uImageSizeInBytes];
	if (!SteamUtils->GetImageRGBA(iImage, pAvatarRGBA, uImageSizeInBytes))
	{
		delete[] pAvatarRGBA;
		return nullptr;
	}
	auto res = c_menu::get().device->CreateTexture(uAvatarWidth,
		uAvatarHeight,
		1,
		D3DUSAGE_DYNAMIC,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&asdgsdgadsg,
		nullptr);
	std::vector<uint8_t> texData;
	texData.resize(uAvatarWidth * uAvatarHeight * 4u);
	CopyConvert(pAvatarRGBA,
		texData.data(),
		uAvatarWidth * uAvatarHeight * 4u);
	D3DLOCKED_RECT rect;
	res = asdgsdgadsg->LockRect(0, &rect, nullptr, D3DLOCK_DISCARD);
	auto src = texData.data();
	auto dst = reinterpret_cast<uint8_t*>(rect.pBits);
	for (auto y = 0u; y < uAvatarHeight; ++y)
	{
		std::copy(src, src + (uAvatarWidth * 4), dst);

		src += uAvatarWidth * 4;
		dst += rect.Pitch;
	}
	res = asdgsdgadsg->UnlockRect(0);
	delete[] pAvatarRGBA;
	return asdgsdgadsg;
}


LPDIRECT3DTEXTURE9* load_custom_image()
{

	int generate_number = 0;
	for (auto& p : std::filesystem::directory_iterator(zGetWorkingPath() / ("custom image\\")))
	{
		generate_number++;
	}

	if (custom_image)
		delete[] custom_image;
	if (lul)
		delete[] lul;


	custom_image = new LPDIRECT3DTEXTURE9[generate_number];
	lul = new  std::filesystem::path[generate_number];
	for (auto j = 0; j < 1; j++)
	{
		int y = 0;
		for (auto& p : std::filesystem::directory_iterator(zGetWorkingPath() / ("custom image\\")))
		{
			lul[y] = p.path();
			y++;

		}
	}


	for (auto i = 0; i < generate_number; D3DXCreateTextureFromFileA(c_menu::get().device, lul[i].u8string().c_str(), &custom_image[i]), i++);



	g_ctx.custom_reload = false;
	g_ctx.custom_reload_picture = true;
	return custom_image;
}

static const char* zweaponnames(const short id)
{
	switch (id)
	{
	case WEAPON_DEAGLE:
		return "deagle";
	case WEAPON_ELITE:
		return "elite";
	case WEAPON_FIVESEVEN:
		return "fiveseven";
	case WEAPON_GLOCK:
		return "glock";
	case WEAPON_AK47:
		return "ak47";
	case WEAPON_AUG:
		return "aug";
	case WEAPON_AWP:
		return "awp";
	case WEAPON_FAMAS:
		return "famas";
	case WEAPON_G3SG1:
		return "g3sg1";
	case WEAPON_GALILAR:
		return "galilar";
	case WEAPON_M249:
		return "m249";
	case WEAPON_M4A1_SILENCER:
		return "m4a1_silencer";
	case WEAPON_M4A1:
		return "m4a1";
	case WEAPON_MAC10:
		return "mac10";
	case WEAPON_P90:
		return "p90";
	case WEAPON_UMP45:
		return "ump45";
	case WEAPON_XM1014:
		return "xm1014";
	case WEAPON_BIZON:
		return "bizon";
	case WEAPON_MAG7:
		return "mag7";
	case WEAPON_NEGEV:
		return "negev";
	case WEAPON_SAWEDOFF:
		return "sawedoff";
	case WEAPON_TEC9:
		return "tec9";
	case WEAPON_HKP2000:
		return "hkp2000";
	case WEAPON_MP5SD:
		return "mp5sd";
	case WEAPON_MP7:
		return "mp7";
	case WEAPON_MP9:
		return "mp9";
	case WEAPON_NOVA:
		return "nova";
	case WEAPON_P250:
		return "p250";
	case WEAPON_SCAR20:
		return "scar20";
	case WEAPON_SG553:
		return "sg556";
	case WEAPON_SSG08:
		return "ssg08";
	case WEAPON_USP_SILENCER:
		return "usp_silencer";
	case WEAPON_CZ75A:
		return "cz75a";
	case WEAPON_REVOLVER:
		return "revolver";
	case WEAPON_KNIFE:
		return "knife";
	case WEAPON_KNIFE_T:
		return "knife_t";
	case WEAPON_KNIFE_M9_BAYONET:
		return "knife_m9_bayonet";
	case WEAPON_KNIFE_BAYONET:
		return "bayonet";
	case WEAPON_KNIFE_FLIP:
		return "knife_flip";
	case WEAPON_KNIFE_GUT:
		return "knife_gut";
	case WEAPON_KNIFE_KARAMBIT:
		return "knife_karambit";
	case WEAPON_KNIFE_TACTICAL:
		return "knife_tactical";
	case WEAPON_KNIFE_FALCHION:
		return "knife_falchion";
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
		return "knife_survival_bowie";
	case WEAPON_KNIFE_BUTTERFLY:
		return "knife_butterfly";
	case WEAPON_KNIFE_PUSH:
		return "knife_push";
	case WEAPON_KNIFE_URSUS:
		return "knife_ursus";
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
		return "knife_gypsy_jackknife";
	case WEAPON_KNIFE_STILETTO:
		return "knife_stiletto";
	case WEAPON_KNIFE_WIDOWMAKER:
		return "knife_widowmaker";
	case WEAPON_KNIFE_SKELETON:
		return "knife_skeleton";
	case WEAPON_KNIFE_OUTDOOR:
		return "knife_outdoor";
	case WEAPON_KNIFE_CANIS:
		return "knife_canis";
	case WEAPON_KNIFE_CORD:
		return "knife_cord";
	case WEAPON_KNIFE_CSS:
		return "knife_css";
	case GLOVE_STUDDED_BLOODHOUND:
		return "studded_bloodhound_gloves";
	case GLOVE_T_SIDE:
		return "t_gloves";
	case GLOVE_CT_SIDE:
		return "ct_gloves";
	case GLOVE_SPORTY:
		return "sporty_gloves";
	case GLOVE_SLICK:
		return "slick_gloves";
	case GLOVE_LEATHER_WRAP:
		return "leather_handwraps";
	case GLOVE_MOTORCYCLE:
		return "motorcycle_gloves";
	case GLOVE_SPECIALIST:
		return "specialist_gloves";
	case GLOVE_HYDRA:
		return "studded_hydra_gloves";
	default:
		return "";
	}
}


std::string fDownloadBytes(const char* const szUrl)
{
	HINTERNET hOpen = NULL;
	HINTERNET hFile = NULL;
	char* lpBuffer = NULL;
	DWORD dwBytesRead = 0;
	//Pointer to dynamic buffer.
	char* data = 0;
	//Dynamic data size.
	DWORD dataSize = 0;

	hOpen = InternetOpenA("smef.pw INDIGO/4.3 paste", NULL, NULL, NULL, NULL);
	if (!hOpen) return (char*)"";

	hFile = InternetOpenUrlA(hOpen, szUrl, NULL, NULL, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, NULL);

	if (!hFile) {
		InternetCloseHandle(hOpen);
		return (char*)"";
	}

	std::string output;
	do {
		char buffer[2000];
		InternetReadFile(hFile, (LPVOID)buffer, _countof(buffer), &dwBytesRead);
		output.append(buffer, dwBytesRead);
	} while (dwBytesRead);

	InternetCloseHandle(hFile);
	InternetCloseHandle(hOpen);

	return output;
}
static std::string old_link_1;
static IDirect3DTexture9* some_texture_test_1;

void UpdatePic(IDirect3DDevice9* thisptr, std::string link)
{
	if (old_link_1 == link)
		return;
	std::string imData = fDownloadBytes(link.c_str());
	D3DXCreateTextureFromFileInMemoryEx(thisptr
		, imData.data(), imData.length(),
		300, 300, D3DUSAGE_DYNAMIC, 0, D3DFMT_A8B8G8R8, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &some_texture_test_1);

	old_link_1 = link;
}

std::string get_wep(int id, int custom_index = -1, bool knife = true)
{
	if (custom_index > -1)
	{
		if (knife)
		{
			switch (custom_index)
			{
			case 0: return crypt_str("weapon_knife");
			case 1: return crypt_str("weapon_bayonet");
			case 2: return crypt_str("weapon_knife_css");
			case 3: return crypt_str("weapon_knife_skeleton");
			case 4: return crypt_str("weapon_knife_outdoor");
			case 5: return crypt_str("weapon_knife_cord");
			case 6: return crypt_str("weapon_knife_canis");
			case 7: return crypt_str("weapon_knife_flip");
			case 8: return crypt_str("weapon_knife_gut");
			case 9: return crypt_str("weapon_knife_karambit");
			case 10: return crypt_str("weapon_knife_m9_bayonet");
			case 11: return crypt_str("weapon_knife_tactical");
			case 12: return crypt_str("weapon_knife_falchion");
			case 13: return crypt_str("weapon_knife_survival_bowie");
			case 14: return crypt_str("weapon_knife_butterfly");
			case 15: return crypt_str("weapon_knife_push");
			case 16: return crypt_str("weapon_knife_ursus");
			case 17: return crypt_str("weapon_knife_gypsy_jackknife");
			case 18: return crypt_str("weapon_knife_stiletto");
			case 19: return crypt_str("weapon_knife_widowmaker");
			}
		}
		else
		{
			switch (custom_index)
			{
			case 0: return crypt_str("ct_gloves"); //-V1037
			case 1: return crypt_str("studded_bloodhound_gloves");
			case 2: return crypt_str("t_gloves");
			case 3: return crypt_str("ct_gloves");
			case 4: return crypt_str("sporty_gloves");
			case 5: return crypt_str("slick_gloves");
			case 6: return crypt_str("leather_handwraps");
			case 7: return crypt_str("motorcycle_gloves");
			case 8: return crypt_str("specialist_gloves");
			case 9: return crypt_str("studded_hydra_gloves");
			}
		}
	}
	else
	{
		switch (id)
		{
		case 0: return crypt_str("knife");
		case 1: return crypt_str("gloves");
		case 2: return crypt_str("weapon_ak47");
		case 3: return crypt_str("weapon_aug");
		case 4: return crypt_str("weapon_awp");
		case 5: return crypt_str("weapon_cz75a");
		case 6: return crypt_str("weapon_deagle");
		case 7: return crypt_str("weapon_elite");
		case 8: return crypt_str("weapon_famas");
		case 9: return crypt_str("weapon_fiveseven");
		case 10: return crypt_str("weapon_g3sg1");
		case 11: return crypt_str("weapon_galilar");
		case 12: return crypt_str("weapon_glock");
		case 13: return crypt_str("weapon_m249");
		case 14: return crypt_str("weapon_m4a1_silencer");
		case 15: return crypt_str("weapon_m4a1");
		case 16: return crypt_str("weapon_mac10");
		case 17: return crypt_str("weapon_mag7");
		case 18: return crypt_str("weapon_mp5sd");
		case 19: return crypt_str("weapon_mp7");
		case 20: return crypt_str("weapon_mp9");
		case 21: return crypt_str("weapon_negev");
		case 22: return crypt_str("weapon_nova");
		case 23: return crypt_str("weapon_hkp2000");
		case 24: return crypt_str("weapon_p250");
		case 25: return crypt_str("weapon_p90");
		case 26: return crypt_str("weapon_bizon");
		case 27: return crypt_str("weapon_revolver");
		case 28: return crypt_str("weapon_sawedoff");
		case 29: return crypt_str("weapon_scar20");
		case 30: return crypt_str("weapon_ssg08");
		case 31: return crypt_str("weapon_sg556");
		case 32: return crypt_str("weapon_tec9");
		case 33: return crypt_str("weapon_ump45");
		case 34: return crypt_str("weapon_usp_silencer");
		case 35: return crypt_str("weapon_xm1014");
		default: return crypt_str("unknown");
		}
	}
}

IDirect3DTexture9* get_skin_preview_1(const char* weapon_name, const std::string& skin_name, IDirect3DDevice9* device)
{
	IDirect3DTexture9* skin_image = nullptr;
	std::string vpk_path;

	if (strcmp(weapon_name, crypt_str("unknown")) && strcmp(weapon_name, crypt_str("knife")) && strcmp(weapon_name, crypt_str("gloves"))) //-V526
	{
		if (skin_name.empty() || skin_name == crypt_str("default"))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/") + std::string(weapon_name) + crypt_str(".png");
		else
			vpk_path = crypt_str("resource/flash/econ/default_generated/") + std::string(weapon_name) + crypt_str("_") + std::string(skin_name) + crypt_str("_light_large.png");
	}
	else
	{
		if (!strcmp(weapon_name, crypt_str("knife")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_knife.png");
		else if (!strcmp(weapon_name, crypt_str("gloves")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else if (!strcmp(weapon_name, crypt_str("unknown")))
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/weapon_snowball.png");

	}
	const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));
	if (handle)
	{
		int file_len = m_basefilesys()->Size(handle);
		char* image = new char[file_len]; //-V121

		m_basefilesys()->Read(image, file_len, handle);
		m_basefilesys()->Close(handle);

		D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
		delete[] image;
	}

	if (!skin_image)
	{
		std::string vpk_path;

		if (strstr(weapon_name, crypt_str("bloodhound")) != NULL || strstr(weapon_name, crypt_str("hydra")) != NULL)
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
		else
			vpk_path = crypt_str("resource/flash/econ/weapons/base_weapons/") + std::string(weapon_name) + crypt_str(".png");

		const auto handle = m_basefilesys()->Open(vpk_path.c_str(), crypt_str("r"), crypt_str("GAME"));

		if (handle)
		{
			int file_len = m_basefilesys()->Size(handle);
			char* image = new char[file_len]; //-V121

			m_basefilesys()->Read(image, file_len, handle);
			m_basefilesys()->Close(handle);

			D3DXCreateTextureFromFileInMemory(device, image, file_len, &skin_image);
			delete[] image;
		}
	}

	return skin_image;
}

void c_menu2::init_styles(ImGuiStyle& style, LPDIRECT3DDEVICE9 device)
{
	style.WindowPadding = ImVec2(0.000000f, 0.000000f);
	style.FramePadding = ImVec2(4.000000f, 3.000000f);
	style.ItemSpacing = ImVec2(8.000000f, 4.000000f);
	style.ItemInnerSpacing = ImVec2(4.000000f, 4.000000f);
	style.IndentSpacing = 21.000000f;
	style.ScrollbarSize = 15.000000f;
	style.GrabMinSize = 1.000000f;
	style.WindowBorderSize = 0.000000f;
	style.ChildBorderSize = 1.000000f;
	style.PopupBorderSize = 0.000000f;
	style.FrameBorderSize = 1.000000f;
	style.WindowRounding = 0.000000f;
	style.ChildRounding = 0.000000f;
	style.FrameRounding = 0.000000f;
	style.PopupRounding = 0.000000f;
	style.ScrollbarRounding = 4.000000f;
	style.GrabRounding = 0.000000f;
	style.WindowTitleAlign = ImVec2(0.000000f, 0.500000f);
	style.ButtonTextAlign = ImVec2(0.500000f, 0.500000f);
	style.SelectableTextAlign = ImVec2(0.500000f, 0.500000f);

	style.Colors[ImGuiCol_Text] = ImVec4(0.900000f, 0.900000f, 0.900000f, 1.000000f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.600000f, 0.600000f, 0.600000f, 1.000000f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.058824f, 0.058824f, 0.058824f, 1.000000f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(57 / 255.f, 57 / 255.f, 57 / 255.f, 1);
	style.Colors[ImGuiCol_Border] = ImVec4(0.500000f, 0.500000f, 0.500000f, 0.500000f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.430000f, 0.430000f, 0.430000f, 0.390000f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.470000f, 0.470000f, 0.690000f, 0.400000f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.420000f, 0.410000f, 0.640000f, 0.690000f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.270000f, 0.270000f, 0.540000f, 0.830000f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.320000f, 0.320000f, 0.630000f, 0.870000f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.400000f, 0.400000f, 0.800000f, 0.200000f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.400000f, 0.400000f, 0.550000f, 0.800000f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.200000f, 0.250000f, 0.300000f, 0.000000f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.400000f, 0.400000f, 0.400000f, 0.300000f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.400000f, 0.400000f, 0.400000f, 0.400000f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.400000f, 0.400000f, 0.400000f, 0.600000f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.900000f, 0.900000f, 0.900000f, 0.500000f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.000000f, 1.000000f, 1.000000f, 0.300000f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.410000f, 0.390000f, 0.800000f, 0.600000f);
	style.Colors[ImGuiCol_Button] = ImVec4(45 / 255.f, 45 / 255.f, 45 / 255.f, 1);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(50 / 255.f, 50 / 255.f, 50 / 255.f, 1);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(45 / 255.f, 45 / 255.f, 45 / 255.f, 1);
	style.Colors[ImGuiCol_Header] = ImVec4(45 / 255.f, 45 / 255.f, 45 / 255.f, 1);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(50 / 255.f, 50 / 255.f, 50 / 255.f, 1);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(45 / 255.f, 45 / 255.f, 45 / 255.f, 1);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.500000f, 0.500000f, 0.500000f, 0.600000f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.600000f, 0.600000f, 0.700000f, 1.000000f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.700000f, 0.700000f, 0.900000f, 1.000000f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.000000f, 1.000000f, 1.000000f, 0.160000f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.780000f, 0.820000f, 1.000000f, 0.600000f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.780000f, 0.820000f, 1.000000f, 0.900000f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.000000f, 0.000000f, 1.000000f, 0.350000f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.000000f, 1.000000f, 0.000000f, 0.900000f);

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x2000, 0x206F, // General Punctuation
		0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
		0x31F0, 0x31FF, // Katakana Phonetic Extensions
		0xFF00, 0xFFEF, // Half-width characters
		0x4e00, 0x9FAF, // CJK Ideograms
		0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
		0x2DE0, 0x2DFF, // Cyrillic Extended-A
		0xA640, 0xA69F, // Cyrillic Extended-B
		0,
	};

	ImFontConfig font_cfg{};
	font_cfg.FontDataOwnedByAtlas = false;
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(sfpro_compressed_data, sfpro_compressed_size, 12, &font_cfg, ranges);

	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&faprolight, sizeof faprolight, 16, &icons_config, icon_ranges);

	if (background == nullptr)
		D3DXCreateTextureFromFileInMemoryEx(device, &backgroundraw, sizeof(backgroundraw), 845, 360, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &background);
}
__forceinline void pad(float x, float y)
{
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + x);
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + y);
}


std::string get_config_dir_1()
{
	std::string folder;
	static TCHAR path[MAX_PATH];

	if (SUCCEEDED(SHGetFolderPath(NULL, 0x001a, NULL, NULL, path)))
		folder = std::string(path) + crypt_str("\\Lolihook\\Configs\\");

	CreateDirectory(folder.c_str(), NULL);
	return folder;
}

void load_config_1()
{
	if (cfg_manager->files.empty())
		return;

	cfg_manager->load(cfg_manager->files.at(g_cfg.selected_config), false);
	c_lua::get().unload_all_scripts();

	for (auto& script : g_cfg.scripts.scripts)
		c_lua::get().load_script(c_lua::get().get_script_id(script));

	scripts_1 = c_lua::get().scripts;

	if (selected_script_1 >= scripts_1.size())
		selected_script_1 = scripts_1.size() - 1; //-V103

	for (auto& current : scripts_1)
	{
		if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
			current.erase(current.size() - 5, 5);
		else if (current.size() >= 4)
			current.erase(current.size() - 4, 4);
	}

	for (auto i = 0; i < g_cfg.skins.skinChanger.size(); ++i)
		all_skins_1[i] = nullptr;

	g_cfg.scripts.scripts.clear();

	cfg_manager->load(cfg_manager->files.at(g_cfg.selected_config), true);
	cfg_manager->config_files();

	eventlogs::get().add(crypt_str("Loaded ") + files_1.at(g_cfg.selected_config) + crypt_str(" config"), false);
}

void save_config_1()
{
	if (cfg_manager->files.empty())
		return;

	g_cfg.scripts.scripts.clear();

	for (auto i = 0; i < c_lua::get().scripts.size(); ++i)
	{
		auto script = c_lua::get().scripts.at(i);

		if (c_lua::get().loaded.at(i))
			g_cfg.scripts.scripts.emplace_back(script);
	}

	cfg_manager->save(cfg_manager->files.at(g_cfg.selected_config));
	cfg_manager->config_files();

	eventlogs::get().add(crypt_str("Saved ") + files_1.at(g_cfg.selected_config) + crypt_str(" config"), false);
}

void remove_config_1()
{
	if (cfg_manager->files.empty())
		return;

	eventlogs::get().add(crypt_str("Removed ") + files_1.at(g_cfg.selected_config) + crypt_str(" config"), false);

	cfg_manager->remove(cfg_manager->files.at(g_cfg.selected_config));
	cfg_manager->config_files();

	files_1 = cfg_manager->files;

	if (g_cfg.selected_config >= files_1.size())
		g_cfg.selected_config = files_1.size() - 1; //-V103

	for (auto& current : files_1)
		if (current.size() > 2)
			current.erase(current.size() - 3, 3);
}

void add_config_1()
{
	auto empty = true;

	for (auto current : g_cfg.new_config_name)
	{
		if (current != ' ')
		{
			empty = false;
			break;
		}
	}

	if (empty)
		g_cfg.new_config_name = crypt_str("config");

	eventlogs::get().add(crypt_str("Added ") + g_cfg.new_config_name + crypt_str(" config"), false);

	if (g_cfg.new_config_name.find(crypt_str(".cfg")) == std::string::npos)
		g_cfg.new_config_name += crypt_str(".cfg");

	cfg_manager->save(g_cfg.new_config_name);
	cfg_manager->config_files();

	g_cfg.selected_config = cfg_manager->files.size() - 1; //-V103
	files_1 = cfg_manager->files;

	for (auto& current : files_1)
		if (current.size() > 2)
			current.erase(current.size() - 3, 3);
}
bool checkbox(const char* label, bool* v)
{
	IM_USE;
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	const float square_sz = ImGui::GetFrameHeight();
	const ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos + ImVec2(10, 5), pos + ImVec2(square_sz + (style.ItemInnerSpacing.x + label_size.x + 0), 0 + label_size.y + style.FramePadding.y * 2) + ImVec2(10, 5));
	ItemSize(total_bb, style.FramePadding.y);
	ItemAdd(total_bb, id);

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);

	if (hovered || held)
		ImGui::SetMouseCursor(7);

	if (pressed)
		*v = !(*v);

	window->DrawList->AddRectFilled(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 0), ImVec2(total_bb.Min.x + 10, total_bb.Min.y + 10), ImColor(37, 37, 37, int(alphamodulate * 253)), 0, 15);
	window->DrawList->AddRect(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 0), ImVec2(total_bb.Min.x + 10, total_bb.Min.y + 10), ImColor(27, 27, 27, int(alphamodulate * 253)), 0, 15);

	if (*v)
	{
		window->DrawList->AddRectFilledMultiColor(ImVec2(total_bb.Min.x + 1, total_bb.Min.y + 1), ImVec2(total_bb.Min.x + 9, total_bb.Min.y + 9), ImColor(255 / 255.f, 0 / 255.f, 255 / 255.f, alphamodulate), ImColor(255 / 255.f, 0 / 255.f, 255 / 255.f, alphamodulate), ImColor(121, 6, 121, int(alphamodulate * 253)), ImColor(121, 6, 121, int(alphamodulate * 253)));
	}

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(214 / 255.f, 214 / 255.f, 214 / 255.f, alphamodulate));
	ImGui::RenderText(ImVec2(total_bb.Min.x + style.ItemInnerSpacing.x + 10, total_bb.Min.y + style.FramePadding.y + -4), label);
	ImGui::PopStyleColor();
	return pressed;
}

bool button(const char* label, const ImVec2& size_arg)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos + ImVec2(11, 3);
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

	if (hovered || held)
		ImGui::SetMouseCursor(7);

	if (!hovered)
		window->DrawList->AddRectFilledMultiColor(ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Max.x, bb.Max.y), ImColor(51, 51, 51, int(alphamodulate * 253)), ImColor(51, 51, 51, int(alphamodulate * 253)), ImColor(39, 39, 40, int(alphamodulate * 253)), ImColor(39, 39, 40, int(alphamodulate * 253)));
	else
		window->DrawList->AddRectFilledMultiColor(ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Max.x, bb.Max.y), ImColor(39, 39, 40, int(alphamodulate * 253)), ImColor(39, 39, 40, int(alphamodulate * 253)), ImColor(51, 51, 51, int(alphamodulate * 253)), ImColor(51, 51, 51, int(alphamodulate * 253)));

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, alphamodulate));
	ImGui::RenderText(ImVec2(bb.Min.x + style.ItemInnerSpacing.x + 0, bb.Min.y + style.FramePadding.y + 0), label);
	ImGui::PopStyleColor();

	return pressed;
}

bool beginchildex(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags)
{
	IM_USE;

	ImGuiContext& g = *GImGui;
	ImGuiWindow* parent_window = g.CurrentWindow;

	flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
	flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);  // Inherit the NoMove flag

	// Size
	const ImVec2 content_avail = GetContentRegionAvail();
	ImVec2 size = ImFloor(size_arg);
	const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
	if (size.x <= 0.0f)
		size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
	if (size.y <= 0.0f)
		size.y = ImMax(content_avail.y + size.y, 4.0f);
	SetNextWindowSize(size);

	// Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
	char title[256];
	if (name)
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%s_%08X", parent_window->Name, name, id);
	else
		ImFormatString(title, IM_ARRAYSIZE(title), "%s/%08X", parent_window->Name, id);

	const float backup_border_size = g.Style.ChildBorderSize;
	if (!border)
		g.Style.ChildBorderSize = 0.0f;
	bool ret = Begin(title, NULL, flags);
	g.Style.ChildBorderSize = backup_border_size;

	ImGuiWindow* child_window = g.CurrentWindow;
	child_window->ChildId = id;
	child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;

	// Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
	// While this is not really documented/defined, it seems that the expected thing to do.
	if (child_window->BeginCount == 1)
		parent_window->DC.CursorPos = child_window->Pos;

	// Process navigation-in immediately so NavInit can run on first frame
	if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayerActiveMask != 0 || child_window->DC.NavHasScroll))
	{
		FocusWindow(child_window);
		NavInitWindow(child_window, false);
		SetActiveID(id + 1, child_window); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
		g.ActiveIdSource = ImGuiInputSource_Nav;
	}
	parent_window->DrawList->AddRectFilled(ImGui::GetWindowPos(), ImGui::GetWindowPos() + size_arg, ImColor(12, 12, 12, int(alphamodulate * 170)), 0);
	parent_window->DrawList->AddRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + size_arg, ImColor(59, 59, 59, int(alphamodulate * 140)), 0);
	parent_window->DrawList->AddRect(ImGui::GetWindowPos() + ImVec2(1, 1), ImGui::GetWindowPos() + size_arg - ImVec2(1, 1), ImColor(2, 2, 2, int(alphamodulate * 170)), 0);
	return ret;
}

bool beginchild(const char* str_id, const ImVec2& size_arg, bool border = false, ImGuiWindowFlags extra_flags = NULL)
{
	IM_USE;

	ImGuiWindow* window = GetCurrentWindow();
	return beginchildex(str_id, window->GetID(str_id), size_arg, border, extra_flags);
}


bool          combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
bool          combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);      // Separate items with \0 within a string, end item-list with \0\0. e.g. "One\0Two\0Three\0"
bool          combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

static float CalcMaxPopupHeightFromItemCount(int items_count)
{
	ImGuiContext& g = *GImGui;
	if (items_count <= 0)
		return FLT_MAX;
	return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

static float CalcMaxPopupHeightFromItemCounttt(int items_count)
{
	ImGuiContext& g = *GImGui;
	if (items_count <= 0)
		return FLT_MAX;
	return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool begincombo2(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
	IM_USE;
	ImGuiContext& g = *GImGui;
	bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
	g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : ImGui::GetFrameHeight();
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	const float expected_w = 240;
	const float w = (flags & ImGuiComboFlags_NoPreview) ? 240 : 240;

	const ImRect frame_bb(window->DC.CursorPos + ImVec2(11, 0), window->DC.CursorPos + ImVec2(w + 0, label_size.y + style.FramePadding.y * 2.0f + 0) + ImVec2(15, 18));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

	ImGui::ItemSize(total_bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);
	bool popup_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

	if (hovered || held)
		ImGui::SetMouseCursor(7);

	const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);

	if (!hovered)
		window->DrawList->AddRectFilledMultiColor(ImVec2(frame_bb.Min.x + 0, frame_bb.Min.y + 18), ImVec2(frame_bb.Max.x - 15, frame_bb.Max.y + 0), ImColor(51, 51, 51, int(alphamodulate * 253)), ImColor(51, 51, 51, int(alphamodulate * 253)), ImColor(39, 39, 40, int(alphamodulate * 253)), ImColor(39, 39, 40, int(alphamodulate * 253)));
	else
		window->DrawList->AddRectFilledMultiColor(ImVec2(frame_bb.Min.x + 0, frame_bb.Min.y + 18), ImVec2(frame_bb.Max.x - 15, frame_bb.Max.y + 0), ImColor(39, 39, 40, int(alphamodulate * 253)), ImColor(39, 39, 40, int(alphamodulate * 253)), ImColor(51, 51, 51, int(alphamodulate * 253)), ImColor(51, 51, 51, int(alphamodulate * 253)));

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, alphamodulate));
	if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
		ImGui::RenderText(frame_bb.Min + style.FramePadding + ImVec2(0, 18), preview_value);
	ImGui::PopStyleColor();

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, alphamodulate));
	if (label_size.x > 0)
		ImGui::RenderText(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y + 0), label);
	ImGui::PopStyleColor();

	if ((pressed || g.NavActivateId == id) && !popup_open)
	{
		if (window->DC.NavLayerCurrent == 0)
			window->NavLastIds[0] = id;
		ImGui::OpenPopupEx(id, ImGuiPopupFlags_None);
		popup_open = true;
	}

	if (!popup_open)
		return false;

	if ((flags & ImGuiComboFlags_HeightMask_) == 0)
		flags |= ImGuiComboFlags_HeightRegular;
	IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));
	int popup_max_height_in_items = -1;
	if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
	else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
	else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
	ImGui::SetNextWindowSizeConstraints(ImVec2(w - 11, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

	char name[16];
	ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size);

	if (ImGuiWindow* popup_window = ImGui::FindWindowByName(name))
		if (popup_window->WasActive)
		{
			ImVec2 size_expected = ImGui::CalcWindowExpectedSize(popup_window);
			if (flags & ImGuiComboFlags_PopupAlignLeft)
				popup_window->AutoPosLastDirection = ImGuiDir_Left;
			ImRect r_outer = ImGui::GetWindowAllowedExtentRect(popup_window);
			ImVec2 pos = ImGui::FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
			ImGui::SetNextWindowPos(pos);
		}

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
	bool ret = ImGui::Begin(name, NULL, window_flags);
	ImGui::PopStyleVar();

	if (!ret)
	{
		ImGui::EndPopup();
		return false;
	}
	return true;
}

bool begincombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
	IM_USE;
	ImGuiContext& g = *GImGui;
	ImGuiCond backup_next_window_size_constraint = g.NextWindowData.SizeCond;
	g.NextWindowData.SizeCond = 0;
	//bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
	//g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const float expected_w = 240;
	const float w = (flags & ImGuiComboFlags_NoPreview) ? 240 : 240;

	const ImRect frame_bb(window->DC.CursorPos + ImVec2(11, 0), window->DC.CursorPos + ImVec2(w + 0, label_size.y + style.FramePadding.y * 2.0f + 0) + ImVec2(15, 18));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &frame_bb))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(frame_bb, id, &hovered, &held);
	bool popup_open = IsPopupOpen(id);

	if (hovered || held)
		SetMouseCursor(7);

	const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);

	if (!hovered)
		window->DrawList->AddRectFilledMultiColor(ImVec2(frame_bb.Min.x + 0, frame_bb.Min.y + 18), ImVec2(frame_bb.Max.x - 15, frame_bb.Max.y + 0), ImColor(51, 51, 51, int(alphamodulate * 253)), ImColor(51, 51, 51, int(alphamodulate * 253)), ImColor(39, 39, 40, int(alphamodulate * 253)), ImColor(39, 39, 40, int(alphamodulate * 253)));
	else
		window->DrawList->AddRectFilledMultiColor(ImVec2(frame_bb.Min.x + 0, frame_bb.Min.y + 18), ImVec2(frame_bb.Max.x - 15, frame_bb.Max.y + 0), ImColor(39, 39, 40, int(alphamodulate * 253)), ImColor(39, 39, 40, int(alphamodulate * 253)), ImColor(51, 51, 51, int(alphamodulate * 253)), ImColor(51, 51, 51, int(alphamodulate * 253)));

	PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, alphamodulate));
	if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
		RenderTextClipped(frame_bb.Min + style.FramePadding, ImVec2(value_x2, frame_bb.Max.y), preview_value, NULL, NULL, ImVec2(0.0f, 0.85f));
	PopStyleColor();

	PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, alphamodulate));
	if (label_size.x > 0)
		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);
	PopStyleColor();

	if ((pressed || g.NavActivateId == id) && !popup_open)
	{
		if (window->DC.NavLayerCurrent == 0)
			window->NavLastIds[0] = id;
		OpenPopupEx(id);
		popup_open = true;
	}

	if (!popup_open)
		return false;

	if ((flags & ImGuiComboFlags_HeightMask_) == 0)
		flags |= ImGuiComboFlags_HeightRegular;
	IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));
	int popup_max_height_in_items = -1;
	if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
	else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
	else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
	SetNextWindowSizeConstraints(ImVec2(w - 11, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCounttt(popup_max_height_in_items)));

	char name[16];
	ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size);

	if (ImGuiWindow* popup_window = FindWindowByName(name))
		if (popup_window->WasActive)
		{
			ImVec2 size_expected = CalcWindowExpectedSize(popup_window);
			if (flags & ImGuiComboFlags_PopupAlignLeft)
				popup_window->AutoPosLastDirection = ImGuiDir_Left;
			ImRect r_outer = GetWindowAllowedExtentRect(popup_window);
			ImVec2 pos = FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
			SetNextWindowPos(pos);
		}

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
	bool ret = Begin(name, NULL, window_flags);
	PopStyleVar();

	if (!ret)
	{
		EndPopup();
		return false;
	}
	return true;
}


static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}

// Getter for the old Combo() API: "item1\0item2\0item3\0"
static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
{
	// FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
	const char* items_separated_by_zeros = (const char*)data;
	int items_count = 0;
	const char* p = items_separated_by_zeros;
	while (*p)
	{
		if (idx == items_count)
			break;
		p += strlen(p) + 1;
		items_count++;
	}
	if (!*p)
		return false;
	if (out_text)
		*out_text = p;
	return true;
}

// Old API, prefer using BeginCombo() nowadays if you can.
bool combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
{
	IM_USE;
	ImGuiContext& g = *GImGui;

	// Call the getter to obtain the preview string which is a parameter to BeginCombo()
	const char* preview_value = NULL;
	if (*current_item >= 0 && *current_item < items_count)
		items_getter(data, *current_item, &preview_value);

	// The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
	if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
		SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

	if (!begincombo2(label, preview_value, ImGuiComboFlags_None))
		return false;

	// FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
	bool value_changed = false;
	for (int i = 0; i < items_count; i++)
	{
		PushID((void*)(intptr_t)i);
		const bool item_selected = (i == *current_item);
		const char* item_text;
		if (!items_getter(data, i, &item_text))
			item_text = "*Unknown item*";
		if (Selectable(item_text, item_selected))
		{
			value_changed = true;
			*current_item = i;
		}
		if (item_selected)
			SetItemDefaultFocus();
		PopID();
	}

	EndCombo();
	return value_changed;
}

bool BeginCombo3(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
	IM_USE;
	// Always consume the SetNextWindowSizeConstraint() call in our early return paths
	ImGuiContext& g = *GImGui;
	bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
	g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const float expected_w = CalcItemWidth();
	const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : expected_w;
	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &frame_bb))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(frame_bb, id, &hovered, &held);
	bool popup_open = IsPopupOpen(id, ImGuiPopupFlags_None);

	const ImU32 frame_col = GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);
	RenderNavHighlight(frame_bb, id);
	if (!(flags & ImGuiComboFlags_NoPreview))
		window->DrawList->AddRectFilled(frame_bb.Min, ImVec2(value_x2, frame_bb.Max.y), frame_col, style.FrameRounding, (flags & ImGuiComboFlags_NoArrowButton) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Left);
	if (!(flags & ImGuiComboFlags_NoArrowButton))
	{
		ImU32 bg_col = GetColorU32((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		ImU32 text_col = GetColorU32(ImGuiCol_Text);
		window->DrawList->AddRectFilled(ImVec2(value_x2, frame_bb.Min.y), frame_bb.Max, bg_col, style.FrameRounding, (w <= arrow_size) ? ImDrawCornerFlags_All : ImDrawCornerFlags_Right);
		if (value_x2 + arrow_size - style.FramePadding.x <= frame_bb.Max.x)
			RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, frame_bb.Min.y + style.FramePadding.y), text_col, ImGuiDir_Down, 1.0f);
	}
	RenderFrameBorder(frame_bb.Min, frame_bb.Max, style.FrameRounding);
	if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
		RenderTextClipped(frame_bb.Min + style.FramePadding, ImVec2(value_x2, frame_bb.Max.y), preview_value, NULL, NULL, ImVec2(0.0f, 0.0f));
	if (label_size.x > 0)
		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

	if ((pressed || g.NavActivateId == id) && !popup_open)
	{
		if (window->DC.NavLayerCurrent == 0)
			window->NavLastIds[0] = id;
		OpenPopupEx(id, ImGuiPopupFlags_None);
		popup_open = true;
	}

	if (!popup_open)
		return false;

	if (has_window_size_constraint)
	{
		g.NextWindowData.Flags |= ImGuiNextWindowDataFlags_HasSizeConstraint;
		g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
	}
	else
	{
		if ((flags & ImGuiComboFlags_HeightMask_) == 0)
			flags |= ImGuiComboFlags_HeightRegular;
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));    // Only one
		int popup_max_height_in_items = -1;
		if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
		else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
		else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
		SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
	}

	char name[16];
	ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

	// Peak into expected window size so we can position it
	if (ImGuiWindow* popup_window = FindWindowByName(name))
		if (popup_window->WasActive)
		{
			ImVec2 size_expected = CalcWindowExpectedSize(popup_window);
			if (flags & ImGuiComboFlags_PopupAlignLeft)
				popup_window->AutoPosLastDirection = ImGuiDir_Left;
			ImRect r_outer = GetWindowAllowedExtentRect(popup_window);
			ImVec2 pos = FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
			SetNextWindowPos(pos);
		}

	// We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

	// Horizontally align ourselves with the framed text
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
	bool ret = Begin(name, NULL, window_flags);
	PopStyleVar();
	if (!ret)
	{
		EndPopup();
		IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
		return false;
	}
	return true;
}

void draw_multicombo_1(std::string name, std::vector<int>& variable, const char* labels[], int count, std::string& preview)
{
	pad(10, 0);
	ImGui::Text((crypt_str(" ") + name).c_str());
	pad(0, -20);

	auto hashname = crypt_str("##") + name; // we dont want to render name of combo

	for (auto i = 0, j = 0; i < count; i++)
	{
		if (variable[i])
		{
			if (j)

				preview += crypt_str(", ") + (std::string)labels[i];
			else
				preview = labels[i];

			j++;
		}
	}

	if (begincombo(hashname.c_str(), preview.c_str(), ImGuiComboFlags_None)) // draw start
	{


		ImGui::Spacing();
		ImGui::BeginGroup();
		{

			for (auto i = 0; i < count; i++)
				ImGui::Selectable(labels[i], (bool*)&variable[i], ImGuiSelectableFlags_DontClosePopups);

		}
		ImGui::EndGroup();
		ImGui::Spacing();

		ImGui::EndCombo();
	}



	preview = crypt_str("None"); // reset preview to use later
}
// Combo box helper allowing to pass an array of strings.
bool combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
	IM_USE;
	const bool value_changed = combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
	return value_changed;
}

// Combo box helper allowing to pass all items in a single string literal holding multiple zero-terminated items "item1\0item2\0"
bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
{
	IM_USE;
	int items_count = 0;
	const char* p = items_separated_by_zeros;       // FIXME-OPT: Avoid computing this, or at least only when combo is open
	while (*p)
	{
		p += strlen(p) + 1;
		items_count++;
	}
	bool value_changed = combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
	return value_changed;
}

bool tab(const char* label, bool selected)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize({ 76,78 }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

	if (hovered || held)
		ImGui::SetMouseCursor(7);

	float t = selected ? 1.0f : 0.0f;
	float ANIM_SPEED = 0.50f; // Bigger = Slower
	if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = selected ? (t_anim) : (1.0f - t_anim);
	}

	ImVec4 col = ImLerp(ImVec4{ 229 / 255.f, 229 / 255.f, 229 / 255.f, alphamodulate }, ImVec4{ 255 / 255.f, 0 / 255.f, 255 / 255.f, alphamodulate }, t);


	window->DrawList->AddLine(ImVec2(bb.Min.x + 20, bb.Min.y + 81), ImVec2(bb.Max.x - 20, bb.Min.y + 81), ImColor(59, 59, 59, int(alphamodulate * 255)));

	if (!selected)
		ImGui::PushStyleColor(ImGuiCol_Text, col);
	else
		ImGui::PushStyleColor(ImGuiCol_Text, col);

	ImGui::RenderText(ImVec2(bb.Min.x + 0 + (76 / 2 - label_size.x / 2), bb.Min.y + style.FramePadding.y + 35), label);
	ImGui::PopStyleColor();

	return pressed;
}

bool subtab(const char* label, bool selected, ImVec2 size_arg = ImVec2(0, 0))
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	float t = selected ? 1.0f : 0.0f;
	float ANIM_SPEED = 0.50f; // Bigger = Slower
	if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = selected ? (t_anim) : (1.0f - t_anim);
	}

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

	if (hovered || held)
		ImGui::SetMouseCursor(7);

	ImVec4 col = ImLerp(ImVec4{ 140 / 255.f, 140 / 255.f, 140 / 255.f, alphamodulate }, ImVec4{ 255 / 255.f, 0 / 255.f, 255 / 255.f, alphamodulate }, t);
	ImVec4 col2 = ImLerp(ImVec4{ 140 / 255.f, 140 / 255.f, 140 / 255.f, 0 / 255.f }, ImVec4{ 255 / 255.f, 0 / 255.f, 255 / 255.f, alphamodulate }, t);

	ImGui::PushStyleColor(ImGuiCol_Text, col);
	ImGui::RenderText(ImVec2(bb.Min.x + (size_arg.x / 2 - label_size.x / 2), bb.Min.y + (size_arg.y / 2 - label_size.y / 2)), label);
	ImGui::PopStyleColor();

	window->DrawList->AddLine(bb.Min + ImVec2(40, size_arg.y), bb.Max - ImVec2(40, 0), ImGui::GetColorU32(col2));

	return pressed;
}
bool LabelClick_1(const char* label, bool* v, const char* unique_id)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	// The concatoff/on thingies were for my weapon config system so if we're going to make that, we still need this aids.
	char Buf[64];
	_snprintf(Buf, 62, crypt_str("%s"), label);

	char getid[128];
	sprintf_s(getid, 128, crypt_str("%s%s"), label, unique_id);


	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(getid);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	const ImRect check_bb(window->DC.CursorPos, ImVec2(label_size.y + style.FramePadding.y * 2 + window->DC.CursorPos.x, window->DC.CursorPos.y + label_size.y + style.FramePadding.y * 2));
	ImGui::ItemSize(check_bb, style.FramePadding.y);

	ImRect total_bb = check_bb;

	if (label_size.x > 0)
	{
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
		const ImRect text_bb(ImVec2(window->DC.CursorPos.x, window->DC.CursorPos.y + style.FramePadding.y), ImVec2(window->DC.CursorPos.x + label_size.x, window->DC.CursorPos.y + style.FramePadding.y + label_size.y));

		ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}

	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
		*v = !(*v);

	if (*v)
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255 / 255.f, 0 / 255.f, 255 / 255.f, 1.f));
	if (label_size.x > 0.0f)
		ImGui::RenderText(ImVec2(check_bb.GetTL().x + 12, check_bb.GetTL().y), Buf);
	if (*v)
		ImGui::PopStyleColor();

	return pressed;

}

void draw_keybind_1(const char* label, key_bind* key_bind, const char* unique_id)
{
	// reset bind if we re pressing esc
	if (key_bind->key == KEY_ESCAPE)
		key_bind->key = KEY_NONE;

	auto clicked = false;
	auto text = (std::string)m_inputsys()->ButtonCodeToString(key_bind->key);

	if (key_bind->key <= KEY_NONE || key_bind->key >= KEY_MAX)
		text = crypt_str("-");

	// if we clicked on keybind
	if (hooks::input_shouldListen && hooks::input_receivedKeyval == &key_bind->key)
	{
		clicked = true;
		text = crypt_str("...");
	}

	auto textsize = ImGui::CalcTextSize(text.c_str()).x + 8 * 1;
	auto labelsize = ImGui::CalcTextSize(label);

	ImGui::Text(label);
	ImGui::SameLine();

	ImGui::SetCursorPosX(ImGui::GetWindowSize().x - (ImGui::GetWindowSize().x - ImGui::CalcItemWidth()) - max(50 * 1, textsize));
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3 * 1);

	if (ImGui::KeybindButton(text.c_str(), unique_id, ImVec2(max(50 * 1, textsize), 23 * 1), clicked))
		clicked = true;


	if (clicked)
	{
		hooks::input_shouldListen = true;
		hooks::input_receivedKeyval = &key_bind->key;
	}

	static auto hold = false, toggle = false;

	switch (key_bind->mode)
	{
	case HOLD:
		hold = true;
		toggle = false;
		break;
	case TOGGLE:
		toggle = true;
		hold = false;
		break;
	}

	if (ImGui::BeginPopup(unique_id))
	{
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6 * 1);
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize(crypt_str("Hold")).x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);

		if (LabelClick_1(crypt_str("Hold"), &hold, unique_id))
		{
			if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else
			{
				toggle = false;
				key_bind->mode = HOLD;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ((ImGui::GetCurrentWindow()->Size.x / 2) - (ImGui::CalcTextSize(crypt_str("Toggle")).x / 2)));
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 11);
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9 * 1);

		if (LabelClick_1(crypt_str("Toggle"), &toggle, unique_id))
		{
			if (toggle)
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}
			else if (hold)
			{
				toggle = false;
				key_bind->mode = HOLD;
			}
			else
			{
				hold = false;
				key_bind->mode = TOGGLE;
			}

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

static const char* PatchFormatStringFloatToInt(const char* fmt)
{
	if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0)
		return "%d";

	const char* fmt_start = ImParseFormatFindStart(fmt);
	const char* fmt_end = ImParseFormatFindEnd(fmt_start);
	if (fmt_end > fmt_start && fmt_end[-1] == 'f')
	{
		if (fmt_start == fmt && fmt_end[0] == 0)
			return "%d";
		ImGuiContext& g = *GImGui;
		ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end);
		return g.TempBuffer;
	}
	return fmt;
}

bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = 240;

	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	const ImRect frame_bb(window->DC.CursorPos + ImVec2(12, 18), window->DC.CursorPos + ImVec2(w + 0, label_size.y + 7 + (style.FramePadding.y * 2)));
	const ImRect total_bb(frame_bb.Min + ImVec2(12, -18), frame_bb.Max + ImVec2(0, 1) + ImVec2(0.0f, 0.0f));

	ImGui::ItemSize(total_bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
		return false;

	if (format == NULL)
		format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
	else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0)
		format = PatchFormatStringFloatToInt(format);
	const bool hovered = ImGui::ItemHoverable(frame_bb, id);
	const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
	bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
	if (!temp_input_is_active)
	{
		const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister(window, id);
		const bool clicked = (hovered && g.IO.MouseClicked[0]);
		if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
		{
			ImGui::SetActiveID(id, window);
			ImGui::SetFocusID(id, window);
			ImGui::FocusWindow(window);
			g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
		}
	}
	if (!hovered)
	{
		window->DrawList->AddRectFilledMultiColor(ImVec2(frame_bb.Min.x + 0, frame_bb.Min.y + 0), ImVec2(frame_bb.Max.x + 0, frame_bb.Max.y + 0), ImColor(51, 51, 51, int(253 * alphamodulate)), ImColor(51, 51, 51, int(253 * alphamodulate)), ImColor(39, 39, 40, int(253 * alphamodulate)), ImColor(39, 39, 40, int(253 * alphamodulate)));
	}
	else
	{
		window->DrawList->AddRectFilledMultiColor(ImVec2(frame_bb.Min.x + 0, frame_bb.Min.y + 0), ImVec2(frame_bb.Max.x + 0, frame_bb.Max.y + 0), ImColor(51, 51, 51, int(253 * alphamodulate)), ImColor(51, 51, 51, int(253 * alphamodulate)), ImColor(39, 39, 40, int(253 * alphamodulate)), ImColor(39, 39, 40, int(253 * alphamodulate)));
	}

	ImRect grab_bb;
	const bool value_changed = ImGui::SliderBehavior2(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);

	grab_bb.Min = frame_bb.Min;
	grab_bb.Min += ImVec2(0, 0);
	grab_bb.Max += ImVec2(0, 2);

	if (!hovered)
	{
		window->DrawList->AddRectFilledMultiColor(ImVec2(grab_bb.Min.x + 0, grab_bb.Min.y + 0), ImVec2(grab_bb.Max.x + 0, grab_bb.Max.y + 0), ImColor(255, 0, 255, int(230 * alphamodulate)), ImColor(255, 0, 255, int(230 * alphamodulate)), ImColor(139, 6, 139, int(230 * alphamodulate)), ImColor(139, 6, 139, int(230 * alphamodulate)));
	}
	else
	{
		window->DrawList->AddRectFilledMultiColor(ImVec2(grab_bb.Min.x + 0, grab_bb.Min.y + 0), ImVec2(grab_bb.Max.x + 0, grab_bb.Max.y + 0), ImColor(139, 6, 139, int(230 * alphamodulate)), ImColor(139, 6, 139, int(230 * alphamodulate)), ImColor(112, 2, 112, int(230 * alphamodulate)), ImColor(112, 2, 112, int(230 * alphamodulate)));
	}

	char value_buf[64];
	const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, alphamodulate));
	ImGui::RenderText(ImVec2(frame_bb.Max.x - ImGui::CalcTextSize(value_buf).x, frame_bb.Min.y + style.FramePadding.y + -19), value_buf);
	ImGui::PopStyleColor();

	if (hovered)
		ImGui::SetMouseCursor(7);

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(198 / 255.f, 198 / 255.f, 198 / 255.f, alphamodulate));
	ImGui::RenderText(ImVec2(frame_bb.Min.x, frame_bb.Min.y + style.FramePadding.y + -19), label);
	ImGui::PopStyleColor();
	return value_changed;
}

bool slider_float(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}

bool slider_int(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

void shadow(ImDrawList* drawlist, ImVec2 pos, ImVec2 size, ImVec4 color)
{
	while (true)
	{
		if (color.w <= 0.019f)
			break;

		drawlist->AddRect(pos, pos + size, ImGui::GetColorU32(color), 8);
		color.w -= color.w / 8;
		pos -= ImVec2(1.f, 1.f);
		size += ImVec2(2.f, 2.f);
	}
}

void render_decorations()
{
	draw->AddRect(ImVec2(p.x + 0, p.y + 0), ImVec2(p.x + 845, p.y + 560), ImColor(59, 59, 59, int(255 * alphamodulate)), 0, 15, 1.000000);
	draw->AddLine(ImVec2(p.x + 0, p.y + 30), ImVec2(p.x + 875, p.y + 30), ImColor(59, 59, 59, int(255 * alphamodulate)), 1.000000);
	draw->AddImage(background, p, p + ImVec2{ 845.000000f, 560.000000f });
	draw->AddRectFilledMultiColor(ImVec2(p.x + 100, p.y + 30), ImVec2(p.x + 437, p.y + 31), ImColor(59, 59, 59, 0), ImColor(255, 0, 255, int(230 * alphamodulate)), ImColor(255, 0, 255, int(230 * alphamodulate)), ImColor(59, 59, 59, 0));
	draw->AddRectFilledMultiColor(ImVec2(p.x + 437, p.y + 30), ImVec2(p.x + 745, p.y + 31), ImColor(255, 0, 255, int(230 * alphamodulate)), ImColor(59, 59, 59, 0), ImColor(59, 59, 59, 0), ImColor(255, 0, 255, int(230 * alphamodulate)));
	draw->AddRectFilled(ImVec2(p.x + 7, p.y + 37), ImVec2(p.x + 85, p.y + 530), ImColor(12, 12, 12, int(170 * alphamodulate)), 0, 15);
	draw->AddRect(ImVec2(p.x + 7, p.y + 37), ImVec2(p.x + 85, p.y + 530), ImColor(59, 59, 59, int(170 * alphamodulate)), 0, 15, 1.000000);
	draw->AddRect(ImVec2(p.x + 8, p.y + 38), ImVec2(p.x + 84, p.y + 529), ImColor(2, 2, 2, int(170 * alphamodulate)), 0, 15, 1.000000);
	draw->AddRectFilled(ImVec2(p.x + 92, p.y + 37), ImVec2(p.x + 838, p.y + 530), ImColor(12, 12, 12, int(170 * alphamodulate)), 0, 15);
	draw->AddRect(ImVec2(p.x + 92, p.y + 37), ImVec2(p.x + 838, p.y + 530), ImColor(59, 59, 59, int(170 * alphamodulate)), 0, 15, 1.000000);
	draw->AddRect(ImVec2(p.x + 93, p.y + 38), ImVec2(p.x + 837, p.y + 529), ImColor(3, 1, 1, int(170 * alphamodulate)), 0, 15, 1.000000);

	draw->AddLine(ImVec2(p.x + 92, p.y + 37 + 26), ImVec2(p.x + 838, p.y + 37 + 26), ImColor(59, 59, 59, int(150 * alphamodulate)), 1.000000);
}

void render_tabs()
{
	ImGui::SetCursorPos({ (float)8, (float)38 });
	ImGui::BeginGroup();
	ImGui::PushFont(c_menu::get().tabs);
	if (tab("A", 1 == selectedtab))
		selectedtab = 1;
	if (tab("B", 2 == selectedtab))
		selectedtab = 2;
	if (tab("C", 3 == selectedtab))
		selectedtab = 3;
	if (tab("D", 4 == selectedtab))
		selectedtab = 4;
	if (tab("E", 5 == selectedtab))
		selectedtab = 5;
	//if (tab("F", 6 == selectedtab))
		//selectedtab = 6;
	ImGui::PopFont();
	ImGui::EndGroup();
}

void render_content()
{
	ImGui::SetCursorPos({ (float)92, (float)38 });
	ImGui::BeginChild("workzone", ImVec2(838 - 92, 530 - 37));

	if (selectedtab == 1)
	{
		static int selectedsubtab = 0;

		ImGui::BeginGroup();
		if (subtab("general", 0 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 0;
		ImGui::SameLine();
		if (subtab("weapons", 1 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 1;
		ImGui::SameLine();
		if (subtab("anti-hit", 2 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 2;
		ImGui::SameLine();
		if (subtab("players", 3 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 3;
		ImGui::EndGroup();

		ImGui::SetCursorPos({ 15,35 });

		if (selectedsubtab == 0)
		{
			ImGui::BeginChild("workzone_rage", ImVec2(838 - 92 - 30, 530 - 72));
			{
				beginchild("##subtabs0", { 352,445 });
				{
					ImGui::Spacing();
					ImGui::Spacing();


					checkbox(crypt_str("Enabled"), &g_cfg.ragebot.enable);

					if (g_cfg.ragebot.enable)
						g_cfg.legitbot.enabled = false;

					slider_int(crypt_str("Max FOV"), &g_cfg.ragebot.field_of_view, 1, 180, NULL, NULL);
					checkbox(crypt_str("Silent aim"), &g_cfg.ragebot.silent_aim);
					checkbox(crypt_str("Auto wall"), &g_cfg.ragebot.autowall);
					checkbox(crypt_str("Zeus bot"), &g_cfg.ragebot.zeus_bot);
					checkbox(crypt_str("Knife bot"), &g_cfg.ragebot.knife_bot);
					checkbox(crypt_str("Auto fire"), &g_cfg.ragebot.autoshoot);
					checkbox(crypt_str("Pitch desync correction"), &g_cfg.ragebot.pitch_antiaim_correction);
					checkbox(crypt_str("Optimize records"), &g_cfg.ragebot.optimizer);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Use this only on low end pc's");
					//checkbox(crypt_str("Anti-aim correction"), &g_cfg.ragebot.resolver);
					//checkbox(crypt_str("Multipoints"), &g_cfg.ragebot.multipointss);
					pad(10, 0);
					draw_keybind_1(crypt_str("Force safe points"), &g_cfg.ragebot.safe_point_key, crypt_str("##HOKEY_FORCE_SAFE_POINTS"));
					pad(10, 0);
					draw_keybind_1(crypt_str("Force body aim"), &g_cfg.ragebot.body_aim_key, crypt_str("##HOKEY_FORCE_BODY_AIM"));
					pad(10, 0);
					draw_keybind_1(crypt_str("Force head"), &g_cfg.ragebot.onshot, crypt_str("##ONSHOT__HOTKEY"));
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_0", { 352,445 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					checkbox(crypt_str("Double tap"), &g_cfg.ragebot.double_tap);

					if (g_cfg.ragebot.double_tap)
					{
						ImGui::SameLine();
						draw_keybind_1(crypt_str(""), &g_cfg.ragebot.double_tap_key, crypt_str("##HOTKEY_DT"));
						//checkbox(crypt_str("Teleport"), &g_cfg.ragebot.slow_teleport);
						//if (g_cfg.ragebot.slow_teleport)
						//{
						//	slider_float(crypt_str("Teleport speed"), &g_cfg.ragebot.teleport_speed, 0.0f, 450.0f, g_cfg.ragebot.teleport_speed ? crypt_str("%.2f") : crypt_str("None"), NULL);
						//}
						//combo(crypt_str("Double tap type"), &g_cfg.ragebot.dtap_type, dtap, ARRAYSIZE(dtap));
					}
					checkbox(crypt_str("Hide shots"), &g_cfg.antiaim.hide_shots);

					if (g_cfg.antiaim.hide_shots)
					{
						ImGui::SameLine();
						draw_keybind_1(crypt_str(""), &g_cfg.antiaim.hide_shots_key, crypt_str("##HOTKEY_HIDESHOTS"));
					}
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}

		if (selectedsubtab == 1)
		{
			ImGui::BeginChild("workzone_rage_weapon", ImVec2(838 - 92 - 30, 530 - 72));
			{
				ImGui::BeginGroup();
				ImGui::PushFont(c_menu::get().menu_wpn_icons);

				if (subtab("J", 0 == hooks::rage_weapon, ImVec2(82, 25)))
					hooks::rage_weapon = 0;
				ImGui::SameLine();
				if (subtab("D", 1 == hooks::rage_weapon, ImVec2(82, 25)))
					hooks::rage_weapon = 1;
				ImGui::SameLine();
				if (subtab("N", 2 == hooks::rage_weapon, ImVec2(82, 25)))
					hooks::rage_weapon = 2;
				ImGui::SameLine();
				if (subtab("W", 3 == hooks::rage_weapon, ImVec2(82, 25)))
					hooks::rage_weapon = 3;
				ImGui::SameLine();
				if (subtab("X", 4 == hooks::rage_weapon, ImVec2(82, 25)))
					hooks::rage_weapon = 4;
				ImGui::SameLine();
				if (subtab("a", 5 == hooks::rage_weapon, ImVec2(82, 25)))
					hooks::rage_weapon = 5;
				ImGui::SameLine();
				if (subtab("Z", 6 == hooks::rage_weapon, ImVec2(82, 25)))
					hooks::rage_weapon = 6;
				ImGui::SameLine();
				if (subtab("f", 7 == hooks::rage_weapon, ImVec2(82, 25)))
					hooks::rage_weapon = 7;
				ImGui::PopFont();
				ImGui::EndGroup();



				beginchild("##subtabs_wpn1", { 352,416 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					combo(crypt_str("Target selection"), &g_cfg.ragebot.weapon[hooks::rage_weapon].selection_type, selection, ARRAYSIZE(selection));

					draw_multicombo_1(crypt_str("Hitboxes"), g_cfg.ragebot.weapon[hooks::rage_weapon].hitboxes, hitboxes, ARRAYSIZE(hitboxes), preview);

					checkbox(crypt_str("Ignore limbs when moving"), &g_cfg.ragebot.weapon[hooks::rage_weapon].ignore_limbs);

					draw_multicombo_1(crypt_str("Multipoint hitboxes"), g_cfg.ragebot.weapon[hooks::rage_weapon].multipoints_hitboxes, hitboxes, ARRAYSIZE(hitboxes), preview);
					slider_float(crypt_str("Multipoint scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].multipoints_scale, 0.0f, 1.0f, g_cfg.ragebot.weapon[hooks::rage_weapon].multipoints_scale ? crypt_str("%.2f") : crypt_str("None"), NULL);
					slider_float(crypt_str("Stomach scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].stomach_scale, 0.0f, 1.0f, g_cfg.ragebot.weapon[hooks::rage_weapon].stomach_scale ? crypt_str("%.2f") : crypt_str("None"), NULL);
					checkbox(crypt_str("Adaptive point scale"), &g_cfg.ragebot.weapon[hooks::rage_weapon].adaptive_ps);

					checkbox(crypt_str("Baim after x misses"), &g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses);

					if (g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses)
						slider_int(crypt_str("Max misses"), &g_cfg.ragebot.weapon[hooks::rage_weapon].max_misses_amount, 0, 6, NULL, NULL);

					checkbox(crypt_str("Prefer safe points"), &g_cfg.ragebot.weapon[hooks::rage_weapon].prefer_safe_points);
				//	checkbox(crypt_str("Prefer body aim"), &g_cfg.ragebot.weapon[hooks::rage_weapon].prefer_body_aim);

					draw_multicombo_1(crypt_str("Body aim conditions"), g_cfg.ragebot.weapon[hooks::rage_weapon].bodyaimcond, bodyaimcond, ARRAYSIZE(bodyaimcond), preview);

					draw_multicombo_1(crypt_str("Prefer head aim"), g_cfg.ragebot.weapon[hooks::rage_weapon].headaimcond, headaimcond, ARRAYSIZE(headaimcond), preview);

					draw_multicombo_1(crypt_str("Head aim only when"), g_cfg.ragebot.weapon[hooks::rage_weapon].headaimonlycond, headaimonlycond, ARRAYSIZE(headaimonlycond), preview);

					slider_int(crypt_str("Min visible dmg"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_visible_damage, 0, 120, NULL, NULL);

					if (g_cfg.ragebot.autowall)
						slider_int(crypt_str("Min wall dmg"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_damage, 0, 120, NULL, NULL);

					pad(10, 0);
					draw_keybind_1(crypt_str("Override min dmg"), &g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key, crypt_str("##HOTKEY__DAMAGE_OVERRIDE"));

					if (g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key > KEY_NONE && g_cfg.ragebot.weapon[hooks::rage_weapon].damage_override_key.key < KEY_MAX)
						slider_int(crypt_str("Min override dmg"), &g_cfg.ragebot.weapon[hooks::rage_weapon].minimum_override_damage, 1, 120, NULL, NULL);
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_wpn2", { 352,416 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					checkbox(crypt_str("Auto stop"), &g_cfg.ragebot.weapon[hooks::rage_weapon].autostop);

					if (g_cfg.ragebot.weapon[hooks::rage_weapon].autostop)
						draw_multicombo_1(crypt_str("Modifiers"), g_cfg.ragebot.weapon[hooks::rage_weapon].autostop_modifiers, autostop_modifiers, ARRAYSIZE(autostop_modifiers), preview);

					if (hooks::rage_weapon == 4 || hooks::rage_weapon == 5 || hooks::rage_weapon == 6)
					{
						checkbox(crypt_str("Auto scope"), &g_cfg.ragebot.weapon[hooks::rage_weapon].autoscope);

						if (g_cfg.ragebot.weapon[hooks::rage_weapon].autoscope == 1)
						{
							combo(crypt_str("Auto scope mode"), &g_cfg.ragebot.weapon[hooks::rage_weapon].autoscope_type, autoscope_type, ARRAYSIZE(autoscope_type));
						}
					}

					draw_multicombo_1(crypt_str("Safe points conditions"), g_cfg.ragebot.weapon[hooks::rage_weapon].safe_points_conditions, safe_points_conditions, ARRAYSIZE(safe_points_conditions), preview);

					checkbox(crypt_str("Hitchance"), &g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance);
					slider_int(crypt_str("Hitchance amount"), &g_cfg.ragebot.weapon[hooks::rage_weapon].hitchance_amount, 1, 100, NULL, NULL);

					if (g_cfg.ragebot.double_tap && hooks::rage_weapon <= 4)
					{
						slider_int(crypt_str("Shift value##DT"), &g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_shift_value, 1, 16, NULL, NULL);
						checkbox(crypt_str("Double tap hitchance"), &g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance);

						if (g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance)
							slider_int(crypt_str("Double tap hitchance"), &g_cfg.ragebot.weapon[hooks::rage_weapon].double_tap_hitchance_amount, 1, 100, NULL, NULL);
					}
					
					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}

		if (selectedsubtab == 2)
		{
			ImGui::BeginChild("workzone_aa", ImVec2(838 - 92 - 30, 530 - 72));
			{
				beginchild("##subtabs_aa_1", { 352,445 });
				{
					ImGui::Spacing();
					ImGui::Spacing();


					checkbox(crypt_str("Enabled"), &g_cfg.antiaim.enable);
					combo(crypt_str("Anti-aim type"), &g_cfg.antiaim.antiaim_type, antiaim_type, ARRAYSIZE(antiaim_type));

					if (g_cfg.antiaim.antiaim_type)
					{
						combo(crypt_str("Desync"), &g_cfg.antiaim.desync, desync, ARRAYSIZE(desync));

						if (g_cfg.antiaim.desync)
						{
							combo(crypt_str("LBY type"), &g_cfg.antiaim.legit_lby_type, lby_type, ARRAYSIZE(lby_type));

							if (g_cfg.antiaim.desync == 1)
							{
								draw_keybind_1(crypt_str("Invert desync"), &g_cfg.antiaim.flip_desync, crypt_str("##HOTKEY_INVERT_DESYNC"));
							}
						}
					}
					else
					{
						pad(0, 3);

						ImGui::PushFont(c_menu::get().Icons2);
						static int aa_type = 1;

						pad(10, 0);
						if (subtab("L", 0 == aa_type, ImVec2(75, 25)))
							aa_type = 0;
						ImGui::SameLine();
						if (subtab("F", 1 == aa_type, ImVec2(75, 25)))
							aa_type = 1;
						ImGui::SameLine();
						if (subtab("J", 2 == aa_type, ImVec2(75, 25)))
							aa_type = 2;
						ImGui::SameLine();
						if (subtab("E", 3 == aa_type, ImVec2(75, 25)))
							aa_type = 3;


						ImGui::PopFont();

						pad(0, 3);


						combo(crypt_str("Pitch"), &g_cfg.antiaim.type[aa_type].pitch, pitch, ARRAYSIZE(pitch));
						combo(crypt_str("Yaw"), &g_cfg.antiaim.type[aa_type].yaw, yaw, ARRAYSIZE(yaw));
						combo(crypt_str("Base angle"), &g_cfg.antiaim.type[aa_type].base_angle, baseangle, ARRAYSIZE(baseangle));

						if (g_cfg.antiaim.type[aa_type].yaw)
						{
							slider_int(g_cfg.antiaim.type[aa_type].yaw == 1 ? crypt_str("Jitter range") : crypt_str("Spin range"), &g_cfg.antiaim.type[aa_type].range, 1, 180, NULL, NULL);

							if (g_cfg.antiaim.type[aa_type].yaw == 2)
								slider_int(crypt_str("Spin speed"), &g_cfg.antiaim.type[aa_type].speed, 1, 15, NULL, NULL);

						}

						combo(crypt_str("Desync"), &g_cfg.antiaim.type[aa_type].desync, desync, ARRAYSIZE(desync));

						if (g_cfg.antiaim.type[aa_type].desync)
						{
							if (aa_type == ANTIAIM_STAND)
							{
								combo(crypt_str("LBY type"), &g_cfg.antiaim.lby_type, lby_type, ARRAYSIZE(lby_type));
							}

							if (aa_type != ANTIAIM_STAND || !g_cfg.antiaim.lby_type)
							{
								slider_int(crypt_str("Desync delta"), &g_cfg.antiaim.type[aa_type].desync_range, 1, 60, NULL, NULL);
								slider_int(crypt_str("Inverted desync delta"), &g_cfg.antiaim.type[aa_type].inverted_desync_range, 1, 60, NULL, NULL);
							}

							if (g_cfg.antiaim.type[aa_type].desync == 1)
							{
								pad(10, 0);
								draw_keybind_1(crypt_str("Invert desync"), &g_cfg.antiaim.flip_desync, crypt_str("##HOTKEY_INVERT_DESYNC"));
							}
						}
						pad(10, 0);
						draw_keybind_1(crypt_str("Freestanding"), &g_cfg.antiaim.freestanding_key, crypt_str("##HOTKEY_FS_KEY"));
						pad(10, 0);
						draw_keybind_1(crypt_str("Manual back"), &g_cfg.antiaim.manual_back, crypt_str("##HOTKEY_INVERT_BACK"));
						pad(10, 0);
						draw_keybind_1(crypt_str("Manual left"), &g_cfg.antiaim.manual_left, crypt_str("##HOTKEY_INVERT_LEFT"));
						pad(10, 0);
						draw_keybind_1(crypt_str("Manual right"), &g_cfg.antiaim.manual_right, crypt_str("##HOTKEY_INVERT_RIGHT"));

						if (g_cfg.antiaim.manual_back.key > KEY_NONE && g_cfg.antiaim.manual_back.key < KEY_MAX || g_cfg.antiaim.manual_left.key > KEY_NONE && g_cfg.antiaim.manual_left.key < KEY_MAX || g_cfg.antiaim.manual_right.key > KEY_NONE && g_cfg.antiaim.manual_right.key < KEY_MAX)
						{
							checkbox(crypt_str("Manual indicator"), &g_cfg.antiaim.flip_indicator);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##invc"), &g_cfg.antiaim.flip_indicator_color, ALPHA);
						}
						ImGui::Spacing();
						ImGui::Spacing();
					}

				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_aa_2", { 352,445 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					checkbox(crypt_str("Enabled"), &g_cfg.antiaim.fakelag);

					combo(crypt_str("Type"), &g_cfg.antiaim.fakelag_type, fakelags, ARRAYSIZE(fakelags));
					slider_int(crypt_str("Limit"), &g_cfg.antiaim.fakelag_amount, 1, 16, NULL, NULL);

					draw_multicombo_1(crypt_str("Triggers"), g_cfg.antiaim.fakelag_enablers, lagstrigger, ARRAYSIZE(lagstrigger), preview);

					checkbox(crypt_str("Disable while shooting"), &g_cfg.antiaim.while_shot);

					auto enabled_fakelag_triggers = false;

					for (auto i = 0; i < ARRAYSIZE(lagstrigger); i++)
						if (g_cfg.antiaim.fakelag_enablers[i])
							enabled_fakelag_triggers = true;

					if (enabled_fakelag_triggers)
						slider_int(crypt_str("Triggers limit"), &g_cfg.antiaim.triggers_fakelag_amount, 1, 16, NULL, NULL);
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}


		if (selectedsubtab == 3)
		{
			ImGui::BeginChild("workzone_aa_misc", ImVec2(838 - 92 - 30, 530 - 72));
			{
				static auto current_player = 0;
				auto player = players_tab;

				static std::vector <Player_list_data> players;
				beginchild("##subtabs_players_1", { 352,445 });
				{
					ImGui::Spacing();
					ImGui::Spacing();


					if (!g_cfg.player_list.refreshing)
					{
						players.clear();

						for (auto player : g_cfg.player_list.players)
							players.emplace_back(player);
					}

					if (!players.empty())
					{
						std::vector <std::string> player_names;

						for (auto player : players)
							player_names.emplace_back(player.name);

						ImGui::PushItemWidth(291);
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
						pad(10, 0);
						ImGui::ListBoxConfigArray(crypt_str("##PLAYERLIST"), &current_player, player_names, 14);
						ImGui::PopStyleVar();
						ImGui::PopItemWidth();
					}

				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_players_2", { 352,445 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					if (!players.empty())
					{
						if (current_player >= players.size())
							current_player = players.size() - 1; //-V103

						checkbox(crypt_str("White list"), &g_cfg.player_list.white_list[players.at(current_player).i]); //-V106 //-V807

						if (!g_cfg.player_list.white_list[players.at(current_player).i]) //-V106
						{
							checkbox(crypt_str("High priority"), &g_cfg.player_list.high_priority[players.at(current_player).i]); //-V106
							checkbox(crypt_str("Force safe points"), &g_cfg.player_list.force_safe_points[players.at(current_player).i]); //-V106
							checkbox(crypt_str("Force body aim"), &g_cfg.player_list.force_body_aim[players.at(current_player).i]); //-V106
							checkbox(crypt_str("Low delta"), &g_cfg.player_list.low_delta[players.at(current_player).i]); //-V106
						}
					}
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
	}


	if (selectedtab == 2)
	{
		static int selectedsubtab = 0;

		ImGui::BeginGroup();
		if (subtab("general", 0 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 0;
		ImGui::SameLine();
		if (subtab("trigger", 1 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 1;
		ImGui::EndGroup();

		ImGui::SetCursorPos({ 15,35 });

		if (selectedsubtab == 0)
		{
			ImGui::BeginChild("workzone_legit", ImVec2(838 - 92 - 30, 530 - 72));
			{
				ImGui::BeginGroup();
				ImGui::PushFont(c_menu::get().menu_wpn_icons);

				if (subtab("A", 0 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 0;
				ImGui::SameLine();
				if (subtab("D", 1 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 1;
				ImGui::SameLine();
				if (subtab("W", 2 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 2;
				ImGui::SameLine();
				if (subtab("N", 3 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 3;
				ImGui::SameLine();
				if (subtab("Z", 4 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 4;
				ImGui::SameLine();
				if (subtab("f", 5 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 5;

				ImGui::PopFont();
				ImGui::EndGroup();

				beginchild("##subtabs_legit_1", { 352,416 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					checkbox(crypt_str("Enabled"), &g_cfg.legitbot.enabled);
					ImGui::SameLine();
					draw_keybind_1(crypt_str(""), &g_cfg.legitbot.key, crypt_str("##HOTKEY_LGBT_KEY"));

					if (g_cfg.legitbot.enabled)
						g_cfg.ragebot.enable = false;

					checkbox(crypt_str("Friendly fire"), &g_cfg.legitbot.friendly_fire);
					checkbox(crypt_str("Auto pistols"), &g_cfg.legitbot.autopistol);

					checkbox(crypt_str("Auto scope"), &g_cfg.legitbot.autoscope);

					if (g_cfg.legitbot.autoscope)
						checkbox(crypt_str("Unscope after shot"), &g_cfg.legitbot.unscope);

					checkbox(crypt_str("Snipers in zoom only"), &g_cfg.legitbot.sniper_in_zoom_only);

					checkbox(crypt_str("Aim if in air"), &g_cfg.legitbot.do_if_local_in_air);
					checkbox(crypt_str("Aim if flashed"), &g_cfg.legitbot.do_if_local_flashed);
					checkbox(crypt_str("Aim thru smoke"), &g_cfg.legitbot.do_if_enemy_in_smoke);
					checkbox(crypt_str("Backtracking"), &g_cfg.legitbot.backtrackl);
					checkbox(crypt_str("Legit resolver"), &g_cfg.legitbot.legit_resolver);
					pad(10, 0);
					draw_keybind_1(crypt_str("Trigger bot key"), &g_cfg.legitbot.autofire_key, crypt_str("##HOTKEY_AUTOFIRE_LGBT_KEY"));
					slider_int(crypt_str("Trigger bot delay"), &g_cfg.legitbot.autofire_delay, 0, 12, false, (!g_cfg.legitbot.autofire_delay ? crypt_str("None") : (g_cfg.legitbot.autofire_delay == 1 ? crypt_str("%d tick") : crypt_str("%d ticks")), NULL));
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_legit_2", { 352,416 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					combo(crypt_str("Hitbox"), &g_cfg.legitbot.weapon[hooks::legit_weapon].priority, LegitHitbox, ARRAYSIZE(LegitHitbox));

					checkbox(crypt_str("Auto stop"), &g_cfg.legitbot.weapon[hooks::legit_weapon].auto_stop);

					combo(crypt_str("Maximum FOV type"), &g_cfg.legitbot.weapon[hooks::legit_weapon].fov_type, LegitFov, ARRAYSIZE(LegitFov));
					slider_float(crypt_str("Maximum FOV amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].fov, 0.f, 30.f, crypt_str("%.2f"), NULL);

					slider_float(crypt_str("Silent FOV"), &g_cfg.legitbot.weapon[hooks::legit_weapon].silent_fov, 0.f, 30.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].silent_fov ? crypt_str("None") : crypt_str("%.2f")), NULL); //-V550

					combo(crypt_str("Smooth type"), &g_cfg.legitbot.weapon[hooks::legit_weapon].smooth_type, LegitSmooth, ARRAYSIZE(LegitSmooth));
					slider_float(crypt_str("Smooth amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].smooth, 1.f, 12.f, crypt_str("%.1f"), NULL);

					slider_int(crypt_str("Auto wall damage"), &g_cfg.legitbot.weapon[hooks::legit_weapon].awall_dmg, 0, 100, false, (!g_cfg.legitbot.weapon[hooks::legit_weapon].awall_dmg ? crypt_str("None") : crypt_str("%d"), NULL));
					slider_int(crypt_str("Trigger bot hitchnce"), &g_cfg.legitbot.weapon[hooks::legit_weapon].autofire_hitchance, 0, 100, false, (!g_cfg.legitbot.weapon[hooks::legit_weapon].autofire_hitchance ? crypt_str("None") : crypt_str("%d"), NULL));
					slider_float(crypt_str("Target switch delay"), &g_cfg.legitbot.weapon[hooks::legit_weapon].target_switch_delay, 0.f, 1.f, NULL, NULL);
					slider_float(crypt_str("Backtrack ticks"), &g_cfg.legitbot.weapon[hooks::legit_weapon].backtrack_ticks, 0, 12, NULL, NULL);

					combo(crypt_str("RCS type"), &g_cfg.legitbot.weapon[hooks::legit_weapon].rcs_type, RCSType, ARRAYSIZE(RCSType));
					slider_float(crypt_str("RCS amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].rcs, 0.f, 100.f, crypt_str("%.0f%%"), 1.f);

					if (g_cfg.legitbot.weapon[hooks::legit_weapon].rcs > 0)
					{
						slider_float(crypt_str("RCS custom FOV"), &g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_fov, 0.f, 30.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_fov ? crypt_str("None") : crypt_str("%.2f")), NULL); //-V550
						slider_float(crypt_str("RCS Custom smooth"), &g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_smooth, 0.f, 12.f, (!g_cfg.legitbot.weapon[hooks::legit_weapon].custom_rcs_smooth ? crypt_str("None") : crypt_str("%.1f")), NULL); //-V550
					}
					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}

		if (selectedsubtab == 1)
		{
			ImGui::BeginChild("workzone_legit2", ImVec2(838 - 92 - 30, 530 - 72));
			{
				ImGui::BeginGroup();
				ImGui::PushFont(c_menu::get().menu_wpn_icons);

				if (subtab("A", 0 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 0;
				ImGui::SameLine();
				if (subtab("D", 1 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 1;
				ImGui::SameLine();
				if (subtab("W", 2 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 2;
				ImGui::SameLine();
				if (subtab("N", 3 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 3;
				ImGui::SameLine();
				if (subtab("Z", 4 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 4;
				ImGui::SameLine();
				if (subtab("f", 5 == hooks::legit_weapon, ImVec2(110, 25)))
					hooks::legit_weapon = 5;

				ImGui::PopFont();
				ImGui::EndGroup();

				beginchild("##subtabs_trigger_1", { 352,416 });
				{

				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_trigger_2", { 352,416 });
				{

				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}


		ImGui::EndChild();
	}


	if (selectedtab == 3)
	{
		static int selectedsubtab = 0;

		ImGui::BeginGroup();
		if (subtab("players", 0 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 0;
		ImGui::SameLine();
		if (subtab("visuals", 1 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 1;
		//ImGui::SameLine();
		//if (subtab("visuals", 2 == selectedsubtab, ImVec2(180, 25)))
			//selectedsubtab = 2;
		ImGui::EndGroup();

		ImGui::SetCursorPos({ 15,35 });
		auto player = g_cfg.player.teams;

		if (selectedsubtab == 0)
		{
			ImGui::BeginChild("workzone_visuals", ImVec2(838 - 92 - 30, 530 - 72));
			{
				ImGui::BeginGroup();

				if (subtab("enemies", 0 == g_cfg.player.teams, ImVec2(235, 25)))
					g_cfg.player.teams = 0;
				ImGui::SameLine();
				if (subtab("teammates", 1 == g_cfg.player.teams, ImVec2(235, 25)))
					g_cfg.player.teams = 1;
				ImGui::SameLine();
				if (subtab("local", 2 == g_cfg.player.teams, ImVec2(235, 25)))
					g_cfg.player.teams = 2;

				ImGui::EndGroup();

				beginchild("##subtabs_players_1", { 352,416 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					checkbox(crypt_str("Enabled"), &g_cfg.player.enable);

					if (player == 0 || ENEMY)
					{
						checkbox(crypt_str("OOF arrows"), &g_cfg.player.arrows);
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##arrowscolor"), &g_cfg.player.arrows_color, ALPHA);

						if (g_cfg.player.arrows)
						{
							slider_int(crypt_str("Arrows distance"), &g_cfg.player.distance, 1, 100, NULL, NULL);
							slider_int(crypt_str("Arrows size"), &g_cfg.player.size, 1, 100, NULL, NULL);
						}
					}

					checkbox(crypt_str("Bounding box"), &g_cfg.player.type[player].box);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##boxcolor"), &g_cfg.player.type[player].box_color, ALPHA);

					checkbox(crypt_str("Name"), &g_cfg.player.type[player].name);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##namecolor"), &g_cfg.player.type[player].name_color, ALPHA);

					checkbox(crypt_str("Health bar"), &g_cfg.player.type[player].health);
					checkbox(crypt_str("Override health color"), &g_cfg.player.type[player].custom_health_color);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##healthcolor"), &g_cfg.player.type[player].health_color, ALPHA);

					for (auto i = 0, j = 0; i < ARRAYSIZE(flags); i++)
					{
						if (g_cfg.player.type[player].flags[i])
						{
							if (j)
								preview += crypt_str(", ") + (std::string)flags[i];
							else
								preview = flags[i];

							j++;
						}
					}

					draw_multicombo_1(crypt_str("Flags"), g_cfg.player.type[player].flags, flags, ARRAYSIZE(flags), preview);
					draw_multicombo_1(crypt_str("Weapon"), g_cfg.player.type[player].weapon, weaponplayer, ARRAYSIZE(weaponplayer), preview);


					if (g_cfg.player.type[player].weapon[WEAPON_ICON] || g_cfg.player.type[player].weapon[WEAPON_TEXT])
					{
						ImGui::Text(crypt_str("Color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##weapcolor"), &g_cfg.player.type[player].weapon_color, ALPHA);
					}

					checkbox(crypt_str("Skeleton"), &g_cfg.player.type[player].skeleton);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##skeletoncolor"), &g_cfg.player.type[player].skeleton_color, ALPHA);

					checkbox(crypt_str("Ammo bar"), &g_cfg.player.type[player].ammo);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##ammocolor"), &g_cfg.player.type[player].ammobar_color, ALPHA);

					checkbox(crypt_str("Foot steps"), &g_cfg.player.type[player].footsteps);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##footstepscolor"), &g_cfg.player.type[player].footsteps_color, ALPHA);

					if (g_cfg.player.type[player].footsteps)
					{
						slider_int(crypt_str("Thickness"), &g_cfg.player.type[player].thickness, 1, 10, NULL, NULL);
						slider_int(crypt_str("Radius"), &g_cfg.player.type[player].radius, 50, 500, NULL, NULL);
					}

					if (player == 0 || ENEMY || player == 1 || TEAM)
					{
						checkbox(crypt_str("Snap lines"), &g_cfg.player.type[player].snap_lines);
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##snapcolor"), &g_cfg.player.type[player].snap_lines_color, ALPHA);

						if (player == 0)
						{
							if (g_cfg.ragebot.enable)
							{
								checkbox(crypt_str("Aimbot points"), &g_cfg.player.show_multi_points);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##showmultipointscolor"), &g_cfg.player.show_multi_points_color, ALPHA);
							}

							checkbox(crypt_str("Aimbot hitboxes"), &g_cfg.player.lag_hitbox);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##lagcompcolor"), &g_cfg.player.lag_hitbox_color, ALPHA);
						}
					}
					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_players_2", { 352,305 });
				{
					ImGui::Spacing();
					ImGui::Spacing();


					if (player != 2 || LOCAL || !g_cfg.player.local_chams_type)
						draw_multicombo_1(crypt_str("Chams"), g_cfg.player.type[player].chams, g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] ? chamsvisact : chamsvis, g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] ? ARRAYSIZE(chamsvisact) : ARRAYSIZE(chamsvis), preview);

					if (player == 2 || player == LOCAL)
					{
						combo(crypt_str("Type"), &g_cfg.player.local_chams_type, local_chams_type, ARRAYSIZE(local_chams_type));
					}

					if (g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] || player == 2 || LOCAL && g_cfg.player.local_chams_type) //-V648
					{
						if (player == LOCAL && g_cfg.player.local_chams_type)
						{
							checkbox(crypt_str("Enable desync chams"), &g_cfg.player.fake_chams_enable);
							checkbox(crypt_str("Visualize fake-lag"), &g_cfg.player.visualize_lag);
							checkbox(crypt_str("Layered"), &g_cfg.player.layered);

							combo(crypt_str("Player chams material"), &g_cfg.player.fake_chams_type, chamstype, ARRAYSIZE(chamstype));
							pad(12, 0);
							ImGui::Text(crypt_str("Color "));
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##fakechamscolor"), &g_cfg.player.fake_chams_color, ALPHA);

							if (g_cfg.player.fake_chams_type != 6)
							{
								checkbox(crypt_str("Fake Double material "), &g_cfg.player.fake_double_material);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##doublematerialcolor"), &g_cfg.player.fake_double_material_color, ALPHA);
							}

							checkbox(crypt_str("Animated material"), &g_cfg.player.fake_animated_material);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##animcolormat"), &g_cfg.player.fake_animated_material_color, ALPHA);
						}
						else
						{
							combo(crypt_str("Player chams material"), &g_cfg.player.type[player].chams_type, chamstype, ARRAYSIZE(chamstype));

							if (g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE])
							{
								pad(12, 0);
								ImGui::Text(crypt_str("Visible "));
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##chamsvisible"), &g_cfg.player.type[player].chams_color, ALPHA);
							}

							if (g_cfg.player.type[player].chams[PLAYER_CHAMS_VISIBLE] && g_cfg.player.type[player].chams[PLAYER_CHAMS_INVISIBLE])
							{
								pad(12, 0);
								ImGui::Text(crypt_str("Invisible "));
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##chamsinvisible"), &g_cfg.player.type[player].xqz_color, ALPHA);
							}

							if (g_cfg.player.type[player].chams_type != 6)
							{
								checkbox(crypt_str("Player Double material "), &g_cfg.player.type[player].double_material);
								ImGui::SameLine();
								ImGui::ColorEdit(crypt_str("##doublematerialcolor"), &g_cfg.player.type[player].double_material_color, ALPHA);
							}

							checkbox(crypt_str("Animated material"), &g_cfg.player.type[player].animated_material);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##animcolormat"), &g_cfg.player.type[player].animated_material_color, ALPHA);

							if (player == 0 || ENEMY)
							{
								checkbox(crypt_str("Backtrack chams"), &g_cfg.player.backtrack_chams);

								if (g_cfg.player.backtrack_chams)
								{
									combo(crypt_str("Backtrack chams material"), &g_cfg.player.backtrack_chams_material, chamstype, ARRAYSIZE(chamstype));

									pad(12, 0);
									ImGui::Text(crypt_str("Color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##backtrackcolor"), &g_cfg.player.backtrack_chams_color, ALPHA);
								}
								//checkbox(crypt_str("Chams on shot"), &g_cfg.misc.chams_on_shot_enable);
								//ImGui::ColorEdit(crypt_str("##onshotcolor"), &g_cfg.misc.chams_on_shot_color, ALPHA);
								//if (g_cfg.misc.chams_on_shot_enable)
								//{
									//draw_combo(crypt_str("Player chams material"), g_cfg.misc.chams_on_shot_type, chamstype, ARRAYSIZE(chamstype));

								//}
							}
							if (player == 0 || ENEMY || player == 1 || TEAM)
							{
								checkbox(crypt_str("Ragdoll chams"), &g_cfg.player.type[player].ragdoll_chams);

								if (g_cfg.player.type[player].ragdoll_chams)
								{
									combo(crypt_str("Ragdoll chams material"), &g_cfg.player.type[player].ragdoll_chams_material, chamstype, ARRAYSIZE(chamstype));
									pad(12, 0);
									ImGui::Text(crypt_str("Color "));
									ImGui::SameLine();
									ImGui::ColorEdit(crypt_str("##ragdollcolor"), &g_cfg.player.type[player].ragdoll_chams_color, ALPHA);
								}
							}
							else if (!g_cfg.player.local_chams_type)
							{
								checkbox(crypt_str("Transparency in scope"), &g_cfg.player.transparency_in_scope);

								if (g_cfg.player.transparency_in_scope)
									slider_float(crypt_str("Alpha"), &g_cfg.player.transparency_in_scope_amount, 0.0f, 1.0f, crypt_str("%.f"), NULL);
							}
						}
					}

					checkbox(crypt_str("Arms chams"), &g_cfg.esp.arms_chams);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##armscolor"), &g_cfg.esp.arms_chams_color, ALPHA);

					if (g_cfg.esp.arms_chams)
					{

						combo(crypt_str("Arms chams material"), &g_cfg.esp.arms_chams_type, chamstype, ARRAYSIZE(chamstype));

						if (g_cfg.esp.arms_chams_type != 6)
						{
							checkbox(crypt_str("Arms double material "), &g_cfg.esp.arms_double_material);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##armsdoublematerial"), &g_cfg.esp.arms_double_material_color, ALPHA);
						}

						checkbox(crypt_str("Arms animated material "), &g_cfg.esp.arms_animated_material);
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##armsanimatedmaterial"), &g_cfg.esp.arms_animated_material_color, ALPHA);
					}

					checkbox(crypt_str("Weapon chams"), &g_cfg.esp.weapon_chams);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##weaponchamscolors"), &g_cfg.esp.weapon_chams_color, ALPHA);

					if (g_cfg.esp.weapon_chams)
					{
						combo(crypt_str("Weapon chams material"), &g_cfg.esp.weapon_chams_type, chamstype, ARRAYSIZE(chamstype));

						if (g_cfg.esp.weapon_chams_type != 6)
						{
							checkbox(crypt_str("Weapon Double material "), &g_cfg.esp.weapon_double_material);
							ImGui::SameLine();
							ImGui::ColorEdit(crypt_str("##weapondoublematerial"), &g_cfg.esp.weapon_double_material_color, ALPHA);
						}

						checkbox(crypt_str("Animated material "), &g_cfg.esp.weapon_animated_material);
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##weaponanimatedmaterial"), &g_cfg.esp.weapon_animated_material_color, ALPHA);
						ImGui::Spacing();
					}
					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
				pad(360, -104);
				beginchild("##subtabs_players_3", { 352,100 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					checkbox(crypt_str("Glow"), &g_cfg.player.type[player].glow);

					if (g_cfg.player.type[player].glow)
					{
						combo(crypt_str("Glow type"), &g_cfg.player.type[player].glow_type, glowtype, ARRAYSIZE(glowtype));
						pad(12, 0);
						ImGui::Text(crypt_str("Color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##glowcolor"), &g_cfg.player.type[player].glow_color, ALPHA);
					}

				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}

		if (selectedsubtab == 1)
		{
			ImGui::BeginChild("workzone_visuals_2", ImVec2(838 - 92 - 30, 530 - 72));
			{
				beginchild("##subtabs_visuals_1", { 352,445 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					checkbox(crypt_str("Grenade prediction"), &g_cfg.esp.grenade_prediction);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##grenpredcolor"), &g_cfg.esp.grenade_prediction_color, ALPHA);

					if (g_cfg.esp.grenade_prediction)
					{
						checkbox(crypt_str("On click"), &g_cfg.esp.on_click);
						pad(12, 0);
						ImGui::Text(crypt_str("Tracer color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##tracergrenpredcolor"), &g_cfg.esp.grenade_prediction_tracer_color, ALPHA);
					}

					checkbox(crypt_str("Grenade projectiles"), &g_cfg.esp.projectiles);

					if (g_cfg.esp.projectiles)
						draw_multicombo_1(crypt_str("Grenade ESP"), g_cfg.esp.grenade_esp, proj_combo, ARRAYSIZE(proj_combo), preview);

					if (g_cfg.esp.grenade_esp[GRENADE_ICON] || g_cfg.esp.grenade_esp[GRENADE_TEXT])
					{
						pad(12, 0);
						ImGui::Text(crypt_str("Color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##projectcolor"), &g_cfg.esp.projectiles_color, ALPHA);
					}

					if (g_cfg.esp.grenade_esp[GRENADE_BOX])
					{
						pad(12, 0);
						ImGui::Text(crypt_str("Box color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##grenade_box_color"), &g_cfg.esp.grenade_box_color, ALPHA);
					}

					if (g_cfg.esp.grenade_esp[GRENADE_GLOW])
					{
						pad(12, 0);
						ImGui::Text(crypt_str("Glow color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##grenade_glow_color"), &g_cfg.esp.grenade_glow_color, ALPHA);
					}

					checkbox(crypt_str("Fire range"), &g_cfg.esp.molotov_timer);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##molotovcolor"), &g_cfg.esp.molotov_timer_color, ALPHA);

					checkbox(crypt_str("Smoke timer"), &g_cfg.esp.smoke_timer);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##smokecolor"), &g_cfg.esp.smoke_timer_color, ALPHA);

					draw_multicombo_1(crypt_str("Weapon ESP"), g_cfg.esp.weapon, weaponesp, ARRAYSIZE(weaponesp), preview);

					if (g_cfg.esp.weapon[WEAPON_ICON] || g_cfg.esp.weapon[WEAPON_TEXT] || g_cfg.esp.weapon[WEAPON_DISTANCE])
					{
						pad(12, 0);
						ImGui::Text(crypt_str("Color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##weaponcolor"), &g_cfg.esp.weapon_color, ALPHA);
					}

					if (g_cfg.esp.weapon[WEAPON_BOX])
					{
						pad(12, 0);
						ImGui::Text(crypt_str("Box color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##weaponboxcolor"), &g_cfg.esp.box_color, ALPHA);
					}

					if (g_cfg.esp.weapon[WEAPON_GLOW])
					{
						pad(12, 0);

						ImGui::Text(crypt_str("Glow color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##weaponglowcolor"), &g_cfg.esp.weapon_glow_color, ALPHA);
					}

					if (g_cfg.esp.weapon[WEAPON_AMMO])
					{
						pad(12, 0);
						ImGui::Text(crypt_str("Ammo bar color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##weaponammocolor"), &g_cfg.esp.weapon_ammo_color, ALPHA);
					}

					draw_multicombo_1(crypt_str("Indicators"), g_cfg.esp.indicators, indicators, ARRAYSIZE(indicators), preview);
					checkbox(crypt_str("Keybind list"), &g_cfg.esp.keybind_list);

					draw_multicombo_1(crypt_str("Removals"), g_cfg.esp.removals, removals, ARRAYSIZE(removals), preview);

					if (g_cfg.esp.removals[REMOVALS_ZOOM])
						checkbox(crypt_str("Fix zoom sensivity"), &g_cfg.esp.fix_zoom_sensivity);

					if (g_cfg.esp.removals[REMOVALS_SCOPE])
					{
						combo(crypt_str("Scope drawing type"), &g_cfg.esp.scope_type, scope_drawing_type, ARRAYSIZE(scope_drawing_type));
						pad(12, 0);
						ImGui::Text(crypt_str("Scope color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("###Scope_color"), &g_cfg.esp.new_scope_color, ALPHA);
					}

					checkbox(crypt_str("Client bullet impacts"), &g_cfg.esp.client_bullet_impacts);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##clientbulletimpacts"), &g_cfg.esp.client_bullet_impacts_color, ALPHA);

					checkbox(crypt_str("Server bullet impacts"), &g_cfg.esp.server_bullet_impacts);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##serverbulletimpacts"), &g_cfg.esp.server_bullet_impacts_color, ALPHA);

					checkbox(crypt_str("Local bullet tracers"), &g_cfg.esp.bullet_tracer);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##bulltracecolor"), &g_cfg.esp.bullet_tracer_color, ALPHA);

					checkbox(crypt_str("Enemy bullet tracers"), &g_cfg.esp.enemy_bullet_tracer);
					ImGui::SameLine();

					ImGui::ColorEdit(crypt_str("##enemybulltracecolor"), &g_cfg.esp.enemy_bullet_tracer_color, ALPHA);
					draw_multicombo_1(crypt_str("Hit marker"), g_cfg.esp.hitmarker, hitmarkers, ARRAYSIZE(hitmarkers), preview);
					checkbox(crypt_str("Damage marker"), &g_cfg.esp.damage_marker);
					checkbox(crypt_str("Kill effect"), &g_cfg.esp.kill_effect);

					if (g_cfg.esp.kill_effect)
						slider_float(crypt_str("Duration"), &g_cfg.esp.kill_effect_duration, 0.01f, 3.0f, crypt_str("%.1f"), NULL);

					combo(crypt_str("Hitsound"), &g_cfg.esp.hitsound, sounds, ARRAYSIZE(sounds));
					checkbox(crypt_str("Killsound"), &g_cfg.esp.killsound);

					checkbox(crypt_str("Velocity graph"), &g_cfg.esp.velocity_graph);
					checkbox(crypt_str("Local trails"), &g_cfg.esp.trails);

					checkbox(crypt_str("Taser range"), &g_cfg.esp.taser_range);
					checkbox(crypt_str("Show spread"), &g_cfg.esp.show_spread);
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##spredcolor"), &g_cfg.esp.show_spread_color, ALPHA);
					checkbox(crypt_str("Penetration crosshair"), &g_cfg.esp.penetration_reticle);
					checkbox(crypt_str("3D penetration crosshair"), &g_cfg.esp.penetration_reticle2);
					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_visuals_2", { 352,150 });
				{
					ImGui::Spacing();
					ImGui::Spacing();
					pad(10, 0);

					draw_keybind_1(crypt_str("Force third person"), &g_cfg.misc.thirdperson_toggle, crypt_str("##TPKEY__HOTKEY"));

					checkbox(crypt_str("Third person when dead"), &g_cfg.misc.thirdperson_when_spectating);

					if (g_cfg.misc.thirdperson_toggle.key > KEY_NONE && g_cfg.misc.thirdperson_toggle.key < KEY_MAX)
						slider_int(crypt_str("Third person distance"), &g_cfg.misc.thirdperson_distance, 100, 300, NULL, NULL);

					slider_int(crypt_str("Field of view"), &g_cfg.esp.fov, 0, 89, NULL, NULL);

					slider_int(crypt_str("Viewmodel field of view"), &g_cfg.esp.viewmodel_fov, 0, 89, NULL, NULL);
					slider_int(crypt_str("Viewmodel X"), &g_cfg.esp.viewmodel_x, -50, 50, NULL, NULL);
					slider_int(crypt_str("Viewmodel Y"), &g_cfg.esp.viewmodel_y, -50, 50, NULL, NULL);
					slider_int(crypt_str("Viewmodel Z"), &g_cfg.esp.viewmodel_z, -50, 50, NULL, NULL);
					slider_int(crypt_str("Viewmodel roll"), &g_cfg.esp.viewmodel_roll, -180, 180, NULL, NULL);


					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
				pad(360, -287);
				beginchild("##subtabs_visuals_3", { 352,80 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					//checkbox(crypt_str("Radar"), &g_cfg.misc.ingame_radar);
					//draw_multicombo_1(crypt_str("Draw"), g_cfg.esp.radar_filters, radar_gowno, ARRAYSIZE(radar_gowno), preview);

					checkbox(crypt_str("Enabled"), &g_cfg.misc.ingame_radar);
					checkbox(crypt_str("Draw local"), &g_cfg.radar.render_local);
					checkbox(crypt_str("Draw enemies"), &g_cfg.radar.render_enemy);
					checkbox(crypt_str("Draw teammates"), &g_cfg.radar.render_team);
					checkbox(crypt_str("Draw planted c4"), &g_cfg.radar.render_planted_c4);
					checkbox(crypt_str("Draw dropped c4"), &g_cfg.radar.render_dropped_c4);
					checkbox(crypt_str("Draw health"), &g_cfg.radar.render_health);
				}
				ImGui::EndChild();
				pad(360, 5);
				beginchild("##subtabs_visuals_4", { 352, 194 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					checkbox(crypt_str("Rain"), &g_cfg.esp.rain);
					if (g_cfg.esp.rain == 1)
						checkbox(crypt_str("Thunder sounds"), &g_cfg.esp.thunder);
					checkbox(crypt_str("Full bright"), &g_cfg.esp.bright);

					combo(crypt_str("Skybox"), &g_cfg.esp.skybox, skybox, ARRAYSIZE(skybox));
					pad(12, 0);
					ImGui::Text(crypt_str("Color "));
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##skyboxcolor"), &g_cfg.esp.skybox_color, NOALPHA);

					if (g_cfg.esp.skybox == 21)
					{
						static char sky_custom[64] = "\0";

						if (!g_cfg.esp.custom_skybox.empty())
							strcpy_s(sky_custom, sizeof(sky_custom), g_cfg.esp.custom_skybox.c_str());

						pad(12, 0);
						ImGui::Text(crypt_str("Name"));
						ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

						if (ImGui::InputText(crypt_str("##customsky"), sky_custom, sizeof(sky_custom)))
							g_cfg.esp.custom_skybox = sky_custom;

						ImGui::PopStyleVar();
					}

					checkbox(crypt_str("Color modulation"), &g_cfg.esp.nightmode);

					if (g_cfg.esp.nightmode)
					{
						pad(12, 0);
						ImGui::Text(crypt_str("World color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##worldcolor"), &g_cfg.esp.world_color, ALPHA);
						pad(12, 0);
						ImGui::Text(crypt_str("Props color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##propscolor"), &g_cfg.esp.props_color, ALPHA);
					}

					checkbox(crypt_str("World modulation"), &g_cfg.esp.world_modulation);

					if (g_cfg.esp.world_modulation)
					{
						slider_float(crypt_str("Bloom"), &g_cfg.esp.bloom, 0.0f, 500.0f, crypt_str("%.f"), NULL);
						slider_float(crypt_str("Exposure"), &g_cfg.esp.exposure, 0.0f, 2000.0f, crypt_str("%.f"), NULL);
						slider_float(crypt_str("Ambient"), &g_cfg.esp.ambient, 0.0f, 1500.0f, crypt_str("%.f"), NULL);
					}

					checkbox(crypt_str("Fog modulation"), &g_cfg.esp.fog);

					if (g_cfg.esp.fog)
					{
						slider_int(crypt_str("Distance"), &g_cfg.esp.fog_distance, 0, 2500, NULL, NULL);
						slider_int(crypt_str("Density"), &g_cfg.esp.fog_density, 0, 100, NULL, NULL);

						pad(12, 0);
						ImGui::Text(crypt_str("Color "));
						ImGui::SameLine();
						ImGui::ColorEdit(crypt_str("##fogcolor"), &g_cfg.esp.fog_color, NOALPHA);
					}

					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}


		ImGui::EndChild();
	}

	if (selectedtab == 4)
	{
		static int selectedsubtab = 0;

		ImGui::BeginGroup();
		if (subtab("general", 0 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 0;
		ImGui::SameLine();
		if (subtab("movement", 1 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 1;
		ImGui::SameLine();
		if (subtab("scripting", 2 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 2;
		ImGui::SameLine();
		if (subtab("steam", 3 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 3;
		ImGui::EndGroup();

		ImGui::SetCursorPos({ 15,35 });

		if (selectedsubtab == 0)
		{
			ImGui::BeginChild("workzone_misc", ImVec2(838 - 92 - 30, 530 - 72));
			{


				beginchild("##subtabs_players_1", { 352,441 });
				{
					ImGui::Spacing();
					ImGui::Spacing();


					//tab_start();
					checkbox(crypt_str("Anti-untrusted"), &g_cfg.misc.anti_untrusted);
					checkbox(crypt_str("Watermark"), &g_cfg.menu.watermark);
					checkbox(crypt_str("Spectators list"), &g_cfg.misc.spectators_list);
					checkbox(crypt_str("Block server advertisements"), &g_cfg.misc.block_adv);
					checkbox(crypt_str("Rank reveal"), &g_cfg.misc.rank_reveal);
					checkbox(crypt_str("Unlock inventory access"), &g_cfg.misc.inventory_access);
					checkbox(crypt_str("Autoaccept"), &g_cfg.misc.autoaccept);
					combo(crypt_str("Player model T"), &g_cfg.player.player_model_t, player_model_t, ARRAYSIZE(player_model_t));
					//padding(0, 3);
					combo(crypt_str("Player model CT"), &g_cfg.player.player_model_ct, player_model_ct, ARRAYSIZE(player_model_ct));
					checkbox(crypt_str("Gravity ragdolls"), &g_cfg.misc.ragdolls);
					if (g_cfg.misc.ragdolls)
					{
						slider_int(crypt_str("Gravity x"), &g_cfg.misc.ragdol_force_x, -1000, 1000, "%.0f", NULL);
						slider_int(crypt_str("Gravity y"), &g_cfg.misc.ragdol_force_y, -1000, 1000, "%.0f", NULL);
						slider_int(crypt_str("Gravity boost z"), &g_cfg.misc.ragdol_force, -100, 100, "%.0f", NULL);
					}
					checkbox(crypt_str("Preserve killfeed"), &g_cfg.esp.preserve_killfeed);

					checkbox(crypt_str("Custom textures"), &g_cfg.misc.customchams);
					//ImGui::Text(crypt_str("Color "));
					ImGui::SameLine();
					ImGui::ColorEdit(crypt_str("##weaponcustomskincolor"), &g_cfg.misc.knife_color_custom, ALPHA);
					combo(crypt_str("Custom knife material"), &g_cfg.misc.customknife, knifecustom, ARRAYSIZE(knifecustom));

					checkbox(crypt_str("Aspect ratio"), &g_cfg.misc.aspect_ratio);

					if (g_cfg.misc.aspect_ratio)
					{
						//padding(0, -5);
						slider_float(crypt_str("Amount"), &g_cfg.misc.aspect_ratio_amount, 1.0f, 50.0f, "%.1f", NULL);
					}
					checkbox(crypt_str("Fake prime"), &g_cfg.misc.fake_prime);

					const char* channels_radio[] = { "None" , "Greatest Hits", "Dance Hits", "German Rap", "Chill", "Top 100", "Best German-Rap", "Hip Hop" };


					pad(12, 0);

					ImGui::Text("Radio");
					combo(crypt_str("Station"), &g_cfg.misc.radiochannel, channels_radio, IM_ARRAYSIZE(channels_radio));

					slider_float(("Radio volume"), &g_cfg.misc.radiovolume, 0.f, 100.f, "%.f", NULL);

					combo(crypt_str("Force mm region"), &g_cfg.misc.region_changer, mmregions, ARRAYSIZE(mmregions));
					if (button(crypt_str("Apply region changes"), ImVec2(330, 26)))
					{
						misc::get().ChangeRegion();
					}

					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_misc_2", { 352,441 });
				{
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::PushItemWidth(330);

					static auto should_update = true;

					if (should_update)
					{
						should_update = false;

						cfg_manager->config_files();
						files_1 = cfg_manager->files;

						for (auto& current : files_1)
							if (current.size() > 2)
								current.erase(current.size() - 3, 3);
					}
					if (button(crypt_str("Open configs folder"), ImVec2(330, 26)))
					{
						std::string folder;

						auto get_dir = [&folder]() -> void
						{
							static TCHAR path[MAX_PATH];

							if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
								folder = std::string(path) + crypt_str("\\Lolihook\\Configs\\");

							CreateDirectory(folder.c_str(), NULL);
						};

						get_dir();
						ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
					}

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
					pad(10, 4);
					ImGui::ListBoxConfigArray(crypt_str("##CONFIGS"), &g_cfg.selected_config, files_1, 7);
					ImGui::PopStyleVar();
					if (button(crypt_str("Refresh configs"), ImVec2(330, 26)))
					{
						cfg_manager->config_files();
						files_1 = cfg_manager->files;

						for (auto& current : files_1)
							if (current.size() > 2)
								current.erase(current.size() - 3, 3);
					}

					static char config_name[64] = "\0";

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
					pad(10, 4);
					ImGui::InputText(crypt_str("##configname"), config_name, sizeof(config_name));
					ImGui::PopStyleVar();
					if (button(crypt_str("Create config"), ImVec2(330, 26)))
					{
						g_cfg.new_config_name = config_name;
						add_config_1();
					}

					static auto next_save = false;
					static auto prenext_save = false;
					static auto clicked_sure = false;
					static auto save_time = m_globals()->m_realtime;
					static auto save_alpha = 1.0f;

					save_alpha = math::clamp(save_alpha + (4.f * ImGui::GetIO().DeltaTime * (!prenext_save ? 1.f : -1.f)), 0.01f, 1.f);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 255);

					if (!next_save)
					{
						clicked_sure = false;

						if (prenext_save && save_alpha <= 0.01f)
							next_save = true;
						if (button(crypt_str("Save config"), ImVec2(330, 26)))
						{
							save_time = m_globals()->m_realtime;
							prenext_save = true;
						}
					}
					else
					{
						if (prenext_save && save_alpha <= 0.01f)
						{
							prenext_save = false;
							next_save = !clicked_sure;
						}

						if (button(crypt_str("Are you sure?"), ImVec2(330, 26)))
						{
							save_config_1();
							prenext_save = true;
							clicked_sure = true;
						}

						if (!clicked_sure && m_globals()->m_realtime > save_time + 1.5f)
						{
							prenext_save = true;
							clicked_sure = true;
						}
					}

					ImGui::PopStyleVar();
					if (button(crypt_str("Load config"), ImVec2(330, 26)))
						load_config_1();

					static auto next_delete = false;
					static auto prenext_delete = false;
					static auto clicked_sure_del = false;
					static auto delete_time = m_globals()->m_realtime;
					static auto delete_alpha = 1.0f;

					delete_alpha = math::clamp(delete_alpha + (4.f * ImGui::GetIO().DeltaTime * (!prenext_delete ? 1.f : -1.f)), 0.01f, 1.f);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 255);

					if (!next_delete)
					{
						clicked_sure_del = false;

						if (prenext_delete && delete_alpha <= 0.01f)
							next_delete = true;
						if (button(crypt_str("Remove config"), ImVec2(330, 26)))
						{
							delete_time = m_globals()->m_realtime;
							prenext_delete = true;
						}
					}
					else
					{
						if (prenext_delete && delete_alpha <= 0.01f)
						{
							prenext_delete = false;
							next_delete = !clicked_sure_del;
						}
						if (button(crypt_str("Are you sure?"), ImVec2(330, 26)))
						{
							remove_config_1();
							prenext_delete = true;
							clicked_sure_del = true;
						}

						if (!clicked_sure_del && m_globals()->m_realtime > delete_time + 1.5f)
						{
							prenext_delete = true;
							clicked_sure_del = true;
						}
					}

					ImGui::PopStyleVar();
					ImGui::PopItemWidth();


					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}

		if (selectedsubtab == 1)
		{
			ImGui::BeginChild("workzone_misc_3", ImVec2(838 - 92 - 30, 530 - 72));
			{


				beginchild("##subtabs_misc_3", { 352,441 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					checkbox(crypt_str("Anti-screenshot"), &g_cfg.misc.anti_screenshot);
					checkbox(crypt_str("Clantag"), &g_cfg.misc.clantag_spammer);
					checkbox(crypt_str("Chat spam"), &g_cfg.misc.chat);
					checkbox(crypt_str("Blockbot"), &g_cfg.misc.blockbot_enabled);

					if (g_cfg.misc.blockbot_enabled == 1)
					{
						pad(10, 0);
						draw_keybind_1(crypt_str("Blockbot key"), &g_cfg.misc.block_bot, crypt_str("##BLOCKBOT__HOTKEY"));
						combo(crypt_str("Blockbot type"), &g_cfg.misc.blockbot_type, blockbot_type_cb, ARRAYSIZE(blockbot_type_cb));
					}
					checkbox(crypt_str("Enable buybot"), &g_cfg.misc.buybot_enable);
					if (g_cfg.misc.buybot_enable)
					{
						combo(crypt_str("Snipers"), &g_cfg.misc.buybot1, mainwep, ARRAYSIZE(mainwep));
						combo(crypt_str("Pistols"), &g_cfg.misc.buybot2, secwep, ARRAYSIZE(secwep));
						draw_multicombo_1(crypt_str("Other"), g_cfg.misc.buybot3, grenades, ARRAYSIZE(grenades), preview);
					}

					draw_multicombo_1(crypt_str("Walkbot"), g_cfg.misc.walkbot, walkbot_s, ARRAYSIZE(walkbot_s), preview);
					pad(10, 0);
					draw_keybind_1(crypt_str("Create point"), &g_cfg.misc.walkbot_key, crypt_str("##WALKBOT__HOTKEY"));

					static char addictname[64] = "\0";

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
					pad(10, 0);
					ImGui::InputText(crypt_str("##addictname"), addictname, sizeof(addictname));
					ImGui::PopStyleVar();
					if (button(crypt_str("Save path"), ImVec2(330, 26)))
						walkbot::get().file(addictname, file_overwrite);
					if (button(crypt_str("Load path"), ImVec2(330, 26)))
						walkbot::get().file(addictname, file_load);
					if (button(crypt_str("Clear path"), ImVec2(330, 26)))
						walkbot::get().clear_dots();

					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_misc_4", { 352,441 });
				{

					ImGui::Spacing();
					ImGui::Spacing();

					checkbox(crypt_str("Auto bunnyhop"), &g_cfg.misc.bunnyhop);
					combo(crypt_str("Auto strafer"), &g_cfg.misc.airstrafe, strafes, ARRAYSIZE(strafes));
					checkbox(crypt_str("Crouch in air"), &g_cfg.misc.crouch_in_air);
					checkbox(crypt_str("Fast stop"), &g_cfg.misc.fast_stop);
					checkbox(crypt_str("Slide walk"), &g_cfg.misc.slidewalk);
					checkbox(crypt_str("Infinite crouch stamina"), &g_cfg.misc.noduck);

					if (g_cfg.misc.noduck)
					{
						pad(10, 0);
						draw_keybind_1(crypt_str("Fake duck"), &g_cfg.misc.fakeduck_key, crypt_str("##FAKEDUCK__HOTKEY"));
					}
					pad(10, 0);
					draw_keybind_1(crypt_str("Slow walk"), &g_cfg.misc.slowwalk_key, crypt_str("##SLOWWALK__HOTKEY"));
					pad(10, 0);
					draw_keybind_1(crypt_str("Auto peek"), &g_cfg.misc.automatic_peek, crypt_str("##AUTOPEEK__HOTKEY"));
					pad(10, 0);
					draw_keybind_1(crypt_str("Edge jump"), &g_cfg.misc.edge_jump, crypt_str("##EDGEJUMP__HOTKEY"));
					pad(10, 0);
					draw_keybind_1(crypt_str("Edge bug"), &g_cfg.misc.edgebug, crypt_str("##EDGEBUG__HOTKEY"));
					pad(10, 0);
					draw_keybind_1(crypt_str("Jump bug"), &g_cfg.misc.jumpbug, crypt_str("##JUMPBUG__HOTKEY"));




					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}

		if (selectedsubtab == 3)
		{
			ImGui::BeginChild("workzone_misc_9", ImVec2(838 - 92 - 30, 530 - 72));
			{


				beginchild("##subtabs_misc_8", { 700,441 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					static auto calculateChildWindowPositionDouble = [](int num) -> ImVec2 {
						return ImVec2(ImGui::GetWindowPos().x + 120 + (ImGui::GetWindowSize().x / 2 - 70) * num + 20 * num, ImGui::GetWindowPos().y + 20);
					};

					auto child_size_p = ImVec2(ImGui::GetWindowSize().x - 140, ImGui::GetWindowSize().y / 13.2 + 160);

					auto child_size_a = ImVec2(ImGui::GetWindowSize().x - 140, ImGui::GetWindowSize().y / 13.2 + 115);

					for (static auto i = 0; i < 1; steam_image(), i++);
					static int animation;
					//ImGui::SetNextWindowPos(calculateChildWindowPositionDouble(0));
					//child_title(crypt_str("Main steam profile"));
					ImGui::Spacing();
					//ImGui::BeginChild("steam6s2", { 520, 200 });
					//{


					ImGui::Columns(3, nullptr, false);

					//ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 10, ImGui::GetCursorPosY()));
					ImGui::SetCursorPos({ 0,0 });


					if (!animation)
						ImGui::Image(image, ImVec2(160, 160));
					else
					{

						if (g_ctx.custom_reload && g_ctx.generate_last_number)
							for (auto i = 0; i < 1; load_custom_image(), i++);



						if (g_ctx.custom_reload_picture && g_ctx.generate_last_number && !g_ctx.custom_reload)
							ImGui::Image(custom_image[g_ctx.custom_counter], ImVec2(160, 160));



						g_ctx.custom_reload = false;


					}

					ImGui::NextColumn();

					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 1, ImGui::GetCursorPosY()));
					//ImGui::SetCursorPos({ 20,20 });



					ImGui::Text("Current user: %s\nTime ingame: %f\nTime in real: %s\nTickrate: %d Fps: %d\nServer time: %f\nSteam level: %d\nPrealpha build", SteamFriends->GetPersonaName(), m_enginetool()->Time(), zGetTimeString().c_str(), zgettickrate(), zgetfps(), m_enginetool()->ServerTime(), SteamUser->GetPlayerSteamLevel());
					ImGui::NextColumn();
					ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() - 20, ImGui::GetCursorPosY()));
					//ui::SliderFloat(crypt_str("Field of view amount"), &g_cfg.legitbot.weapon[hooks::legit_weapon].fov, 0.f, 100.f, crypt_str("%.0f"));

					//draw_combo("Type", &animation, { "Steam", "Custom" });
					//draw_combo(crypt_str("Type"), &animation, steam_type, ARRAYSIZE(steam_type));
					combo(crypt_str(""), &animation, steam_type, ARRAYSIZE(steam_type));
					if (animation)
					{
						//ui::SetCursorPos(ImVec2(ui::GetCursorPosX() - 25 - pfix/2 + g_cfg.legitbot.weapon[hooks::legit_weapon].fov, ui::GetCursorPosY()));
						if (button("Reload custom", ImVec2(pfix - 100, 0)))
						{
							g_ctx.custom_reload = true;
							g_ctx.custom_reload_picture = false;
						}

						if (button(crypt_str("Open custom folder"), ImVec2(pfix - 100, 0)))
						{
							std::string folder;

							auto get_dir = [&folder]() -> void
							{
								static TCHAR path[MAX_PATH];

								if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, NULL, path)))
									folder = std::string(path) + crypt_str("\\.base\\custom image\\");

								CreateDirectory(folder.c_str(), NULL);
							};

							get_dir();
							ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
						}
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip("Support jpg arry, like name1,name2,name3...\nUsed free video to jpg converter 30fps");

					}
					//}

					//ImGui::EndChild();

					//child_title(crypt_str("Steam friends"));
					ImGui::Spacing();
					//ImGui::BeginChild("steam6s9", { 520, 200 });
					//{


					enum k_EFriendFlags
					{
						k_EFriendFlagNone = 0x00,
						k_EFriendFlagBlocked = 0x01,
						k_EFriendFlagFriendshipRequested = 0x02,
						k_EFriendFlagImmediate = 0x04,			// "regular" friend
						k_EFriendFlagClanMember = 0x08,
						k_EFriendFlagOnGameServer = 0x10,
						//	k_EFriendFlagHasPlayedWith	= 0x20,
						//	k_EFriendFlagFriendOfFriend	= 0x40,
						k_EFriendFlagRequestingFriendship = 0x80,
						k_EFriendFlagRequestingInfo = 0x100,
						k_EFriendFlagAll = 0xFFFF,
					};
					static auto friends = SteamFriends->GetFriendCount(k_EFriendFlagAll);


					for (static auto i = 0; i < 1; friends_images = new IDirect3DTexture9 * [friends], i++); //creates


					for (static auto i = 0; i < friends; friends_images[i] = steam_image_friends(SteamFriends->GetFriendByIndex(i, k_EFriendFlagAll)), i++);

					for (auto i = 0; i < friends; i++)
					{
						ImGui::Image(friends_images[i], ImVec2(48, 48));
						ImGui::SameLine();
						ImGui::Text("%s Steam level %d", SteamFriends->GetFriendPersonaName(SteamFriends->GetFriendByIndex(i, k_EFriendFlagAll)),
							SteamFriends->GetFriendSteamLevel(SteamFriends->GetFriendByIndex(i, k_EFriendFlagAll)));
					}
					//}


					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}


		if (selectedsubtab == 2)
		{
			ImGui::BeginChild("workzone_misc_4", ImVec2(838 - 92 - 30, 530 - 72));
			{
				beginchild("##subtabs_misc_5", { 352,441 });
				{
					ImGui::Spacing();
					ImGui::Spacing();


					ImGui::PushItemWidth(330);
					static auto should_update = true;

					if (should_update)
					{
						should_update = false;
						scripts_1 = c_lua::get().scripts;

						for (auto& current : scripts_1)
						{
							if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
								current.erase(current.size() - 5, 5);
							else if (current.size() >= 4)
								current.erase(current.size() - 4, 4);
						}
					}
					if (button(crypt_str("Open scripts folder"), ImVec2(330, 26)))
					{
						std::string folder;

						auto get_dir = [&folder]() -> void
						{
							static TCHAR path[MAX_PATH];

							if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, NULL, path)))
								folder = std::string(path) + crypt_str("\\Lolihook\\Scripts\\");

							CreateDirectory(folder.c_str(), NULL);
						};

						get_dir();
						ShellExecute(NULL, crypt_str("open"), folder.c_str(), NULL, NULL, SW_SHOWNORMAL);
					}

					ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);

					if (scripts_1.empty())
					{
						pad(10, 4);
						ImGui::ListBoxConfigArray(crypt_str("##LUAS"), &selected_script_1, scripts_1, 7);
					}
					else
					{
						auto backup_scripts = scripts_1;

						for (auto& script : scripts_1)
						{
							auto script_id = c_lua::get().get_script_id(script + crypt_str(".lua"));

							if (script_id == -1)
								continue;

							if (c_lua::get().loaded.at(script_id))
								scripts_1.at(script_id) += crypt_str(" [loaded]");
						}
						pad(10, 0);
						ImGui::ListBoxConfigArray(crypt_str("##LUAS"), &selected_script_1, scripts_1, 7);
						scripts_1 = std::move(backup_scripts);
					}

					ImGui::PopStyleVar();
					if (button(crypt_str("Refresh scripts"), ImVec2(330, 26)))
					{
						c_lua::get().refresh_scripts();
						scripts_1 = c_lua::get().scripts;

						if (selected_script_1 >= scripts_1.size())
							selected_script_1 = scripts_1.size() - 1; //-V103

						for (auto& current : scripts_1)
						{
							if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
								current.erase(current.size() - 5, 5);
							else if (current.size() >= 4)
								current.erase(current.size() - 4, 4);
						}
					}
					if (button(crypt_str("Edit script"), ImVec2(330, 26)))
					{
						loaded_editing_script_1 = false;
						editing_script_1 = scripts_1.at(selected_script_1);
					}
					if (button(crypt_str("Load script"), ImVec2(330, 26)))
					{
						c_lua::get().load_script(selected_script_1);
						c_lua::get().refresh_scripts();

						scripts_1 = c_lua::get().scripts;

						if (selected_script_1 >= scripts_1.size())
							selected_script_1 = scripts_1.size() - 1; //-V103

						for (auto& current : scripts_1)
						{
							if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
								current.erase(current.size() - 5, 5);
							else if (current.size() >= 4)
								current.erase(current.size() - 4, 4);
						}

						eventlogs::get().add(crypt_str("Loaded ") + scripts_1.at(selected_script_1) + crypt_str(" script"), false); //-V106
					}
					if (button(crypt_str("Unload script"), ImVec2(330, 26)))
					{
						c_lua::get().unload_script(selected_script_1);
						c_lua::get().refresh_scripts();

						scripts_1 = c_lua::get().scripts;

						if (selected_script_1 >= scripts_1.size())
							selected_script_1 = scripts_1.size() - 1; //-V103

						for (auto& current : scripts_1)
						{
							if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
								current.erase(current.size() - 5, 5);
							else if (current.size() >= 4)
								current.erase(current.size() - 4, 4);
						}

						eventlogs::get().add(crypt_str("Unloaded ") + scripts_1.at(selected_script_1) + crypt_str(" script"), false); //-V106
					}
					if (button(crypt_str("Reload all scripts"), ImVec2(330, 26)))
					{
						c_lua::get().reload_all_scripts();
						c_lua::get().refresh_scripts();

						scripts_1 = c_lua::get().scripts;

						if (selected_script_1 >= scripts_1.size())
							selected_script_1 = scripts_1.size() - 1; //-V103

						for (auto& current : scripts_1)
						{
							if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
								current.erase(current.size() - 5, 5);
							else if (current.size() >= 4)
								current.erase(current.size() - 4, 4);
						}
					}
					if (button(crypt_str("Unload all scripts"), ImVec2(330, 26)))
					{
						c_lua::get().unload_all_scripts();
						c_lua::get().refresh_scripts();

						scripts_1 = c_lua::get().scripts;

						if (selected_script_1 >= scripts_1.size())
							selected_script_1 = scripts_1.size() - 1; //-V103

						for (auto& current : scripts_1)
						{
							if (current.size() >= 5 && current.at(current.size() - 1) == 'c')
								current.erase(current.size() - 5, 5);
							else if (current.size() >= 4)
								current.erase(current.size() - 4, 4);
						}
					}

					//ImGui::PopItemWidth();
					//tab_end();


					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_misc_6", { 352,441 });
				{

					ImGui::Spacing();
					ImGui::Spacing();


					checkbox(crypt_str("Developer mode"), &g_cfg.scripts.developer_mode);
					checkbox(crypt_str("Allow HTTP requests"), &g_cfg.scripts.allow_http);
					checkbox(crypt_str("Allow files read or write"), &g_cfg.scripts.allow_file);
					ImGui::Spacing();

					auto previous_check_box = false;

					for (auto& current : c_lua::get().scripts)
					{
						auto& items = c_lua::get().items.at(c_lua::get().get_script_id(current));

						for (auto& item : items)
						{
							std::string item_name;

							auto first_point = false;
							auto item_str = false;

							for (auto& c : item.first)
							{
								if (c == '.')
								{
									if (first_point)
									{
										item_str = true;
										continue;
									}
									else
										first_point = true;
								}

								if (item_str)
									item_name.push_back(c);
							}

							switch (item.second.type)
							{
							case NEXT_LINE:
								previous_check_box = false;
								break;
							case CHECK_BOX:
								previous_check_box = true;
								checkbox(item_name.c_str(), &item.second.check_box_value);
								break;
							case COMBO_BOX:
								previous_check_box = false;
								combo(item_name.c_str(), &item.second.combo_box_value, [](void* data, int idx, const char** out_text)
									{
										auto labels = (std::vector <std::string>*)data;
										*out_text = labels->at(idx).c_str(); //-V106
										return true;
									}, &item.second.combo_box_labels, item.second.combo_box_labels.size());
								break;
							case SLIDER_INT:
								previous_check_box = false;
								slider_int(item_name.c_str(), &item.second.slider_int_value, item.second.slider_int_min, item.second.slider_int_max, NULL, NULL);
								break;
							case SLIDER_FLOAT:
								previous_check_box = false;
								slider_float(item_name.c_str(), &item.second.slider_float_value, item.second.slider_float_min, item.second.slider_float_max, NULL, NULL);
								break;
							case COLOR_PICKER:
								if (previous_check_box)
									previous_check_box = false;
								else
									pad(12, 0);
								ImGui::Text((item_name + ' ').c_str());

								ImGui::SameLine();
								ImGui::ColorEdit((crypt_str("##") + item_name).c_str(), &item.second.color_picker_value, ALPHA, true);
								break;
							}
						}
					}

					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
	}

	if (selectedtab == 5)
	{
		static int selectedsubtab = 0;

		ImGui::BeginGroup();
		if (subtab("inventory", 0 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 0;
		ImGui::SameLine();
		if (subtab("case", 1 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 1;
		ImGui::SameLine();
		if (subtab("profile", 2 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 2;
		ImGui::SameLine();
		if (subtab("medals", 3 == selectedsubtab, ImVec2(180, 25)))
			selectedsubtab = 3;
		//ImGui::SameLine();
		//if (subtab("characters", 4 == selectedsubtab, ImVec2(180, 25)))
			//selectedsubtab = 4;
		ImGui::EndGroup();

		ImGui::SetCursorPos({ 15,35 });

		if (selectedsubtab == 0)
		{
			ImGui::BeginChild("workzone_changers", ImVec2(838 - 92 - 30, 530 - 72));
			{
				static int index = 0;
				static wskin weaponSkin;

				beginchild("##subtabs_changers_1", { 352,441 });
				{
					ImGui::Spacing();
					ImGui::Spacing();



					checkbox("Enable inventory", &g_cfg.inventory.activate_inventory);
					checkbox("Enable autorarity", &g_cfg.inventory.autorarity);

					if (weaponSkin.wId == WEAPON_NONE)
						weaponSkin.wId = WEAPON_DEAGLE;

					if (ImGui::BeginCombo2("Weapon", k_inventory_names.at(weaponSkin.wId), ImGuiComboFlags_HeightLargest, k_inventory_names.size()))
					{
						for (const auto& weapon : k_inventory_names)
						{
							if (ImGui::Selectable(weapon.second, weaponSkin.wId == weapon.first, 0, ImVec2()))
							{
								weaponSkin.wId = weapon.first;
								weaponSkin.paintKit = 0;
								//skinImage = nullptr;
							}
						}
						ImGui::EndCombo();
					}



					auto weaponName = zweaponnames(weaponSkin.wId);
					pad(12, 0);
					ImGui::Text("Skin");

					if (ImGui::BeginCombo2("##Paint Kit", weaponSkin.paintKit > 0 ? _inv.inventory.skinInfo[weaponSkin.paintKit].name.c_str() : "", ImGuiComboFlags_HeightLargest, _inv.inventory.skinInfo.size()))
					{
						int lastID = ImGui::GetItemID();

						for (auto skin : _inv.inventory.skinInfo)
						{
							for (auto names : skin.second.weaponName)
							{
								if (weaponName != names)
									continue;

								ImGui::PushID(lastID++);

								if (ImGui::Selectable(skin.second.name.c_str(), skin.first == weaponSkin.paintKit, 0, ImVec2()))
									weaponSkin.paintKit = skin.first;

								ImGui::PopID();
							}
						}
						ImGui::EndCombo();
					}


					slider_float("Wear", &weaponSkin.wear, 0.f, 1.f, NULL, NULL);
					slider_int("Seed", &weaponSkin.seed, 0, 100, NULL, NULL);

					if (!g_cfg.inventory.autorarity)
						combo(crypt_str("Rarity"), &weaponSkin.quality, skin_rarity, ARRAYSIZE(skin_rarity));


					static char skinname[64] = "\0";

					pad(12, 0);
					ImGui::InputText(("##skinname"), skinname, sizeof(skinname));
					pad(12, 0);
					ImGui::InputInt("Stattrak", &weaponSkin.stattrak);



					static int stickerkit[4] = { 0,0,0,0 };


					if (weaponSkin.wId <= 100 && weaponSkin.wId != 42 && weaponSkin.wId != 59)
					{

						if (ImGui::BeginCombo2("Sticker 1", g_Stickers[stickerkit[0]].name.c_str(), ImGuiComboFlags_HeightLargest, g_Stickers.size()))
						{
							int lastID = ImGui::GetItemID();

							for (auto skin : fosso)
							{
								{
									ImGui::PushID(lastID++);
									if (ImGui::Selectable(skin.second.name.c_str(), skin.second.paintKit == stickerkit[0], 0, ImVec2()))
										stickerkit[0] = skin.second.paintKit;
									ImGui::PopID();
								}
							}
							ImGui::EndCombo();
						}

						if (ImGui::BeginCombo2("Sticker 2", g_Stickers[stickerkit[1]].name.c_str(), ImGuiComboFlags_HeightLargest, g_Stickers.size()))
						{
							int lastID = ImGui::GetItemID();

							for (auto skin : fosso)
							{
								{
									ImGui::PushID(lastID++);
									if (ImGui::Selectable(skin.second.name.c_str(), skin.second.paintKit == stickerkit[1], 0, ImVec2()))
										stickerkit[1] = skin.second.paintKit;
									ImGui::PopID();
								}
							}
							ImGui::EndCombo();
						}

					}



					if (button("Add", ImVec2(ImGui::GetWindowSize().x - 39, 26)))
					{
						weaponSkin.sicker[0] = stickerkit[0];
						weaponSkin.sicker[1] = stickerkit[1];
						weaponSkin.sicker[2] = stickerkit[2];
						weaponSkin.sicker[3] = stickerkit[3];

						std::string str(skinname);
						if (str.length() > 0)
							weaponSkin.name = str;
						g_InventorySkins.insert({ g_csgo.fRandomInt(20000, 200000), weaponSkin });
						_inv.inventory.itemCount = g_InventorySkins.size();
						stickerkit[0] = 0;
						stickerkit[1] = 0;
						stickerkit[2] = 0;
						stickerkit[3] = 0;
						//index = 0;
					}
					if (button("Apply", ImVec2(ImGui::GetWindowSize().x - 39, 26)))
					{
						m_engine()->ExecuteClientCmd("econ_clear_inventory_images");
						write.SendClientHello();
						write.SendMatchmakingClient2GCHello();
					}
					if (g_InventorySkins.size() > 0)
					{
						if (button("Delete selected element", ImVec2(ImGui::GetWindowSize().x - 39, 26)))
						{
							// if (g_InventorySkins[index] != NULL)
							g_InventorySkins.erase(index);
							_inv.inventory.itemCount = g_InventorySkins.size();
						}
					}

					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_changers_2", { 352,441 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					if (weaponSkin.wId > 0 && weaponSkin.wId <= 100)
					{
						std::string name(zweaponnames(weaponSkin.wId));
						std::string smallfix = "weapon_" + name + "_" + _inv.inventory.skinInfo[weaponSkin.paintKit].cdnName;
						auto url = FindURl(smallfix);
						{
							UpdatePic(c_menu::get().device, url);
							if (some_texture_test_1)
							{
								ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 5, ImGui::GetCursorPos().y - 20));
								ImGui::Image(some_texture_test_1, ImVec2(250, 200));
							}

						}
					}
					else
					{
						std::string name(zweaponnames(weaponSkin.wId));
						std::string smallfix = name + "_" + _inv.inventory.skinInfo[weaponSkin.paintKit].cdnName;
						auto url = FindURl(smallfix);
						{
							UpdatePic(c_menu::get().device, url);
							if (some_texture_test_1) {
								//ImGImGui::SetCursorPosY(ImGImGui::GetCursorPosY() - 250);
								ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + 5, ImGui::GetCursorPos().y - 20));
								ImGui::Image(some_texture_test_1, ImVec2(250, 200));
							}

						}
					}
					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}

		if (selectedsubtab == 1)
		{
			ImGui::BeginChild("subtabs_changers_3", ImVec2(838 - 92 - 30, 530 - 72));
			{


				beginchild("##subtabs_changers_4", { 352,441 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					std::call_once(init_lists, [&]() {
						crate_list.init(item_manager::crates);
						key_list.init(item_manager::keys);
						});

					static auto vector_getter_item_definition = [](void* vec, int idx, const char** out_text) {
						auto& vector = *static_cast<std::vector<menu_item_econ_item>*>(vec);
						if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
						*out_text = vector.at(idx).name.c_str();
						return true;
					};

					static auto vector_getter = [](void* vec, int idx, const char** out_text) {
						auto& vector = *static_cast<std::vector<std::string>*>(vec);
						if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
						*out_text = vector.at(idx).c_str();
						return true;
					};

					static auto vector_getter_pair = [](void* vec, int idx, const char** out_text) {
						auto& vector = *static_cast<std::vector<std::pair<int32_t, std::string>>*>(vec);
						if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
						*out_text = vector.at(idx).second.c_str();
						return true;
					};

					//ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.20f, 0.20f, 0.20f, 1.0f));
					//ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

					//ImGui::BeginGroup();

					//ImGui::BeginChild("it3emss", { 500, 530 });
					ImGui::Spacing();
					slider_float("Knife chance (more = better items)", &fakeopening_chances::knife_chance, 0.f, 100.f, NULL, NULL);
					slider_float("Stattrak chance", &fakeopening_chances::statstark_chance, 0.f, 100.f, NULL, NULL);
					slider_float("Factory new wear chance", &fakeopening_chances::factory_new_chance, 0.f, 100.f, NULL, NULL);

					pad(12, 0);
					ImGui::Text("Case creator");

					pad(12, 0);
					if (ImGui::InputText("Filter##crate", crate_list.filter_buffer.data(), crate_list.filter_buffer.size())) {
						crate_list.apply_filter_items();
						update_teams();
					}
					pad(12, 0);
					ImGui::PushItemWidth(330);
					if (ImGui::ListBox("Crates", &crate_list.current_item, vector_getter_item_definition, static_cast<void*>(&crate_list.items_filtered), crate_list.items_filtered.size())) {
						update_teams();
					}
					ImGui::PopItemWidth();
					//ImGImGui::Separator();

					if (crate_list.current_item < (int)crate_list.items_filtered.size()) {
						auto& crate_item = crate_list.items_filtered.at(crate_list.current_item);
						auto attributes_crate = crate_item.item->get_attributes();
						auto tournament_id = std::find(attributes_crate.begin(), attributes_crate.end(), 137);
						auto is_tournament_crate = tournament_id != attributes_crate.end() && !csgo_teams_filtered.empty();

						checkbox("Add key to open this crate", &add_key_to_crate);
						slider_int("Count", &crate_create_size, 1, 50, NULL, NULL);
						checkbox("Add crate/key as unacknowledged", &crate_as_unacknowledged);

						ImGui::PushItemWidth(330);
						if (is_tournament_crate) {
							ImGui::ListBox("Stage", &stage_tournament_crate, vector_getter, static_cast<void*>(&item_manager::tournament_event_stages), item_manager::tournament_event_stages.size()); ImGui::Separator();
							if (ImGui::ListBox("Team 0", &team0_tournament_crate, vector_getter_pair, static_cast<void*>(&csgo_teams_filtered), csgo_teams_filtered.size())) { update_players(tournament_id->value); } ImGui::Separator();
							if (ImGui::ListBox("Team 1", &team1_tournament_crate, vector_getter_pair, static_cast<void*>(&csgo_teams_filtered), csgo_teams_filtered.size())) { update_players(tournament_id->value); } ImGui::Separator();
							ImGui::ListBox("Mvp player", &player_tournament_crate, vector_getter_pair, static_cast<void*>(&csgo_players_filtered), csgo_players_filtered.size()); ImGui::Separator();
						}
						ImGui::PopItemWidth();
						if (button("Create & add to inventory", ImVec2(220, 26))) {
							auto inventory = sdk::inventory_manager->get_local_player_inventory();

							for (auto i = 0; i < crate_create_size; i++) {
								auto last_ids = inventory->get_last_values_ids();
								auto item = c_econ_item::create_econ_item();
								item->get_account_id() = inventory->get_steam_id();
								item->get_def_index() = crate_item.item->get_definition_index();
								item->get_item_id() = last_ids.first + 1;
								item->get_inventory() = crate_as_unacknowledged ? 0 : last_ids.second + 1;
								item->get_flags() = 0;
								item->get_original_id() = 0;

								item->set_origin(8);
								item->set_level(1);
								item->set_in_use(false);

								item->set_quality(ITEM_QUALITY_SKIN);
								item->set_rarity(ITEM_RARITY_DEFAULT);

								if (is_tournament_crate) {
									auto s = stage_tournament_crate + 1;
									auto t0 = csgo_teams_filtered[team0_tournament_crate].first;
									auto t1 = csgo_teams_filtered[team1_tournament_crate].first;
									auto p = csgo_players_filtered[player_tournament_crate].first;
									item->set_attribute_value(138, &s);
									item->set_attribute_value(139, &t0);
									item->set_attribute_value(140, &t1);
									if (p > 0)
										item->set_attribute_value(223, &p);
								}

								for (auto& attr : attributes_crate)
									item->set_attribute_value(attr.id, &attr.value);

								inventory->add_econ_item(item, 1, 0, 1);
							}

							if (add_key_to_crate) {
								auto key_id = crate_item.item->get_associated_items().empty() ? -1 : crate_item.item->get_associated_items().back();
								if (key_id != -1) {
									for (auto i = 0; i < crate_create_size; i++) {
										auto last_ids = inventory->get_last_values_ids();
										auto item = c_econ_item::create_econ_item();
										item->get_account_id() = inventory->get_steam_id();
										item->get_def_index() = key_id;
										item->get_item_id() = last_ids.first + 1;
										item->get_inventory() = crate_as_unacknowledged ? 0 : last_ids.second + 1;
										item->get_flags() = 0;
										item->get_original_id() = 0;

										item->set_origin(8);
										item->set_level(1);
										item->set_in_use(false);

										item->set_quality(ITEM_QUALITY_SKIN);
										item->set_rarity(ITEM_RARITY_DEFAULT);

										inventory->add_econ_item(item, 1, 0, 1);
									}
								}
							}
						}
					}
					pad(12, 0);

					ImGui::Text("Key creator");
					pad(12, 0);
					if (ImGui::InputText("Filter##keys", key_list.filter_buffer.data(), key_list.filter_buffer.size()))
						key_list.apply_filter_items();
					ImGui::PushItemWidth(330);
					ImGui::ListBox("Keys", &key_list.current_item, vector_getter_item_definition, static_cast<void*>(&key_list.items_filtered), key_list.items_filtered.size());
					ImGui::PopItemWidth();
					if (key_list.current_item < (int)key_list.items_filtered.size()) {
						auto& key_item = key_list.items_filtered.at(key_list.current_item);

						//ImGImGui::SliderInt("Count##key", &key_create_size, 1, 50);
						//ImGImGui::Checkbox("Add key as unacknowledged", &key_as_unacknowledged);
						slider_int("Count key", &key_create_size, 1, 50, NULL, NULL);
						checkbox("Add key as unacknowledged", &key_as_unacknowledged);

						if (button("Create & add to inventory##key", ImVec2(220, 26))) {
							auto inventory = sdk::inventory_manager->get_local_player_inventory();
							for (auto i = 0; i < key_create_size; i++) {
								auto last_ids = inventory->get_last_values_ids();
								auto item = c_econ_item::create_econ_item();
								item->get_account_id() = inventory->get_steam_id();
								item->get_def_index() = key_item.item->get_definition_index();
								item->get_item_id() = last_ids.first + 1;
								item->get_inventory() = key_as_unacknowledged ? 0 : last_ids.second + 1;
								item->get_flags() = 0;
								item->get_original_id() = 0;

								item->set_origin(8);
								item->set_level(1);
								item->set_in_use(false);

								item->set_quality(ITEM_QUALITY_SKIN);
								item->set_rarity(ITEM_RARITY_DEFAULT);

								inventory->add_econ_item(item, 1, 0, 1);
							}
						}
					}

					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_changers_5", { 352,441 });
				{

					ImGui::Spacing();
					ImGui::Spacing();



					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}

		if (selectedsubtab == 2)
		{
			ImGui::BeginChild("subtabs_changers_1337", ImVec2(838 - 92 - 30, 530 - 72));
			{


				beginchild("##subtabs_changers_1338", { 352,441 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					checkbox("Enable profile changer", &g_cfg.profilechanger.enable);




					pad(10, 0);
					ImGui::InputInt("Friend", &g_cfg.profilechanger.profile_friend);
					pad(10, 0);
					ImGui::InputInt("Leader", &g_cfg.profilechanger.profile_leader);
					pad(10, 0);
					ImGui::InputInt("Teach", &g_cfg.profilechanger.profile_teach);


					ImGui::SingleSelect("Rank", &g_cfg.profilechanger.profile_rank, {
	"Off",
	"Silver 1",
	"Silver 2",
	"Silver 3",
	"Silver 4",
	"Silver elite",
	"Silver elite master",
	"Gold nova 1",
	"Gold nova 2",
	"Gold nova 3",
	"Gold nova master",
	"Master guardian 1",
	"Master guardian 2",
	"Master guardian elite",
	"Distinguished master guardian",
	"Legendary eagle",
	"Legendary eagle master",
	"Supreme master first class",
	"The global elite"
						});
					pad(10, 0);
					ImGui::InputInt("Wins", &g_cfg.profilechanger.profile_rank_wins);
					slider_int("Level", &g_cfg.profilechanger.profile_lvl, 0, 40, NULL, NULL);
					slider_int("Level xp", &g_cfg.profilechanger.profile_xp, 0, 5000, NULL, NULL);

					if (button("Apply", ImVec2(ImGui::GetWindowSize().x - 39, 26)))
					{
						write.SendClientHello();
						write.SendMatchmakingClient2GCHello();
					}


					ImGui::SingleSelect("Rank wing", &g_cfg.profilechanger.r_rank, {
	"Off",
	"Silver 1",
	"Silver 2",
	"Silver 3",
	"Silver 4",
	"Silver elite",
	"Silver elite master",
	"Gold nova 1",
	"Gold nova 2",
	"Gold nova 3",
	"Gold nova master",
	"Master guardian 1",
	"Master guardian 2",
	"Master guardian elite",
	"Distinguished master guardian",
	"Legendary eagle",
	"Legendary eagle master",
	"Supreme master first class",
	"The global elite"
						});
					pad(10, 0);
					ImGui::InputInt("Wins wing", &g_cfg.profilechanger.r_wins);

					if (button("Apply", ImVec2(ImGui::GetWindowSize().x - 39, 26)))
						write.SendClientGcRankUpdate1();



					ImGui::SingleSelect("Rank zone", &g_cfg.profilechanger.t_rank, {
   "Off",
   "Lab rat 1",
   "Lab rat 2",
   "Sprinting Hare 1",
   "Sprinting Hare 2",
   "Wild Scout 1",
   "Wild Scout Elite",
   "Hunter Fox 1",
   "Hunter Fox 2",
   "Hunter Fox 3",
   "Hunter Fox Elite",
   "Timber Wolf",
   "Ember Wolf",
   "Wildfire Wolf",
   "The Howling Alpha"
						});
					pad(10, 0);
					ImGui::InputInt("Wins zone", &g_cfg.profilechanger.t_wins);

					if (button("Apply", ImVec2(ImGui::GetWindowSize().x - 39, 26)))
						write.SendClientGcRankUpdate2();

					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_changers_1339", { 352,441 });
				{

					ImGui::Spacing();
					ImGui::Spacing();



					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}

		if (selectedsubtab == 3)
		{
			ImGui::BeginChild("subtabs_changers_1340", ImVec2(838 - 92 - 30, 530 - 72));
			{


				beginchild("##subtabs_changers_1341", { 352,441 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					static int paintKit = 874;
					static int wId = 0;

					pad(10, 0);
					ImGui::Text("Medal skin");

					if (ImGui::BeginCombo2("##PaintKit_yea", fosso[paintKit].name.c_str(), ImGuiComboFlags_HeightLargest, fosso.size()))
					{
						int lastID = ImGui::GetItemID();

						for (auto skin : fosso)
						{
							{
								ImGui::PushID(lastID++);
								if (ImGui::Selectable(skin.second.name.c_str(), skin.second.paintKit == paintKit, 0, ImVec2()))
									paintKit = skin.second.paintKit;
								ImGui::PopID();
							}
						}
						ImGui::EndCombo();
					}
					if (button("Add medal", ImVec2(ImGui::GetWindowSize().x - 39, 26)))
						g_MedalSkins[g_csgo.fRandomInt(200001, 1000000)] = { paintKit , 0 };



					if (button("Apply medals", ImVec2(ImGui::GetWindowSize().x - 39, 26)))
					{
						paintKit;
						m_engine()->ExecuteClientCmd("econ_clear_inventory_images");
						write.SendClientHello();
						write.SendMatchmakingClient2GCHello();
					}


					if (g_MedalSkins.size() > 0)
					{
						if (button("Delete selected element", ImVec2(ImGui::GetWindowSize().x - 39, 26)))
							g_MedalSkins.erase(wId);

					}

					//}
					//ImGui::EndChild();

					//ImGui::SameLine(0, 25);

					//ImGui::BeginChild("Medals list", ImVec2(328, 460)); {

					int i = 0;
					for (auto weapon : g_MedalSkins) {

						std::stringstream whatever;
						whatever << ++i;
						if (ImGui::Selectable(std::string(whatever.str() + " " + fosso[weapon.second.paintKit].name).c_str(), wId == weapon.first)) {
							wId = weapon.first;
						}
					}
					i = 0;

					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
				ImGui::SameLine();
				beginchild("##subtabs_changers_1342", { 352,441 });
				{

					ImGui::Spacing();
					ImGui::Spacing();



					ImGui::Spacing();
					ImGui::Spacing();
				}
				ImGui::EndChild();
			}
			ImGui::EndChild();
		}
		ImGui::EndChild();
	}

}


class dot
{
public:
	dot(SourceEngine::Vector p, SourceEngine::Vector v) {
		m_vel = v;
		m_pos = p;
	}

	void update();
	void draw();

	SourceEngine::Vector m_pos, m_vel;
};

std::vector<dot*> dots = { };

void dot::update() {
	auto opacity = 240 / 255.0f;

	m_pos += m_vel * (opacity);
}

void dot::draw() {
	int opacity = 55.0f * (240 / 255.0f);

	ImGui::GetWindowDrawList()->AddRectFilled({ m_pos.x - 2, m_pos.y - 2 }, { m_pos.x + 2, m_pos.y + 2 }, ImColor(9, 169, 232, 195));

	auto t = std::find(dots.begin(), dots.end(), this);
	if (t == dots.end()) {
		return;
	}

	for (auto i = t; i != dots.end(); i++) {
		if ((*i) == this) continue;

		auto dist = (m_pos - (*i)->m_pos).Length2D();

		if (dist < 128) {
			int alpha = opacity * (dist / 128);
			ImGui::GetWindowDrawList()->AddLine({ m_pos.x - 1, m_pos.y - 2 }, { (*i)->m_pos.x - 2, (*i)->m_pos.y - 1 }, ImColor(9, 169, 232, 195));
		}
	}
}

void dot_draw() {
	struct screen_size {
		int x, y;
	}; screen_size sc;

	sc.x = 1920, sc.y = 1080;

	int s = rand() % 12;

	if (s == 0) {
		dots.push_back(new dot(SourceEngine::Vector(rand() % (int)sc.x, -16, 0), SourceEngine::Vector((rand() % 7) - 3, rand() % 3 + 1, 0)));
	}
	else if (s == 1) {
		dots.push_back(new dot(SourceEngine::Vector(rand() % (int)sc.x, (int)sc.y + 16, 0), SourceEngine::Vector((rand() % 7) - 3, -1 * (rand() % 3 + 1), 0)));
	}
	else if (s == 2) {
		dots.push_back(new dot(SourceEngine::Vector(-16, rand() % (int)sc.y, 0), SourceEngine::Vector(rand() % 3 + 1, (rand() % 7) - 3, 0)));
	}
	else if (s == 3) {
		dots.push_back(new dot(SourceEngine::Vector((int)sc.x + 16, rand() % (int)sc.y, 0), SourceEngine::Vector(-1 * (rand() % 3 + 1), (rand() % 7) - 3, 0)));
	}

	auto alph = 135.0f * (240 / 255.0f);
	auto a_int = (int)(alph);

	ImGui::GetWindowDrawList()->AddRectFilled({ 0, 0 }, { (float)sc.x, (float)sc.y }, ImColor(a_int, 0, 0, 0));

	for (auto i = dots.begin(); i < dots.end();) {
		if ((*i)->m_pos.y < -20 || (*i)->m_pos.y > sc.y + 20 || (*i)->m_pos.x < -20 || (*i)->m_pos.x > sc.x + 20) {
			delete (*i);
			i = dots.erase(i);
		}
		else {
			(*i)->update();
			i++;
		}
	}

	for (auto i = dots.begin(); i < dots.end(); i++) {
		(*i)->draw();
	}
}

void dot_destroy() {
	for (auto i = dots.begin(); i < dots.end(); i++) {
		delete (*i);
	}

	dots.clear();
}

void c_menu2::render_menu(bool is_opened)
{
	if (is_opened)
	{
		ImGui::SetNextWindowPos({ 0,0 });
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0,0,0,0 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::Begin("##menu2", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoMove);
		{
			//dot_draw();
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();

		if (is_opened && alphamodulate <= 1.f)
			alphamodulate += 0.05f;
		else if (!is_opened && alphamodulate >= 0.f)
		{
			alphamodulate -= 0.05f;
		}

		auto flags = ImGuiWindowFlags_NoTitleBar | NULL | NULL | NULL | ImGuiWindowFlags_NoResize | NULL | NULL | NULL | NULL;

		ImGui::SetNextWindowSize({ 845.000000f,560.000000f });


		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alphamodulate);
		ImGui::Begin("Edited", nullptr, flags);
		{
			p = ImGui::GetWindowPos();
			draw = ImGui::GetWindowDrawList();

			render_decorations();
			render_tabs();
			render_content();

			draw->AddText({ p.x + 7, p.y + 8 }, ImColor(255, 0, 255, int(230 * alphamodulate)), "L O L I H O O K . T O P");
		}
		ImGui::End();
		ImGui::PopStyleVar(1);

		shadow(ImGui::GetOverlayDrawList(), p, { 845.000000f,560.000000f }, ImVec4(255 / 255.F, 0 / 255.F, 255 / 255.F, 0.25f * alphamodulate));
	}
}