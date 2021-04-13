#include "..\..\includes.hpp"

struct m_keybind
{
	std::string m_name;
	std::string m_mode;

	m_keybind(std::string name, std::string mode) :
		m_name(name), m_mode(mode)
	{

	}
};

class misc : public singleton <misc> 
{
public:
	void watermark();
	void get_keys();
	void keybind_list();
	void NoDuck(CUserCmd* cmd);
	void edgebug(CUserCmd* pCmd);
	void jumpbug(CUserCmd* pCmd);
	void null_strafes(CUserCmd* pCmd);

	void ChangeRegion();
	void ChatSpamer();
	void AutoCrouch(CUserCmd* cmd);
	void SlideWalk(CUserCmd* cmd);
	void FakePrime() noexcept;
	void blockbot(CUserCmd* cmd);
	void automatic_peek(CUserCmd* cmd, float wish_yaw);
	void ViewModel();
	void FullBright();
	void PovArrows(player_t* e, Color color);
	void NightmodeFix();
	void zeus_range();
	void desync_arrows();
	void aimbot_hitboxes();
	void ragdolls();
	void rank_reveal();
	void fast_stop(CUserCmd* m_pcmd);
	void spectators_list();
	bool can_shift_shot(int ticks);

	bool double_tap(CUserCmd* m_pcmd);
	void hide_shots(CUserCmd* m_pcmd, bool should_work);
	bool recharging_double_tap = false;
	int recharging_time = 0;

	bool should_tp = false;
	bool double_tap_enabled = false;
	bool double_tap_key = false;

	bool hide_shots_enabled = false;
	bool hide_shots_key = false;
	std::vector<m_keybind> m_keybinds;
};