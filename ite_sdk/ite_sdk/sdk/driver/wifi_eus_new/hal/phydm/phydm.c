/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

/* ************************************************************
 * include files
 * ************************************************************ */

#include "mp_precomp.h"
#include "phydm_precomp.h"


const u16 phy_rate_table[] = {	/*20M*/
	1, 2, 5, 11,
	6, 9, 12, 18, 24, 36, 48, 54,
	6, 13, 19, 26, 39, 52, 58, 65,		/*MCS0~7*/
	13, 26, 39, 52, 78, 104, 117, 130		/*MCS8~15*/
};

void
phydm_traffic_load_decision(
	void	*dm_void
)
{
	struct dm_struct	*dm = (struct dm_struct *)dm_void;
	u8		bit_shift_num = 0;

	/*---TP & Trafic-load calculation---*/

	if (dm->last_tx_ok_cnt > *dm->num_tx_bytes_unicast)
		dm->last_tx_ok_cnt = *dm->num_tx_bytes_unicast;

	if (dm->last_rx_ok_cnt > *dm->num_rx_bytes_unicast)
		dm->last_rx_ok_cnt = *dm->num_rx_bytes_unicast;

	dm->cur_tx_ok_cnt = *dm->num_tx_bytes_unicast - dm->last_tx_ok_cnt;
	dm->cur_rx_ok_cnt = *dm->num_rx_bytes_unicast - dm->last_rx_ok_cnt;
	dm->last_tx_ok_cnt = *dm->num_tx_bytes_unicast;
	dm->last_rx_ok_cnt = *dm->num_rx_bytes_unicast;

	bit_shift_num = 17 + (PHYDM_WATCH_DOG_PERIOD - 1); /*AP:  <<3(8bit), >>20(10^6,M), >>0(1sec)*/
													/*WIN&CE:  <<3(8bit), >>20(10^6,M), >>1(2sec)*/

	dm->tx_tp = ((dm->tx_tp) >> 1) + (u32)(((dm->cur_tx_ok_cnt) >> bit_shift_num) >> 1);
	dm->rx_tp = ((dm->rx_tp) >> 1) + (u32)(((dm->cur_rx_ok_cnt) >> bit_shift_num) >> 1);

	dm->total_tp = dm->tx_tp + dm->rx_tp;

	/*[Calculate TX/RX state]*/
	if (dm->tx_tp > (dm->rx_tp << 1))
		dm->txrx_state_all = TX_STATE;
	else if (dm->rx_tp > (dm->tx_tp << 1))
		dm->txrx_state_all = RX_STATE;
	else
		dm->txrx_state_all = BI_DIRECTION_STATE;

	/*[Calculate consecutive idlel time]*/
	if (dm->total_tp == 0)
		dm->consecutive_idlel_time += PHYDM_WATCH_DOG_PERIOD;
	else
		dm->consecutive_idlel_time = 0;

	/*[Traffic load decision]*/
	dm->pre_traffic_load = dm->traffic_load;

	if (dm->cur_tx_ok_cnt > 1875000 || dm->cur_rx_ok_cnt > 1875000) {		/* ( 1.875M * 8bit ) / 2sec= 7.5M bits /sec )*/

		dm->traffic_load = TRAFFIC_HIGH;
		/**/
	} else if (dm->cur_tx_ok_cnt > 500000 || dm->cur_rx_ok_cnt > 500000) { /*( 0.5M * 8bit ) / 2sec =  2M bits /sec )*/

		dm->traffic_load = TRAFFIC_MID;
		/**/
	} else if (dm->cur_tx_ok_cnt > 100000 || dm->cur_rx_ok_cnt > 100000)  { /*( 0.1M * 8bit ) / 2sec =  0.4M bits /sec )*/

		dm->traffic_load = TRAFFIC_LOW;
		/**/
	} else {
		dm->traffic_load = TRAFFIC_ULTRA_LOW;
		/**/
	}

	/*
	PHYDM_DBG(dm, DBG_COMMON_FLOW, "cur_tx_ok_cnt = %d, cur_rx_ok_cnt = %d, last_tx_ok_cnt = %d, last_rx_ok_cnt = %d\n",
		dm->cur_tx_ok_cnt, dm->cur_rx_ok_cnt, dm->last_tx_ok_cnt, dm->last_rx_ok_cnt);

	PHYDM_DBG(dm, DBG_COMMON_FLOW, "tx_tp = %d, rx_tp = %d\n",
		dm->tx_tp, dm->rx_tp);
	*/

}

void
phydm_init_cck_setting(
	struct dm_struct		*dm
)
{
#if (RTL8192E_SUPPORT == 1)
	u32 value_824, value_82c;
#endif

	dm->is_cck_high_power = (boolean) odm_get_bb_reg(dm, ODM_REG(CCK_RPT_FORMAT, dm), ODM_BIT(CCK_RPT_FORMAT, dm));

	phydm_config_cck_rx_antenna_init(dm);
	phydm_config_cck_rx_path(dm, BB_PATH_A);

#if (RTL8192E_SUPPORT == 1)
	if (dm->support_ic_type & (ODM_RTL8192E)) {
		/* 0x824[9] = 0x82C[9] = 0xA80[7]  those registers setting should be equal or CCK RSSI report may be incorrect */
		value_824 = odm_get_bb_reg(dm, 0x824, BIT(9));
		value_82c = odm_get_bb_reg(dm, 0x82c, BIT(9));

		if (value_824 != value_82c)
			odm_set_bb_reg(dm, 0x82c, BIT(9), value_824);
		odm_set_bb_reg(dm, 0xa80, BIT(7), value_824);
		dm->cck_agc_report_type = (boolean)value_824;

		PHYDM_DBG(dm, ODM_COMP_INIT, "cck_agc_report_type = (( %d )), ext_lna_gain = (( %d ))\n", dm->cck_agc_report_type, dm->ext_lna_gain);
	}
#endif

#if ((RTL8703B_SUPPORT == 1) || (RTL8723D_SUPPORT == 1) || (RTL8710B_SUPPORT == 1))
	if (dm->support_ic_type & (ODM_RTL8703B | ODM_RTL8723D | ODM_RTL8710B)) {
		dm->cck_agc_report_type = odm_get_bb_reg(dm, 0x950, BIT(11)) ? 1 : 0; /*1: 4bit LNA, 0: 3bit LNA */

		if (dm->cck_agc_report_type != 1) {
			pr_debug("[Warning] 8703B/8723D/8710B CCK should be 4bit LNA, ie. 0x950[11] = 1\n");
			/**/
		}
	}
#endif

#if (RTL8821C_SUPPORT == 1)
	if (dm->support_ic_type & ODM_RTL8821C) {
		dm->cck_new_agc = odm_get_bb_reg(dm, 0xa9c, BIT(17)) ? true : false;          /*1: new agc  0: old agc*/
		if (dm->cck_new_agc == 0 && dm->default_rf_set_8821c == SWITCH_TO_BTG)
			dm->cck_agc_report_type = 1;
	}
#endif

#if ((RTL8723D_SUPPORT == 1) || (RTL8822B_SUPPORT == 1) || (RTL8197F_SUPPORT == 1) || (RTL8710B_SUPPORT == 1))
	if (dm->support_ic_type & (ODM_RTL8723D | ODM_RTL8822B | ODM_RTL8197F | ODM_RTL8710B))
		dm->cck_new_agc = odm_get_bb_reg(dm, 0xa9c, BIT(17)) ? true : false;          /*1: new agc  0: old agc*/
	else
#endif
	{
		dm->cck_new_agc = false;
		/**/
	}

	phydm_get_cck_rssi_table_from_reg(dm);

}

void
phydm_init_hw_info_by_rfe(
	struct dm_struct		*dm
)
{
#if (RTL8822B_SUPPORT == 1)
	if (dm->support_ic_type & ODM_RTL8822B)
		phydm_init_hw_info_by_rfe_type_8822b(dm);
#endif
#if (RTL8821C_SUPPORT == 1)
	if (dm->support_ic_type & ODM_RTL8821C)
		phydm_init_hw_info_by_rfe_type_8821c(dm);
#endif
#if (RTL8197F_SUPPORT == 1)
	if (dm->support_ic_type & ODM_RTL8197F)
		phydm_init_hw_info_by_rfe_type_8197f(dm);
#endif
}

void
phydm_common_info_self_init(
	struct dm_struct		*dm
)
{
	phydm_init_cck_setting(dm);
	dm->rf_path_rx_enable = (u8) odm_get_bb_reg(dm, ODM_REG(BB_RX_PATH, dm), ODM_BIT(BB_RX_PATH, dm));
#if (DM_ODM_SUPPORT_TYPE != ODM_CE)
	dm->is_net_closed = &dm->BOOLEAN_temp;

	phydm_init_debug_setting(dm);
#endif
	phydm_init_trx_antenna_setting(dm);
	//phydm_init_soft_ml_setting(dm);

	dm->phydm_sys_up_time = 0;

	if (dm->support_ic_type & ODM_IC_1SS)
		dm->num_rf_path = 1;
	else if (dm->support_ic_type & ODM_IC_2SS)
		dm->num_rf_path = 2;
	else if (dm->support_ic_type & ODM_IC_3SS)
		dm->num_rf_path = 3;
	else if (dm->support_ic_type & ODM_IC_4SS)
		dm->num_rf_path = 4;
	else
		dm->num_rf_path = 1;

	dm->tx_rate = 0xFF;
	dm->rssi_min_by_path = 0xFF;

	dm->number_linked_client = 0;
	dm->pre_number_linked_client = 0;
	dm->number_active_client = 0;
	dm->pre_number_active_client = 0;

	dm->last_tx_ok_cnt = 0;
	dm->last_rx_ok_cnt = 0;
	dm->tx_tp = 0;
	dm->rx_tp = 0;
	dm->total_tp = 0;
	dm->traffic_load = TRAFFIC_LOW;

	dm->nbi_set_result = 0;
	dm->is_init_hw_info_by_rfe = false;
	dm->pre_dbg_priority = BB_DBGPORT_RELEASE;
	dm->tp_active_th = 5;
	dm->disable_phydm_watchdog = 0;

	dm->u8_dummy = 0xf;
	dm->u16_dummy = 0xffff;
	dm->u32_dummy = 0xffffffff;

	/*odm_memory_set(dm, &(dm->pause_lv_table.lv_dig), 0, sizeof(struct phydm_pause_lv));*/
	dm->pause_lv_table.lv_cckpd = PHYDM_PAUSE_RELEASE;
	dm->pause_lv_table.lv_dig = PHYDM_PAUSE_RELEASE;

}

void
phydm_cmn_sta_info_update(
	void	*dm_void,
	u8	macid
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;
	struct cmn_sta_info			*sta = dm->phydm_sta_info[macid];
	struct ra_sta_info				*ra = NULL;

	if (is_sta_active(sta)) {
		ra = &sta->ra_info;
	} else {
		PHYDM_DBG(dm, DBG_RA_MASK, "[Warning] %s invalid sta_info\n", __func__);
		return;
	}

	PHYDM_DBG(dm, DBG_RA_MASK, "%s ======>\n", __func__);
	PHYDM_DBG(dm, DBG_RA_MASK, "MACID=%d\n", sta->mac_id);

	/*[Calculate TX/RX state]*/
	if (sta->tx_moving_average_tp > (sta->rx_moving_average_tp << 1))
		ra->txrx_state= TX_STATE;
	else if (sta->rx_moving_average_tp > (sta->tx_moving_average_tp << 1))
		ra->txrx_state = RX_STATE;
	else
		ra->txrx_state = BI_DIRECTION_STATE;

	 ra->is_noisy = dm->noisy_decision;

}

