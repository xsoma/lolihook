


#include "../../includes.hpp"


class CustomSkins
{
private:
	IMaterial * modelskin[33];
	IMaterial * glovskin[2];
public:
	void setup_materials();
	void draw_skins(std::function<void()> original);
	void create_materials();
	void knife_correct_load();
	bool is_sleeve;
	bool is_arm;
	bool is_weapon;
	bool isplayer;
	bool is_knife;
	bool is_knife_wep;
	bool is_glove;

};


extern CustomSkins knowledge;