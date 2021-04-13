#include "../includes.hpp"
#include "inventorychanger.h"
#include "../protobuffs/Protobuffs.h"
#include "../protobuffs/ProtoWriter.h"
#include "../protobuffs/Messages.h"
#include "../skinchanger/enrage parser, thanks!/parserz.hpp"







static void apply_medals(ProtoWriter& object)
{
	uint32_t steamid = SteamUser->GetSteamID().GetAccountID();

	for (auto MedalIndex : g_MedalSkins)
	{

		ProtoWriter medal(19);
		medal.add(Field(CSOEconItem::account_id, TYPE_UINT32, (int64_t)steamid));
		medal.add(Field(CSOEconItem::origin, TYPE_UINT32, (int64_t)9));
		medal.add(Field(CSOEconItem::rarity, TYPE_UINT32, (int64_t)6));
		medal.add(Field(CSOEconItem::quantity, TYPE_UINT32, (int64_t)1));
		medal.add(Field(CSOEconItem::quality, TYPE_UINT32, (int64_t)4));
		medal.add(Field(CSOEconItem::level, TYPE_UINT32, (int64_t)1));




		auto _time = (uint32_t)std::time(0);
		auto _timeval = std::string{ reinterpret_cast<const char*>((void*)&_time), 4 };
		ProtoWriter Time(3);
		Time.add(Field(CSOEconItemAttribute::def_index, TYPE_UINT32, (int64_t)222));
		Time.add(Field(CSOEconItemAttribute::value_bytes, TYPE_STRING, _timeval));
		medal.add(Field(CSOEconItem::attribute, TYPE_STRING, Time.serialize()));




		medal.add(Field(CSOEconItem::def_index, TYPE_UINT64, (int64_t)MedalIndex.second.paintKit));
		medal.add(Field(CSOEconItem::inventory, TYPE_UINT64, (int64_t)MedalIndex.first));
		medal.add(Field(CSOEconItem::id, TYPE_UINT64, (int64_t)MedalIndex.first));
		if (MedalIndex.second.equip_state == 1)
		{
			ProtoWriter eqip_state(3);
			eqip_state.add(Field(CSOEconItemEquipped::new_class, TYPE_UINT64, (int64_t)(0)));
			eqip_state.add(Field(CSOEconItemEquipped::new_slot, TYPE_UINT64, (int64_t)(55)));
			medal.add(Field(CSOEconItem::equipped_state, TYPE_STRING, eqip_state.serialize()));
		}


		object.add(Field(SubscribedType::object_data, TYPE_STRING, medal.serialize()));
	}


	

}








std::string Inventory::Changer(void* pubDest, uint32_t* pcubMsgSize)
{
	ProtoWriter msg((void*)((DWORD)pubDest + 8), *pcubMsgSize - 8, 11);
	if (msg.getAll(CMsgClientWelcome::outofdate_subscribed_caches).empty())
		return msg.serialize();

	ProtoWriter cache(msg.get(CMsgClientWelcome::outofdate_subscribed_caches).String(), 4);
	// If not have items in inventory, Create null inventory
	FixNullInventory(cache);
	// Add custom items
	auto objects = cache.getAll(CMsgSOCacheSubscribed::objects);
	for (size_t i = 0; i < objects.size(); i++)
	{
		ProtoWriter object(objects[i].String(), 2);

		if (!object.has(SubscribedType::type_id))
			continue;

		if (object.get(SubscribedType::type_id).Int32() == 1)
		{
			object.clear(SubscribedType::object_data);

			ClearEquipState(object);
			apply_medals(object);
			AddAllItems(object);
			
			cache.replace(Field(CMsgSOCacheSubscribed::objects, TYPE_STRING, object.serialize()), i);
		}
	}
	msg.replace(Field(CMsgClientWelcome::outofdate_subscribed_caches, TYPE_STRING, cache.serialize()), 0);

	return msg.serialize();
}

