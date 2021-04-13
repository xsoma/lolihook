/////////////////////////////////////////////////////
//                                                   
//    Generated with QuarcStudio. (ex. ImGui Editor) 
//    QuarcStudio is designed with love by pers0na2. 
//    All rights to QuarcStudio belong to pers0na2.  
//                                                   
/////////////////////////////////////////////////////


#ifndef QGUIHEADERGUARD
#define QGUIHEADERGUARD

#include <string>

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#define IM_USE using namespace ImGui; 
#include <imgui_internal.h>

#include "fusionrawdata.h"

#include "sffont.hpp"
#include "hashes.h"
#include "faprolight.hpp"

namespace qgui
{
	ImVec2 p;
	ImDrawList* draw;
	IDirect3DTexture9* logotype;
	ImFont* roboto_small;
	ImFont* roboto_medium;


	const char* tabsname[] = { "Tab 1","Tab 2","Tab 3", "Tab 4" };
	const char* tabsicons[] = { ICON_FA_QRCODE,ICON_FA_QRCODE,ICON_FA_QRCODE, ICON_FA_QRCODE };
	static int selected_tab = 0;
	static int selected_subtab = 1;
	static int etc = 1;

	void init_styles(ImGuiStyle& style, LPDIRECT3DDEVICE9 device)
	{
		style.WindowPadding = ImVec2(0.000000f, 0.000000f);
		style.FramePadding = ImVec2(4.000000f, 3.000000f);
		style.ItemSpacing = ImVec2(8.000000f, 4.000000f);
		style.ItemInnerSpacing = ImVec2(4.000000f, 4.000000f);
		style.IndentSpacing = 21.000000f;
		style.ScrollbarSize = 15.000000f;
		style.GrabMinSize = 1.000000f;
		style.WindowBorderSize = 0.000000f;
		style.ChildBorderSize = 1.000000f;
		style.PopupBorderSize = 0.000000f;
		style.FrameBorderSize = 1.000000f;
		style.WindowRounding = 0.000000f;
		style.ChildRounding = 0.000000f;
		style.FrameRounding = 0.000000f;
		style.PopupRounding = 0.000000f;
		style.ScrollbarRounding = 4.000000f;
		style.GrabRounding = 0.000000f;
		style.WindowTitleAlign = ImVec2(0.000000f, 0.500000f);
		style.ButtonTextAlign = ImVec2(0.500000f, 0.500000f);
		style.SelectableTextAlign = ImVec2(0.500000f, 0.500000f);

		style.Colors[ImGuiCol_Text] = ImVec4(0.900000f, 0.900000f, 0.900000f, 1.000000f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.600000f, 0.600000f, 0.600000f, 1.000000f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.058824f, 0.058824f, 0.058824f, 1.000000f);
		style.Colors[ImGuiCol_ChildBg] = ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(57 / 255.f, 57 / 255.f, 57 / 255.f, 1);
		style.Colors[ImGuiCol_Border] = ImVec4(0.500000f, 0.500000f, 0.500000f, 0.500000f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.000000f, 0.000000f, 0.000000f, 0.000000f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.430000f, 0.430000f, 0.430000f, 0.390000f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.470000f, 0.470000f, 0.690000f, 0.400000f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.420000f, 0.410000f, 0.640000f, 0.690000f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.270000f, 0.270000f, 0.540000f, 0.830000f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.320000f, 0.320000f, 0.630000f, 0.870000f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.400000f, 0.400000f, 0.800000f, 0.200000f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.400000f, 0.400000f, 0.550000f, 0.800000f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.200000f, 0.250000f, 0.300000f, 0.000000f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.400000f, 0.400000f, 0.400000f, 0.300000f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.400000f, 0.400000f, 0.400000f, 0.400000f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.400000f, 0.400000f, 0.400000f, 0.600000f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.900000f, 0.900000f, 0.900000f, 0.500000f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.000000f, 1.000000f, 1.000000f, 0.300000f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.410000f, 0.390000f, 0.800000f, 0.600000f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.350000f, 0.400000f, 0.610000f, 0.620000f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.400000f, 0.480000f, 0.710000f, 0.790000f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.460000f, 0.540000f, 0.800000f, 1.000000f);
		style.Colors[ImGuiCol_Header] = ImVec4(45 / 255.f, 45 / 255.f, 45 / 255.f, 1);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(50 / 255.f, 50 / 255.f, 50 / 255.f, 1);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(45 / 255.f, 45 / 255.f, 45 / 255.f, 1);
		style.Colors[ImGuiCol_Separator] = ImVec4(0.500000f, 0.500000f, 0.500000f, 0.600000f);
		style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.600000f, 0.600000f, 0.700000f, 1.000000f);
		style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.700000f, 0.700000f, 0.900000f, 1.000000f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.000000f, 1.000000f, 1.000000f, 0.160000f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.780000f, 0.820000f, 1.000000f, 0.600000f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.780000f, 0.820000f, 1.000000f, 0.900000f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.000000f, 0.000000f, 1.000000f, 0.350000f);
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.000000f, 1.000000f, 0.000000f, 0.900000f);

		if (logotype == nullptr)
			D3DXCreateTextureFromFileInMemoryEx(device, &logotyperaw, sizeof(logotyperaw), 71, 22, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &logotype);

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x2000, 0x206F, // General Punctuation
			0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
			0x31F0, 0x31FF, // Katakana Phonetic Extensions
			0xFF00, 0xFFEF, // Half-width characters
			0x4e00, 0x9FAF, // CJK Ideograms
			0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
			0x2DE0, 0x2DFF, // Cyrillic Extended-A
			0xA640, 0xA69F, // Cyrillic Extended-B
			0,
		};

		ImFontConfig font_cfg{};
		font_cfg.FontDataOwnedByAtlas = false;
		ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(sfpro_compressed_data, sfpro_compressed_size, 15, &font_cfg, ranges);

		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
		ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&faprolight, sizeof faprolight, 16, &icons_config, icon_ranges);

		roboto_small = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(sfpro_compressed_data, sfpro_compressed_size, 10, &font_cfg, ranges);
		roboto_medium = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(sfpro_compressed_data, sfpro_compressed_size, 13, &font_cfg, ranges);
	}

	void checkbox(const char* label, const char* desc, bool* v)
	{
		ImGuiContext& g = *GImGui;
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImVec2 desc_size = ImGui::CalcTextSize(desc, NULL, true);
		const ImGuiStyle& style = g.Style;
		const float square_sz = ImGui::GetFrameHeight() - 4;
		const ImRect total_bb(p + ImVec2(15, 0), p + ImVec2(square_sz + (style.ItemInnerSpacing.x + label_size.x + 0), 0 + label_size.y + style.FramePadding.y * 2) + ImVec2(15, desc_size.x > 0 ? 20 : 0));

		float height = ImGui::GetFrameHeight() + (desc_size.x > 0 ? 20 : 0);
		float width = height * 1.5 + label_size.x;

		ImGui::Spacing();
		ImGui::SameLine(15);
		ImGui::InvisibleButton(label, ImVec2(width, height));

		if (ImGui::IsItemClicked())
			*v = !*v;

		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(7);

		float t = *v ? 1.0f : 0.0f;
		float ANIM_SPEED = 0.30f; // Bigger = Slower
		if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *v ? (t_anim) : (1.0f - t_anim);
		}


		draw_list->AddRect(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 0), ImVec2(total_bb.Min.x + 16, total_bb.Min.y + 16), ImColor(56, 56, 56, 255), 3, 15, 1.000000);

		if (t != 0.f)
		{
			const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 9.0f));
			ImGui::RenderCheckMark(draw_list, total_bb.Min + ImVec2(pad + 1, pad + 1), ImColor(133 / 255.f, 17 / 255.f, 31 / 255.f, t), square_sz - pad * 2.0f - 3);
		}


		if (t != 1.f)
			draw_list->AddRectFilled(ImVec2(total_bb.Min.x + 1, total_bb.Min.y + 1 + (15 * t)), ImVec2(total_bb.Min.x + 15, total_bb.Min.y + 15), ImColor(0.058824f, 0.058824f, 0.058824f, 1.000000f), 3, 15);

		ImGui::PushFont(roboto_medium);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255 / 255.f, 254 / 255.f, 254 / 255.f, 255 / 255.f));
		ImGui::RenderText(ImVec2(total_bb.Min.x + style.ItemInnerSpacing.x + 20, total_bb.Min.y + style.FramePadding.y - 2), label);
		ImGui::PopFont();
		ImGui::PopStyleColor();
		ImGui::PushFont(roboto_medium);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(97 / 255.f, 97 / 255.f, 97 / 255.f, 255 / 255.f));
		ImGui::RenderText(ImVec2(total_bb.Min.x, total_bb.Min.y + 20), desc);
		ImGui::PopStyleColor();
		ImGui::PopFont();
	}

	bool button(const char* label, const ImVec2& size_arg)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		if (hovered || held)
			ImGui::SetMouseCursor(0);


		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
		ImGui::RenderText(ImVec2(bb.Min.x + style.ItemInnerSpacing.x + 0, bb.Min.y + style.FramePadding.y + 0), label);
		ImGui::PopStyleColor();

		return pressed;
	}

	static float CalcMaxPopupHeightFromItemCount(int items_count)
	{
		ImGuiContext& g = *GImGui;
		if (items_count <= 0)
			return FLT_MAX;
		return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
	}

	bool          combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
	bool          combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);      // Separate items with \0 within a string, end item-list with \0\0. e.g. "One\0Two\0Three\0"
	bool          combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

	bool begincombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
	{
		IM_USE;
		ImGuiContext& g = *GImGui;
		bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
		g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : ImGui::GetFrameHeight();
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const float expected_w = 192;
		const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : expected_w;

		const ImRect frame_bb(window->DC.CursorPos + ImVec2(15, 0), window->DC.CursorPos + ImVec2(w + 0, label_size.y + style.FramePadding.y * 2.0f + 0));
		const ImRect total_bb(frame_bb.Min/* - ImVec2(0, 30)*/, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

		ImGui::ItemSize(total_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);
		bool popup_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

		if (hovered || held)
			ImGui::SetMouseCursor(7);

		const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);

		window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, ImColor(57, 57, 57), 3);

		if (!(flags & ImGuiComboFlags_NoArrowButton))
		{
			ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
			window->DrawList->AddLine(frame_bb.Max - ImVec2(20, 13), frame_bb.Max - ImVec2(15, 8), text_col);
			window->DrawList->AddLine(frame_bb.Max - ImVec2(15, 8), frame_bb.Max - ImVec2(10, 14), text_col);
			/*ImVec2(value_x2 + style.FramePadding.y + 0, frame_bb.Min.y + style.FramePadding.y + 2), text_col, ImGuiDir_Down, 0.8f);*/
		}

		ImGui::PushFont(roboto_medium);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(140 / 255.f, 140 / 255.f, 140 / 255.f, 255 / 255.f));
		if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
			ImGui::RenderText(frame_bb.Min + style.FramePadding + ImVec2(0, 0), label);
		ImGui::PopStyleColor();
		ImGui::PopFont();

		ImGui::PushFont(roboto_medium);
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
		if (label_size.x > 0)
			ImGui::RenderText(frame_bb.Min + style.FramePadding + ImVec2(ImGui::CalcTextSize(label).x + 5, 0), preview_value);
		ImGui::PopStyleColor();
		ImGui::PopFont();

		if ((pressed || g.NavActivateId == id) && !popup_open)
		{
			if (window->DC.NavLayerCurrent == 0)
				window->NavLastIds[0] = id;
			ImGui::OpenPopupEx(id, ImGuiPopupFlags_None);
			popup_open = true;
		}

		if (!popup_open)
			return false;

		if ((flags & ImGuiComboFlags_HeightMask_) == 0)
			flags |= ImGuiComboFlags_HeightRegular;
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));
		int popup_max_height_in_items = -1;
		if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
		else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
		else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
		ImGui::SetNextWindowSizeConstraints(ImVec2(177, 0.0f), ImVec2(177, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

		char name[16];
		ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size);

		if (ImGuiWindow* popup_window = ImGui::FindWindowByName(name))
			if (popup_window->WasActive)
			{
				ImVec2 size_expected = ImGui::CalcWindowExpectedSize(popup_window);
				if (flags & ImGuiComboFlags_PopupAlignLeft)
					popup_window->AutoPosLastDirection = ImGuiDir_Left;
				ImRect r_outer = ImGui::GetWindowAllowedExtentRect(popup_window);
				ImVec2 pos = ImGui::FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
				ImGui::SetNextWindowPos(pos - ImVec2(0, 2));
			}

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
		bool ret = ImGui::Begin(name, NULL, window_flags);
		ImGui::PopStyleVar();

		if (!ret)
		{
			ImGui::EndPopup();
			return false;
		}
		return true;
	}

	static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
	{
		const char* const* items = (const char* const*)data;
		if (out_text)
			*out_text = items[idx];
		return true;
	}

	// Getter for the old Combo() API: "item1\0item2\0item3\0"
	static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
	{
		// FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
		const char* items_separated_by_zeros = (const char*)data;
		int items_count = 0;
		const char* p = items_separated_by_zeros;
		while (*p)
		{
			if (idx == items_count)
				break;
			p += strlen(p) + 1;
			items_count++;
		}
		if (!*p)
			return false;
		if (out_text)
			*out_text = p;
		return true;
	}

	// Old API, prefer using BeginCombo() nowadays if you can.
	bool combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
	{
		IM_USE;
		ImGuiContext& g = *GImGui;

		// Call the getter to obtain the preview string which is a parameter to BeginCombo()
		const char* preview_value = NULL;
		if (*current_item >= 0 && *current_item < items_count)
			items_getter(data, *current_item, &preview_value);

		// The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
		if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
			SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

		if (!begincombo(label, preview_value, ImGuiComboFlags_None))
			return false;

		// Display items
		ImGui::PushFont(roboto_medium);
		// FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
		bool value_changed = false;
		for (int i = 0; i < items_count; i++)
		{
			PushID((void*)(intptr_t)i);
			const bool item_selected = (i == *current_item);
			const char* item_text;
			if (!items_getter(data, i, &item_text))
				item_text = "*Unknown item*";
			if (Selectable(item_text, item_selected))
			{
				value_changed = true;
				*current_item = i;
			}
			if (item_selected)
				SetItemDefaultFocus();
			PopID();
		}
		ImGui::PopFont();
		EndCombo();
		return value_changed;
	}

	// Combo box helper allowing to pass an array of strings.
	bool combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
	{
		IM_USE;
		const bool value_changed = combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
		return value_changed;
	}

	// Combo box helper allowing to pass all items in a single string literal holding multiple zero-terminated items "item1\0item2\0"
	bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
	{
		IM_USE;
		int items_count = 0;
		const char* p = items_separated_by_zeros;       // FIXME-OPT: Avoid computing this, or at least only when combo is open
		while (*p)
		{
			p += strlen(p) + 1;
			items_count++;
		}
		bool value_changed = combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
		return value_changed;
	}

	bool tab(const char* label, const char* icon, bool selected)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImVec2 icon_size = ImGui::CalcTextSize(icon, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize({ 28 + label_size.x + style.FramePadding.x * 4.0f,55 }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		if (hovered || held)
			ImGui::SetMouseCursor(0);

		if (selected)
		{
			window->DrawList->AddRectFilledMultiColor(ImVec2(bb.Min.x + 0, bb.Min.y + 0), ImVec2(bb.Max.x + 0, bb.Max.y + 0), ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0), ImColor(255, 17, 46, 150), ImColor(255, 17, 46, 150));
		}
		else
		{
			if (!hovered)
			{

			}
			else
			{
				window->DrawList->AddRectFilledMultiColor(ImVec2(bb.Min.x + 0, bb.Min.y + 0), ImVec2(bb.Max.x + 0, bb.Max.y + 0), ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0), ImColor(255, 17, 46, 80), ImColor(255, 17, 46, 80));
			}
		}


		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
		ImGui::RenderText(ImVec2(bb.Min.x + 10, bb.Min.y + style.FramePadding.y + 19), icon);
		ImGui::PopStyleColor();


		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
		ImGui::RenderText(ImVec2(bb.Min.x + 31, bb.Min.y + style.FramePadding.y + 17), label);
		ImGui::PopStyleColor();

		return pressed;
	}

	void tab1(const char* label, const char* icon, int* operand, int selected)
	{
		ImGuiContext& g = *GImGui;
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImGuiStyle& style = g.Style;
		const float square_sz = ImGui::GetFrameHeight() - 4;
		const ImRect total_bb(p, p + ImVec2(28 + label_size.x + style.FramePadding.x * 4.0f, 55));

		if (ImGui::InvisibleButton(label, ImVec2(28 + label_size.x + style.FramePadding.x * 4.0f, 55)))
			*operand = selected;

		if (ImGui::IsItemHovered())
			draw_list->AddRectFilledMultiColor(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 0), ImVec2(total_bb.Max.x + 0, total_bb.Max.y + 0), ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0), ImColor(255 / 255.f, 17 / 255.f, 46 / 255.f, 0.2f), ImColor(255 / 255.f, 17 / 255.f, 46 / 255.f, 0.2f)),
			ImGui::SetMouseCursor(7);

		float t = *operand == selected ? 1.0f : 0.0f;
		float ANIM_SPEED = 0.20f; // Bigger = Slower
		if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *operand == selected ? (t_anim) : (1.0f - t_anim);
		}

		draw_list->AddRectFilledMultiColor(ImVec2(total_bb.Min.x + 0, total_bb.Min.y + 0), ImVec2(total_bb.Max.x + 0, total_bb.Max.y + 0), ImColor(0, 0, 0, 0), ImColor(0, 0, 0, 0), ImColor(255 / 255.f, 17 / 255.f, 46 / 255.f, t / 2.f), ImColor(255 / 255.f, 17 / 255.f, 46 / 255.f, t / 2.f));

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
		ImGui::RenderText(ImVec2(total_bb.Min.x + 10, total_bb.Min.y + style.FramePadding.y + 19), icon);
		ImGui::PopStyleColor();

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
		ImGui::RenderText(ImVec2(total_bb.Min.x + 31, total_bb.Min.y + style.FramePadding.y + 17), label);
		ImGui::PopStyleColor();
	}


	void subtab(const char* label, int* operand, int selected)
	{
		ImGuiContext& g = *GImGui;
		ImVec2 p = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImGuiStyle& style = g.Style;
		const float square_sz = ImGui::GetFrameHeight() - 4;
		const ImRect total_bb(p, p + ImVec2(180, 33));

		ImGui::InvisibleButton(label, ImVec2(180, 33));

		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(7);

		float t = *operand == selected ? 1.0f : 0.0f;
		float ANIM_SPEED = 0.20f; // Bigger = Slower
		if (g.LastActiveId == g.CurrentWindow->GetID(label)) {
			float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
			t = *operand == selected ? (t_anim) : (1.0f - t_anim);
		}

		draw_list->AddRectFilled(total_bb.Min, total_bb.Max, ImColor(
			37 / 255.f, 37 / 255.f, 37 / 255.f, t
		));

		draw_list->AddRectFilled(total_bb.Min, total_bb.Min + ImVec2(3, 33), ImColor(
			255 / 255.f, 17 / 255.f, 46 / 255.f, t / 2.f
		));

		if (t != 0.f)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255 / 255.f));
			ImGui::RenderText(ImVec2(total_bb.Min.x + 15, total_bb.Min.y + 33 / 2 - ImGui::CalcTextSize(label).y / 2), label);
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(97 / 255.f, 97 / 255.f, 97 / 255.f, 255 / 255.f));
			ImGui::RenderText(ImVec2(total_bb.Min.x + 15, total_bb.Min.y + 33 / 2 - ImGui::CalcTextSize(label).y / 2), label);
			ImGui::PopStyleColor();
		}

		if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
			*operand = selected;
	}

	bool category(const char* label)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize({ 180,30 }, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255 / 255.f));
		ImGui::RenderText(ImVec2(bb.Min.x + 15, bb.Min.y + 33 / 2 - ImGui::CalcTextSize(label).y / 2), label);
		ImGui::PopStyleColor();

		return pressed;
	}

	static const char* PatchFormatStringFloatToInt(const char* fmt)
	{
		if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0)
			return "%d";

		const char* fmt_start = ImParseFormatFindStart(fmt);
		const char* fmt_end = ImParseFormatFindEnd(fmt_start);
		if (fmt_end > fmt_start && fmt_end[-1] == 'f')
		{
			if (fmt_start == fmt && fmt_end[0] == 0)
				return "%d";
			ImGuiContext& g = *GImGui;
			ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end);
			return g.TempBuffer;
		}
		return fmt;
	}

	bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const float w = 193;

		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImRect frame_bb(window->DC.CursorPos + ImVec2(15, 0), window->DC.CursorPos + ImVec2(w + 0, 5));
		const ImRect total_bb(frame_bb.Min + ImVec2(0, -5), frame_bb.Max + ImVec2(-20, 0) + ImVec2(style.ItemInnerSpacing.x + label_size.x, 0.0f));

		ImGui::ItemSize(total_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
			return false;

		if (format == NULL)
			format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;
		else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0)
			format = PatchFormatStringFloatToInt(format);
		const bool hovered = ImGui::ItemHoverable(frame_bb, id);
		const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
		bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
		if (!temp_input_is_active)
		{
			const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister(window, id);
			const bool clicked = (hovered && g.IO.MouseClicked[0]);
			if (focus_requested || clicked || g.NavActivateId == id || g.NavInputId == id)
			{
				ImGui::SetActiveID(id, window);
				ImGui::SetFocusID(id, window);
				ImGui::FocusWindow(window);
				g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
			}
		}

		window->DrawList->AddRectFilled(frame_bb.Min, frame_bb.Max, ImColor(97, 97, 97), 5);


		ImRect grab_bb;
		const bool value_changed = ImGui::SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);

		grab_bb.Min = frame_bb.Min;
		grab_bb.Max += ImVec2(1, 2);

		window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, ImColor(255, 255, 255), 5);
		window->DrawList->AddRectFilled(grab_bb.Max - ImVec2(7, 7), grab_bb.Max + ImVec2(2, 2), ImColor(150, 150, 150), 16);

		char value_buf[64];
		const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);

		if (hovered)
			ImGui::SetMouseCursor(7);

		return value_changed;
	}

	bool slider_float(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
	{
		return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
	}

	bool slider_int(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
	{
		return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
	}

	bool beginchildex(const char* name, ImGuiID id, const ImVec2& size_arg, bool border, ImGuiWindowFlags flags)
	{
		IM_USE;

		ImGuiContext& g = *GImGui;
		ImGuiWindow* parent_window = g.CurrentWindow;

		flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;
		flags |= (parent_window->Flags & ImGuiWindowFlags_NoMove);  // Inherit the NoMove flag

		// Size
		const ImVec2 content_avail = GetContentRegionAvail();
		ImVec2 size = ImFloor(size_arg);
		const int auto_fit_axises = ((size.x == 0.0f) ? (1 << ImGuiAxis_X) : 0x00) | ((size.y == 0.0f) ? (1 << ImGuiAxis_Y) : 0x00);
		if (size.x <= 0.0f)
			size.x = ImMax(content_avail.x + size.x, 4.0f); // Arbitrary minimum child size (0.0f causing too much issues)
		if (size.y <= 0.0f)
			size.y = ImMax(content_avail.y + size.y, 4.0f);
		SetNextWindowSize(size);

		// Build up name. If you need to append to a same child from multiple location in the ID stack, use BeginChild(ImGuiID id) with a stable value.
		char title[256];
		if (name)
			ImFormatString(title, IM_ARRAYSIZE(title), "%s/%s_%08X", parent_window->Name, name, id);
		else
			ImFormatString(title, IM_ARRAYSIZE(title), "%s/%08X", parent_window->Name, id);

		const float backup_border_size = g.Style.ChildBorderSize;
		if (!border)
			g.Style.ChildBorderSize = 0.0f;
		bool ret = Begin(title, NULL, flags);
		g.Style.ChildBorderSize = backup_border_size;

		ImGuiWindow* child_window = g.CurrentWindow;
		child_window->ChildId = id;
		child_window->AutoFitChildAxises = (ImS8)auto_fit_axises;

		// Set the cursor to handle case where the user called SetNextWindowPos()+BeginChild() manually.
		// While this is not really documented/defined, it seems that the expected thing to do.
		if (child_window->BeginCount == 1)
			parent_window->DC.CursorPos = child_window->Pos;

		// Process navigation-in immediately so NavInit can run on first frame
		if (g.NavActivateId == id && !(flags & ImGuiWindowFlags_NavFlattened) && (child_window->DC.NavLayerActiveMask != 0 || child_window->DC.NavHasScroll))
		{
			FocusWindow(child_window);
			NavInitWindow(child_window, false);
			SetActiveID(id + 1, child_window); // Steal ActiveId with another arbitrary id so that key-press won't activate child item
			g.ActiveIdSource = ImGuiInputSource_Nav;
		}
		parent_window->DrawList->AddRectFilled(ImGui::GetWindowPos(), ImGui::GetWindowPos() + size_arg, ImColor(19, 19, 19), 5);
		parent_window->DrawList->AddRect(ImGui::GetWindowPos(), ImGui::GetWindowPos() + size_arg, ImColor(37, 37, 37), 5);
		return ret;
	}

	bool beginchild(const char* str_id, const ImVec2& size_arg, bool border = false, ImGuiWindowFlags extra_flags = NULL)
	{
		IM_USE;

		ImGuiWindow* window = GetCurrentWindow();
		return beginchildex(str_id, window->GetID(str_id), size_arg, border, extra_flags);
	}

	const char* const KeyNames[] = {
	"Unknown",
	"VK_LBUTTON",
	"VK_RBUTTON",
	"VK_CANCEL",
	"VK_MBUTTON",
	"VK_XBUTTON1",
	"VK_XBUTTON2",
	"Unknown",
	"VK_BACK",
	"VK_TAB",
	"Unknown",
	"Unknown",
	"VK_CLEAR",
	"VK_RETURN",
	"Unknown",
	"Unknown",
	"VK_SHIFT",
	"VK_CONTROL",
	"VK_MENU",
	"VK_PAUSE",
	"VK_CAPITAL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"VK_ESCAPE",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"VK_SPACE",
	"VK_PRIOR",
	"VK_NEXT",
	"VK_END",
	"VK_HOME",
	"VK_LEFT",
	"VK_UP",
	"VK_RIGHT",
	"VK_DOWN",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"VK_SNAPSHOT",
	"VK_INSERT",
	"VK_DELETE",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"VK_LWIN",
	"VK_RWIN",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"VK_NUMPAD0",
	"VK_NUMPAD1",
	"VK_NUMPAD2",
	"VK_NUMPAD3",
	"VK_NUMPAD4",
	"VK_NUMPAD5",
	"VK_NUMPAD6",
	"VK_NUMPAD7",
	"VK_NUMPAD8",
	"VK_NUMPAD9",
	"VK_MULTIPLY",
	"VK_ADD",
	"VK_SEPARATOR",
	"VK_SUBTRACT",
	"VK_DECIMAL",
	"VK_DIVIDE",
	"VK_F1",
	"VK_F2",
	"VK_F3",
	"VK_F4",
	"VK_F5",
	"VK_F6",
	"VK_F7",
	"VK_F8",
	"VK_F9",
	"VK_F10",
	"VK_F11",
	"VK_F12",
	"VK_F13",
	"VK_F14",
	"VK_F15",
	"VK_F16",
	"VK_F17",
	"VK_F18",
	"VK_F19",
	"VK_F20",
	"VK_F21",
	"VK_F22",
	"VK_F23",
	"VK_F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_NUMLOCK",
	"VK_SCROLL",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_LSHIFT",
	"VK_RSHIFT",
	"VK_LCONTROL",
	"VK_RCONTROL",
	"VK_LMENU",
	"VK_RMENU"
	};

	bool Hotkey(const char* label, int* k)
	{
		IM_USE;

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		ImGuiIO& io = g.IO;
		const ImGuiStyle& style = g.Style;

		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		ImVec2 size = ImGui::CalcItemSize({ 140,24 }, 84, 24);
		const ImRect frame_bb(window->DC.CursorPos + ImVec2(15, 0), window->DC.CursorPos + size);
		const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

		ImGui::ItemSize(total_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(total_bb, id))
			return false;

		const bool focus_requested = FocusableItemRegister(window, id);
		const bool focus_requested_by_code = focus_requested && (g.FocusRequestCurrWindow == window && g.FocusRequestCurrCounterRegular == window->DC.FocusCounterRegular);
		const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

		const bool hovered = ImGui::ItemHoverable(frame_bb, id);

		if (hovered) {
			ImGui::SetHoveredID(id);
			g.MouseCursor = ImGuiMouseCursor_TextInput;
		}

		const bool user_clicked = hovered && io.MouseClicked[0];

		if (focus_requested || user_clicked) {
			if (g.ActiveId != id) {
				// Start edition
				memset(io.MouseDown, 0, sizeof(io.MouseDown));
				memset(io.KeysDown, 0, sizeof(io.KeysDown));
				*k = 0;
			}
			ImGui::SetActiveID(id, window);
			ImGui::FocusWindow(window);
		}
		else if (io.MouseClicked[0]) {
			// Release focus when we click outside
			if (g.ActiveId == id)
				ImGui::ClearActiveID();
		}

		bool value_changed = false;
		int key = *k;

		if (g.ActiveId == id) {
			for (auto i = 0; i < 5; i++) {
				if (io.MouseDown[i]) {
					switch (i) {
					case 0:
						key = VK_LBUTTON;
						break;
					case 1:
						key = VK_RBUTTON;
						break;
					case 2:
						key = VK_MBUTTON;
						break;
					case 3:
						key = VK_XBUTTON1;
						break;
					case 4:
						key = VK_XBUTTON2;
						break;
					}
					value_changed = true;
					ImGui::ClearActiveID();
				}
			}
			if (!value_changed) {
				for (auto i = VK_BACK; i <= VK_RMENU; i++) {
					if (io.KeysDown[i]) {
						key = i;
						value_changed = true;
						ImGui::ClearActiveID();
					}
				}
			}

			if (IsKeyPressedMap(ImGuiKey_Escape)) {
				*k = 0;
				ImGui::ClearActiveID();
			}
			else {
				*k = key;
			}
		}

		// Render
		// Select which buffer we are going to display. When ImGuiInputTextFlags_NoLiveEdit is Set 'buf' might still be the old value. We Set buf to NULL to prevent accidental usage from now on.

		char buf_display[64] = "None";

		ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImColor(57, 57, 57), 0, 3);

		if (*k != 0 && g.ActiveId != id) {
			strcpy_s(buf_display, KeyNames[*k]);
		}
		else if (g.ActiveId == id) {
			strcpy_s(buf_display, "...");
		}

		const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y); // Not using frame_bb.Max because we have adjusted size
		ImVec2 render_pos = frame_bb.Min + style.FramePadding;
		ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect);

		//if (label_size.x > 0)
		//	ImGui::RenderText(ImVec2(total_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), label);

		return value_changed;
	}


	////////////////////////////////////////////////

	void closebutton()
	{
		ImGui::SetCursorPos({ 853 - 32, 22 });
		if (ImGui::InvisibleButton("X", { 16,16 }))
			exit(0);
		ImGui::SetCursorPos({ 853 - 32, 22 });
		ImGui::TextColored(ImColor(255, 255, 255), ICON_FA_TIMES);

	}

	void addcombo(const char* label, const char* desc, int* current_item, const char* items_separated_by_zeros, int height_in_items)
	{
		ImGui::PushFont(roboto_medium);
		ImGui::Spacing();
		ImGui::SameLine(15);
		ImGui::TextColored(ImColor(255, 255, 255, 255), label);

		ImGui::Spacing();
		ImGui::SameLine(15);
		ImGui::TextColored(ImColor(97, 97, 97, 255), desc);
		ImGui::PopFont();
		combo(label, current_item, items_separated_by_zeros, height_in_items);
		ImGui::Spacing();
	}

	void addkeybind(const char* label, const char* desc, int* keybind)
	{
		ImGui::PushFont(roboto_medium);
		ImGui::Spacing();
		ImGui::SameLine(15);
		ImGui::TextColored(ImColor(255, 255, 255, 255), label);

		ImGui::Spacing();
		ImGui::SameLine(15);
		ImGui::TextColored(ImColor(97, 97, 97, 255), desc);
		ImGui::PopFont();
		Hotkey(label, keybind);
		ImGui::Spacing();
	}

	void addsliderint(const char* label, const char* desc, int* v, int v_min, int v_max)
	{
		ImGui::PushFont(roboto_medium);
		ImGui::Spacing();
		ImGui::SameLine(15);
		ImGui::TextColored(ImColor(255, 255, 255, 255), label);

		std::string inputint = "##";
		inputint.append(label);

		ImGui::Spacing();
		ImGui::SameLine(15);
		ImGui::TextColored(ImColor(97, 97, 97, 255), desc);
		ImGui::PopFont();
		ImGui::Spacing();
		slider_int(label, v, v_min, v_max, NULL, NULL);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::CalcTextSize(std::to_string(*v).c_str()).x + 10);
		ImGui::InputInt(inputint.c_str(), v, 0, 0);
	}

	void addsliderfloat(const char* label, const char* desc, float* v, float v_min, float v_max)
	{
		ImGui::PushFont(roboto_medium);
		ImGui::Spacing();
		ImGui::SameLine(15);
		ImGui::TextColored(ImColor(255, 255, 255, 255), label);

		std::string inputint = "##";
		inputint.append(label);

		ImGui::Spacing();
		ImGui::SameLine(15);
		ImGui::TextColored(ImColor(97, 97, 97, 255), desc);
		ImGui::PopFont();
		ImGui::Spacing();
		slider_float(label, v, v_min, v_max, NULL, NULL);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::CalcTextSize(std::to_string(*v).c_str()).x + 10);
		ImGui::InputFloat(inputint.c_str(), v, 0, 0);
	}


	////////////////////////////////////////////////

	void render_tabs()
	{
		ImGui::SetCursorPos({ 100,0 });
		ImGui::BeginGroup();
		{
			for (auto i = 0; i < ARRAYSIZE(tabsname); i++)
			{
				tab1(tabsname[i], tabsicons[i], &selected_tab, i);

				if (i != ARRAYSIZE(tabsname) - 1)
					ImGui::SameLine();
			}
		}
		ImGui::EndGroup();
	}

	void render_content()
	{
		if (selected_tab == 0)
		{
			ImGui::SetCursorPos({ 13, 69 });
			ImGui::BeginGroup();
			{
				beginchild("##subtabs", { 181,160 });
				{
					ImGui::Spacing();
					ImGui::Spacing();
					subtab("Category 1", &selected_subtab, 1);
					subtab("Category 2", &selected_subtab, 2);
					subtab("Category 3", &selected_subtab, 3);
					subtab("Category 4", &selected_subtab, 4);
				}
				ImGui::EndChild();
				ImGui::Spacing();
				ImGui::Spacing();
				beginchild("##subtabs2", { 181,231 });
				{
					ImGui::Spacing();
					ImGui::Spacing();

					category("Buttons Select");
					subtab("Button 1", &etc, 1);
					subtab("Button 2", &etc, 2);
					subtab("Button 3", &etc, 3);
					subtab("Button 4", &etc, 4);
					subtab("Button 5", &etc, 5);
				}
				ImGui::EndChild();
			}
			ImGui::EndGroup();

			ImGui::SetCursorPos({ 214, 69 });
			ImGui::BeginChild("##content", ImVec2(629, 476));
			{
				if (selected_subtab == 0)
				{
					beginchild("##subtabs3", { 299,310 });
					{
						ImGui::Spacing();
						ImGui::Spacing();

						category("Child Name");
						static bool checkboxbool2;
						checkbox("label2", "Lorem ipsum dolor sit amet.", &checkboxbool2);
						static bool checkboxbool;
						checkbox("label", "", &checkboxbool);

						static int combo_int;
						addcombo("Combo Preview", "Lorem ipsum dolor sit amet.", &combo_int, ("Checkbox\0\Slider\0\Button\0\rCombo\0\rTab\0\rSub Tab\0\0"), 10);

						static int sliderint;
						addsliderint("Slider", "Lorem ipsum dolor sit amet.", &sliderint, 0, 100);

						static int keybind;
						addkeybind("Keybind", "Lorem ipsum dolor sit amet.", &keybind);
					}
					ImGui::EndChild();
				}
				ImGui::EndChild();

			}
		}
	}

	void render_menu()
	{
		auto flags = ImGuiWindowFlags_NoTitleBar | NULL | NULL | NULL | ImGuiWindowFlags_NoResize | NULL | NULL | NULL | NULL;

		ImGui::SetNextWindowSize({ 853.000000f,559.000000f });

		ImGui::Begin("Edited", nullptr, flags);
		{
			p = ImGui::GetWindowPos();
			draw = ImGui::GetWindowDrawList();

			draw->AddRectFilled(ImVec2(p.x + 0, p.y + 0), ImVec2(p.x + 853, p.y + 54), ImColor(19, 19, 19, 255), 0, 15);
			draw->AddLine(ImVec2(p.x + 0, p.y + 54), ImVec2(p.x + 853, p.y + 54), ImColor(36, 36, 36, 255), 1.000000);

			ImGui::SetCursorPos({ 16,16 });
			ImGui::Image(logotype, { 71,22 });

			render_tabs();
			render_content();
			closebutton();
		}
		ImGui::End();
	}
}

#endif // !QGUIHEADERGUARD