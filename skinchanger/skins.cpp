
#include "../includes.hpp"

#include <Windows.h>
#include <WinInet.h>
#include "skins.h"
#include "../inventory/items.h"
#include "parser.h"
#include <array>
#include "../inventory/inventorychanger.h"

Skins g_Skins;
struct skin_info
{
	int seed = -1;
	int paintkit;
	int rarity = 0;
	std::string tag_name;
	std::string cdn_name;
};

#pragma comment(lib, "Wininet")

CreateClientClassFn GetWearableCreateFn()
{
	auto clazz = m_client()->GetAllClasses();
	// Please, if you gonna paste it into a cheat use classids here. I use names because they
	// won't change in the foreseeable future and i dont need high speed, but chances are
	// you already have classids, so use them instead, they are faster.
	while (strcmp(clazz->m_pNetworkName, "CEconWearable"))
		clazz = clazz->m_pNext;
	return clazz->m_pCreateFn;
}

const weapon_info* GetWeaponInfo(const short& defindex)
{
	const auto entry = k_weapon_info.find(defindex);

	return entry == k_weapon_info.end() ? nullptr : &entry->second;
}

void Skins::EraseOverrideIfExistsByIndex(const short definition_index)
{
	if (const auto original_item = GetWeaponInfo(definition_index))
	{
		if (!original_item->icon)
			return;

		const auto override_entry = m_icon_overrides.find(original_item->icon);
		if (override_entry != std::end(m_icon_overrides))
			m_icon_overrides.erase(override_entry);
	}
}

const char* Skins::GetIconOverride(const std::string& original)
{
	return m_icon_overrides.count(original) > 0 ? m_icon_overrides.at(original).data() : nullptr;
}

int GetWeaponRarity(std::string rarity)
{
	if (rarity == "default")
		return 0;
	else if (rarity == "common")
		return 1;
	else if (rarity == "uncommon")
		return 2;
	else if (rarity == "rare")
		return 3;
	else if (rarity == "mythical")
		return 4;
	else if (rarity == "legendary")
		return 5;
	else if (rarity == "ancient")
		return 6;
	else if (rarity == "immortal")
		return 7;
	else if (rarity == "unusual")
		return 99;

	return 0;
}



void ParseMedals()
{
	valve_parser::Document items_document;
	bool item_document_loaded = items_document.Load(".\\csgo\\scripts\\items\\items_game.txt", valve_parser::ENCODING::UTF8);
	if (!item_document_loaded)
		return;

	auto items = items_document.BreadthFirstSearch("items");
	if (!items || !items->ToObject())
		return;

	int i = 0;

	for (auto child : items->children)
	{
		if (child->ToObject())
		{
			std::string object_id = child->ToObject()->name.toString();

			if ((atoi(object_id.c_str()) >= 874) && (atoi(object_id.c_str()) <= 6133))
			{
				std::string object_prefab = child->ToObject()->GetKeyByName("prefab")->ToKeyValue()->Value.toString();

				if ((object_prefab != "collectible_untradable") && (object_prefab != "operation_coin") && (object_prefab != "prestige_coin")
					&& (object_prefab != "attendance_pin") && (object_prefab != "commodity_pin") && (object_prefab != "season1_coin")
					&& (object_prefab != "season2_coin") && (object_prefab != "season3_coin") && (object_prefab != "season4_coin")
					&& (object_prefab != "season5_coin") && (object_prefab != "season6_coin") && (object_prefab != "season7_coin")
					&& (object_prefab != "season8_coin")) continue;

				std::string object_name = child->ToObject()->GetKeyByName("name")->ToKeyValue()->Value.toString();
				
				
				fosso[atoi(object_id.c_str())] = { object_name, atoi(object_id.c_str()) };

				i++;
				if ((atoi(object_id.c_str()) == 6133))
					break;
			}
		}
	}
	
	//for (size_t i = 0; i < G::MedalMap.size(); i++)
	//{
	//	G::MedalList.push_back(G::MedalMap.at(i).medal_name.c_str());
	//}
}

namespace fzdetail
{
	constexpr auto process_pattern_byte(const std::pair<char, char> pair) -> std::pair<std::uint8_t, bool>
	{
		return { std::uint8_t(pair.first), pair.second == 'x' };
	}

	template <std::size_t N>
	constexpr auto select_pattern_byte(const char(&pattern)[N], const char(&mask)[N], std::size_t n) -> std::pair<char, char>
	{
		return { pattern[n], mask[n] };
	}

	template <typename>
	struct sig_processor {};

	template <std::size_t... Indices>
	struct sig_processor<std::index_sequence<Indices...>>
	{
		template <std::size_t N>
		static constexpr auto process(const char(&pattern)[N], const char(&mask)[N])->std::array<std::pair<std::uint8_t, bool>, N>
		{
			return { process_pattern_byte(select_pattern_byte(pattern, mask, Indices))... };
		}
	};
}

