

#include "public.h"
#include "scd_drv.h"
#include "vfmw_intf.h"
#include "vdm_hal_api.h"
#ifdef HIVDEC_SMMU_SUPPORT
#include "smmu.h"
#endif
// cppcheck-suppress *
#define scd_check_cfg_addr_return(scdcfg, else_print, start_phy, end_phy) \
do { \
	if (((scdcfg) == 0) || ((scdcfg) < (start_phy)) || ((scdcfg) > (end_phy)) { \
		dprint(PRN_FATAL, "%s (%s) is out of range\n", __func__, else_print); \
		return SCDDRV_ERR; \
	} \
} while (0)
static scd_drv_sleep_stage_e  e_scd_drv_sleep_stage = SCDDRV_SLEEP_STAGE_NONE;
static scd_state_reg_s g_scd_state_reg;
static scd_state_e scd_state = SCD_IDLE;

static VOID print_scd_vtrl_reg(void);

SINT32 scd_drv_reset_scd(void)
{
	UINT32 tmp;
	UINT32 i;
	UINT32 reg_rst_ok;
	UINT32 reg;
	UINT32 *p_scd_reset_reg   = NULL;
	UINT32 *p_scd_reset_ok_reg = NULL;

	p_scd_reset_reg   = (UINT32 *) mem_phy_2_vir(g_soft_rst_req_addr);
	p_scd_reset_ok_reg = (UINT32 *) mem_phy_2_vir(g_soft_rst_ok_addr);

	if (p_scd_reset_reg == NULL || p_scd_reset_ok_reg == NULL) {
		dprint(PRN_FATAL, "scd reset register map fail\n");
		return VF_ERR_SYS;
	}

	tmp = rd_scd_reg(REG_SCD_INT_MASK);


	reg = *(volatile UINT32 *)p_scd_reset_reg;
	*(volatile UINT32 *)p_scd_reset_reg =
		reg | (UINT32)(1 << SCD_RESET_CTRL_BIT);

	for (i = 0; i < RESET_SCD_COUNT; i++) {
		reg_rst_ok = *(volatile UINT32 *)p_scd_reset_ok_reg;
		if (reg_rst_ok & (1 << SCD_RESET_OK_BIT))
			break;
		VFMW_OSAL_U_DELAY(VFMW_OSAL_DELAY_TIME);
	}

	if (i >= RESET_SCD_COUNT)
		dprint(PRN_FATAL, "%s reset failed\n", __func__);
	else
		dprint(PRN_ALWS, "%s reset success\n", __func__);

	*(volatile UINT32 *)p_scd_reset_reg =
		reg & (UINT32)(~(1 << SCD_RESET_CTRL_BIT));

	wr_scd_reg(REG_SCD_INT_MASK, tmp);

	scd_state = SCD_IDLE;
	return FMW_OK;
}

SINT32 scd_drv_prepare_sleep(void)
{
	SINT32 ret = SCDDRV_OK;

	VFMW_OSAL_SEMA_DOWN(G_SCD_SEM);
	if (e_scd_drv_sleep_stage == SCDDRV_SLEEP_STAGE_NONE) {
		if (scd_state == SCD_IDLE) {
			dprint(PRN_ALWS, "%s, idle state\n", __func__);
			e_scd_drv_sleep_stage = SCDDRV_SLEEP_STAGE_SLEEP;
		} else {
			dprint(PRN_ALWS, "%s, decoded state\n", __func__);
			e_scd_drv_sleep_stage = SCDDRV_SLEEP_STAGE_PREPARE;
		}

		ret = SCDDRV_OK;
	} else {
		ret = SCDDRV_ERR;
	}

	VFMW_OSAL_SEMA_UP(G_SCD_SEM);
	return ret;
}

scd_drv_sleep_stage_e scd_drv_get_sleep_stage(void)
{
	return e_scd_drv_sleep_stage;
}

VOID scd_drv_set_sleep_stage(scd_drv_sleep_stage_e sleep_state)
{
	VFMW_OSAL_SEMA_DOWN(G_SCD_SEM);
	e_scd_drv_sleep_stage = sleep_state;
	VFMW_OSAL_SEMA_UP(G_SCD_SEM);
}

VOID scd_drv_force_sleep(void)
{
	dprint(PRN_ALWS, "%s, force state\n", __func__);
	VFMW_OSAL_SEMA_DOWN(G_SCD_SEM);
	if (e_scd_drv_sleep_stage != SCDDRV_SLEEP_STAGE_SLEEP) {
		scd_drv_reset_scd();
		e_scd_drv_sleep_stage = SCDDRV_SLEEP_STAGE_SLEEP;
	}
	VFMW_OSAL_SEMA_UP(G_SCD_SEM);
}

VOID scd_drv_exit_sleep(void)
{
	VFMW_OSAL_SEMA_DOWN(G_SCD_SEM);
	e_scd_drv_sleep_stage = SCDDRV_SLEEP_STAGE_NONE;
	VFMW_OSAL_SEMA_UP(G_SCD_SEM);
}

static SINT32 scd_drv_check_address(UADDR scdcfg, hi_u32 start_phy, hi_u32 end_phy)
{
	if ((scdcfg == 0) || (scdcfg < start_phy) || (scdcfg > end_phy))
		return SCDDRV_ERR;

	return SCDDRV_OK;
}

SINT32 scd_drv_check_cfg_address(
	scd_config_reg_s *p_sm_ctrl_reg, mem_buffer_s *p_scd_mem_map)
{
	UINT32 i;
	SINT32 ret = SCDDRV_OK;
	hi_u32 start_output_phy_addr;
	hi_u32 end_output_phy_addr;
	SINT32 up_msg_max_offset   = (SM_MAX_UPMSG_SIZE + UP_MSG_MAX_OFFSET) & (~UP_MSG_MAX_OFFSET);
	UINT32 max_scd_buf_num     = (p_sm_ctrl_reg->scd_output_buf_num +
		SCD_SHAREFD_OUTPUT_BUF) < SCD_SHAREFD_MAX ?
		(p_sm_ctrl_reg->scd_output_buf_num + SCD_SHAREFD_OUTPUT_BUF) :
		SCD_SHAREFD_MAX;

	for (i = SCD_SHAREFD_MESSAGE_POOL; i < max_scd_buf_num; i++) {
		if ((p_scd_mem_map[i].share_fd == INVALID_SHAREFD)
			|| (p_scd_mem_map[i].is_mapped == 0)) {
			dprint(PRN_FATAL, "%s parameter error: buffer no map. index: %d, isMapped: %d, shareFd: %d\n",
				__func__, i, p_scd_mem_map[i].is_mapped,
				p_scd_mem_map[i].share_fd);
			return SCDDRV_ERR;
		}
	}

	if ((p_sm_ctrl_reg->down_msg_phy_addr == 0)
		|| (p_sm_ctrl_reg->down_msg_phy_addr <
			p_scd_mem_map[SCD_SHAREFD_MESSAGE_POOL].start_phy_addr)
			|| (p_sm_ctrl_reg->down_msg_phy_addr
			> (p_scd_mem_map[SCD_SHAREFD_MESSAGE_POOL].start_phy_addr
			   + p_scd_mem_map[SCD_SHAREFD_MESSAGE_POOL].size))) {
		dprint(PRN_FATAL, "%s down_msg_phy_addr is out of range\n", __func__);
		return SCDDRV_ERR;
	}
	if ((p_sm_ctrl_reg->up_msg_phy_addr == 0)
		|| (p_sm_ctrl_reg->up_msg_phy_addr <
			p_scd_mem_map[SCD_SHAREFD_MESSAGE_POOL].start_phy_addr)
		|| (p_sm_ctrl_reg->up_msg_phy_addr
			> (p_scd_mem_map[SCD_SHAREFD_MESSAGE_POOL].start_phy_addr
			   + p_scd_mem_map[SCD_SHAREFD_MESSAGE_POOL].size
			   /* - up_msg_max_offset */))) {
		dprint(PRN_FATAL, "%s up_msg_phy_addr  is out of range\n", __func__);
		return SCDDRV_ERR;
	}
	if ((p_sm_ctrl_reg->up_len < 0)
		|| (p_sm_ctrl_reg->up_len > up_msg_max_offset)) {
		dprint(PRN_FATAL, "%s up_len is out of range\n", __func__);
		return SCDDRV_ERR;
	}


	for (i = SCD_SHAREFD_OUTPUT_BUF; i < max_scd_buf_num; i++) {
		start_output_phy_addr = p_scd_mem_map[i].start_phy_addr;
		end_output_phy_addr   = p_scd_mem_map[i].start_phy_addr +
			p_scd_mem_map[i].size;
		ret = SCDDRV_OK;
		ret += scd_drv_check_address(p_sm_ctrl_reg->buffer_first,
			start_output_phy_addr, end_output_phy_addr);
		ret += scd_drv_check_address(p_sm_ctrl_reg->buffer_last,
			start_output_phy_addr, end_output_phy_addr);
		ret += scd_drv_check_address(p_sm_ctrl_reg->buffer_ini,
			p_sm_ctrl_reg->buffer_first, p_sm_ctrl_reg->buffer_last);

		if (ret == SCDDRV_OK)
			break;
	}

	return ret;
}

SINT32 scd_drv_write_reg(scd_config_reg_s *p_sm_ctrl_reg, mem_buffer_s *p_scd_mem_map)
{
	SINT32 ret;

	if (scd_state != SCD_IDLE)
		return SCDDRV_ERR;

	ret = scd_drv_check_cfg_address(p_sm_ctrl_reg, p_scd_mem_map);
	if (ret) {
		dprint(PRN_FATAL, "scd_drv_check_cfg_address check failed\n");
		return SCDDRV_ERR;
	}

	scd_state = SCD_WORKING;
	wr_scd_reg(REG_SCD_INI_CLR, 1);

	// LIST_ADDRESS
	wr_scd_reg(REG_LIST_ADDRESS, (unsigned int)p_sm_ctrl_reg->down_msg_phy_addr);

	// UP_ADDRESS
	wr_scd_reg(REG_UP_ADDRESS, (UINT32) p_sm_ctrl_reg->up_msg_phy_addr);

	// UP_LEN
	wr_scd_reg(REG_UP_LEN, (UINT32) p_sm_ctrl_reg->up_len);

	// BUFFER_FIRST
	wr_scd_reg(REG_BUFFER_FIRST, (UINT32) p_sm_ctrl_reg->buffer_first);

	// BUFFER_LAST
	wr_scd_reg(REG_BUFFER_LAST, (UINT32) p_sm_ctrl_reg->buffer_last);

	// BUFFER_INI
	wr_scd_reg(REG_BUFFER_INI, (UINT32) p_sm_ctrl_reg->buffer_ini);

	// SCD_PROTOCOL
#ifdef ENV_SOS_KERNEL
	wr_scd_reg(REG_SCD_PROTOCOL, (UINT32)((p_sm_ctrl_reg->scd_lowdly_enable << SCD_LOWDLY_ENABLE_OFFSET) |
	    (1 << SCD_DRV_REG_OFFSET) | ((p_sm_ctrl_reg->slice_check_flag << SLICE_CHECK_FLAG_OFFSET) & SLICE_CHECK_FLAG_OPR) |
	    (p_sm_ctrl_reg->scd_protocol & SCD_PROTOCOL_OFFSET)));
#ifdef HIVDEC_SMMU_SUPPORT
	smmu_set_master_reg(SCD, SECURE_ON, SMMU_OFF);
#endif

#else
	wr_scd_reg(REG_SCD_PROTOCOL, (UINT32)((p_sm_ctrl_reg->scd_lowdly_enable << SCD_LOWDLY_ENABLE_OFFSET) |
		((p_sm_ctrl_reg->slice_check_flag << SLICE_CHECK_FLAG_OFFSET) & SLICE_CHECK_FLAG_OPR) |
		(p_sm_ctrl_reg->scd_protocol & SCD_PROTOCOL_OFFSET)));
#ifdef HIVDEC_SMMU_SUPPORT
	smmu_set_master_reg(SCD, SECURE_OFF, SMMU_ON);
#endif

#endif

#ifndef SCD_BUSY_WAITTING
	wr_scd_reg(REG_SCD_INT_MASK, 0);
#endif

	print_scd_vtrl_reg();

	// SCD_START
	wr_scd_reg(REG_SCD_START, 0);
	wr_scd_reg(REG_SCD_START, (UINT32)(p_sm_ctrl_reg->scd_start & 0x01));

	return SCDDRV_OK;
}

VOID scd_drv_save_state_reg(void)
{
	g_scd_state_reg.scd_protocol = rd_scd_reg(REG_SCD_PROTOCOL);
	g_scd_state_reg.scd_over     = rd_scd_reg(REG_SCD_OVER);
	g_scd_state_reg.scd_int      = rd_scd_reg(REG_SCD_INT);
	g_scd_state_reg.scd_num      = rd_scd_reg(REG_SCD_NUM);
	g_scd_state_reg.scd_roll_addr = rd_scd_reg(REG_ROLL_ADDR);
	g_scd_state_reg.src_eaten    = rd_scd_reg(REG_SRC_EATEN);
	g_scd_state_reg.up_len       = rd_scd_reg(REG_UP_LEN);
}

VOID scd_drv_init(void)
{
	hi_s32 ret;

	ret = memset_s(&g_scd_state_reg, sizeof(g_scd_state_reg), 0,
		sizeof(g_scd_state_reg));
	if (ret != EOK) {
		dprint(PRN_FATAL, " %s %d memset_s err in function\n", __func__, __LINE__);
		return;
	}
	e_scd_drv_sleep_stage = SCDDRV_SLEEP_STAGE_NONE;
	scd_state = SCD_IDLE;
}

VOID scd_drv_deinit(void)
{
	e_scd_drv_sleep_stage = SCDDRV_SLEEP_STAGE_NONE;
	scd_state = SCD_IDLE;
}

VOID scd_drv_isr(void)
{
	SINT32 dat;

	dat = rd_scd_reg(REG_SCD_OVER) & 0x01;

	if ((dat & 1) == 0) {
		dprint(PRN_FATAL, "End0: SM_SCDIntServeProc()\n");
		return;
	}

	scd_drv_save_state_reg();
	wr_scd_reg(REG_SCD_INI_CLR, 1);
	VFMW_OSAL_GIVE_EVENT(G_SCDHWDONEEVENT);
}

VOID scd_drv_get_reg_state(scd_state_reg_s *p_scd_state_reg)
{
	hi_s32 ret;

	ret = memcpy_s(p_scd_state_reg, sizeof(*p_scd_state_reg),
		&g_scd_state_reg, sizeof(g_scd_state_reg));
	if (ret != EOK) {
		dprint(PRN_FATAL, " %s %d memcpy_s err in function\n", __func__, __LINE__);
		return;
	}
	scd_state = SCD_IDLE;
}

SINT32 wait_scd_finish(void)
{
	SINT32 i;

	if (scd_state == SCD_WORKING) {
		for (i = 0; i < SCD_TIME_OUT_COUNT; i++) {
			if ((rd_scd_reg(REG_SCD_OVER) & 1))
				return SCDDRV_OK;
		}

		return SCDDRV_ERR;
	} else {
		return SCDDRV_OK;
	}
}

static VOID print_scd_vtrl_reg(void)
{
	scd_config_reg_s sm_ctrl_reg;
	hi_s32 ret;

	ret = memset_s(&sm_ctrl_reg, sizeof(sm_ctrl_reg), 0, sizeof(sm_ctrl_reg));
	if (ret != EOK) {
		dprint(PRN_FATAL, " %s %d memset_s err in function\n", __func__, __LINE__);
		return;
	}

	sm_ctrl_reg.down_msg_phy_addr = rd_scd_reg(REG_LIST_ADDRESS);
	sm_ctrl_reg.up_msg_phy_addr   = rd_scd_reg(REG_UP_ADDRESS);
	sm_ctrl_reg.up_len          = rd_scd_reg(REG_UP_LEN);
	sm_ctrl_reg.buffer_first    = rd_scd_reg(REG_BUFFER_FIRST);
	sm_ctrl_reg.buffer_last     = rd_scd_reg(REG_BUFFER_LAST);
	sm_ctrl_reg.buffer_ini      = rd_scd_reg(REG_BUFFER_INI);
	sm_ctrl_reg.scd_protocol    = rd_scd_reg(REG_SCD_PROTOCOL);
	sm_ctrl_reg.scd_start       = rd_scd_reg(REG_SCD_START);

	dprint(PRN_SCD_REGMSG, "***Print Scd Vtrl Reg Now\n");
	dprint(PRN_SCD_REGMSG, "down_msg_phy_addr : %pK\n", (void *)(uintptr_t)(sm_ctrl_reg.down_msg_phy_addr));
	dprint(PRN_SCD_REGMSG, "up_msg_phy_addr : %pK\n", (void *)(uintptr_t)(sm_ctrl_reg.up_msg_phy_addr));
	dprint(PRN_SCD_REGMSG, "up_len : %d\n", sm_ctrl_reg.up_len);
	dprint(PRN_SCD_REGMSG, "buffer_first : %pK\n", (void *)(uintptr_t)(sm_ctrl_reg.buffer_first));
	dprint(PRN_SCD_REGMSG, "buffer_last : %pK\n", (void *)(uintptr_t)(sm_ctrl_reg.buffer_last));
	dprint(PRN_SCD_REGMSG, "buffer_ini : %pK\n", (void *)(uintptr_t)(sm_ctrl_reg.buffer_ini));
	dprint(PRN_SCD_REGMSG, "scd_protocol : %d\n", sm_ctrl_reg.scd_protocol);
	dprint(PRN_SCD_REGMSG, "scd_start : %d\n", sm_ctrl_reg.scd_start);
}

#ifdef ENV_ARMLINUX_KERNEL
SINT32 scd_drv_is_scd_idle(void)
{
	SINT32 ret = SCDDRV_OK;

	if (scd_state == SCD_IDLE) {
		ret = SCDDRV_OK;
	} else if (scd_state == SCD_WORKING) {
		ret = SCDDRV_ERR;
	} else {
		ret = SCDDRV_ERR;
		dprint(PRN_ERROR, "%s : scd_state : %d is wrong\n", __func__, scd_state);
	}
	return ret;
}

#endif