void
phydm_common_info_self_update(
	struct dm_struct		*dm
)
{
	u8	sta_cnt = 0, num_active_client = 0;
	u32	i, one_entry_macid = 0;
	u32	ma_rx_tp = 0;
	struct cmn_sta_info	*sta;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

	PADAPTER	adapter =  (PADAPTER)dm->adapter;

	PMGNT_INFO	mgnt_info = &((PADAPTER)adapter)->MgntInfo;

	sta = dm->phydm_sta_info[0];
	if (mgnt_info->mAssoc) {
		sta->dm_ctrl |= STA_DM_CTRL_ACTIVE;
		for (i = 0; i < 6; i++)
			sta->mac_addr[i] = mgnt_info->Bssid[i];
	} else if (GetFirstClientPort(adapter)) {
		//void	*client_adapter = GetFirstClientPort(adapter);
		struct _ADAPTER	*client_adapter = GetFirstClientPort(adapter);

		sta->dm_ctrl |= STA_DM_CTRL_ACTIVE;
		for (i = 0; i < 6; i++)
			sta->mac_addr[i] = client_adapter->MgntInfo.Bssid[i];
	} else {
		sta->dm_ctrl = sta->dm_ctrl & (~STA_DM_CTRL_ACTIVE);
		for (i = 0; i < 6; i++)
			sta->mac_addr[i] = 0;
	}

	/* STA mode is linked to AP */
	if (is_sta_active(sta) && !ACTING_AS_AP(adapter))
		dm->bsta_state = true;
	else
		dm->bsta_state = false;
#endif

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		sta = dm->phydm_sta_info[i];
		if (is_sta_active(sta)) {
			sta_cnt++;

			if (sta_cnt == 1)
				one_entry_macid = i;

			phydm_cmn_sta_info_update(dm, (u8)i);
			#if (BEAMFORMING_SUPPORT == 1)
			//phydm_get_txbf_device_num(dm, (u8)i);
			#endif

			ma_rx_tp = sta->rx_moving_average_tp + sta->tx_moving_average_tp;
			PHYDM_DBG(dm, DBG_COMMON_FLOW, "TP[%d]: ((%d )) bit/sec\n", i, ma_rx_tp);

			if (ma_rx_tp > ACTIVE_TP_THRESHOLD)
				num_active_client++;
		}
	}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	dm->is_linked = (sta_cnt != 0) ? true : false;
#endif

	if (sta_cnt == 1) {
		dm->is_one_entry_only = true;
		dm->one_entry_macid = one_entry_macid;
		dm->one_entry_tp = ma_rx_tp;

		dm->tp_active_occur = 0;

		PHYDM_DBG(dm, DBG_COMMON_FLOW, "one_entry_tp=((%d)), pre_one_entry_tp=((%d))\n",
			dm->one_entry_tp, dm->pre_one_entry_tp);

		if ((dm->one_entry_tp > dm->pre_one_entry_tp) && (dm->pre_one_entry_tp <= 2)) {
			if ((dm->one_entry_tp - dm->pre_one_entry_tp) > dm->tp_active_th)
				dm->tp_active_occur = 1;
		}
		dm->pre_one_entry_tp = dm->one_entry_tp;
	} else
		dm->is_one_entry_only = false;

	dm->pre_number_linked_client = dm->number_linked_client;
	dm->pre_number_active_client = dm->number_active_client;

	dm->number_linked_client = sta_cnt;
	dm->number_active_client = num_active_client;

	/*Traffic load information update*/
	phydm_traffic_load_decision(dm);

	dm->phydm_sys_up_time += PHYDM_WATCH_DOG_PERIOD;

	dm->is_dfs_band = phydm_is_dfs_band(dm);
	dm->phy_dbg_info.show_phy_sts_cnt = 0;

}

void
phydm_common_info_self_reset(
	struct dm_struct		*dm
)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
	dm->phy_dbg_info.num_qry_beacon_pkt = 0;
#endif
}

void *
phydm_get_structure(
	struct dm_struct		*dm,
	u8			structure_type
)

{
	void	*structure = NULL;
#if RTL8195A_SUPPORT
	switch (structure_type) {
	case	PHYDM_FALSEALMCNT:
		structure = &false_alm_cnt;
		break;

	case	PHYDM_CFOTRACK:
		structure = &dm_cfo_track;
		break;

	case	PHYDM_ADAPTIVITY:
		structure = &dm->adaptivity;
		break;

	default:
		break;
	}

#else
	switch (structure_type) {
	case	PHYDM_FALSEALMCNT:
		structure = &dm->false_alm_cnt;
		break;

	case	PHYDM_CFOTRACK:
		structure = &dm->dm_cfo_track;
		break;

	case	PHYDM_ADAPTIVITY:
		structure = &dm->adaptivity;
		break;

	case	PHYDM_DFS:
		structure = &dm->dfs;
		break;

	default:
		break;
	}

#endif
	return	structure;
}

void
phydm_hw_setting(
	struct dm_struct		*dm
)
{
#if (RTL8821A_SUPPORT == 1)
	if (dm->support_ic_type & ODM_RTL8821)
		odm_hw_setting_8821a(dm);
#endif

#if (RTL8814A_SUPPORT == 1)
	if (dm->support_ic_type & ODM_RTL8814A)
		phydm_hwsetting_8814a(dm);
#endif

#if (RTL8822B_SUPPORT == 1)
	if (dm->support_ic_type & ODM_RTL8822B)
		phydm_hwsetting_8822b(dm);
#endif

#if (RTL8812A_SUPPORT == 1)
	if (dm->support_ic_type & ODM_RTL8812)
		phydm_hwsetting_8812a(dm);
#endif

#if (RTL8197F_SUPPORT == 1)
	if (dm->support_ic_type & ODM_RTL8197F)
		phydm_hwsetting_8197f(dm);
#endif
}


#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN))
u64
phydm_supportability_init_win(
	void		*dm_void
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;
	u64			support_ability = 0;

	switch (dm->support_ic_type) {
	/*---------------N Series--------------------*/
	#if (RTL8188E_SUPPORT == 1)
	case	ODM_RTL8188E:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR			|
			ODM_BB_PRIMARY_CCA;
		break;
	#endif

	#if (RTL8192E_SUPPORT == 1)
	case	ODM_RTL8192E:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR			|
			ODM_BB_PRIMARY_CCA;
		break;
	#endif

	#if (RTL8723B_SUPPORT == 1)
	case	ODM_RTL8723B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR		|
			ODM_BB_PRIMARY_CCA;
		break;
	#endif

	#if (RTL8703B_SUPPORT == 1)
	case	ODM_RTL8703B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8723D_SUPPORT == 1)
	case	ODM_RTL8723D:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/* ODM_BB_PWR_TRAIN	| */
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8710B_SUPPORT == 1)
	case	ODM_RTL8710B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8188F_SUPPORT == 1)
	case	ODM_RTL8188F:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	/*---------------AC Series-------------------*/

	#if ((RTL8812A_SUPPORT == 1) || (RTL8821A_SUPPORT == 1))
	case	ODM_RTL8812:
	case	ODM_RTL8821:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_DYNAMIC_TXPWR	|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8814A_SUPPORT == 1)
	case ODM_RTL8814A:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_DYNAMIC_TXPWR	|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8814B_SUPPORT == 1)
	case ODM_RTL8814B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8822B_SUPPORT == 1)
	case ODM_RTL8822B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR			|
			ODM_BB_ADAPTIVE_SOML;
		break;
	#endif

	#if (RTL8821C_SUPPORT == 1)
	case ODM_RTL8821C:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	default:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;

			pr_debug("[Warning] Supportability Init Warning !!!\n");
		break;

	}

	return support_ability;
}
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_CE))
u64
phydm_supportability_init_ce(
	void		*dm_void
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;
	u64			support_ability = 0;

	switch (dm->support_ic_type) {
	/*---------------N Series--------------------*/
	#if (RTL8188E_SUPPORT == 1)
	case	ODM_RTL8188E:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR			|
			ODM_BB_PRIMARY_CCA;
		break;
	#endif

	#if (RTL8192E_SUPPORT == 1)
	case	ODM_RTL8192E:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR			|
			ODM_BB_PRIMARY_CCA;
		break;
	#endif

	#if (RTL8723B_SUPPORT == 1)
	case	ODM_RTL8723B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR			|
			ODM_BB_PRIMARY_CCA;
		break;
	#endif

	#if (RTL8703B_SUPPORT == 1)
	case	ODM_RTL8703B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8723D_SUPPORT == 1)
	case	ODM_RTL8723D:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/* ODM_BB_PWR_TRAIN	| */
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8710B_SUPPORT == 1)
	case	ODM_RTL8710B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8188F_SUPPORT == 1)
	case	ODM_RTL8188F:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	/*---------------AC Series-------------------*/

	#if ((RTL8812A_SUPPORT == 1) || (RTL8821A_SUPPORT == 1))
	case	ODM_RTL8812:
	case	ODM_RTL8821:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8814A_SUPPORT == 1)
	case ODM_RTL8814A:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8814B_SUPPORT == 1)
	case ODM_RTL8814B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8822B_SUPPORT == 1)
	case ODM_RTL8822B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8821C_SUPPORT == 1)
	case ODM_RTL8821C:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	default:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;

			pr_debug("[Warning] Supportability Init Warning !!!\n");
		break;

	}

	return support_ability;
}
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_AP))
u64
phydm_supportability_init_ap(
	void		*dm_void
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;
	u64			support_ability = 0;

	switch (dm->support_ic_type) {
	/*---------------N Series--------------------*/
	#if (RTL8188E_SUPPORT == 1)
	case	ODM_RTL8188E:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR			|
			ODM_BB_PRIMARY_CCA;
		break;
	#endif

	#if (RTL8192E_SUPPORT == 1)
	case	ODM_RTL8192E:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR			|
			ODM_BB_PRIMARY_CCA;
		break;
	#endif

	#if (RTL8723B_SUPPORT == 1)
	case	ODM_RTL8723B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if ((RTL8198F_SUPPORT == 1) || (RTL8197F_SUPPORT == 1))
	case	ODM_RTL8198F:
	case	ODM_RTL8197F:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ADAPTIVE_SOML	|
			ODM_BB_ENV_MONITOR		|
			ODM_BB_LNA_SAT_CHK		|
			ODM_BB_PRIMARY_CCA;
		break;
	#endif

	/*---------------AC Series-------------------*/

	#if (RTL8881A_SUPPORT == 1)
	case	ODM_RTL8881A:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8814A_SUPPORT == 1)
	case ODM_RTL8814A:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8814B_SUPPORT == 1)
	case ODM_RTL8814B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8822B_SUPPORT == 1)
	case ODM_RTL8822B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			/*ODM_BB_ADAPTIVE_SOML	|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR	;
		break;
	#endif

	#if (RTL8821C_SUPPORT == 1)
	case ODM_RTL8821C:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;

		break;
	#endif

	default:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;

			pr_debug("[Warning] Supportability Init Warning !!!\n");
		break;

	}

	#if 0
	/*[Config Antenna Diveristy]*/
	if (*(dm->enable_antdiv))
		support_ability |= ODM_BB_ANT_DIV;

	/*[Config Adaptivity]*/
	if (*(dm->enable_adaptivity))
		support_ability |= ODM_BB_ADAPTIVITY;
	#endif

	return support_ability;
}
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_IOT))
u64
phydm_supportability_init_iot(
	void		*dm_void
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;
	u64			support_ability = 0;

	switch (dm->support_ic_type) {
	#if (RTL8710B_SUPPORT == 1)
	case	ODM_RTL8710B:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	#if (RTL8195A_SUPPORT == 1)
	case	ODM_RTL8195A:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;
		break;
	#endif

	default:
		support_ability |=
			ODM_BB_DIG				|
			ODM_BB_RA_MASK			|
			/*ODM_BB_DYNAMIC_TXPWR	|*/
			ODM_BB_FA_CNT			|
			ODM_BB_RSSI_MONITOR		|
			ODM_BB_CCK_PD			|
			/*ODM_BB_PWR_TRAIN		|*/
			ODM_BB_RATE_ADAPTIVE	|
			ODM_BB_CFO_TRACKING		|
			ODM_BB_ENV_MONITOR;

			pr_debug("[Warning] Supportability Init Warning !!!\n");
		break;

	}

	return support_ability;
}
#endif