auto get_module_info(const char* module_name) -> std::pair<std::uintptr_t, std::size_t>
{
	const auto module = GetModuleHandleA(module_name);
	if (!module)
		return { 0, 0 };
	MODULEINFO module_info;
	K32GetModuleInformation(GetCurrentProcess(), module, &module_info, sizeof(MODULEINFO));
	return { std::uintptr_t(module_info.lpBaseOfDll), module_info.SizeOfImage };
}



template <std::size_t N>
auto find_pattern(const char* module_name, const char(&pattern)[N], const char(&mask)[N]) -> std::uintptr_t
{
	const std::array<std::pair<std::uint8_t, bool>, N> signature = fzdetail::sig_processor<std::make_index_sequence<N>>::process(pattern, mask);

	const auto info = get_module_info(module_name);
	const auto address = reinterpret_cast<std::uint8_t*>(info.first);
	const auto size = info.second;

	const auto ret = std::search(address, address + size, signature.begin(), signature.end(),
		[](std::uint8_t curr, std::pair<std::uint8_t, bool> curr_pattern)
		{
			return (!curr_pattern.second) || curr == curr_pattern.first;
		});

	return ret == address + size ? 0 : std::uintptr_t(ret);
}

template <typename Key, typename Value>
struct Node_t
{
	int previous_id;		//0x0000
	int next_id;			//0x0004
	void* _unknown_ptr;		//0x0008
	int _unknown;			//0x000C
	Key key;				//0x0010
	Value value;			//0x0014
};


template <typename Key, typename Value>
struct Head_t
{
	Node_t<Key, Value>* memory;		//0x0000
	int allocation_count;			//0x0004
	int grow_size;					//0x0008
	int start_element;				//0x000C
	int next_available;				//0x0010
	int _unknown;					//0x0014
	int last_element;				//0x0018
}; //Size=0x001C

// could use CUtlString but this is just easier and CUtlString isn't needed anywhere else
struct String_t
{
	char* buffer;	//0x0000
	int capacity;	//0x0004
	int grow_size;	//0x0008
	int length;		//0x000C
}; //Size=0x0010

class CCStrike15ItemSystem;
class CCStrike15ItemSchema;
struct CPaintKit
{
	int id;						//0x0000

	String_t name;				//0x0004
	String_t description;		//0x0014
	String_t item_name;			//0x0024
	String_t material_name;		//0x0034
	String_t image_inventory;	//0x0044

	char pad_0x0054[0x8C];		//0x0054
}; //Size=0x00E0

struct CStickerKit
{
	int id;

	int item_rarity;

	String_t name;
	String_t description;
	String_t item_name;
	String_t material_name;
	String_t image_inventory;

	int tournament_event_id;
	int tournament_team_id;
	int tournament_player_id;
	bool is_custom_sticker_material;

	float rotate_end;
	float rotate_start;

	float scale_min;
	float scale_max;

	float wear_min;
	float wear_max;

	String_t image_inventory2;
	String_t image_inventory_large;

	std::uint32_t pad0[4];
};
auto zget_export(const char* module_name, const char* export_name) -> void*
{
	HMODULE mod;
	while (!((mod = GetModuleHandleA(module_name))))
		Sleep(100);

	return reinterpret_cast<void*>(GetProcAddress(mod, export_name));
}


#include "enrage parser, thanks!/parserz.hpp"

