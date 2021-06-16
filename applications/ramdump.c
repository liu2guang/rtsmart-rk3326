/*
 * @File:   hexdump.c
 * @Author: liu2guang
 * @Date:   2018-10-12 14:56:10
 *
 * @LICENSE: MIT
 * https://github.com/rtpkgs/libtest/blob/master/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-10-12     liu2guang    update RT-Thread hexdump toolkis.
 */

#include "rtthread.h" 
#include <stdio.h>

void hexdump(const void *p, rt_uint32_t len)
{
    unsigned char *buf = (unsigned char *)p;
    int i = 0, j=  0;
    
    rt_kprintf("Dump %p %dBytes\n", p, len); 
    rt_kprintf("Offset            00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"); 

    for (i=0; i<len; i+=16)
    {
        rt_kprintf("%p: ", &buf[i+j]);

        for (j=0; j<16; j++)
        {
            if (i+j < len)
            {
                rt_kprintf("%02X ", buf[i+j]);
            }
            else
            {
                rt_kprintf("   ");
            }
        } 
        rt_kprintf(" ");

        for (j=0; j<16; j++)
        {
            if (i+j < len)
            {
                rt_kprintf("%c", ((unsigned int)((buf[i+j]) - ' ') < 127u - ' ') ? buf[i+j] : '.');
            }
        }
        rt_kprintf("\n");
    }
}

int ramdump(int argc, char *argv[])
{
    rt_uint32_t addr = 0; 
    rt_uint32_t len = 0; 

    if(argc != 3)
    {
		rt_kprintf("example: ramdump 0x0000000000000000 len\n");
        return -1; 
    }

    sscanf(argv[1],"%x", &addr);  
    sscanf(argv[2],"%d", &len); 

    hexdump((const rt_uint8_t *)addr, len); 
    
    return 0; 
}
MSH_CMD_EXPORT(ramdump, dump ram info.); 
