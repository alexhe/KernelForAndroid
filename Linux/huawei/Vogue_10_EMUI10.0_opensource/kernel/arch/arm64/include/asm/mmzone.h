/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_MMZONE_H
#define __ASM_MMZONE_H

#ifdef CONFIG_NUMA

#include <asm/numa.h>

extern struct pglist_data *node_data[]; //helin: nodes array.
#define NODE_DATA(nid)		(node_data[(nid)]) //helin: node index in array.

#endif /* CONFIG_NUMA */
#endif /* __ASM_MMZONE_H */
