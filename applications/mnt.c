/*
 * RT-Thread Secure
 * 
 * Copyright (c) 2021, Shanghai Real-Thread Electronic Technology Co., Ltd.
 *
 * All rights reserved.

 */
#include <rtthread.h>
#include <dfs.h>
#include <dfs_fs.h>
#include <dfs_file.h>

int mnt_init(void)
{
    rt_thread_mdelay(200);
    if (dfs_mount("sd0", "/", "elm", 0, 0) != 0)
    {
        rt_kprintf("file system initialization fail!\n");
    }

    return 0;
}
INIT_APP_EXPORT(mnt_init);