void
phydm_fwoffload_ability_init(
	struct dm_struct		*dm,
	enum phydm_offload_ability	offload_ability
)
{
	switch (offload_ability) {
	case	PHYDM_PHY_PARAM_OFFLOAD:
		if (dm->support_ic_type & (ODM_RTL8814A | ODM_RTL8822B | ODM_RTL8821C))
			dm->fw_offload_ability |= PHYDM_PHY_PARAM_OFFLOAD;
		break;

	case	PHYDM_RF_IQK_OFFLOAD:
		dm->fw_offload_ability |= PHYDM_RF_IQK_OFFLOAD;
		break;

	default:
		PHYDM_DBG(dm, ODM_COMP_INIT, "fwofflad, wrong init type!!\n");
		break;

	}

	PHYDM_DBG(dm, ODM_COMP_INIT,
		"fw_offload_ability = %x\n", dm->fw_offload_ability);

}
void
phydm_fwoffload_ability_clear(
	struct dm_struct		*dm,
	enum phydm_offload_ability	offload_ability
)
{
	switch (offload_ability) {
	case	PHYDM_PHY_PARAM_OFFLOAD:
		if (dm->support_ic_type & (ODM_RTL8814A | ODM_RTL8822B | ODM_RTL8821C))
			dm->fw_offload_ability &= (~PHYDM_PHY_PARAM_OFFLOAD);
		break;

	case	PHYDM_RF_IQK_OFFLOAD:
		dm->fw_offload_ability &= (~PHYDM_RF_IQK_OFFLOAD);
		break;

	default:
		PHYDM_DBG(dm, ODM_COMP_INIT, "fwofflad, wrong init type!!\n");
		break;

	}

	PHYDM_DBG(dm, ODM_COMP_INIT,
		"fw_offload_ability = %x\n", dm->fw_offload_ability);

}

void
phydm_supportability_init(
	void		*dm_void
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;
	u64	support_ability;

	if (*dm->mp_mode == true) {
		support_ability = 0;

		/**/
	} else {
		#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN))
		support_ability = phydm_supportability_init_win(dm);
		#elif (DM_ODM_SUPPORT_TYPE & (ODM_AP))
		support_ability = phydm_supportability_init_ap(dm);
		#elif(DM_ODM_SUPPORT_TYPE & (ODM_CE))
		support_ability = phydm_supportability_init_ce(dm);
		#elif(DM_ODM_SUPPORT_TYPE & (ODM_IOT))
		support_ability = phydm_supportability_init_iot(dm);
		#endif

		/*[Config Antenna Diveristy]*/
		if (IS_FUNC_EN(dm->enable_antdiv))
			support_ability |= ODM_BB_ANT_DIV;

		/*[Config Adaptive SOML]*/
		if (IS_FUNC_EN(dm->en_adap_soml))
			support_ability |= ODM_BB_ADAPTIVE_SOML;

		/*[Config Adaptivity]*/
		if (IS_FUNC_EN(dm->enable_adaptivity))
			support_ability |= ODM_BB_ADAPTIVITY;
	}
	odm_cmn_info_init(dm, ODM_CMNINFO_ABILITY, support_ability);
	PHYDM_DBG(dm, ODM_COMP_INIT, "IC = ((0x%x)), Supportability Init = ((0x%llx))\n", dm->support_ic_type, dm->support_ability);
}

void
phydm_rfe_init(
	void			*dm_void
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;

	PHYDM_DBG(dm, ODM_COMP_INIT, "RFE_Init\n");
#if (RTL8822B_SUPPORT == 1)
	if (dm->support_ic_type == ODM_RTL8822B) {
		phydm_rfe_8822b_init(dm);
		/**/
	}
#endif
}

void
phydm_dm_early_init(
	struct dm_struct	*dm
)
{
	#if (DM_ODM_SUPPORT_TYPE & ODM_WIN)
	halrf_init(dm);
	#endif
}

void
odm_dm_init(
	struct dm_struct		*dm
)
{
	halrf_init(dm);
	phydm_supportability_init(dm);
	phydm_rfe_init(dm);
	phydm_common_info_self_init(dm);
	phydm_rx_phy_status_init(dm);
	phydm_auto_dbg_engine_init(dm);
	phydm_dig_init(dm);
	phydm_cck_pd_init(dm);
	phydm_env_monitor_init(dm);
	phydm_adaptivity_init(dm);
	phydm_ra_info_init(dm);
	phydm_rssi_monitor_init(dm);
	phydm_cfo_tracking_init(dm);
	phydm_rf_init(dm);
	phydm_dc_cancellation(dm);
#ifdef PHYDM_TXA_CALIBRATION
	phydm_txcurrentcalibration(dm);
	phydm_get_pa_bias_offset(dm);
#endif
	odm_antenna_diversity_init(dm);
	phydm_adaptive_soml_init(dm);
#ifdef CONFIG_DYNAMIC_RX_PATH
	phydm_dynamic_rx_path_init(dm);
#endif
	phydm_path_diversity_init(dm);
	phydm_pow_train_init(dm);
	phydm_dynamic_tx_power_init(dm);
#if (PHYDM_LA_MODE_SUPPORT == 1)
	adc_smp_init(dm);
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
	phydm_beamforming_init(dm);
#endif
#if (RTL8188E_SUPPORT == 1)
	odm_ra_info_init_all(dm);
#endif

	phydm_primary_cca_init(dm);

	#ifdef CONFIG_PSD_TOOL
	phydm_psd_init(dm);
	#endif

	#ifdef CONFIG_SMART_ANTENNA
	phydm_smt_ant_init(dm);
	#endif

}

void
odm_dm_reset(
	struct dm_struct		*dm
)
{
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;

	odm_ant_div_reset(dm);
	phydm_set_edcca_threshold_api(dm, dig_t->cur_ig_value);
}