void Inventory::FixNullInventory(ProtoWriter& cache)
{
	bool inventory_exist = false;
	auto objects = cache.getAll(CMsgSOCacheSubscribed::objects);
	for (size_t i = 0; i < objects.size(); i++)
	{
		ProtoWriter object(objects[i].String(), 2);
		if (!object.has(SubscribedType::type_id))
			continue;
		if (object.get(SubscribedType::type_id).Int32() != 1)
			continue;
		inventory_exist = true;
		break;
	}
	if (!inventory_exist)
	{
		ProtoWriter null_object(2);
		null_object.add(Field(SubscribedType::type_id, TYPE_INT32, (int64_t)1));

		cache.add(Field(CMsgSOCacheSubscribed::objects, TYPE_STRING, null_object.serialize()));
	}
}

void Inventory::ClearEquipState(ProtoWriter& object)
{
	auto object_data = object.getAll(SubscribedType::object_data);
	for (size_t j = 0; j < object_data.size(); j++)
	{
		ProtoWriter item(object_data[j].String(), 19);

		if (item.getAll(CSOEconItem::equipped_state).empty())
			continue;

		// create NOT equiped state for item 
		ProtoWriter null_equipped_state(2);
		null_equipped_state.replace(Field(CSOEconItemEquipped::new_class, TYPE_UINT32, (int64_t)0));
		null_equipped_state.replace(Field(CSOEconItemEquipped::new_slot, TYPE_UINT32, (int64_t)0));
		// unequip all 
		auto equipped_state = item.getAll(CSOEconItem::equipped_state);
		for (size_t k = 0; k < equipped_state.size(); k++)
			item.replace(Field(CSOEconItem::equipped_state, TYPE_STRING, null_equipped_state.serialize()), k);

		object.replace(Field(SubscribedType::object_data, TYPE_STRING, item.serialize()), j);
	}
}

static auto fis_knife(const int i) -> bool
{
	return (i >= 500 && i < 5027) || i == 59 || i == 42;
}

static auto fis_glove(const int i) -> bool
{
	return (i >= 5027 && i <= 5035 || i == 4725); //hehe boys
}

void Inventory::AddAllItems(ProtoWriter& object)
{
	for (auto& x : g_InventorySkins)
	{
		if (x.second.in_use_ct && x.second.in_use_t)
			AddItem(object, x.first, x.second.wId, x.second.quality, x.second.paintKit, x.second.seed, x.second.wear, x.second.name, x.second.sicker, 4);
		else if (x.second.in_use_ct)
			AddItem(object, x.first, x.second.wId, x.second.quality, x.second.paintKit, x.second.seed, x.second.wear, x.second.name, x.second.sicker, 3);
		else if (x.second.in_use_t)
			AddItem(object, x.first, x.second.wId, x.second.quality, x.second.paintKit, x.second.seed, x.second.wear, x.second.name, x.second.sicker, 2);
		else
			AddItem(object, x.first, x.second.wId, x.second.quality, x.second.paintKit, x.second.seed, x.second.wear, x.second.name, x.second.sicker );
		//write.EquipWeapon(x.second.wId, _inv.inventory.skinInfo[x.second.paintKit].rarity, Protobuffs::GetSlotID(x.second.wId));
	}
		
}

bool is_uncommon(int index)
{
	switch (index)
	{
	case WEAPON_DEAGLE:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AWP:
	case WEAPON_M4A1:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_HKP2000:
	case WEAPON_USP_SILENCER:
		return true;
	default:
		return false;
	}
}







