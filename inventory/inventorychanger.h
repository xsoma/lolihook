#pragma once
#define START_MUSICKIT_INDEX 1500000
#define START_ITEM_INDEX     2000000
#include <string>
#include <unordered_map>

struct wskin
{
	int wId;
	int paintKit;
	int quality;
	float wear;
	int seed;
	std::string name;
	bool in_use_t;
	bool in_use_ct;
	int stattrak;


	int sicker[4];

};

inline std::unordered_map<int, wskin> g_InventorySkins;

struct wmedal
{
	int paintKit;
	int equip_state;
};

inline std::map<int, wmedal> g_MedalSkins;

struct wmedalp
{
	std::string name;
	int paintKit;
};

inline std::map<int, wmedalp> fosso;

class ProtoWriter;
class Inventory
{
public:
	void FixNullInventory(ProtoWriter& cache);
	void ClearEquipState(ProtoWriter& object);
	void AddAllItems(ProtoWriter& object);
	void AddItem(ProtoWriter& object, int index, int itemIndex, int rarity, int paintKit, int seed, float wear, std::string name, int ctickers[4], int inusefix = 0);
	bool Presend(uint32_t& unMsgType, void* pubData, uint32_t& cubData);
	static int GetAvailableClassID(int definition_index);
	std::string Changer(void* pubDest, uint32_t* pcubMsgSize);

	void force_full_update();
};

inline Inventory* g_Inventory;


