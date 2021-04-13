#pragma once
#include "..\..\includes.hpp"
#include "..\lagcompensation\animation_system.h"

enum HitscanMode : int {
	NORMAL,
	LETHAL,
	ALWAYS
};

class target
{
public:
	player_t* e;

	adjust_data* last_record;
	adjust_data* history_record;

	target()
	{
		e = nullptr;

		last_record = nullptr;
		history_record = nullptr;
	}

	target(player_t* e, adjust_data* last_record, adjust_data* history_record)
	{
		this->e = e;

		this->last_record = last_record;
		this->history_record = history_record;
	}
};

class scan_point
{
public:
	Vector point;
	int hitbox;
	bool center;
	bool safe;

	scan_point()
	{
		point.Zero();
		hitbox = -1;
		center = false;
		safe = false;
	}

	scan_point(const Vector& point, const int& hitbox, const bool& center)
	{
		this->point = point;
		this->hitbox = hitbox;
		this->center = center;
	}

	void reset()
	{
		point.Zero();
		hitbox = -1;
		center = false;
		safe = false;
	}
};

class scan_data
{
public:
	scan_point point;
	bool visible;
	int damage;
	int hitbox;

	scan_data()
	{
		reset();
	}

	void reset()
	{
		point.reset();
		visible = false;
		damage = -1;
		hitbox = -1;
	}

	bool valid()
	{
		return damage >= 1 && hitbox != -1;
	}
};

struct Last_target
{
	adjust_data record;
	scan_data data;
	float distance;
};

class scanned_target
{
public:
	adjust_data* record;
	scan_data data;

	float fov;
	float distance;
	int health;

	scanned_target()
	{
		reset();
	}

	scanned_target(const scanned_target& data)
	{
		this->record = data.record;
		this->data = data.data;
		this->fov = data.fov;
		this->distance = data.distance;
		this->health = data.health;
	}

	scanned_target& operator=(const scanned_target& data)
	{
		this->record = data.record;
		this->data = data.data;
		this->fov = data.fov;
		this->distance = data.distance;
		this->health = data.health;

		return *this;
	}

	scanned_target(adjust_data* record, const scan_data& data)
	{
		this->record = record;
		this->data = data;

		Vector viewangles;
		m_engine()->GetViewAngles(viewangles);

		auto aim_angle = math::calculate_angle(g_ctx.globals.eye_pos, data.point.point);
		auto fov = math::get_fov(viewangles, aim_angle);

		this->fov = fov;
		this->distance = g_ctx.globals.eye_pos.DistTo(data.point.point);
		this->health = record->player->m_iHealth();
	}

	void reset()
	{
		record = nullptr;
		data.reset();

		fov = 0.0f;
		distance = 0.0f;
		health = 0;
	}
};

class aim : public singleton <aim>
{
	void automatic_revolver(CUserCmd* cmd);
	float GetPointScale(float flHitboxRadius, Vector* vecPos, Vector* veсPoint, int iHitbox);
	int hitchance(const Vector& aim_angle);
	void prepare_targets();
	adjust_data* get_record(std::deque <adjust_data>* records, bool history);
	int get_minimum_damage(bool visible, int health);
	bool bounding_box_check(adjust_data* record);
	void scan_targets();
	bool automatic_stop(CUserCmd* cmd);
	void find_best_target();
	void fire(CUserCmd* cmd);
	//int hitchance(const Vector& aim_angle);
	void build_seed_table();
	bool calculate_hitchance(int& final_hitchance, player_t* player);
	bool weaponIsMaxAccuracy(float inaccuracy);

	bool CanHit(Vector start, Vector end, player_t* record, int box, matrix3x4_t* bones);

	std::vector <scanned_target> scanned_targets;
	scanned_target final_target;
	
	inline static std::vector<std::pair<float, float>> precomputed_seeds = {};

	__forceinline Vector CalculateSpread(int seed, float inaccuracy, float spread)
	{
		auto wepInfo = g_ctx.globals.weapon->get_csweapon_info();
		int itemDefIndex;
		float recoilIndex, r1, r2, r3, r4, s1, c1, s2, c2;

		if (!wepInfo || !wepInfo->iBullets)
			return { };

		itemDefIndex = g_ctx.globals.weapon->m_iItemDefinitionIndex();
		recoilIndex = wepInfo->iRecoilSeed;

		r1 = std::get<0>(precomputed_seeds[seed]);
		r2 = std::get<1>(precomputed_seeds[seed]);
		r3 = std::get<0>(precomputed_seeds[seed]);
		r4 = std::get<1>(precomputed_seeds[seed]);

		if (itemDefIndex == WEAPON_NEGEV && recoilIndex < 3.f)
		{
			for (int i{ 3 }; i > recoilIndex; i--)
			{
				r1 *= r1;
				r3 *= r3;
			}

			r1 = 1.f - r1;
			r3 = 1.f - r3;
		}

		c1 = std::cos(r2);
		c2 = std::cos(r4);
		s1 = std::sin(r2);
		s2 = std::sin(r4);

		return {
			(c1 * (r1 * inaccuracy)) + (c2 * (r3 * spread)),
			(s1 * (r1 * inaccuracy)) + (s2 * (r3 * spread)),
			0.f
		};
	}

public:
	void run(CUserCmd* cmd);
	void scan(adjust_data* record, scan_data& data, const Vector& shoot_position = g_ctx.globals.eye_pos);
	std::vector <int> get_hitboxes(adjust_data* record);
	std::vector <scan_point> get_points(adjust_data* record, int hitbox, bool from_aim = true);
	//bool hitbox_intersection(player_t* e, matrix3x4_t* matrix, int hitbox, const Vector& start, const Vector& end);
	bool hitbox_intersection(player_t* e, matrix3x4_t* matrix, int hitbox, const Vector& start, const Vector& end, float* safe = nullptr);

	std::vector <target> targets;
	std::vector <adjust_data> backup;

	int last_target_index = -1;
	Last_target last_target[65];

	Vector last_shoot_position;
	bool should_stop;
};