void
phydm_support_ability_debug(
	void		*dm_void,
	u32		*const dm_value,
	u32			*_used,
	char			*output,
	u32			*_out_len
)
{
	struct dm_struct		*dm = (struct dm_struct *)dm_void;
	u64			pre_support_ability, one = 1;
	u32 used = *_used;
	u32 out_len = *_out_len;

	pre_support_ability = dm->support_ability;

	PDM_SNPF(out_len, used, output + used, out_len - used, "\n%s\n",
		       "================================");
	if (dm_value[0] == 100) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "[Supportability] PhyDM Selection\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "%s\n", "================================");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "00. (( %s ))DIG\n",
			       ((dm->support_ability & ODM_BB_DIG) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "01. (( %s ))RA_MASK\n",
			       ((dm->support_ability & ODM_BB_RA_MASK) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "02. (( %s ))DYN_TXPWR\n",
			       ((dm->support_ability & ODM_BB_DYNAMIC_TXPWR) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "03. (( %s ))FA_CNT\n",
			       ((dm->support_ability & ODM_BB_FA_CNT) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "04. (( %s ))RSSI_MNTR\n",
			       ((dm->support_ability & ODM_BB_RSSI_MONITOR) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "05. (( %s ))CCK_PD\n",
			       ((dm->support_ability & ODM_BB_CCK_PD) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "06. (( %s ))ANT_DIV\n",
			       ((dm->support_ability & ODM_BB_ANT_DIV) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "07. (( %s ))SMT_ANT\n",
			       ((dm->support_ability & ODM_BB_SMT_ANT) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "08. (( %s ))PWR_TRAIN\n",
			       ((dm->support_ability & ODM_BB_PWR_TRAIN) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "09. (( %s ))RA\n",
			       ((dm->support_ability & ODM_BB_RATE_ADAPTIVE) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "10. (( %s ))PATH_DIV\n",
			       ((dm->support_ability & ODM_BB_PATH_DIV) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "11. (( %s ))DFS\n",
			       ((dm->support_ability & ODM_BB_DFS) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "12. (( %s ))DYN_ARFR\n",
			       ((dm->support_ability & ODM_BB_DYNAMIC_ARFR) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "13. (( %s ))ADAPTIVITY\n",
			       ((dm->support_ability & ODM_BB_ADAPTIVITY) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "14. (( %s ))CFO_TRACK\n",
			       ((dm->support_ability & ODM_BB_CFO_TRACKING) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "15. (( %s ))ENV_MONITOR\n",
			       ((dm->support_ability & ODM_BB_ENV_MONITOR) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "16. (( %s ))PRI_CCA\n",
			       ((dm->support_ability & ODM_BB_PRIMARY_CCA) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "17. (( %s ))ADPTV_SOML\n",
			       ((dm->support_ability & ODM_BB_ADAPTIVE_SOML) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "18. (( %s ))NA_SAT_CHK\n",
			       ((dm->support_ability & ODM_BB_LNA_SAT_CHK) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "19. (( %s ))DYN_RX_PATH\n",
			       ((dm->support_ability & ODM_BB_DYNAMIC_RX_PATH) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "%s\n", "================================");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "[Supportability] PhyDM offload ability\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "%s\n", "================================");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "00. (( %s ))PHY PARAM OFFLOAD\n",
			       ((dm->fw_offload_ability & PHYDM_PHY_PARAM_OFFLOAD) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "01. (( %s ))RF IQK OFFLOAD\n",
			       ((dm->fw_offload_ability & PHYDM_RF_IQK_OFFLOAD) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			       "%s\n", "================================");

	}
	/*
	else if(dm_value[0] == 101)
	{
		dm->support_ability = 0 ;
		dbg_print("Disable all support_ability components\n");
		PDM_SNPF((output+used, out_len-used,"%s\n", "Disable all support_ability components"));
	}
	*/
	else {
		if (dm_value[1] == 1) { /* enable */
			dm->support_ability |= (one << dm_value[0]);
			if (BIT(dm_value[0]) & ODM_BB_PATH_DIV)
				phydm_path_diversity_init(dm);
		} else if (dm_value[1] == 2)	/* disable */
			dm->support_ability &= ~(one << dm_value[0]);
		else
			PDM_SNPF(out_len, used, output + used,
				       out_len - used, "%s\n",
				       "[Warning!!!]  1:enable,  2:disable");
	}
	PDM_SNPF(out_len, used, output + used, out_len - used,
		       "pre-support_ability  =  0x%llx\n",
		         pre_support_ability);
	PDM_SNPF(out_len, used, output + used, out_len - used,
		       "Curr-support_ability =  0x%llx\n",
		       dm->support_ability);
	PDM_SNPF(out_len, used, output + used, out_len - used, "%s\n",
		       "================================");

	*_used = used;
	*_out_len = out_len;
}

void
phydm_watchdog_lps_32k(
	struct dm_struct		*dm
)
{
	PHYDM_DBG(dm, DBG_COMMON_FLOW, "%s ======>\n", __func__);

	phydm_common_info_self_update(dm);
	phydm_rssi_monitor_check(dm);
	phydm_dig_lps_32k(dm);
	phydm_common_info_self_reset(dm);
}

void
phydm_watchdog_lps(
	struct dm_struct		*dm
)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
	PHYDM_DBG(dm, DBG_COMMON_FLOW, "%s ======>\n", __func__);

	phydm_common_info_self_update(dm);
	phydm_rssi_monitor_check(dm);
	phydm_basic_dbg_message(dm);
	phydm_receiver_blocking(dm);
	odm_false_alarm_counter_statistics(dm);
	phydm_dig_by_rssi_lps(dm);
	phydm_cck_pd_th(dm);
	phydm_adaptivity(dm);
	#if (DM_ODM_SUPPORT_TYPE & (ODM_CE))
	odm_antenna_diversity(dm); /*enable AntDiv in PS mode, request from SD4 Jeff*/
	#endif
	phydm_common_info_self_reset(dm);
#endif
}

void
phydm_watchdog_mp(
	struct dm_struct		*dm
)
{
#ifdef CONFIG_DYNAMIC_RX_PATH
	phydm_dynamic_rx_path_caller(dm);
#endif
}

void
phydm_pause_dm_watchdog(
	void					*dm_void,
	enum phydm_pause_type		pause_type
)
{
	struct dm_struct			*dm = (struct dm_struct *)dm_void;

	if (pause_type == PHYDM_PAUSE) {
		dm->disable_phydm_watchdog = 1;
		PHYDM_DBG(dm, ODM_COMP_API, "PHYDM Stop\n");
	} else {
		dm->disable_phydm_watchdog = 0;
		PHYDM_DBG(dm, ODM_COMP_API, "PHYDM Start\n");
	}
}

u8
phydm_pause_func(
	void						*dm_void,
	enum phydm_func_idx	pause_func,
	enum phydm_pause_type	pause_type,
	enum phydm_pause_level	pause_lv,
	u8						val_lehgth,
	u32						*val_buf
)
{
	struct dm_struct	*dm = (struct dm_struct *)dm_void;
	s8	*pause_lv_pre = &dm->s8_dummy;
	u32	*bkp_val = &dm->u32_dummy;
	u32	ori_val[5] = {0};
	u64	pause_func_bitmap = (u64)BIT(pause_func);
	u8	i;



	PHYDM_DBG(dm, ODM_COMP_API, "[%s][%s] LV=%d, Len=%d\n", __func__,
		((pause_type == PHYDM_PAUSE) ? "Pause" : "Resume"),  pause_lv, val_lehgth);

	if (pause_lv >= PHYDM_PAUSE_MAX_NUM) {
		PHYDM_DBG(dm, ODM_COMP_API, "[WARNING] Wrong LV=%d\n", pause_lv);
		return PAUSE_FAIL;
	}

	if (pause_func == F00_DIG) {
		PHYDM_DBG(dm, ODM_COMP_API, "[DIG]\n");

		if (val_lehgth != 1) {
			PHYDM_DBG(dm, ODM_COMP_API, "[WARNING] val_length != 1\n");
			return PAUSE_FAIL;
		}

		ori_val[0] = (u32)(dm->dm_dig_table.cur_ig_value); /*0xc50*/
		pause_lv_pre = &dm->pause_lv_table.lv_dig;
		bkp_val = (u32*)(&dm->dm_dig_table.rvrt_val);
		dm->phydm_func_handler.pause_phydm_handler = phydm_set_dig_val; /*function pointer hook*/

	} else

#ifdef PHYDM_SUPPORT_CCKPD
	if (pause_func == F05_CCK_PD) {

		PHYDM_DBG(dm, ODM_COMP_API, "[CCK_PD]\n");

		if (val_lehgth != 2) {
			PHYDM_DBG(dm, ODM_COMP_API, "[WARNING] val_length != 2\n");
			return PAUSE_FAIL;
		}

		ori_val[0] = dm->dm_cckpd_table.cur_cck_cca_thres; /*0xa0a*/
		ori_val[1] = dm->dm_cckpd_table.cck_cca_th_aaa;	/*0xaaa*/
		pause_lv_pre = &dm->pause_lv_table.lv_cckpd;
		bkp_val = &dm->dm_cckpd_table.rvrt_val[0];
		dm->phydm_func_handler.pause_phydm_handler = phydm_set_cckpd_val; /*function pointer hook*/

	} else
#endif

#ifdef CONFIG_PHYDM_ANTENNA_DIVERSITY
	if (pause_func == F06_ANT_DIV) {
		PHYDM_DBG(dm, ODM_COMP_API, "[AntDiv]\n");

		if (val_lehgth != 1) {
			PHYDM_DBG(dm, ODM_COMP_API, "[WARNING] val_length != 1\n");
			return PAUSE_FAIL;
		}

		ori_val[0] = (u32)(dm->dm_fat_table.rx_idle_ant); /*default antenna*/
		pause_lv_pre = &dm->pause_lv_table.lv_antdiv;
		bkp_val = (u32*)(&dm->dm_fat_table.rvrt_val);
		dm->phydm_func_handler.pause_phydm_handler = phydm_set_antdiv_val; /*function pointer hook*/

	} else
#endif

	if (pause_func == F13_ADPTVTY) {
		PHYDM_DBG(dm, ODM_COMP_API, "[Adaptivity]\n");

		if (val_lehgth != 2) {
			PHYDM_DBG(dm, ODM_COMP_API, "[WARNING] val_length != 2\n");
			return PAUSE_FAIL;
		}

		ori_val[0] = (u32)(dm->adaptivity.th_l2h);	/*th_l2h*/
		ori_val[1] = (u32)(dm->adaptivity.th_h2l);	/*th_h2l*/
		pause_lv_pre = &dm->pause_lv_table.lv_adapt;
		bkp_val = (u32 *)(&dm->adaptivity.rvrt_val);
		dm->phydm_func_handler.pause_phydm_handler = phydm_set_edcca_val; /*function pointer hook*/

	} else

	{
		PHYDM_DBG(dm, ODM_COMP_API, "[WARNING] error func idx\n");
		return PAUSE_FAIL;
	}

	PHYDM_DBG(dm, ODM_COMP_API, "Pause_LV{new , pre} = {%d ,%d}\n", pause_lv, *pause_lv_pre);

	if ((pause_type == PHYDM_PAUSE) || (pause_type == PHYDM_PAUSE_NO_SET)) {
		if (pause_lv <= *pause_lv_pre) {
			PHYDM_DBG(dm, ODM_COMP_API, "[PAUSE FAIL] Pre_LV >= Curr_LV\n");
			return PAUSE_FAIL;
		}

		if (!(dm->pause_ability & pause_func_bitmap)) {
			for (i = 0; i < val_lehgth; i ++)
				bkp_val[i] = ori_val[i];
		}

		dm->pause_ability |= pause_func_bitmap;
		PHYDM_DBG(dm, ODM_COMP_API, "pause_ability=0x%llx\n", dm->pause_ability);

		if (pause_type == PHYDM_PAUSE) {
			for (i = 0; i < val_lehgth; i ++) {
				PHYDM_DBG(dm, ODM_COMP_API, "[PAUSE SUCCESS] val_idx[%d]{New, Ori}={0x%x, 0x%x}\n",i, val_buf[i], bkp_val[i]);
				/**/
			}
			dm->phydm_func_handler.pause_phydm_handler(dm, val_buf, val_lehgth);
		} else {
			for (i = 0; i < val_lehgth; i ++) {
				PHYDM_DBG(dm, ODM_COMP_API, "[PAUSE NO Set: SUCCESS] val_idx[%d]{Ori}={0x%x}\n",i, bkp_val[i]);
				/**/
			}
		}

		*pause_lv_pre = pause_lv;
		return PAUSE_SUCCESS;

	} else if (pause_type == PHYDM_RESUME) {
		dm->pause_ability &= ~pause_func_bitmap;
		PHYDM_DBG(dm, ODM_COMP_API, "pause_ability=0x%llx\n", dm->pause_ability);

		*pause_lv_pre = PHYDM_PAUSE_RELEASE;

		for (i = 0; i < val_lehgth; i ++) {
			PHYDM_DBG(dm, ODM_COMP_API, "[RESUME] val_idx[%d]={0x%x}\n", i, bkp_val[i]);
		}

		dm->phydm_func_handler.pause_phydm_handler(dm, bkp_val, val_lehgth);

		return PAUSE_SUCCESS;
	} else {
		PHYDM_DBG(dm, ODM_COMP_API, "[WARNING] error pause_type\n");
		return PAUSE_FAIL;
	}

}

void
phydm_pause_func_console(
	void		*dm_void,
	char		input[][16],
	u32		*_used,
	char		*output,
	u32		*_out_len,
	u32		input_num
)
{
	struct dm_struct	*dm = (struct dm_struct *)dm_void;
	char		help[] = "-h";
	u32		var1[10] = {0};
	u32		used = *_used;
	u32		out_len = *_out_len;
	u32		i;
	u8		val_length = 0;
	u32		val_buf[5] = {0};
	u8		set_result = 0;
	enum phydm_func_idx	func = (enum phydm_func_idx)0;
	enum phydm_pause_type	pause_type = (enum phydm_pause_type)0;
	enum phydm_pause_level	pause_lv = (enum phydm_pause_level)0;

	if ((strcmp(input[1], help) == 0)) {
		PDM_SNPF(out_len, used, output + used, out_len - used, "{Func} {1:pause, 2:Resume} {lv} Val[5:0]\n");

	} else {
		for (i = 0; i < 10; i++) {
			if (input[i + 1]) {
				PHYDM_SSCANF(input[i + 1], DCMD_HEX, &var1[i]);
			}
		}

		func = (enum phydm_func_idx)var1[0];
		pause_type = (enum phydm_pause_type)var1[1];
		pause_lv = (enum phydm_pause_level)var1[2];


		for (i = 0; i < 5; i++) {
			val_buf[i] = var1[3 + i];
		}

		if (func == F00_DIG) {
			PDM_SNPF(out_len, used, output + used, out_len - used, "[DIG]\n");
			val_length = 1;

		} else if (func == F05_CCK_PD) {
			PDM_SNPF(out_len, used, output + used, out_len - used, "[CCK_PD]\n");
			val_length = 2;
		} else if (func == F06_ANT_DIV) {
			PDM_SNPF(out_len, used, output + used, out_len - used, "[Ant_Div]\n");
			val_length = 1;
		} else if (func == F13_ADPTVTY) {
			PDM_SNPF(out_len, used, output + used, out_len - used, "[Adaptivity]\n");
			val_length = 2;
		} else {
			PDM_SNPF(out_len, used, output + used, out_len - used, "[Set Function Error]\n");
			val_length = 0;
		}

		if (val_length != 0) {

			PDM_SNPF(out_len, used, output + used, out_len - used, "{%s, lv=%d} val = %d, %d}\n",
				       ((pause_type == PHYDM_PAUSE) ? "Pause" : "Resume"),
				       pause_lv, var1[3], var1[4]);

			set_result= phydm_pause_func(dm, func, pause_type, pause_lv, val_length, val_buf);
		}

		PDM_SNPF(out_len, used, output + used, out_len - used, "set_result = %d\n",
			set_result);
	}


	*_used = used;
	*_out_len = out_len;
}

u8
phydm_stop_dm_watchdog_check(
	void					*dm_void
)
{
	struct dm_struct			*dm = (struct dm_struct *)dm_void;

	if (dm->disable_phydm_watchdog == 1) {
		PHYDM_DBG(dm, DBG_COMMON_FLOW, "Disable phydm\n");
		return true;
	} else
		return false;

}

/*
 * 2011/09/20 MH This is the entry pointer for all team to execute HW out source DM.
 * You can not add any dummy function here, be care, you can only use DM structure
 * to perform any new ODM_DM.
 *   */
void
phydm_watchdog(
	struct dm_struct		*dm
)
{
	PHYDM_DBG(dm, DBG_COMMON_FLOW, "%s ======>\n", __func__);

	phydm_common_info_self_update(dm);
	phydm_rssi_monitor_check(dm);
	phydm_basic_dbg_message(dm);
	phydm_auto_dbg_engine(dm);
	phydm_receiver_blocking(dm);

	if (phydm_stop_dm_watchdog_check(dm) == true)
		return;

	phydm_hw_setting(dm);

	#ifdef PHYDM_TDMA_DIG_SUPPORT
	if (dm->original_dig_restore == 0)
		phydm_tdma_dig_timer_check(dm);
	else
	#endif
	{
		odm_false_alarm_counter_statistics(dm);
		//phydm_noisy_detection(dm);
		phydm_dig(dm);
		phydm_cck_pd_th(dm);
	}

#ifdef PHYDM_POWER_TRAINING_SUPPORT
	phydm_update_power_training_state(dm);
#endif
	phydm_adaptivity(dm);
	phydm_ra_info_watchdog(dm);
	odm_path_diversity(dm);
	phydm_cfo_tracking(dm);
	/* odm_dynamic_tx_power(dm); */
	phydm_dynamic_tx_power(dm);
	odm_antenna_diversity(dm);
	phydm_adaptive_soml(dm);
#ifdef CONFIG_DYNAMIC_RX_PATH
	phydm_dynamic_rx_path(dm);
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
	phydm_beamforming_watchdog(dm);
#endif

	halrf_watchdog(dm);
	phydm_primary_cca(dm);

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	odm_dtc(dm);
#endif

	phydm_env_mntr_watchdog(dm);

#ifdef PHYDM_LNA_SAT_CHK_SUPPORT
	phydm_lna_sat_chk_watchdog(dm);
#endif

	phydm_common_info_self_reset(dm);

}


/*
 * Init /.. Fixed HW value. Only init time.
 *   */
void
odm_cmn_info_init(
	struct dm_struct		*dm,
	enum odm_cmninfo	cmn_info,
	u64			value
)
{
	/*  */
	/* This section is used for init value */
	/*  */
	switch	(cmn_info) {
	/*  */
	/* Fixed ODM value. */
	/*  */
	case	ODM_CMNINFO_ABILITY:
		dm->support_ability = (u64)value;
		break;

	case	ODM_CMNINFO_RF_TYPE:
		dm->rf_type = (u8)value;
		break;

	case	ODM_CMNINFO_PLATFORM:
		dm->support_platform = (u8)value;
		break;

	case	ODM_CMNINFO_INTERFACE:
		dm->support_interface = (u8)value;
		break;

	case	ODM_CMNINFO_MP_TEST_CHIP:
		dm->is_mp_chip = (u8)value;
		break;

	case	ODM_CMNINFO_IC_TYPE:
		dm->support_ic_type = (u32)value;
		break;

	case	ODM_CMNINFO_CUT_VER:
		dm->cut_version = (u8)value;
		break;

	case	ODM_CMNINFO_FAB_VER:
		dm->fab_version = (u8)value;
		break;

	case	ODM_CMNINFO_RFE_TYPE:
		#if (RTL8821C_SUPPORT == 1)
		if (dm->support_ic_type & ODM_RTL8821C)
			dm->rfe_type_expand = (u8)value; /**/
		else
		#endif
			dm->rfe_type = (u8)value;
		phydm_init_hw_info_by_rfe(dm);
		break;

	case    ODM_CMNINFO_RF_ANTENNA_TYPE:
		dm->ant_div_type = (u8)value;
		break;

	case	ODM_CMNINFO_WITH_EXT_ANTENNA_SWITCH:
		dm->with_extenal_ant_switch = (u8)value;
		break;

	case    ODM_CMNINFO_BE_FIX_TX_ANT:
		dm->dm_fat_table.b_fix_tx_ant = (u8)value;
		break;

	case	ODM_CMNINFO_BOARD_TYPE:
		if (!dm->is_init_hw_info_by_rfe)
			dm->board_type = (u8)value;
		break;

	case	ODM_CMNINFO_PACKAGE_TYPE:
		if (!dm->is_init_hw_info_by_rfe)
			dm->package_type = (u8)value;
		break;

	case	ODM_CMNINFO_EXT_LNA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->ext_lna = (u8)value;
		break;

	case	ODM_CMNINFO_5G_EXT_LNA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->ext_lna_5g = (u8)value;
		break;

	case	ODM_CMNINFO_EXT_PA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->ext_pa = (u8)value;
		break;

	case	ODM_CMNINFO_5G_EXT_PA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->ext_pa_5g = (u8)value;
		break;

	case	ODM_CMNINFO_GPA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->type_gpa = (u16)value;
		break;

	case	ODM_CMNINFO_APA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->type_apa = (u16)value;
		break;

	case	ODM_CMNINFO_GLNA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->type_glna = (u16)value;
		break;

	case	ODM_CMNINFO_ALNA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->type_alna = (u16)value;
		break;

	case	ODM_CMNINFO_EXT_TRSW:
		if (!dm->is_init_hw_info_by_rfe)
			dm->ext_trsw = (u8)value;
		break;
	case	ODM_CMNINFO_EXT_LNA_GAIN:
		dm->ext_lna_gain = (u8)value;
		break;
	case	ODM_CMNINFO_PATCH_ID:
		dm->iot_table.win_patch_id = (u8)value;
		break;
	case	ODM_CMNINFO_BINHCT_TEST:
		dm->is_in_hct_test = (boolean)value;
		break;
	case	ODM_CMNINFO_BWIFI_TEST:
		dm->wifi_test = (u8)value;
		break;
	case	ODM_CMNINFO_SMART_CONCURRENT:
		dm->is_dual_mac_smart_concurrent = (boolean)value;
		break;
	case	ODM_CMNINFO_DOMAIN_CODE_2G:
		dm->odm_regulation_2_4g = (u8)value;
		break;
	case	ODM_CMNINFO_DOMAIN_CODE_5G:
		dm->odm_regulation_5g = (u8)value;
		break;
#if (DM_ODM_SUPPORT_TYPE &  (ODM_AP))
	case	ODM_CMNINFO_CONFIG_BB_RF:
		dm->config_bbrf = (boolean)value;
		break;
#endif
	case	ODM_CMNINFO_IQKPAOFF:
		dm->rf_calibrate_info.is_iqk_pa_off = (boolean)value;
		break;
	case	ODM_CMNINFO_REGRFKFREEENABLE:
		dm->rf_calibrate_info.reg_rf_kfree_enable = (u8)value;
		break;
	case	ODM_CMNINFO_RFKFREEENABLE:
		dm->rf_calibrate_info.rf_kfree_enable = (u8)value;
		break;
	case	ODM_CMNINFO_NORMAL_RX_PATH_CHANGE:
		dm->normal_rx_path = (u8)value;
		break;
	case	ODM_CMNINFO_EFUSE0X3D8:
		dm->efuse0x3d8 = (u8)value;
		break;
	case	ODM_CMNINFO_EFUSE0X3D7:
		dm->efuse0x3d7 = (u8)value;
		break;
	case	ODM_CMNINFO_ADVANCE_OTA:
		dm->p_advance_ota = (u8)value;
		break;

#ifdef CONFIG_PHYDM_DFS_MASTER
	case	ODM_CMNINFO_DFS_REGION_DOMAIN:
		dm->dfs_region_domain = (u8)value;
		break;
#endif
	case	ODM_CMNINFO_SOFT_AP_SPECIAL_SETTING:
		dm->soft_ap_special_setting = (u32)value;
		break;

	case	ODM_CMNINFO_DPK_EN:
		/*dm->dpk_en = (u1Byte)value;*/
		halrf_cmn_info_set(dm, HALRF_CMNINFO_DPK_EN, (u64)value);
		break;

	case	ODM_CMNINFO_HP_HWID:
		dm->hp_hw_id = (boolean)value;
		break;
	/* To remove the compiler warning, must add an empty default statement to handle the other values. */
	default:
		/* do nothing */
		break;

	}

}


void
odm_cmn_info_hook(
	struct dm_struct		*dm,
	enum odm_cmninfo	cmn_info,
	void			*value
)
{
	/*  */
	/* Hook call by reference pointer. */
	/*  */
	switch	(cmn_info) {
	/*  */
	/* Dynamic call by reference pointer. */
	/*  */
	case	ODM_CMNINFO_TX_UNI:
		dm->num_tx_bytes_unicast = (u64 *)value;
		break;

	case	ODM_CMNINFO_RX_UNI:
		dm->num_rx_bytes_unicast = (u64 *)value;
		break;

	case	ODM_CMNINFO_BAND:
		dm->band_type = (u8 *)value;
		break;

	case	ODM_CMNINFO_SEC_CHNL_OFFSET:
		dm->sec_ch_offset = (u8 *)value;
		break;

	case	ODM_CMNINFO_SEC_MODE:
		dm->security = (u8 *)value;
		break;

	case	ODM_CMNINFO_BW:
		dm->band_width = (u8 *)value;
		break;

	case	ODM_CMNINFO_CHNL:
		dm->channel = (u8 *)value;
		break;

	case	ODM_CMNINFO_SCAN:
		dm->is_scan_in_process = (boolean *)value;
		break;

	case	ODM_CMNINFO_POWER_SAVING:
		dm->is_power_saving = (boolean *)value;
		break;

	case	ODM_CMNINFO_ONE_PATH_CCA:
		dm->one_path_cca = (u8 *)value;
		break;

	case	ODM_CMNINFO_DRV_STOP:
		dm->is_driver_stopped = (boolean *)value;
		break;

	case	ODM_CMNINFO_PNP_IN:
		dm->is_driver_is_going_to_pnp_set_power_sleep = (boolean *)value;
		break;

	case	ODM_CMNINFO_INIT_ON:
		dm->pinit_adpt_in_progress = (boolean *)value;
		break;

	case	ODM_CMNINFO_ANT_TEST:
		dm->antenna_test = (u8 *)value;
		break;

	case	ODM_CMNINFO_NET_CLOSED:
		dm->is_net_closed = (boolean *)value;
		break;

	case	ODM_CMNINFO_FORCED_RATE:
		dm->forced_data_rate = (u16 *)value;
		break;
	case ODM_CMNINFO_ANT_DIV:
		dm->enable_antdiv = (u8 *)value;
		break;

	case ODM_CMNINFO_ADAPTIVE_SOML:
		dm->en_adap_soml = (u8 *)value;
		break;

	case ODM_CMNINFO_ADAPTIVITY:
		dm->enable_adaptivity = (u8 *)value;
		break;

	case	ODM_CMNINFO_P2P_LINK:
		dm->dm_dig_table.is_p2p_in_process = (u8 *)value;
		break;

	case	ODM_CMNINFO_IS1ANTENNA:
		dm->is_1_antenna = (boolean *)value;
		break;

	case	ODM_CMNINFO_RFDEFAULTPATH:
		dm->rf_default_path = (u8 *)value;
		break;

	case	ODM_CMNINFO_FCS_MODE:
		dm->is_fcs_mode_enable = (boolean *)value;
		break;
	/*add by YuChen for beamforming PhyDM*/
	case	ODM_CMNINFO_HUBUSBMODE:
		dm->hub_usb_mode = (u8 *)value;
		break;
	case	ODM_CMNINFO_FWDWRSVDPAGEINPROGRESS:
		dm->is_fw_dw_rsvd_page_in_progress = (boolean *)value;
		break;
	case	ODM_CMNINFO_TX_TP:
		dm->current_tx_tp = (u32 *)value;
		break;
	case	ODM_CMNINFO_RX_TP:
		dm->current_rx_tp = (u32 *)value;
		break;
	case	ODM_CMNINFO_SOUNDING_SEQ:
		dm->sounding_seq = (u8 *)value;
		break;
#ifdef CONFIG_PHYDM_DFS_MASTER
	case	ODM_CMNINFO_DFS_MASTER_ENABLE:
		dm->dfs_master_enabled = (u8 *)value;
		break;
#endif
	case	ODM_CMNINFO_FORCE_TX_ANT_BY_TXDESC:
		dm->dm_fat_table.p_force_tx_ant_by_desc = (u8 *)value;
		break;
	case	ODM_CMNINFO_SET_S0S1_DEFAULT_ANTENNA:
		dm->dm_fat_table.p_default_s0_s1 = (u8 *)value;
		break;
	case	ODM_CMNINFO_SOFT_AP_MODE:
		dm->soft_ap_mode = (u32 *)value;
		break;
	case ODM_CMNINFO_MP_MODE:
		dm->mp_mode = (u8 *)value;
		break;
	case	ODM_CMNINFO_INTERRUPT_MASK:
		dm->interrupt_mask = (u32 *)value;
		break;
	case ODM_CMNINFO_BB_OPERATION_MODE:
		dm->bb_op_mode = (u8 *)value;
		break;
	case ODM_CMNINFO_BF_ANTDIV_DECISION:
		dm->dm_fat_table.is_no_csi_feedback = (boolean *)value;
		break;

	default:
		/*do nothing*/
		break;

	}

}
/*
 * Update band/CHannel/.. The values are dynamic but non-per-packet.
 *   */
void
odm_cmn_info_update(
	struct dm_struct		*dm,
	u32			cmn_info,
	u64			value
)
{
	/*  */
	/* This init variable may be changed in run time. */
	/*  */
	switch	(cmn_info) {
	case ODM_CMNINFO_LINK_IN_PROGRESS:
		dm->is_link_in_process = (boolean)value;
		break;

	case	ODM_CMNINFO_ABILITY:
		dm->support_ability = (u64)value;
		break;

	case	ODM_CMNINFO_RF_TYPE:
		dm->rf_type = (u8)value;
		break;

	case	ODM_CMNINFO_WIFI_DIRECT:
		dm->is_wifi_direct = (boolean)value;
		break;

	case	ODM_CMNINFO_WIFI_DISPLAY:
		dm->is_wifi_display = (boolean)value;
		break;

	case	ODM_CMNINFO_LINK:
		dm->is_linked = (boolean)value;
		break;

	case	ODM_CMNINFO_CMW500LINK:
		dm->iot_table.is_linked_cmw500 = (boolean)value;
		break;

	case	ODM_CMNINFO_STATION_STATE:
		dm->bsta_state = (boolean)value;
		break;

	case	ODM_CMNINFO_RSSI_MIN:
		dm->rssi_min = (u8)value;
		break;

	case	ODM_CMNINFO_RSSI_MIN_BY_PATH:
		dm->rssi_min_by_path = (u8)value;
		break;

	case	ODM_CMNINFO_DBG_COMP:
		dm->debug_components = (u64)value;
		break;

	case	ODM_CMNINFO_DBG_LEVEL:
		dm->debug_level = (u32)value;
		break;

#ifdef ODM_CONFIG_BT_COEXIST
	/* The following is for BT HS mode and BT coexist mechanism. */
	case ODM_CMNINFO_BT_ENABLED:
		dm->bt_info_table.is_bt_enabled = (boolean)value;
		break;

	case ODM_CMNINFO_BT_HS_CONNECT_PROCESS:
		dm->bt_info_table.is_bt_connect_process = (boolean)value;
		break;

	case ODM_CMNINFO_BT_HS_RSSI:
		dm->bt_info_table.bt_hs_rssi = (u8)value;
		break;

	case	ODM_CMNINFO_BT_OPERATION:
		dm->bt_info_table.is_bt_hs_operation = (boolean)value;
		break;

	case	ODM_CMNINFO_BT_LIMITED_DIG:
		dm->bt_info_table.is_bt_limited_dig = (boolean)value;
		break;
#endif

	case	ODM_CMNINFO_AP_TOTAL_NUM:
		dm->ap_total_num = (u8)value;
		break;

#ifdef CONFIG_PHYDM_DFS_MASTER
	case	ODM_CMNINFO_DFS_REGION_DOMAIN:
		dm->dfs_region_domain = (u8)value;
		break;
#endif

	case	ODM_CMNINFO_BT_CONTINUOUS_TURN:
		dm->is_bt_continuous_turn = (boolean)value;
		break;

#if 0
	case	ODM_CMNINFO_OP_MODE:
		dm->op_mode = (u8)value;
		break;

	case	ODM_CMNINFO_BAND:
		dm->band_type = (u8)value;
		break;

	case	ODM_CMNINFO_SEC_CHNL_OFFSET:
		dm->sec_ch_offset = (u8)value;
		break;

	case	ODM_CMNINFO_SEC_MODE:
		dm->security = (u8)value;
		break;

	case	ODM_CMNINFO_BW:
		dm->band_width = (u8)value;
		break;

	case	ODM_CMNINFO_CHNL:
		dm->channel = (u8)value;
		break;
#endif
	default:
		/* do nothing */
		break;
	}


}

u32
phydm_cmn_info_query(
	struct dm_struct		*dm,
	enum phydm_info_query		info_type
)
{
	struct phydm_fa_struct		*fa_t = &dm->false_alm_cnt;
	struct phydm_dig_struct	*dig_t = &dm->dm_dig_table;
	struct ccx_info			*ccx_info = &dm->dm_ccx_info;

	switch (info_type) {
	/*=== [FA Relative] ===========================================*/
	case PHYDM_INFO_FA_OFDM:
		return fa_t->cnt_ofdm_fail;

	case PHYDM_INFO_FA_CCK:
		return fa_t->cnt_cck_fail;

	case PHYDM_INFO_FA_TOTAL:
		return fa_t->cnt_all;

	case PHYDM_INFO_CCA_OFDM:
		return fa_t->cnt_ofdm_cca;

	case PHYDM_INFO_CCA_CCK:
		return fa_t->cnt_cck_cca;

	case PHYDM_INFO_CCA_ALL:
		return fa_t->cnt_cca_all;

	case PHYDM_INFO_CRC32_OK_VHT:
		return fa_t->cnt_vht_crc32_ok;

	case PHYDM_INFO_CRC32_OK_HT:
		return fa_t->cnt_ht_crc32_ok;

	case PHYDM_INFO_CRC32_OK_LEGACY:
		return fa_t->cnt_ofdm_crc32_ok;

	case PHYDM_INFO_CRC32_OK_CCK:
		return fa_t->cnt_cck_crc32_ok;

	case PHYDM_INFO_CRC32_ERROR_VHT:
		return fa_t->cnt_vht_crc32_error;

	case PHYDM_INFO_CRC32_ERROR_HT:
		return fa_t->cnt_ht_crc32_error;

	case PHYDM_INFO_CRC32_ERROR_LEGACY:
		return fa_t->cnt_ofdm_crc32_error;

	case PHYDM_INFO_CRC32_ERROR_CCK:
		return fa_t->cnt_cck_crc32_error;

	case PHYDM_INFO_EDCCA_FLAG:
		return fa_t->edcca_flag;

	case PHYDM_INFO_OFDM_ENABLE:
		return fa_t->ofdm_block_enable;

	case PHYDM_INFO_CCK_ENABLE:
		return fa_t->cck_block_enable;

	case PHYDM_INFO_DBG_PORT_0:
		return fa_t->dbg_port0;

	case PHYDM_INFO_CRC32_OK_HT_AGG:
		return fa_t->cnt_ht_crc32_ok_agg;

	case PHYDM_INFO_CRC32_ERROR_HT_AGG:
		return fa_t->cnt_ht_crc32_error_agg;

	/*=== [DIG] ================================================*/

	case PHYDM_INFO_CURR_IGI:
		return dig_t->cur_ig_value;

	/*=== [RSSI] ===============================================*/
	case PHYDM_INFO_RSSI_MIN:
		return (u32)dm->rssi_min;

	case PHYDM_INFO_RSSI_MAX:
		return (u32)dm->rssi_max;

	case PHYDM_INFO_CLM_RATIO :
		return (u32)ccx_info->clm_ratio;
	case PHYDM_INFO_NHM_RATIO :
		return (u32)ccx_info->nhm_ratio;
	default:
		return 0xffffffff;

	}
}


#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
void
odm_init_all_work_items(struct dm_struct	*dm)
{
	void		*adapter = dm->adapter;
#if USE_WORKITEM

#ifdef CONFIG_DYNAMIC_RX_PATH
	odm_initialize_work_item(dm,
			 &dm->dm_drp_table.phydm_dynamic_rx_path_workitem,
		 (RT_WORKITEM_CALL_BACK)phydm_dynamic_rx_path_workitem_callback,
				 (void *)adapter,
				 "DynamicRxPathWorkitem");

#endif

#ifdef CONFIG_ADAPTIVE_SOML
	odm_initialize_work_item(dm,
			 &dm->dm_soml_table.phydm_adaptive_soml_workitem,
		 (RT_WORKITEM_CALL_BACK)phydm_adaptive_soml_workitem_callback,
				 (void *)adapter,
				 "AdaptiveSOMLWorkitem");
#endif

#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
	odm_initialize_work_item(dm,
		 &dm->dm_swat_table.phydm_sw_antenna_switch_workitem,
			 (RT_WORKITEM_CALL_BACK)odm_sw_antdiv_workitem_callback,
				 (void *)adapter,
				 "AntennaSwitchWorkitem");
#endif
#if (defined(CONFIG_HL_SMART_ANTENNA))
	odm_initialize_work_item(dm,
			 &dm->dm_sat_table.hl_smart_antenna_workitem,
		 (RT_WORKITEM_CALL_BACK)phydm_beam_switch_workitem_callback,
				 (void *)adapter,
				 "hl_smart_ant_workitem");

	odm_initialize_work_item(dm,
		 &dm->dm_sat_table.hl_smart_antenna_decision_workitem,
		 (RT_WORKITEM_CALL_BACK)phydm_beam_decision_workitem_callback,
				 (void *)adapter,
				 "hl_smart_ant_decision_workitem");
#endif

	odm_initialize_work_item(
		dm,
		&dm->path_div_switch_workitem,
		(RT_WORKITEM_CALL_BACK)odm_path_div_chk_ant_switch_workitem_callback,
		(void *)adapter,
		"SWAS_WorkItem");

	odm_initialize_work_item(
		dm,
		&dm->cck_path_diversity_workitem,
		(RT_WORKITEM_CALL_BACK)odm_cck_tx_path_diversity_work_item_callback,
		(void *)adapter,
		"CCKTXPathDiversityWorkItem");

	odm_initialize_work_item(
		dm,
		&dm->ra_rpt_workitem,
		(RT_WORKITEM_CALL_BACK)halrf_update_init_rate_work_item_callback,
		(void *)adapter,
		"ra_rpt_workitem");

#if (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY)) || (defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
	odm_initialize_work_item(
		dm,
		&dm->fast_ant_training_workitem,
		(RT_WORKITEM_CALL_BACK)odm_fast_ant_training_work_item_callback,
		(void *)adapter,
		"fast_ant_training_workitem");
#endif

#endif /*#if USE_WORKITEM*/

#if (BEAMFORMING_SUPPORT == 1)
	odm_initialize_work_item(
		dm,
		&dm->beamforming_info.txbf_info.txbf_enter_work_item,
		(RT_WORKITEM_CALL_BACK)hal_com_txbf_enter_work_item_callback,
		(void *)adapter,
		"txbf_enter_work_item");

	odm_initialize_work_item(
		dm,
		&dm->beamforming_info.txbf_info.txbf_leave_work_item,
		(RT_WORKITEM_CALL_BACK)hal_com_txbf_leave_work_item_callback,
		(void *)adapter,
		"txbf_leave_work_item");

	odm_initialize_work_item(
		dm,
		&dm->beamforming_info.txbf_info.txbf_fw_ndpa_work_item,
		(RT_WORKITEM_CALL_BACK)hal_com_txbf_fw_ndpa_work_item_callback,
		(void *)adapter,
		"txbf_fw_ndpa_work_item");

	odm_initialize_work_item(
		dm,
		&dm->beamforming_info.txbf_info.txbf_clk_work_item,
		(RT_WORKITEM_CALL_BACK)hal_com_txbf_clk_work_item_callback,
		(void *)adapter,
		"txbf_clk_work_item");

	odm_initialize_work_item(
		dm,
		&dm->beamforming_info.txbf_info.txbf_rate_work_item,
		(RT_WORKITEM_CALL_BACK)hal_com_txbf_rate_work_item_callback,
		(void *)adapter,
		"txbf_rate_work_item");

	odm_initialize_work_item(
		dm,
		&dm->beamforming_info.txbf_info.txbf_status_work_item,
		(RT_WORKITEM_CALL_BACK)hal_com_txbf_status_work_item_callback,
		(void *)adapter,
		"txbf_status_work_item");

	odm_initialize_work_item(
		dm,
		&dm->beamforming_info.txbf_info.txbf_reset_tx_path_work_item,
		(RT_WORKITEM_CALL_BACK)hal_com_txbf_reset_tx_path_work_item_callback,
		(void *)adapter,
		"txbf_reset_tx_path_work_item");

	odm_initialize_work_item(
		dm,
		&dm->beamforming_info.txbf_info.txbf_get_tx_rate_work_item,
		(RT_WORKITEM_CALL_BACK)hal_com_txbf_get_tx_rate_work_item_callback,
		(void *)adapter,
		"txbf_get_tx_rate_work_item");
#endif

	odm_initialize_work_item(
		dm,
		&dm->adaptivity.phydm_pause_edcca_work_item,
		(RT_WORKITEM_CALL_BACK)phydm_pause_edcca_work_item_callback,
		(void *)adapter,
		"phydm_pause_edcca_work_item");

	odm_initialize_work_item(
		dm,
		&dm->adaptivity.phydm_resume_edcca_work_item,
		(RT_WORKITEM_CALL_BACK)phydm_resume_edcca_work_item_callback,
		(void *)adapter,
		"phydm_resume_edcca_work_item");

#if (PHYDM_LA_MODE_SUPPORT == 1)
	odm_initialize_work_item(
		dm,
		&dm->adcsmp.adc_smp_work_item,
		(RT_WORKITEM_CALL_BACK)adc_smp_work_item_callback,
		(void *)adapter,
		"adc_smp_work_item");

	odm_initialize_work_item(
		dm,
		&dm->adcsmp.adc_smp_work_item_1,
		(RT_WORKITEM_CALL_BACK)adc_smp_work_item_callback,
		(void *)adapter,
		"adc_smp_work_item_1");
#endif

}

void
odm_free_all_work_items(struct dm_struct	*dm)
{
#if USE_WORKITEM

#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
	odm_free_work_item(&dm->dm_swat_table.phydm_sw_antenna_switch_workitem);
#endif

#ifdef CONFIG_DYNAMIC_RX_PATH
	odm_free_work_item(&dm->dm_drp_table.phydm_dynamic_rx_path_workitem);
#endif

#ifdef CONFIG_ADAPTIVE_SOML
	odm_free_work_item(&dm->dm_soml_table.phydm_adaptive_soml_workitem);
#endif


#if (defined(CONFIG_HL_SMART_ANTENNA))
	odm_free_work_item(&dm->dm_sat_table.hl_smart_antenna_workitem);
	odm_free_work_item(&dm->dm_sat_table.hl_smart_antenna_decision_workitem);
#endif

	odm_free_work_item(&dm->path_div_switch_workitem);
	odm_free_work_item(&dm->cck_path_diversity_workitem);
#if (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY)) || (defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
	odm_free_work_item(&dm->fast_ant_training_workitem);
#endif
	odm_free_work_item(&dm->ra_rpt_workitem);
	/*odm_free_work_item((&dm->sbdcnt_workitem));*/
#endif

#if (BEAMFORMING_SUPPORT == 1)
	odm_free_work_item((&dm->beamforming_info.txbf_info.txbf_enter_work_item));
	odm_free_work_item((&dm->beamforming_info.txbf_info.txbf_leave_work_item));
	odm_free_work_item((&dm->beamforming_info.txbf_info.txbf_fw_ndpa_work_item));
	odm_free_work_item((&dm->beamforming_info.txbf_info.txbf_clk_work_item));
	odm_free_work_item((&dm->beamforming_info.txbf_info.txbf_rate_work_item));
	odm_free_work_item((&dm->beamforming_info.txbf_info.txbf_status_work_item));
	odm_free_work_item((&dm->beamforming_info.txbf_info.txbf_reset_tx_path_work_item));
	odm_free_work_item((&dm->beamforming_info.txbf_info.txbf_get_tx_rate_work_item));
#endif

	odm_free_work_item((&dm->adaptivity.phydm_pause_edcca_work_item));
	odm_free_work_item((&dm->adaptivity.phydm_resume_edcca_work_item));

#if (PHYDM_LA_MODE_SUPPORT == 1)
	odm_free_work_item((&dm->adcsmp.adc_smp_work_item));
	odm_free_work_item((&dm->adcsmp.adc_smp_work_item_1));
#endif

}
#endif /*#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)*/

void
odm_init_all_timers(
	struct dm_struct	*dm
)
{
#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
	odm_ant_div_timers(dm, INIT_ANTDIV_TIMMER);
#endif

	phydm_adaptive_soml_timers(dm, INIT_SOML_TIMMER);

#ifdef PHYDM_LNA_SAT_CHK_SUPPORT
	phydm_lna_sat_chk_timers(dm, INIT_LNA_SAT_CHK_TIMMER);
#endif

#ifdef CONFIG_DYNAMIC_RX_PATH
	phydm_dynamic_rx_path_timers(dm, INIT_DRP_TIMMER);
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	odm_initialize_timer(dm, &dm->path_div_switch_timer,
		(void *)odm_path_div_chk_ant_switch_callback, NULL, "PathDivTimer");
	odm_initialize_timer(dm, &dm->cck_path_diversity_timer,
		(void *)odm_cck_tx_path_diversity_callback, NULL, "cck_path_diversity_timer");
	odm_initialize_timer(dm, &dm->sbdcnt_timer,
			     (void *)phydm_sbd_callback, NULL, "SbdTimer");
#if (BEAMFORMING_SUPPORT == 1)
	odm_initialize_timer(dm, &dm->beamforming_info.txbf_info.txbf_fw_ndpa_timer,
		(void *)hal_com_txbf_fw_ndpa_timer_callback, NULL, "txbf_fw_ndpa_timer");
#endif
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
#if (BEAMFORMING_SUPPORT == 1)
	odm_initialize_timer(dm, &dm->beamforming_info.beamforming_timer,
		(void *)beamforming_sw_timer_callback, NULL, "beamforming_timer");
#endif
#endif
}

void
odm_cancel_all_timers(
	struct dm_struct	*dm
)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	/* 2012/01/12 MH Temp BSOD fix. We need to find NIC allocate mem fail reason in win7*/
	if (dm->adapter == NULL)
		return;
#endif

#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
	odm_ant_div_timers(dm, CANCEL_ANTDIV_TIMMER);
#endif

	phydm_adaptive_soml_timers(dm, CANCEL_SOML_TIMMER);

#ifdef PHYDM_LNA_SAT_CHK_SUPPORT
	phydm_lna_sat_chk_timers(dm, CANCEL_LNA_SAT_CHK_TIMMER);
#endif


#ifdef CONFIG_DYNAMIC_RX_PATH
	phydm_dynamic_rx_path_timers(dm, CANCEL_DRP_TIMMER);
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	odm_cancel_timer(dm, &dm->path_div_switch_timer);
	odm_cancel_timer(dm, &dm->cck_path_diversity_timer);
	odm_cancel_timer(dm, &dm->sbdcnt_timer);
#if (BEAMFORMING_SUPPORT == 1)
	odm_cancel_timer(dm, &dm->beamforming_info.txbf_info.txbf_fw_ndpa_timer);
#endif
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
#if (BEAMFORMING_SUPPORT == 1)
	odm_cancel_timer(dm, &dm->beamforming_info.beamforming_timer);
#endif
#endif

}


void
odm_release_all_timers(
	struct dm_struct	*dm
)
{
#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
	odm_ant_div_timers(dm, RELEASE_ANTDIV_TIMMER);
#endif
	phydm_adaptive_soml_timers(dm, RELEASE_SOML_TIMMER);

#ifdef PHYDM_LNA_SAT_CHK_SUPPORT
	phydm_lna_sat_chk_timers(dm, RELEASE_LNA_SAT_CHK_TIMMER);
#endif

#ifdef CONFIG_DYNAMIC_RX_PATH
	phydm_dynamic_rx_path_timers(dm, RELEASE_DRP_TIMMER);
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	odm_release_timer(dm, &dm->path_div_switch_timer);
	odm_release_timer(dm, &dm->cck_path_diversity_timer);
	odm_release_timer(dm, &dm->sbdcnt_timer);
#if (BEAMFORMING_SUPPORT == 1)
	odm_release_timer(dm, &dm->beamforming_info.txbf_info.txbf_fw_ndpa_timer);
#endif
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
#if (BEAMFORMING_SUPPORT == 1)
	odm_release_timer(dm, &dm->beamforming_info.beamforming_timer);
#endif
#endif
}

#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
void
odm_init_all_threads(
	struct dm_struct	*dm
)
{
#ifdef TPT_THREAD
	k_tpt_task_init(dm->priv);
#endif
}

void
odm_stop_all_threads(
	struct dm_struct	*dm
)
{
#ifdef TPT_THREAD
	k_tpt_task_stop(dm->priv);
#endif
}
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
/* Justin: According to the current RRSI to adjust Response Frame TX power, 2012/11/05 */
void odm_dtc(struct dm_struct *dm)
{
#ifdef CONFIG_DM_RESP_TXAGC
#define DTC_BASE            35	/* RSSI higher than this value, start to decade TX power */
#define DTC_DWN_BASE       (DTC_BASE-5)	/* RSSI lower than this value, start to increase TX power */

	/* RSSI vs TX power step mapping: decade TX power */
	static const u8 dtc_table_down[] = {
		DTC_BASE,
		(DTC_BASE + 5),
		(DTC_BASE + 10),
		(DTC_BASE + 15),
		(DTC_BASE + 20),
		(DTC_BASE + 25)
	};

	/* RSSI vs TX power step mapping: increase TX power */
	static const u8 dtc_table_up[] = {
		DTC_DWN_BASE,
		(DTC_DWN_BASE - 5),
		(DTC_DWN_BASE - 10),
		(DTC_DWN_BASE - 15),
		(DTC_DWN_BASE - 15),
		(DTC_DWN_BASE - 20),
		(DTC_DWN_BASE - 20),
		(DTC_DWN_BASE - 25),
		(DTC_DWN_BASE - 25),
		(DTC_DWN_BASE - 30),
		(DTC_DWN_BASE - 35)
	};

	u8 i;
	u8 dtc_steps = 0;
	u8 sign;
	u8 resp_txagc = 0;

#if 0
	/* As DIG is disabled, DTC is also disable */
	if (!(dm->support_ability & ODM_XXXXXX))
		return;
#endif

	if (dm->rssi_min > DTC_BASE) {
		/* need to decade the CTS TX power */
		sign = 1;
		for (i = 0; i < ARRAY_SIZE(dtc_table_down); i++) {
			if ((dtc_table_down[i] >= dm->rssi_min) || (dtc_steps >= 6))
				break;
			else
				dtc_steps++;
		}
	}
#if 0
	else if (dm->rssi_min > DTC_DWN_BASE) {
		/* needs to increase the CTS TX power */
		sign = 0;
		dtc_steps = 1;
		for (i = 0; i < ARRAY_SIZE(dtc_table_up); i++) {
			if ((dtc_table_up[i] <= dm->rssi_min) || (dtc_steps >= 10))
				break;
			else
				dtc_steps++;
		}
	}
#endif
	else {
		sign = 0;
		dtc_steps = 0;
	}

	resp_txagc = dtc_steps | (sign << 4);
	resp_txagc = resp_txagc | (resp_txagc << 5);
	odm_write_1byte(dm, 0x06d9, resp_txagc);

	PHYDM_DBG(dm, ODM_COMP_PWR_TRAIN, "%s rssi_min:%u, set RESP_TXAGC to %s %u\n",
		__func__, dm->rssi_min, sign ? "minus" : "plus", dtc_steps);
#endif /* CONFIG_RESP_TXAGC_ADJUST */
}

#endif /* #if (DM_ODM_SUPPORT_TYPE == ODM_CE) */


/*<20170126, BB-Kevin>8188F D-CUT DC cancellation and 8821C*/
void
phydm_dc_cancellation(
	struct dm_struct	*dm

)
{
#ifdef PHYDM_DC_CANCELLATION
	u32		offset_i_hex[PHYDM_MAX_RF_PATH] = {0};
	u32		offset_q_hex[PHYDM_MAX_RF_PATH] = {0};
	u32		reg_value32[PHYDM_MAX_RF_PATH] = {0};
	u8		path = RF_PATH_A;

	if (!(dm->support_ic_type & ODM_DC_CANCELLATION_SUPPORT))
		return;

	if ((dm->support_ic_type & ODM_RTL8188F) && (dm->cut_version < ODM_CUT_D))
		return;

	/*DC_Estimation (only for 2x2 ic now) */

	for (path = RF_PATH_A; path < PHYDM_MAX_RF_PATH; path++) {
		if (dm->support_ic_type & (ODM_RTL8188F | ODM_RTL8710B)) {
			if (!phydm_set_bb_dbg_port(dm,
				BB_DBGPORT_PRIORITY_2, 0x235)) {/*set debug port to 0x235*/
				PHYDM_DBG(dm, ODM_COMP_API,
					"[DC Cancellation] Set Debug port Fail");
				return;
			}
		} else if (dm->support_ic_type & (ODM_RTL8821C | ODM_RTL8822B)) {
			if (!phydm_set_bb_dbg_port(dm, BB_DBGPORT_PRIORITY_2, 0x200)) {
				/*set debug port to 0x200*/
				PHYDM_DBG(dm, ODM_COMP_API,
					"[DC Cancellation] Set Debug port Fail");
				return;
			}
			phydm_bb_dbg_port_header_sel(dm, 0x0);
			if (dm->rf_type > RF_1T1R) {
				if (!phydm_set_bb_dbg_port(dm, BB_DBGPORT_PRIORITY_2, 0x202)) {
					/*set debug port to 0x200*/
					PHYDM_DBG(dm, ODM_COMP_API,
						"[DC Cancellation] Set Debug port Fail");
					return;
				}
				phydm_bb_dbg_port_header_sel(dm, 0x0);
			}
		}

		odm_write_dig(dm, 0x7E);

		if (dm->support_ic_type & ODM_IC_11N_SERIES)
			odm_set_bb_reg(dm, 0x88c, BIT(21)|BIT(20), 0x3);
		else {
			odm_set_bb_reg(dm, 0xc00, BIT(1)|BIT(0), 0x0);
			if (dm->rf_type > RF_1T1R)
				odm_set_bb_reg(dm, 0xe00, BIT(1)|BIT(0), 0x0);
		}
		odm_set_bb_reg(dm, 0xa78, MASKBYTE1, 0x0); /*disable CCK DCNF*/

		PHYDM_DBG(dm, ODM_COMP_API, "DC cancellation Begin!!!");

		phydm_stop_ck320(dm, true);	/*stop ck320*/

		/* the same debug port both for path-a and path-b*/
		reg_value32[path] = phydm_get_bb_dbg_port_value(dm);

		phydm_stop_ck320(dm, false);	/*start ck320*/

		if (dm->support_ic_type & ODM_IC_11N_SERIES) {
			odm_set_bb_reg(dm, 0x88c, BIT(21)|BIT(20), 0x0);
		} else {
			odm_set_bb_reg(dm, 0xc00, BIT(1)|BIT(0), 0x3);
			odm_set_bb_reg(dm, 0xe00, BIT(1)|BIT(0), 0x3);
		}
		odm_write_dig(dm, 0x20);
		phydm_release_bb_dbg_port(dm);

		PHYDM_DBG(dm, ODM_COMP_API, "DC cancellation OK!!!");
	}

	/*DC_Cancellation*/
	odm_set_bb_reg(dm, 0xa9c, BIT(20), 0x1); /*DC compensation to CCK data path*/
	if (dm->support_ic_type & (ODM_RTL8188F | ODM_RTL8710B)) {
		offset_i_hex[0] = (reg_value32[0] & 0xffc0000) >> 18;
		offset_q_hex[0] = (reg_value32[0] & 0x3ff00) >> 8;

		/*Before filling into registers, offset should be multiplexed (-1)*/
		offset_i_hex[0] = (offset_i_hex[0] >= 0x200) ? (0x400 - offset_i_hex[0]) : (0x1ff - offset_i_hex[0]);
		offset_q_hex[0] = (offset_q_hex[0] >= 0x200) ? (0x400 - offset_q_hex[0]) : (0x1ff - offset_q_hex[0]);

		odm_set_bb_reg(dm, 0x950, 0x1ff, offset_i_hex[0]);
		odm_set_bb_reg(dm, 0x950, 0x1ff0000, offset_q_hex[0]);
	} else if (dm->support_ic_type & (ODM_RTL8821C | ODM_RTL8822B)) {

		/* Path-a */
		offset_i_hex[0] = (reg_value32[0] & 0xffc00) >> 10;
		offset_q_hex[0] = reg_value32[0] & 0x3ff;

		/*Before filling into registers, offset should be multiplexed (-1)*/
		offset_i_hex[0] = 0x400 - offset_i_hex[0];
		offset_q_hex[0] = 0x400 - offset_q_hex[0];

		odm_set_bb_reg(dm, 0xc10, 0x3c000000, ((0x3c0 & offset_i_hex[0]) >> 6));
		odm_set_bb_reg(dm, 0xc10, 0xfc00, (0x3f & offset_i_hex[0]));
		odm_set_bb_reg(dm, 0xc14, 0x3c000000, ((0x3c0 & offset_q_hex[0]) >> 6));
		odm_set_bb_reg(dm, 0xc14, 0xfc00, (0x3f & offset_q_hex[0]));

		/* Path-b */
		if (dm->rf_type > RF_1T1R) {

			offset_i_hex[1] = (reg_value32[1] & 0xffc00) >> 10;
			offset_q_hex[1] = reg_value32[1] & 0x3ff;

		/*Before filling into registers, offset should be multiplexed (-1)*/
			offset_i_hex[1] = 0x400 - offset_i_hex[1];
			offset_q_hex[1] = 0x400 - offset_q_hex[1];

			odm_set_bb_reg(dm, 0xe10, 0x3c000000, ((0x3c0 & offset_i_hex[1]) >> 6));
			odm_set_bb_reg(dm, 0xe10, 0xfc00, (0x3f & offset_i_hex[1]));
			odm_set_bb_reg(dm, 0xe14, 0x3c000000, ((0x3c0 & offset_q_hex[1]) >> 6));
			odm_set_bb_reg(dm, 0xe14, 0xfc00, (0x3f & offset_q_hex[1]));
		}
	}
#endif
}

void
phydm_receiver_blocking(
	void *dm_void
)
{
#ifdef CONFIG_RECEIVER_BLOCKING
	struct dm_struct		*dm = (struct dm_struct *)dm_void;
	u32	channel = *dm->channel;
	u8	bw = *dm->band_width;
	u32	bb_regf0 = odm_get_bb_reg(dm, 0xf0, MASKDWORD);

	if (!(dm->support_ic_type & ODM_RECEIVER_BLOCKING_SUPPORT))
		return;

	if ((dm->support_ic_type & ODM_RTL8188E && ((bb_regf0 & 0xf000) >> 12) < 8) ||
		dm->support_ic_type & ODM_RTL8192E) { /*8188E_T version*/
		if (dm->consecutive_idlel_time > 10 && *dm->mp_mode == false && dm->adaptivity_enable == true) {
			if ((bw == CHANNEL_WIDTH_20) && (channel == 1)) {
				phydm_nbi_setting(dm, FUNC_ENABLE, channel, 20, 2410, PHYDM_DONT_CARE);
				dm->is_receiver_blocking_en = true;
			} else if ((bw == CHANNEL_WIDTH_20) && (channel == 13)) {
				phydm_nbi_setting(dm, FUNC_ENABLE, channel, 20, 2473, PHYDM_DONT_CARE);
				dm->is_receiver_blocking_en = true;
			} else if (dm->is_receiver_blocking_en && channel != 1 && channel != 13) {
				phydm_nbi_enable(dm, FUNC_DISABLE);
				odm_set_bb_reg(dm, 0xc40, 0x1f000000, 0x1f);
				dm->is_receiver_blocking_en = false;
			}
			return;
		}
	} else if ((dm->support_ic_type & ODM_RTL8188E && ((bb_regf0 & 0xf000) >> 12) >= 8)) { /*8188E_S version*/
		if (dm->consecutive_idlel_time > 10 && *dm->mp_mode == false && dm->adaptivity_enable == true) {
			if ((bw == CHANNEL_WIDTH_20) && (channel == 13)) {
				phydm_nbi_setting(dm, FUNC_ENABLE, channel, 20, 2473, PHYDM_DONT_CARE);
				dm->is_receiver_blocking_en = true;
			} else if (dm->is_receiver_blocking_en && channel != 13) {
				phydm_nbi_enable(dm, FUNC_DISABLE);
				odm_set_bb_reg(dm, 0xc40, 0x1f000000, 0x1f);
				dm->is_receiver_blocking_en = false;
			}
			return;
		}
	}

	if (dm->is_receiver_blocking_en) {
		phydm_nbi_enable(dm, FUNC_DISABLE);
		odm_set_bb_reg(dm, 0xc40, 0x1f000000, 0x1f);
		dm->is_receiver_blocking_en = false;
	}

#endif
}
