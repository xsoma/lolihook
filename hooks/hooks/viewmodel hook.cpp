#include "../includes.hpp"


#include "../skinchanger/skins.h"
static int fixed_p;
void Skins::skinfix(int itemfix)
{
	fixed_p = itemfix;
}


void shonax_hooks::Viewmodel(recvProxyData& pData, void* pStruct, void* pOut)
{


	int default_t = m_modelinfo()->GetModelIndex("models/weapons/v_knife_default_t.mdl");
	int default_ct = m_modelinfo()->GetModelIndex("models/weapons/v_knife_default_ct.mdl");
	int iBayonet = m_modelinfo()->GetModelIndex("models/weapons/v_knife_bayonet.mdl");
	int iButterfly = m_modelinfo()->GetModelIndex("models/weapons/v_knife_butterfly.mdl");
	int iFlip = m_modelinfo()->GetModelIndex("models/weapons/v_knife_flip.mdl");
	int iGut = m_modelinfo()->GetModelIndex("models/weapons/v_knife_gut.mdl");
	int iKarambit = m_modelinfo()->GetModelIndex("models/weapons/v_knife_karam.mdl");
	int iM9Bayonet = m_modelinfo()->GetModelIndex("models/weapons/v_knife_m9_bay.mdl");
	int iHuntsman = m_modelinfo()->GetModelIndex("models/weapons/v_knife_tactical.mdl");
	int iFalchion = m_modelinfo()->GetModelIndex("models/weapons/v_knife_falchion_advanced.mdl");
	int iDagger = m_modelinfo()->GetModelIndex("models/weapons/v_knife_push.mdl");
	int iBowie = m_modelinfo()->GetModelIndex("models/weapons/v_knife_survival_bowie.mdl");
	int iGunGame = m_modelinfo()->GetModelIndex("models/weapons/v_knife_gg.mdl");
	int Navaja = m_modelinfo()->GetModelIndex("models/weapons/v_knife_gypsy_jackknife.mdl");
	int Stiletto = m_modelinfo()->GetModelIndex("models/weapons/v_knife_stiletto.mdl");
	int Ursus = m_modelinfo()->GetModelIndex("models/weapons/v_knife_ursus.mdl");
	int Talon = m_modelinfo()->GetModelIndex("models/weapons/v_knife_widowmaker.mdl");
	int d1 = m_modelinfo()->GetModelIndex("models/weapons/v_knife_css.mdl");
	int d2 = m_modelinfo()->GetModelIndex("models/weapons/v_knife_skeleton.mdl");
	int d3 = m_modelinfo()->GetModelIndex("models/weapons/v_knife_outdoor.mdl");
	int d4 = m_modelinfo()->GetModelIndex("models/weapons/v_knife_canis.mdl");
	int d5 = m_modelinfo()->GetModelIndex("models/weapons/v_knife_cord.mdl");


	const auto local = g_ctx.local();


	if (local && g_cfg.inventory.activate_inventory)
	{
		if (local->is_alive() && (
			pData.value._int == default_t ||
			pData.value._int == default_ct ||
			pData.value._int == iBayonet ||
			pData.value._int == iFlip ||
			pData.value._int == iGunGame ||
			pData.value._int == iGut ||
			pData.value._int == iKarambit ||
			pData.value._int == iM9Bayonet ||
			pData.value._int == iHuntsman ||
			pData.value._int == iBowie ||
			pData.value._int == iButterfly ||
			pData.value._int == iFalchion ||
			pData.value._int == iDagger ||
			pData.value._int == Navaja ||
			pData.value._int == Stiletto ||
			pData.value._int == Ursus ||
			pData.value._int == Talon ||
			pData.value._int == d1 ||
			pData.value._int == d2 ||
			pData.value._int == d3 ||
			pData.value._int == d4 ||
			pData.value._int == d5))
		{


			if (fixed_p)
				pData.value._int = fixed_p;




		}
	}
	if (viewmodel_hook)
		viewmodel_hook(pData, pStruct, pOut);

}