void Skins::ParseSkins() const
{

	parser::parse();



	const auto V_UCS2ToUTF8 = static_cast<int(*)(const wchar_t* ucs2, char* utf8, int len)>(zget_export("vstdlib.dll", "V_UCS2ToUTF8"));


	const auto sig_address = find_pattern("client.dll", "\xE8\x00\x00\x00\x00\xFF\x76\x0C\x8D\x48\x04\xE8", "x????xxxxxxx");

	// Skip the opcode, read rel32 address
	const auto item_system_offset = *reinterpret_cast<std::int32_t*>(sig_address + 1);

	// Add the offset to the end of the instruction
	const auto item_system_fn = reinterpret_cast<CCStrike15ItemSystem * (*)()>(sig_address + 5 + item_system_offset);

	// Skip VTable, first member variable of ItemSystem is ItemSchema
	const auto item_schema = reinterpret_cast<CCStrike15ItemSchema*>(std::uintptr_t(item_system_fn()) + sizeof(void*));


	// Dump sticker kits
	{
		const auto sticker_sig = find_pattern("client.dll", "\x53\x8D\x48\x04\xE8\x00\x00\x00\x00\x8B\x4D\x10", "xxxxx????xxx") + 4;

		// Skip the opcode, read rel32 address
		const auto get_sticker_kit_definition_offset = *reinterpret_cast<std::intptr_t*>(sticker_sig + 1);

		// Add the offset to the end of the instruction
		const auto get_sticker_kit_definition_fn = reinterpret_cast<CPaintKit * (__thiscall*)(CCStrike15ItemSchema*, int)>(sticker_sig + 5 + get_sticker_kit_definition_offset);

		// The last offset is head_element, we need that

		//	push    ebp
		//	mov     ebp, esp
		//	push    ebx
		//	push    esi
		//	push    edi
		//	mov     edi, ecx
		//	mov     eax, [edi + 2BCh]

		// Skip instructions, skip opcode, read offset
		const auto start_element_offset = *reinterpret_cast<intptr_t*>(std::uintptr_t(get_sticker_kit_definition_fn) + 8 + 2);

		// Calculate head base from start_element's offset
		const auto head_offset = start_element_offset - 12;

		const auto map_head = reinterpret_cast<Head_t<int, CStickerKit*>*>(std::uintptr_t(item_schema) + head_offset);

		for (auto i = 0; i <= map_head->last_element; ++i)
		{
			const auto sticker_kit = map_head->memory[i].value;

			char sticker_name_if_valve_fucked_up_their_translations[64];

			auto sticker_name_ptr = sticker_kit->item_name.buffer + 1;

			if (strstr(sticker_name_ptr, "StickerKit_dhw2014_dignitas"))
			{
				strcpy_s(sticker_name_if_valve_fucked_up_their_translations, "StickerKit_dhw2014_teamdignitas");
				strcat_s(sticker_name_if_valve_fucked_up_their_translations, sticker_name_ptr + 27);
				sticker_name_ptr = sticker_name_if_valve_fucked_up_their_translations;
			}
			
			const auto wide_name = m_localize()->Find(sticker_name_ptr);
			char name[256];
			V_UCS2ToUTF8(wide_name, name, sizeof(name));

			//game_data::sticker_kits.push_back({ sticker_kit->id, name });
			
			g_Stickers[sticker_kit->id] = { name, sticker_kit->id };
		}

		//std::sort(game_data::sticker_kits.begin(), game_data::sticker_kits.end());
		g_Stickers[0] = { "None", 0 };
		//game_data::sticker_kits.insert(game_data::sticker_kits.begin(), { 0, "None" });
	}














	ParseMedals();

	std::unordered_map<std::string, std::set<std::string>> weaponSkins;
	std::unordered_map<std::string, skin_info> skinMap;
	std::unordered_map<std::string, std::string> skinNames;

	valve_parser::Document doc;
	auto r = doc.Load(R"(.\csgo\scripts\items\items_game.txt)", valve_parser::ENCODING::UTF8);
	if (!r)
		return;

	valve_parser::Document english;
	r = english.Load(R"(.\csgo\resource\csgo_english.txt)", valve_parser::ENCODING::UTF16_LE);
	if (!r)
		return;

	auto weapon_skin_combo = doc.BreadthFirstSearch("weapon_icons");
	if (!weapon_skin_combo || !weapon_skin_combo->ToObject())
		return;

	auto paint_kits_rarity = doc.BreadthFirstSearchMultiple("paint_kits_rarity");
	if (paint_kits_rarity.empty())
		return;

	auto skin_data_vec = doc.BreadthFirstSearchMultiple("paint_kits");
	if (skin_data_vec.empty())
		return;

	auto paint_kit_names = english.BreadthFirstSearch("Tokens");
	if (!paint_kit_names || !paint_kit_names->ToObject())
		return;

	std::array weaponNames = {
		std::string("deagle"),
		std::string("elite"),
		std::string("fiveseven"),
		std::string("glock"),
		std::string("ak47"),
		std::string("aug"),
		std::string("awp"),
		std::string("famas"),
		std::string("g3sg1"),
		std::string("galilar"),
		std::string("m249"),
		std::string("m4a1_silencer"),
		std::string("m4a1"),
		std::string("mac10"),
		std::string("p90"),
		std::string("ump45"),
		std::string("xm1014"),
		std::string("bizon"),
		std::string("mag7"),
		std::string("negev"),
		std::string("sawedoff"),
		std::string("tec9"),
		std::string("hkp2000"),
		std::string("mp5sd"),
		std::string("mp7"),
		std::string("mp9"),
		std::string("nova"),
		std::string("p250"),
		std::string("scar20"),
		std::string("sg556"),
		std::string("ssg08"),
		std::string("usp_silencer"),
		std::string("cz75a"),
		std::string("revolver"),
		std::string("knife_m9_bayonet"),
		std::string("bayonet"),
		std::string("knife_flip"),
		std::string("knife_gut"),
		std::string("knife_karambit"),
		std::string("knife_tactical"),
		std::string("knife_falchion"),
		std::string("knife_survival_bowie"),
		std::string("knife_butterfly"),
		std::string("knife_push"),
		std::string("knife_ursus"),
		std::string("knife_gypsy_jackknife"),
		std::string("knife_stiletto"),
		std::string("knife_widowmaker"),
		std::string("knife_skeleton"),
		std::string("knife_outdoor"),
		std::string("knife_canis"),
		std::string("knife_cord"),
		std::string("knife_css"),
		std::string("studded_bloodhound_gloves"),
		std::string("sporty_gloves"),
		std::string("slick_gloves"),
		std::string("leather_handwraps"),
		std::string("motorcycle_gloves"),
		std::string("specialist_gloves"),
		std::string("studded_brokenfang_gloves"),
		std::string("studded_hydra_gloves"),
		
		//brokenfang_gloves
	};
	
	for (const auto& child : weapon_skin_combo->children)
	{
		if (child->ToObject())
		{
			for (const auto& weapon : weaponNames)
			{
				auto skin_name = child->ToObject()->GetKeyByName("icon_path")->ToKeyValue()->Value.toString();
				const auto pos = skin_name.find(weapon);
				if (pos != std::string::npos)
				{
					const auto pos2 = skin_name.find_last_of('_');
					weaponSkins[weapon].insert(
						skin_name.substr(pos + weapon.length() + 1,
							pos2 - pos - weapon.length() - 1)
					);
					break;
				}
			}
		}
	}

	for (const auto& skin_data : skin_data_vec)
	{
		if (skin_data->ToObject())
		{
			for (const auto& skin : skin_data->children)
			{
				if (skin->ToObject())
				{
					skin_info si;
					si.paintkit = skin->ToObject()->name.toInt();

					if (si.paintkit == 0)
						continue;

					auto skin_name = skin->ToObject()->GetKeyByName("name")->ToKeyValue()->Value.toString();
					si.cdn_name = skin_name;
					auto tag_node = skin->ToObject()->GetKeyByName("description_tag");
					if (tag_node)
					{
						auto tag = tag_node->ToKeyValue()->Value.toString();
						tag = tag.substr(1, std::string::npos);
						std::transform(tag.begin(), tag.end(), tag.begin(), towlower);
						si.tag_name = tag;
					}



					auto key_val = skin->ToObject()->GetKeyByName("seed");
					if (key_val != nullptr)
						si.seed = key_val->ToKeyValue()->Value.toInt();

					skinMap[skin_name] = si;
				}
			}
		}
	}

	for (const auto& child : paint_kit_names->children)
	{
		if (child->ToKeyValue())
		{
			auto key = child->ToKeyValue()->Key.toString();
			std::transform(key.begin(), key.end(), key.begin(), towlower);
			if (key.find("paintkit") != std::string::npos &&
				key.find("tag") != std::string::npos)
			{
				skinNames[key] = child->ToKeyValue()->Value.toString();
			}
		}
	}

	for (const auto& rarity : paint_kits_rarity)
	{
		if (rarity->ToObject())
		{
			for (const auto& child : rarity->children)
			{
				if (child->ToKeyValue())
				{
					std::string paint_kit_name = child->ToKeyValue()->Key.toString();
					std::string paint_kit_rarity = child->ToKeyValue()->Value.toString();

					auto skinInfo = &skinMap[paint_kit_name];

					skinInfo->rarity = GetWeaponRarity(paint_kit_rarity);
				}
			}
		}
	}

	for (auto weapon : weaponNames)
	{
		for (auto skin : weaponSkins[weapon])
		{
			skinInfo* info = &_inv.inventory.skinInfo[skinMap[skin].paintkit];
			info->weaponName.push_back(weapon);
			info->cdnName = skin;
			info->name = skinNames[skinMap[skin].tag_name].c_str();
			info->rarity = skinMap[skin].rarity;
			_inv.inventory.skinInfo.insert({ skinMap[skin].paintkit, *info });
		}
	}
}