void Inventory::AddItem(ProtoWriter& object, int index, int itemIndex, int rarity, int paintKit, int seed, float wear, std::string name, int ctickers[4], int inusefix)
{
	uint32_t steamid = SteamUser->GetSteamID().GetAccountID();
	if (!steamid)
		return;
	ProtoWriter item(19);
	item.add(Field(CSOEconItem::id, TYPE_UINT64, (int64_t)index));
	item.add(Field(CSOEconItem::account_id, TYPE_UINT32, (int64_t)steamid));
	item.add(Field(CSOEconItem::def_index, TYPE_UINT32, (int64_t)itemIndex));
	item.add(Field(CSOEconItem::inventory, TYPE_UINT32, (int64_t)index));
	item.add(Field(CSOEconItem::origin, TYPE_UINT32, (int64_t)24));
	item.add(Field(CSOEconItem::quantity, TYPE_UINT32, (int64_t)1));
	item.add(Field(CSOEconItem::level, TYPE_UINT32, (int64_t)1));
	item.add(Field(CSOEconItem::style, TYPE_UINT32, (int64_t)0));
	item.add(Field(CSOEconItem::flags, TYPE_UINT32, (int64_t)0));
	item.add(Field(CSOEconItem::in_use, TYPE_BOOL, (int64_t)false));
	item.add(Field(CSOEconItem::original_id, TYPE_UINT64, (int64_t)itemIndex));
	if (is_uncommon(itemIndex))
		rarity++;
	if (fis_knife(itemIndex) || fis_glove(itemIndex))
	{
		item.add(Field(CSOEconItem::quality, TYPE_UINT32, (int64_t)3));
		rarity = 6;
	}
	if (paintKit == 309)
		item.add(Field(CSOEconItem::rarity, TYPE_UINT32, (int64_t)7));
	else
		 if (itemIndex == 4725)
			 item.add(Field(CSOEconItem::rarity, TYPE_UINT32, (int64_t)7));
		 else
			 if (g_cfg.inventory.autorarity && (!fis_knife(itemIndex) && !fis_glove(itemIndex)))
				 item.add(Field(CSOEconItem::rarity, TYPE_UINT32, (int64_t)rarity_skins[paintKit].rarity));
			 else
				item.add(Field(CSOEconItem::rarity, TYPE_UINT32, (int64_t)rarity));




	if (name.length() > 0)
		item.add(Field(CSOEconItem::custom_name, TYPE_STRING, name));


	enum TeamID : int
	{
		TEAM_UNASSIGNED,
		TEAM_SPECTATOR,
		TEAM_TERRORIST,
		TEAM_COUNTER_TERRORIST,
	};

	
	if (inusefix == 4)
	{
		ProtoWriter eqip_state(3);
		eqip_state.add(Field(CSOEconItemEquipped::new_class, TYPE_UINT64, (int64_t)(inusefix - 1)));
		eqip_state.add(Field(CSOEconItemEquipped::new_slot, TYPE_UINT64, (int64_t)(Protobuffs::GetSlotID(itemIndex))));
		item.add(Field(CSOEconItem::equipped_state, TYPE_STRING, eqip_state.serialize()));

		ProtoWriter zeqip_state(3);
		zeqip_state.add(Field(CSOEconItemEquipped::new_class, TYPE_UINT64, (int64_t)(inusefix - 2)));
		zeqip_state.add(Field(CSOEconItemEquipped::new_slot, TYPE_UINT64, (int64_t)(Protobuffs::GetSlotID(itemIndex))));
		item.add(Field(CSOEconItem::equipped_state, TYPE_STRING, zeqip_state.serialize()));
	}
	else if (inusefix)
	{
		ProtoWriter eqip_state(3);
		eqip_state.add(Field(CSOEconItemEquipped::new_class, TYPE_UINT64, (int64_t)(inusefix)));
		eqip_state.add(Field(CSOEconItemEquipped::new_slot, TYPE_UINT64, (int64_t)(Protobuffs::GetSlotID(itemIndex))));
		item.add(Field(CSOEconItem::equipped_state, TYPE_STRING, eqip_state.serialize()));

	}

	// Paint Kit
	float _PaintKitAttributeValue = (float)paintKit;
	auto PaintKitAttributeValue = std::string{ reinterpret_cast<const char*>((void*)&_PaintKitAttributeValue), 4 };
	ProtoWriter PaintKitAttribute(3);
	PaintKitAttribute.add(Field(CSOEconItemAttribute::def_index, TYPE_UINT32, (int64_t)6));
	PaintKitAttribute.add(Field(CSOEconItemAttribute::value_bytes, TYPE_STRING, PaintKitAttributeValue));
	item.add(Field(CSOEconItem::attribute, TYPE_STRING, PaintKitAttribute.serialize()));

	// Paint Seed
	float _SeedAttributeValue = (float)seed;
	auto SeedAttributeValue = std::string{ reinterpret_cast<const char*>((void*)&_SeedAttributeValue), 4 };
	ProtoWriter SeedAttribute(3);
	SeedAttribute.add(Field(CSOEconItemAttribute::def_index, TYPE_UINT32, (int64_t)7));
	SeedAttribute.add(Field(CSOEconItemAttribute::value_bytes, TYPE_STRING, SeedAttributeValue));
	item.add(Field(CSOEconItem::attribute, TYPE_STRING, SeedAttribute.serialize()));

	// Paint Wear
	float _WearAttributeValue = wear;
	auto WearAttributeValue = std::string{ reinterpret_cast<const char*>((void*)&_WearAttributeValue), 4 };
	ProtoWriter WearAttribute(3);
	WearAttribute.add(Field(CSOEconItemAttribute::def_index, TYPE_UINT32, (int64_t)8));
	WearAttribute.add(Field(CSOEconItemAttribute::value_bytes, TYPE_STRING, WearAttributeValue));
	item.add(Field(CSOEconItem::attribute, TYPE_STRING, WearAttribute.serialize()));




	// Stickers
	for (int j = 0; j < 4; j++)
	{
		int _StickerAtributeValue = ctickers[j];
		auto StickerAtributeValue = std::string{ reinterpret_cast<const char*>((void*)&_StickerAtributeValue), 4 };

		ProtoWriter WearAttribute(3);
		WearAttribute.add(Field(CSOEconItemAttribute::def_index, TYPE_UINT32, (int64_t)113 + 4 * j));
		WearAttribute.add(Field(CSOEconItemAttribute::value_bytes, TYPE_STRING, StickerAtributeValue));
		item.add(Field(CSOEconItem::attribute, TYPE_STRING, WearAttribute.serialize()));


		float _AttributeValue = 1.0f;
		auto AttributeValue = std::string{ reinterpret_cast<const char*>((void*)&_AttributeValue), 4 };

		ProtoWriter Attribute(3);
		Attribute.add(Field(CSOEconItemAttribute::def_index, TYPE_UINT32, (int64_t)115 + 4 * j));
		Attribute.add(Field(CSOEconItemAttribute::value_bytes, TYPE_STRING, AttributeValue));
		item.add(Field(CSOEconItem::attribute, TYPE_STRING, Attribute.serialize()));


		//item.attribute().add(make_econ_item_attribute(113 + 4 * j, uint32_t(289 + j))); // Sticker Kit
		//item.attribute().add(make_econ_item_attribute(114 + 4 * j, float(0.001f)));     // Sticker Wear
		//item.attribute().add(make_econ_item_attribute(115 + 4 * j, float(1.f)));        // Sticker Scale
		//item.attribute().add(make_econ_item_attribute(116 + 4 * j, float(0.f)));        // Sticker Rotation
	}







	object.add(Field(SubscribedType::object_data, TYPE_STRING, item.serialize()));
}

