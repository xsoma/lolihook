#include "customskinchanger.h"
#include <Windows.h>
#include <iosfwd>
#include <fstream>
#include "listskins.h"



CustomSkins knowledge;



void CustomSkins::setup_materials()
{
	static bool on = false;
	if (!on)
	{
		for (int i = 0; i < 1; i++)
			modelskin[i] = m_materialsystem()->FindMaterial(skinlist[i], "Model textures");
		on = true;
	}
}





IMaterial* CreateMaterialzTest(bool lit, const std::string& material_data)
{
	static auto created = 0;
	std::string type = lit ? crypt_str("VertexLitGeneric") : crypt_str("UnlitGeneric");

	auto matname = crypt_str("legendware_") + std::to_string(created);
	++created;

	auto keyValues = new KeyValues(matname.c_str());
	static auto key_values_address = util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 51 33 C0 C7 45"));

	using KeyValuesFn = void(__thiscall*)(void*, const char*);
	reinterpret_cast <KeyValuesFn> (key_values_address)(keyValues, type.c_str());

	static auto load_from_buffer_address = util::FindSignature(crypt_str("client.dll"), crypt_str("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"));
	using LoadFromBufferFn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*);

	reinterpret_cast <LoadFromBufferFn> (load_from_buffer_address)(keyValues, matname.c_str(), material_data.c_str(), nullptr, nullptr, nullptr);

	auto material = m_materialsystem()->CreateMaterial(matname.c_str(), keyValues);
	material->IncrementReferenceCount();

	return material;
}



void CustomSkins::draw_skins(std::function<void()> original)
{






	static IMaterial* materials_knife_custom[] =
	{
		CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/lightningstrikeskin" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
		CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/dark_vulcano_m9" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
		CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_bubble_crystal" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
		CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_crimson_crystal" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
		CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_inferno_crystal" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
		CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_lava_crystal" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
	CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_nebula_crystal" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
	CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_supernova_crystal" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
	CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_venomus_crystal" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
	CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_vertex_crystal" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
	CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_volcano_crystal" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),


	CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_grafit" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
	CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_evolve" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#")),
	CreateMaterialzTest(true, crypt_str(R"#("VertexLitGeneric" 
			{
				"$basetexture"				"vgui/m9_red_mat" 
				"$ignorez"					"0" 
				"$envmap"					"env_cubemap" 
				"$normalmapalphaenvmapmask" "1" 
				"$envmapcontrast"			"1" 
				"$nofog"					"1" 
				"$model"					"1" 
				"$nocull" 					"0" 
				"$selfillum" 				"1" 
				"$halflambert"				"1" 
				"$znearer" 					"0" 
				"$flat" 					"1"
		        "$wireframe"				"0"
			}
		)#"))





	};




	
	//15

	if (g_cfg.misc.customchams && !isplayer && materials_knife_custom && g_ctx.local())
	{
		if ((g_ctx.globals.weapon && is_knife && is_knife_wep && !is_arm && !is_sleeve))
		{
			switch (g_cfg.misc.customknife)
			{
			case 1:
                m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 2:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 3:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 4:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 5:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 6:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 7:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 8:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 9:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 10:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 11:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 12:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 13:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			case 14:
				m_modelrender()->ForcedMaterialOverride(materials_knife_custom[g_cfg.misc.customknife - 1]);
				break;
			}
		}
		

		//original();
		//m_modelrender()->ForcedMaterialOverride(nullptr);


		//clean shit
		
		
		
		
		//ok im god
			
		


	}

	




}






void CustomSkins::create_materials()
{
	//none
}

