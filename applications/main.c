/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <rtthread.h>
// #include <sal_types.h>
// #include <init.h>

int main(int argc, char** argv)
{
    rt_kprintf("Hi, This is RT-Thread !!\n"); 
    rt_kprintf("Hi, This is Rk3326/PX30!!\n"); 

    // init_ipicli(); 
    return 0;
}

// int cmd_show_help(int argc, char *argv[])
// {
//     show_help(argv[1]); 
//     return 0; 
// }
// MSH_CMD_EXPORT_ALIAS(cmd_show_help, show_help, show_help); 

// int cmd_excute_command(int argc, char *argv[])
// {
//     excute_command(argv[1]); 
//     return 0; 
// }
// MSH_CMD_EXPORT_ALIAS(cmd_excute_command, excute_command, excute_command); 