static auto zis_knife(const int i) -> bool
{
	return (i >= 500 && i < 550) || i == 59 || i == 42;
}
static auto zis_glove(const int i) -> bool
{
	return (i >= 5027 && i <= 5035 || i == 4725); //hehe boys
}







enum class EStickerAttributeType
{
	Index,
	Wear,
	Scale,
	Rotation
};

static uint16_t s_iwoff = 0;

static void* o_uint_fn;

static unsigned int __fastcall hooked_uint_fn(void* thisptr, void*, int slot, EStickerAttributeType attribute, unsigned fl)
{
	weapon_t* item = reinterpret_cast<weapon_t*>(uintptr_t(thisptr) - s_iwoff);



	for (auto& wweap : g_InventorySkins)
	{
		if ((!wweap.second.in_use_ct && !wweap.second.in_use_t) && item->m_iItemDefinitionIndex() != WEAPON_KNIFE && item->m_iItemDefinitionIndex() != WEAPON_KNIFE_T)
			continue;

		if (wweap.second.wId != item->m_iItemDefinitionIndex())
			continue;

		if (!wweap.second.in_use_t && g_ctx.local()->m_iTeamNum() == 2)
			continue;

		if (!wweap.second.in_use_ct && g_ctx.local()->m_iTeamNum() == 3)
			continue;


		if (attribute == EStickerAttributeType::Index)
		{
			switch (slot)
			{
			case 0:
				return wweap.second.sicker[0];
				break;
			case 1:
				return wweap.second.sicker[1];
				break;
			case 2:
				return wweap.second.sicker[2];
				break;
			case 3:
				return wweap.second.sicker[3];
				break;
			default:
				break;
			}
		}


	}



	

	return reinterpret_cast<decltype(hooked_uint_fn)*>(o_uint_fn)(thisptr, nullptr, slot, attribute, fl);
}





