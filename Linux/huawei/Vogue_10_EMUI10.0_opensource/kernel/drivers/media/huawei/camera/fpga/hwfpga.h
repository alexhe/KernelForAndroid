
/*
 *  Hisilicon K3 SOC camera driver source file
 *
 *  Copyright (C) Huawei Technology Co., Ltd.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __HW_ALAN_KERNEL_FPGA_INTERFACE_H__
#define __HW_ALAN_KERNEL_FPGA_INTERFACE_H__

#include <linux/videodev2.h>
#include <media/huawei/camera.h>
#include <media/huawei/fpga_cfg.h>
#include <media/videobuf2-core.h>

#include "hwcam_intf.h"

struct hwfpga_notify_intf_t;

struct hwfpga_notify_vtbl_t {
	void (*error)(struct hwfpga_notify_intf_t *i, hwfpga_event_t *fpga_ev);
};

struct hwfpga_notify_intf_t {
	struct hwfpga_notify_vtbl_t *vtbl;
};

struct hwfpga_intf_t;

struct hwfpga_vtbl_t {
	char const *(*get_name)(const struct hwfpga_intf_t *i);
	int (*power_on)(const struct hwfpga_intf_t *i);
	int (*power_off)(const struct hwfpga_intf_t *i);
	int (*load_firmware)(const struct hwfpga_intf_t *i);
	int (*enable)(const struct hwfpga_intf_t *i);
	int (*disable)(const struct hwfpga_intf_t *i);
	int (*init)(const struct hwfpga_intf_t *i);
	int (*close)(const struct hwfpga_intf_t *i);
	int (*fpga_get_dt_data) (const struct hwfpga_intf_t *, struct device_node *);
	int (*fpga_register_attribute)(const struct hwfpga_intf_t *, struct device *);
	int (*checkdevice)(const struct hwfpga_intf_t *i);
};

struct hwfpga_intf_t {
	struct hwfpga_vtbl_t *vtbl;
};

struct fpga_t {
	struct hwfpga_intf_t intf;
	char name[HWFPGA_NAME_SIZE];
	struct hwfpga_notify_intf_t *notify;
};

static inline char const *hwfpga_intf_get_name(const struct hwfpga_intf_t *i)
{
	return i->vtbl->get_name(i);
}

static inline int hwfpga_intf_power_on(const struct hwfpga_intf_t *i)
{
	return i->vtbl->power_on(i);
}

static inline int hwfpga_intf_power_off(const struct hwfpga_intf_t *i)
{
	return i->vtbl->power_off(i);
}

static inline int hwfpga_intf_load_firmware(const struct hwfpga_intf_t *i)
{
	return i->vtbl->load_firmware(i);
}

static inline int hwfpga_intf_enable(const struct hwfpga_intf_t *i)
{
	return i->vtbl->enable(i);
}

static inline void hwfpga_intf_notify_error(struct hwfpga_notify_intf_t *i,
	                                    hwfpga_event_t *fpga_ev)
{
	return i->vtbl->error(i, fpga_ev);
}

extern int32_t hwfpga_register(struct platform_device *pdev,
	const struct hwfpga_intf_t *i, struct hwfpga_notify_intf_t **notify);
void hwfpga_unregister(struct platform_device *pdev);
#endif // __HW_ALAN_KERNEL_FPGA_INTERFACE_H__