uint8_t* ssdgfadefault(HMODULE hModule, const char* szSignature)
{
	static auto pattern_to_byte = [](const char* pattern) {
		auto bytes = std::vector<int>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current) {
			if (*current == '?') {
				++current;
				if (*current == '?')
					++current;
				bytes.push_back(-1);
			}
			else {
				bytes.push_back(strtoul(current, &current, 16));
			}
		}
		return bytes;
	};

	//auto Module = GetModuleHandleA(szModule);

	auto dosHeader = (PIMAGE_DOS_HEADER)hModule;
	auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)hModule + dosHeader->e_lfanew);

	auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
	auto patternBytes = pattern_to_byte(szSignature);
	auto scanBytes = reinterpret_cast<std::uint8_t*>(hModule);

	auto s = patternBytes.size();
	auto d = patternBytes.data();

	for (auto i = 0ul; i < sizeOfImage - s; ++i) {
		bool found = true;
		for (auto j = 0ul; j < s; ++j) {
			if (scanBytes[i + j] != d[j] && d[j] != -1) {
				found = false;
				break;
			}
		}
		if (found) {
			return &scanBytes[i];
		}
	}

	return nullptr;
};

template<class T>
static T* FindHudElement(const char* name)
{
	static auto pThis = *reinterpret_cast<DWORD**>(ssdgfadefault(GetModuleHandleA("client.dll"), ("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08")) + 1);

	static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(ssdgfadefault(GetModuleHandleA("client.dll"), ("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
	return (T*)find_hud_element(pThis, name);
}

struct hud_weapons_t {
	std::int32_t* get_weapon_count() {
		return reinterpret_cast<std::int32_t*>(std::uintptr_t(this) + 0x80);
	}
};



void Inventory::force_full_update()
{
	static auto fn = reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(ssdgfadefault(GetModuleHandleA("client.dll"), ("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C")));
	
	auto element = FindHudElement<std::uintptr_t*>(("CCSGO_HudWeaponSelection"));

	auto hud_weapons = reinterpret_cast<hud_weapons_t*>(std::uintptr_t(element) - 0xA0);

	if (hud_weapons == nullptr)
		return;

	if (!*hud_weapons->get_weapon_count())
		return;

	for (std::int32_t i = 0; i < *hud_weapons->get_weapon_count(); i++)
		i = fn(hud_weapons, i);


}






bool Inventory::Presend(uint32_t& unMsgType, void* pubData, uint32_t& cubData)
{
	uint32_t MessageType = unMsgType & 0x7FFFFFFF;
	if (MessageType == k_EMsgGCAdjustItemEquippedState) {

		ProtoWriter msg((void*)((DWORD)pubData + 8), cubData - 8, 19);

		if (!msg.has(CMsgAdjustItemEquippedState::item_id)
			|| !msg.has(CMsgAdjustItemEquippedState::new_class)
			|| !msg.has(CMsgAdjustItemEquippedState::new_slot))
			return true;

		



		uint32_t item_id = msg.get(CMsgAdjustItemEquippedState::item_id).UInt32();
		uint32_t new_class = msg.get(CMsgAdjustItemEquippedState::new_class).UInt32();


	
		
		if (item_id > 200000)
		{
			for (auto& f : g_MedalSkins)
				f.second.equip_state = 0;


			auto medals = g_MedalSkins[item_id];
			g_MedalSkins[item_id].equip_state = 1;
		}
		

		if (item_id > 20000 && item_id < 200000)
		{
			auto weapon = g_InventorySkins[item_id];

			if (new_class == 2)
			{
				for (auto& skins : g_InventorySkins)
				{
					if (Protobuffs::GetSlotID(skins.second.wId) == Protobuffs::GetSlotID(weapon.wId))
						skins.second.in_use_t = false;
				}
				g_InventorySkins[item_id].in_use_t = true;
				
			
			}
			else if (new_class == 3)
			{
				for (auto& skins : g_InventorySkins)
				{
					if (Protobuffs::GetSlotID(skins.second.wId) == Protobuffs::GetSlotID(weapon.wId))
						skins.second.in_use_ct = false;
				}

				g_InventorySkins[item_id].in_use_ct = true;
			
			
			}
		}


		m_engine()->ExecuteClientCmd("econ_clear_inventory_images");


		write.SendClientHello();
		write.SendMatchmakingClient2GCHello();


		m_clientstate()->iDeltaTick = -1;
		force_full_update();
		


		

		return false;
	}
	return true;
}