void ApplyStickerHooks(weapon_t* item)
{
	if (!s_iwoff)
		s_iwoff = 0x00002DC0 + 0xC; //m_Item

	void**& iw_vt = *reinterpret_cast<void***>(uintptr_t(item) + s_iwoff);

	static void** iw_hook_vt = nullptr;

	if (!iw_hook_vt)
	{
		size_t len = 0;
		for (; iw_vt[len]; ++len);
		iw_hook_vt = new void* [len];

		memcpy(iw_hook_vt, iw_vt, len * sizeof(void*));

		o_uint_fn = iw_hook_vt[5];
		iw_hook_vt[5] = &hooked_uint_fn;
	}

	iw_vt = iw_hook_vt;
}










bool Skins::ApplyCustomSkin(player_t* localPlayer, weapon_t* pWeapon, short nWeaponIndex)
{
	switch (nWeaponIndex)
	{
	case WEAPON_KNIFE_BAYONET:
	case WEAPON_KNIFE_BUTTERFLY:
	case WEAPON_KNIFE_FALCHION:
	case WEAPON_KNIFE_FLIP:
	case WEAPON_KNIFE_GUT:
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
	case WEAPON_KNIFE_KARAMBIT:
	case WEAPON_KNIFE_M9_BAYONET:
	case WEAPON_KNIFE_PUSH:
	case WEAPON_KNIFE_STILETTO:
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
	case WEAPON_KNIFE_TACTICAL:
	case WEAPON_KNIFE_URSUS:
	case WEAPON_KNIFE_WIDOWMAKER:
	case WEAPON_KNIFE_SKELETON:
	case WEAPON_KNIFE_OUTDOOR:
	case WEAPON_KNIFE_CANIS:
	case WEAPON_KNIFE_CORD:
	case WEAPON_KNIFE_CSS:
	case WEAPON_KNIFE_T:
	case WEAPON_KNIFE:
		nWeaponIndex = WEAPON_KNIFE;
	}
	

	for (auto& wweap : g_InventorySkins)
	{
		if ((!wweap.second.in_use_ct && !wweap.second.in_use_t))
			continue;

		if (wweap.second.wId != nWeaponIndex)
			continue;

		if (!wweap.second.in_use_t && localPlayer->m_iTeamNum() == 2)
			continue;

		if (!wweap.second.in_use_ct && localPlayer->m_iTeamNum() == 3)
			continue;


		
		if (wweap.second.sicker[0] || wweap.second.sicker[1] || wweap.second.sicker[2] || wweap.second.sicker[3])
			ApplyStickerHooks(pWeapon);


		pWeapon->m_nFallbackPaintKit() = wweap.second.paintKit;
		pWeapon->m_iEntityQuality() = wweap.second.quality;
		pWeapon->m_nFallbackSeed() = wweap.second.seed;
		pWeapon->m_nFallbackStatTrak() = wweap.second.stattrak;
#undef max
		pWeapon->m_flFallbackWear() = std::max(wweap.second.wear, 0.00001f);

		if (wweap.second.name.length())
			strcpy(pWeapon->m_szCustomName(), wweap.second.name.c_str());
		pWeapon->m_iItemIDHigh() = -1;
		

	}
	
	for (auto& wweap : g_InventorySkins)
	{
		if ((!wweap.second.in_use_ct && !wweap.second.in_use_t))
			continue;

		if (zis_knife(wweap.second.wId) && zis_knife(nWeaponIndex))
		{

			if (!wweap.second.in_use_t && localPlayer->m_iTeamNum() == 2)
				continue;

			if (!wweap.second.in_use_ct && localPlayer->m_iTeamNum() == 3)
				continue;


			pWeapon->m_nFallbackPaintKit() = wweap.second.paintKit;
			pWeapon->m_iEntityQuality() = wweap.second.quality;
			pWeapon->m_nFallbackSeed() = wweap.second.seed;
			pWeapon->m_nFallbackStatTrak() = wweap.second.stattrak;
			pWeapon->m_flFallbackWear() = std::max(wweap.second.wear, 0.00001f);

			if (wweap.second.name.length())
				strcpy(pWeapon->m_szCustomName(), wweap.second.name.c_str());


			pWeapon->m_iItemIDHigh() = -1;

			if (wweap.second.wId != WEAPON_NONE && zis_knife(wweap.second.wId)) {
				static auto old_definition_index = pWeapon->m_iItemDefinitionIndex();

				pWeapon->m_iItemDefinitionIndex() = wweap.second.wId;
				pWeapon->m_iEntityQuality() = 3;
				const auto& replacement_item = k_weapon_info.at(pWeapon->m_iItemDefinitionIndex());
				skinfix(m_modelinfo()->GetModelIndex(replacement_item.model));
				pWeapon->set_model_index(m_modelinfo()->GetModelIndex(replacement_item.model));
				auto worldmodel_handle = pWeapon->m_hWeaponWorldModel();
				if (worldmodel_handle) {
					worldmodel_handle->set_model_index(m_modelinfo()->GetModelIndex(replacement_item.model) + 1);
				}
				iconfix(replacement_item.icon);
				pWeapon->PreDataUpdate(DATA_UPDATE_CREATED);

				
			}

		}

		
	}


	
	


	


	return true;
}














static auto fnEquip
= reinterpret_cast<int(__thiscall*)(void*, void*)>(
	util::FindSignature("client.dll", "55 8B EC 83 EC 10 53 8B 5D 08 57 8B F9")
	);

