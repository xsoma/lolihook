#pragma once
#include "../includes.hpp"

class c_menu2 : public singleton<c_menu2> {
public:
	void render_menu(bool is_open);
	void init_styles(ImGuiStyle& style, LPDIRECT3DDEVICE9 device);
};
