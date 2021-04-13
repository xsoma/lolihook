#pragma once
#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"
#include <map>
#include <deque>

template<typename T>
class Singleton
{
protected:
	Singleton() {}
	~Singleton() {}

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton&&) = delete;

public:
	static T& Get()
	{
		static T inst{};
		return inst;
	}
};

struct backtrackData
{
	float simtime;
	Vector hitboxPos;
};

struct player_bt {

	matrix3x4_t bone[128];

};

class NewBacktrack : public Singleton<NewBacktrack>
{
public:
	void LegitBacktrack(CUserCmd* cmd);
private:

};
extern player_bt bt_data[64][12];
extern backtrackData headPositions[64][12];
