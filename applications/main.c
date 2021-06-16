/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2020-04-16     bigmagic       first version
 */

#include <rtthread.h>

#ifdef RT_USING_DFS_ROMFS
#include "dfs_romfs.h"
#endif /* RT_USING_DFS_ROMFS */ 

// extern const struct romfs_dirent romfs_root; 

#define RGB16_TO_RGB24(p0, p1, p2, v1, v2)\
{\
    unsigned int r1,g1,b1,gb1,g2,b2,gb2;\
    r1 = (v1 & 0xF800F800);\
    g1 = (v1 & 0x07E007E0) << 5;\
    b1 = (v1 & 0x001F001F) << 3;\
    gb1 = g1 | b1;\
    p0 = ((gb1 & 0x0000FFFF) << 8) | (r1 << 16) | (r1 >> 24);\
    g2 = (v2 & 0x07E007E0) << 5;\
    b2 = (v2 & 0x001F001F) << 3;\
    gb2 = g2 | b2;\
    p1 = (gb1 & 0xFFFF0000) | (v2 & 0xF800) | ((gb2 >> 8) & 0xff);\
    p2 = (gb2 << 24) | ((v2 & 0xF8000000) >> 8) | (gb2 >> 16);\
}

int main(int argc, char** argv)
{
    rt_kprintf("Hi, This is RT-Thread !!\n"); 
    rt_kprintf("Hi, This is Rk3326/PX30 4!!\n"); 

    if (dfs_mount(RT_NULL, "/", "rom", 0, &(romfs_root)) == 0)
    {
        rt_kprintf("ROM file system initializated!\n");
    }
    else
    {
        rt_kprintf("ROM file system initializate failed!\n");
    }

    int p0 = 0, p1 = 0, p2 = 0;
    int v1 = 0xF81F242F, v2 = 0xF81F242F;

    RGB16_TO_RGB24(p0, p1, p2, v1, v2); 

    // 
    // 1111100000011111 0010010000101111
    // 111110000000000011111000001000001000010001111000
    rt_kprintf("p0 = 0x%.8x\n", p0); 
    rt_kprintf("p1 = 0x%.8x\n", p1); 
    rt_kprintf("p2 = 0x%.8x\n", p2); 

    return 0;
}
