#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

class weapon_t;
class Box;

class worldesp : public singleton <worldesp> 
{
public:
	void paint_traverse();
	void DrawBeam(Vector src, Vector end, Color color);
	void PlayerTrails();
	void skybox_changer();
	void fog_changer();
	void world_modulation(entity_t* entity);
	void molotov_timer(entity_t* entity);
	void smoke_timer(entity_t* entity);
	void grenade_projectiles(entity_t* entity);
	void bomb_timer(entity_t* entity);
	void bomb_timer2(entity_t* entity);

	void dropped_weapons(entity_t* entity);

	bool changed = false;
	std::string backup_skybox = "";

	float scaleDamageArmor(float flDamage, int armor_value)
	{
		float flArmorRatio = 0.5f;
		float flArmorBonus = 0.5f;
		if (armor_value > 0) {
			float flNew = flDamage * flArmorRatio;
			float flArmor = (flDamage - flNew) * flArmorBonus;

			if (flArmor > static_cast<float>(armor_value)) {
				flArmor = static_cast<float>(armor_value) * (1.f / flArmorBonus);
				flNew = flDamage - flArmor;
			}

			flDamage = flNew;
		}
		return flDamage;
	}
};