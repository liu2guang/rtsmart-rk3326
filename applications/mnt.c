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
    return 0;
}
INIT_ENV_EXPORT(mnt_init);