static auto fnInitializeAttributes
= reinterpret_cast<int(__thiscall*)(void*)>(
	util::FindSignature("client.dll", "55 8B EC 83 E4 F8 83 EC 0C 53 56 8B F1 8B 86")
	);


void Skins::FrameStageNotify(bool frame_end)
{
	
	if (!g_ctx.local())
		return;
	
	player_info_t info;
	m_engine()->GetPlayerInfo(m_engine()->GetLocalPlayer(), &info);
	const auto wearables = g_ctx.local()->m_hMyWearables();



	/*enum ClassID
	{
		CAI_BaseNPC,
		CAK47,
		CBaseAnimating,
		CBaseAnimatingOverlay,
		CBaseAttributableItem,
		CBaseButton,
		CBaseCombatCharacter,
		CBaseCombatWeapon,
		CBaseCSGrenade,
		CBaseCSGrenadeProjectile,
		CBaseDoor,
		CBaseEntity,
		CBaseFlex,
		CBaseGrenade,
		CBaseParticleEntity,
		CBasePlayer,
		CBasePropDoor,
		CBaseTeamObjectiveResource,
		CBaseTempEntity,
		CBaseToggle,
		CBaseTrigger,
		CBaseViewModel,
		CBaseVPhysicsTrigger,
		CBaseWeaponWorldModel,
		CBeam,
		CBeamSpotlight,
		CBoneFollower,
		CBRC4Target,
		CBreachCharge,
		CBreachChargeProjectile,
		CBreakableProp,
		CBreakableSurface,
		CBumpMine,
		CBumpMineProjectile,
		CC4,
		CCascadeLight,
		CChicken,
		CColorCorrection,
		CColorCorrectionVolume,
		CCSGameRulesProxy,
		CCSPlayer,
		CCSPlayerResource,
		CCSRagdoll,
		CCSTeam,
		CDangerZone,
		CDangerZoneController,
		CDEagle,
		CDecoyGrenade,
		CDecoyProjectile,
		CDrone,
		CDronegun,
		CDynamicLight,
		CDynamicProp,
		CEconEntity,
		CEconWearable,
		CEmbers,
		CEntityDissolve,
		CEntityFlame,
		CEntityFreezing,
		CEntityParticleTrail,
		CEnvAmbientLight,
		CEnvDetailController,
		CEnvDOFController,
		CEnvGasCanister,
		CEnvParticleScript,
		CEnvProjectedTexture,
		CEnvQuadraticBeam,
		CEnvScreenEffect,
		CEnvScreenOverlay,
		CEnvTonemapController,
		CEnvWind,
		CFEPlayerDecal,
		CFireCrackerBlast,
		CFireSmoke,
		CFireTrail,
		CFish,
		CFists,
		CFlashbang,
		CFogController,
		CFootstepControl,
		CFunc_Dust,
		CFunc_LOD,
		CFuncAreaPortalWindow,
		CFuncBrush,
		CFuncConveyor,
		CFuncLadder,
		CFuncMonitor,
		CFuncMoveLinear,
		CFuncOccluder,
		CFuncReflectiveGlass,
		CFuncRotating,
		CFuncSmokeVolume,
		CFuncTrackTrain,
		CGameRulesProxy,
		CGrassBurn,
		CHandleTest,
		CHEGrenade,
		CHostage,
		CHostageCarriableProp,
		CIncendiaryGrenade,
		CInferno,
		CInfoLadderDismount,
		CInfoMapRegion,
		CInfoOverlayAccessor,
		CItem_Healthshot,
		CItemCash,
		CItemDogtags,
		CKnife,
		CKnifeGG,
		CLightGlow,
		CMapVetoPickController,
		CMaterialModifyControl,
		CMelee,
		CMolotovGrenade,
		CMolotovProjectile,
		CMovieDisplay,
		CParadropChopper,
		CParticleFire,
		CParticlePerformanceMonitor,
		CParticleSystem,
		CPhysBox,
		CPhysBoxMultiplayer,
		CPhysicsProp,
		CPhysicsPropMultiplayer,
		CPhysMagnet,
		CPhysPropAmmoBox,
		CPhysPropLootCrate,
		CPhysPropRadarJammer,
		CPhysPropWeaponUpgrade,
		CPlantedC4,
		CPlasma,
		CPlayerPing,
		CPlayerResource,
		CPointCamera,
		CPointCommentaryNode,
		CPointWorldText,
		CPoseController,
		CPostProcessController,
		CPrecipitation,
		CPrecipitationBlocker,
		CPredictedViewModel,
		CProp_Hallucination,
		CPropCounter,
		CPropDoorRotating,
		CPropJeep,
		CPropVehicleDriveable,
		CRagdollManager,
		CRagdollProp,
		CRagdollPropAttached,
		CRopeKeyframe,
		CSCAR17,
		CSceneEntity,
		CSensorGrenade,
		CSensorGrenadeProjectile,
		CShadowControl,
		CSlideshowDisplay,
		CSmokeGrenade,
		CSmokeGrenadeProjectile,
		CSmokeStack,
		CSnowball,
		CSnowballPile,
		CSnowballProjectile,
		CSpatialEntity,
		CSpotlightEnd,
		CSprite,
		CSpriteOriented,
		CSpriteTrail,
		CStatueProp,
		CSteamJet,
		CSun,
		CSunlightShadowControl,
		CSurvivalSpawnChopper,
		CTablet,
		CTeam,
		CTeamplayRoundBasedRulesProxy,
		CTEArmorRicochet,
		CTEBaseBeam,
		CTEBeamEntPoint,
		CTEBeamEnts,
		CTEBeamFollow,
		CTEBeamLaser,
		CTEBeamPoints,
		CTEBeamRing,
		CTEBeamRingPoint,
		CTEBeamSpline,
		CTEBloodSprite,
		CTEBloodStream,
		CTEBreakModel,
		CTEBSPDecal,
		CTEBubbles,
		CTEBubbleTrail,
		CTEClientProjectile,
		CTEDecal,
		CTEDust,
		CTEDynamicLight,
		CTEEffectDispatch,
		CTEEnergySplash,
		CTEExplosion,
		CTEFireBullets,
		CTEFizz,
		CTEFootprintDecal,
		CTEFoundryHelpers,
		CTEGaussExplosion,
		CTEGlowSprite,
		CTEImpact,
		CTEKillPlayerAttachments,
		CTELargeFunnel,
		CTEMetalSparks,
		CTEMuzzleFlash,
		CTEParticleSystem,
		CTEPhysicsProp,
		CTEPlantBomb,
		CTEPlayerAnimEvent,
		CTEPlayerDecal,
		CTEProjectedDecal,
		CTERadioIcon,
		CTEShatterSurface,
		CTEShowLine,
		CTesla,
		CTESmoke,
		CTESparks,
		CTESprite,
		CTESpriteSpray,
		CTest_ProxyToggle_Networkable,
		CTestTraceline,
		CTEWorldDecal,
		CTriggerPlayerMovement,
		CTriggerSoundOperator,
		CVGuiScreen,
		CVoteController,
		CWaterBullet,
		CWaterLODControl,
		CWeaponAug,
		CWeaponAWP,
		CWeaponBaseItem,
		CWeaponBizon,
		CWeaponCSBase,
		CWeaponCSBaseGun,
		CWeaponCycler,
		CWeaponElite,
		CWeaponFamas,
		CWeaponFiveSeven,
		CWeaponG3SG1,
		CWeaponGalil,
		CWeaponGalilAR,
		CWeaponGlock,
		CWeaponHKP2000,
		CWeaponM249,
		CWeaponM3,
		CWeaponM4A1,
		CWeaponMAC10,
		CWeaponMag7,
		CWeaponMP5Navy,
		CWeaponMP7,
		CWeaponMP9,
		CWeaponNegev,
		CWeaponNOVA,
		CWeaponP228,
		CWeaponP250,
		CWeaponP90,
		CWeaponSawedoff,
		CWeaponSCAR20,
		CWeaponScout,
		CWeaponSG550,
		CWeaponSG552,
		CWeaponSG556,
		CWeaponShield,
		CWeaponSSG08,
		CWeaponTaser,
		CWeaponTec9,
		CWeaponTMP,
		CWeaponUMP45,
		CWeaponUSP,
		CWeaponXM1014,
		CWeaponZoneRepulsor,
		CWorld,
		CWorldVguiText,
		DustTrail,
		MovieExplosion,
		ParticleSmokeGrenade,
		RocketTrail,
		SmokeTrail,
		SporeExplosion,
		SporeTrail
	};



	player_info_t LocalPlayerInfo;

	auto hWearables = g_cl.m_local->m_hMyWearables();
	if (hWearables)
	{
		Weapon* pWearable = (Weapon*)g_csgo.m_entlist->GetClientEntityFromHandle(hWearables[0]);
		if (!pWearable)
		{
			int iEntry = g_csgo.m_entlist->GetHighestEntityIndex() + 1;

			for (int i = 0; i < g_csgo.m_entlist->GetHighestEntityIndex(); i++)
			{
				auto pEntity = g_csgo.m_entlist->GetClientEntity(i);

				if (pEntity && pEntity->GetClientClass()->m_ClassID == CRopeKeyframe)
				{
					iEntry = i;
					break;
				}
			}

			for (ClientClass* pClass = g_csgo.m_client->GetAllClasses(); pClass; pClass = pClass->m_pNext)
			{
				if (pClass->m_ClassID != CEconWearable)
					continue;

				int	iSerial = g_csgo.RandomInt(0x0, 0xFFF);

				pClass->m_pCreate(iEntry, iSerial);
				hWearables[0] = iEntry | iSerial << 16;

				break;
			}


			pWearable = (Weapon*)g_csgo.m_entlist->GetClientEntity(iEntry);
			if (pWearable)
			{


				for (auto& wglove : g_InventorySkins)
				{



					if (!zis_glove(wglove.second.wId))
						continue;

					if (!wglove.second.in_use_t && g_cl.m_local->m_iTeamNum() == 2)
						continue;

					if (!wglove.second.in_use_ct && g_cl.m_local->m_iTeamNum() == 3)
						continue;

					g_csgo.m_engine->GetPlayerInfo(g_csgo.m_engine->GetLocalPlayer(), &LocalPlayerInfo);

					pWearable->m_iItemIDHigh() = -1;
					pWearable->m_iItemDefinitionIndex() = wglove.second.wId; 
					pWearable->m_flFallbackWear() = wglove.second.wear;
					pWearable->m_nFallbackSeed() = wglove.second.seed;
					pWearable->m_nFallbackPaintKit() = wglove.second.paintKit;
					pWearable->m_iEntityQuality() = 4;
					pWearable->m_iAccountID() = LocalPlayerInfo.m_xuid;
					const auto& replacement_item = k_weapon_info.at(wglove.second.wId);
					pWearable->SetModelIndex(g_csgo.m_model_info->GetModelIndex(replacement_item.model) + 2);
					fnEquip(pWearable, g_cl.m_local);
					g_cl.m_local->m_nBody() = 1;
					fnInitializeAttributes(pWearable);
					g_pIClientLeafSystem->CreateRenderableHandle(pWearable);

				}

			}
		}
	}*/
    
	if (frame_end && !m_entitylist()->GetClientEntityFromHandle(wearables[0]))
	{
		static auto gloveHandle = CBaseHandle(0);
		auto glove = reinterpret_cast<weapon_t*>(m_entitylist()->GetClientEntityFromHandle(wearables[0]));
		
		if (!glove)
		{
			const auto ourGlove = (weapon_t*)m_entitylist()->GetClientEntityFromHandle(gloveHandle);
			if (ourGlove)
			{
				wearables[0] = gloveHandle;
				glove = ourGlove;
			}
		}
		
		if (!g_ctx.local()->is_alive())
		{
			if (glove)
			{
				glove->SetDestroyedOnRecreateEntities();
				glove->Release();
			}
			return;
		}
		
		
		for (auto& wglove : g_InventorySkins)
		{
		
		
			if (!zis_glove(wglove.second.wId))
				continue;
		
			if (!wglove.second.in_use_t && g_ctx.local()->m_iTeamNum() == 2)
				continue;
		
			if (!wglove.second.in_use_ct && g_ctx.local()->m_iTeamNum() == 3)
				continue;
		
		
			static int whatever_fix;
			if (!glove)
			{
				static auto create_wearable_fn = GetWearableCreateFn();
				const auto entry = m_entitylist()->GetHighestEntityIndex() + 1;
				const auto serial = rand() % 0x1000;
		
				create_wearable_fn(entry, serial);
				glove = reinterpret_cast<weapon_t*>(m_entitylist()->GetClientEntity(entry));
				whatever_fix = entry;
				glove->set_abs_origin({ 10000.f, 10000.f, 10000.f });
				const auto wearable_handle = reinterpret_cast<CBaseHandle*>(&wearables[0]);
				*wearable_handle = entry | serial << 16;
				gloveHandle = wearables[0];
		
			}
		
			if (glove)
			{
				glove->m_iItemDefinitionIndex() = wglove.second.wId;
				glove->m_iItemIDHigh() = -1;
				glove->m_iEntityQuality() = 4;
				glove->m_iAccountID() = info.steamID64;
				glove->m_nFallbackSeed() = wglove.second.seed;
				glove->m_nFallbackStatTrak() = -1;
				glove->m_flFallbackWear() = wglove.second.wear;
				glove->m_nFallbackPaintKit() = wglove.second.paintKit;
				const auto& replacement_item = k_weapon_info.at(glove->m_iItemDefinitionIndex());
				glove->set_model_index(m_modelinfo()->GetModelIndex(replacement_item.model) + 2 /*enjoy nopaste things*/);
	
				fnEquip(glove, g_ctx.local()); //follow entity, owner etc.
				g_ctx.local()->m_nBody() = 1; //remove default arms in 3th person mode
	
				g_pIClientLeafSystem->CreateRenderableHandle(glove); //render our glove in 3th person mode
	
	
				glove->PreDataUpdate(DataUpdateType_t::DATA_UPDATE_CREATED);
		
			}
		
		}
		
	}
	//else
	{
		const auto weapons = g_ctx.local()->m_hMyWeapons();
		if (!weapons)
			return;
		for (auto i = 0; weapons[i].IsValid(); i++)
		{
			auto weapon = static_cast<weapon_t*>(m_entitylist()->GetClientEntityFromHandle(weapons[i]));
			if (!weapon)
				continue;
			const auto weaponIndex = weapon->m_iItemDefinitionIndex();
			//weapon->m_OriginalOwnerXuidLow() = info.m_xuid_low;
			//weapon->m_OriginalOwnerXuidHigh() = info.m_xuid_high;
			ApplyCustomSkin(g_ctx.local(), weapon, weaponIndex);
			//apply_sticker_changer(weapon);

			weapon->m_iAccountID() = info.steamID64;
			
		}
		
		

	}
	

	

}











