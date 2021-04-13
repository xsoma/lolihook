#pragma once
#include "..\..\includes.hpp"

class weapon_info_t;
class weapon_t;

struct fire_bullet_data
{
	Vector src;
	trace_t enter_trace;
	Vector direction;
	CTraceFilter filter;
	float trace_length;
	float trace_length_remaining;
	float current_damage;
	int penetrate_count;
};

class autowall : public singleton <autowall>
{
public:
	struct returninfo_t
	{
		bool valid = false;

		bool visible = false;
		int damage = -1;
		int hitbox = -1;

		returninfo_t()
		{
			valid = false;

			visible = false;
			damage = -1;
			hitbox = -1;
		}

		returninfo_t(bool visible, int damage, int hitbox)
		{
			valid = true;

			this->visible = visible;
			this->damage = damage;
			this->hitbox = hitbox;
		}
	};

	struct FireBulletData
	{
		Vector src;
		trace_t enter_trace;
		Vector direction;
		CTraceFilter filter;
		weapon_info_t* wpn_data;
		float trace_length;
		float trace_length_remaining;
		float length_to_end;
		float current_damage;
		int penetrate_count;
	};

	bool is_breakable_entity(IClientEntity* e);
	void scale_damage(player_t* e, CGameTrace& enterTrace, weapon_info_t* weaponData, float& currentDamage);
	bool trace_to_exit(CGameTrace& enterTrace, CGameTrace& exitTrace, Vector startPosition, const Vector& direction);
	bool handle_bullet_penetration(weapon_info_t* weaponData, CGameTrace& enterTrace, Vector& eyePosition, const Vector& direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, float ff_damage_reduction_bullets, float ff_damage_bullet_penetration, bool draw_impact = false);
	returninfo_t wall_penetration(const Vector& eye_pos, Vector& point, IClientEntity* e);
	bool fire_bullet(weapon_t* pWeapon, Vector& direction, bool& visible, float& currentDamage, int& hitbox, IClientEntity* e = nullptr, float length = 0.f, const Vector& pos = { 0.f,0.f,0.f });
	float can_hit(Vector& vecEyePos, Vector& point);
};


/*class weapon_info_t;
class weapon_t;

class c_autowall : public singleton< c_autowall > {
public:
	struct returninfo_t {
		int m_damage;
		int m_hitgroup;
		int m_hitbox;
		int m_penetration_count;
		bool m_did_penetrate_wall;
		float m_thickness;
		bool m_visible = false;
		bool valid = false;
		Vector m_end;
		IClientEntity* m_hit_entity;

		returninfo_t(bool visible, int damage, int hitgroup, int hitbox, int penetration_count, bool did_penetrate_wall, float thickness, IClientEntity* hit_entity) {
			m_visible = visible;
			m_damage = damage;
			m_hitgroup = hitgroup;
			m_hitbox = hitbox;
			m_penetration_count = penetration_count;
			m_did_penetrate_wall = did_penetrate_wall;
			m_thickness = thickness;
			m_hit_entity = hit_entity;
			valid = true;
		}
	};

	struct firebulletdata_t {
		Vector m_start;
		Vector m_end;
		Vector m_current_position;
		Vector m_direction;

		ITraceFilter* m_filter;
		trace_t m_enter_trace;

		float m_thickness;
		float m_current_damage;
		int m_penetration_count;
	};

	returninfo_t calculate_return_info(Vector start, Vector end, IClientEntity* from_entity = nullptr, IClientEntity* to_entity = nullptr, int specific_hitgroup = -1);
	void scale_damage(player_t* e, weapon_info_t* weapon_info, int hitgroup, float& current_damage);
	bool handle_bullet_penetration(weapon_info_t* info, firebulletdata_t& bullet);
	bool trace_to_exit(trace_t* enter_trace, Vector start, Vector dir, trace_t* exit_trace);

	void trace_line(Vector& start, Vector& end, unsigned int mask, IClientEntity* ignore, trace_t* trace);
	void clip_trace_to_player(Vector& start, Vector& end, player_t* e, unsigned int mask, ITraceFilter* filter, trace_t* old_trace);
	bool is_breakable_entity(IClientEntity* e);
};*/