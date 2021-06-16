/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "drv_lcd.h" 
#include "rthw.h"
#include "mmu.h"

struct rk3326_lcd
{
    struct rt_device device;
    struct rt_device_graphic_info info; 
};
static struct rk3326_lcd lcd; 

#define LCD_WIDTH           (320) 
#define LCD_HEIGHT          (480) 
#define LCD_FB_BASE         (0x3DE00000) 
#define LCD_BITS_PER_PIXEL  24
#define LCD_BYTES_PER_PIXEL (LCD_BITS_PER_PIXEL / 8)
#define LCD_BUF_SIZE        (LCD_WIDTH * LCD_HEIGHT * LCD_BYTES_PER_PIXEL)

static rt_err_t rk3326_lcd_init(rt_device_t device)
{
    lcd.info.width          = LCD_WIDTH;
    lcd.info.height         = LCD_HEIGHT;
    lcd.info.pixel_format   = RTGRAPHIC_PIXEL_FORMAT_RGB888;
    lcd.info.bits_per_pixel = LCD_BITS_PER_PIXEL;
    lcd.info.framebuffer    = (void *)(LCD_FB_BASE); 
    
    return RT_EOK; 
}

static rt_err_t rk3326_lcd_control(rt_device_t device, int cmd, void *args)
{
    switch(cmd)
    {
    case RTGRAPHIC_CTRL_RECT_UPDATE:
        rt_hw_dcache_invalidate_range((unsigned long)lcd.info.framebuffer, LCD_BUF_SIZE);
        break;

    case RTGRAPHIC_CTRL_POWERON: 
        break;

    case RTGRAPHIC_CTRL_POWEROFF: 
        break;

    case RTGRAPHIC_CTRL_GET_INFO:
        rt_memcpy(args, &lcd.info, sizeof(lcd.info));
        break;

    case RTGRAPHIC_CTRL_SET_MODE:
        break;

    case RTGRAPHIC_CTRL_GET_EXT:
        break;
    }

    return RT_EOK;
}

int rt_hw_lcd_init(void)
{
    rt_err_t ret; 
    
    rt_memset(&lcd, 0x00, sizeof(lcd)); 

    lcd.device.type    = RT_Device_Class_Graphic;
    lcd.device.init    = rk3326_lcd_init;
    lcd.device.open    = RT_NULL;
    lcd.device.close   = RT_NULL;
    lcd.device.read    = RT_NULL;
    lcd.device.write   = RT_NULL;
    lcd.device.control = rk3326_lcd_control;

    lcd.device.user_data = (void *)&lcd.info;

    ret = rt_device_register(&lcd.device, "lcd", RT_DEVICE_FLAG_RDWR);

    return ret;
}
INIT_DEVICE_EXPORT(rt_hw_lcd_init); 

int lcd_test(void)
{
    struct rk3326_lcd *tlcd;

    tlcd = (struct rk3326_lcd *)rt_device_find("lcd");
    rt_device_open(&(tlcd->device), RT_DEVICE_FLAG_WRONLY); 

    while (1)
    {
        if (tlcd->info.pixel_format == RTGRAPHIC_PIXEL_FORMAT_RGB565)
         {
            /* red */
            for (int i = 0; i < LCD_BUF_SIZE / 2; i++)
            {
                tlcd->info.framebuffer[2 * i] = 0x00;
                tlcd->info.framebuffer[2 * i + 1] = 0xF8;
            }
            tlcd->device.control(&tlcd->device, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
            rt_thread_mdelay(1000);
            /* green */
            for (int i = 0; i < LCD_BUF_SIZE / 2; i++)
            {
                tlcd->info.framebuffer[2 * i] = 0xE0;
                tlcd->info.framebuffer[2 * i + 1] = 0x07;
            }
            tlcd->device.control(&tlcd->device, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
            rt_thread_mdelay(1000);
            /* blue */
            for (int i = 0; i < LCD_BUF_SIZE / 2; i++)
            {
                tlcd->info.framebuffer[2 * i] = 0x1F;
                tlcd->info.framebuffer[2 * i + 1] = 0x00;
            }
         }
         else if (tlcd->info.pixel_format == RTGRAPHIC_PIXEL_FORMAT_RGB888)
         {
             /* red */
             for (int i = 0; i < LCD_BUF_SIZE / 3; i++)
             {
                 tlcd->info.framebuffer[3 * i] = 0x00;
                 tlcd->info.framebuffer[3 * i + 1] = 0x00;
                 tlcd->info.framebuffer[3 * i + 2] = 0xff;
             }
             tlcd->device.control(&tlcd->device, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
             rt_thread_mdelay(1000);
             /* green */
             for (int i = 0; i < LCD_BUF_SIZE / 3; i++)
             {
                 tlcd->info.framebuffer[3 * i] = 0x00;
                 tlcd->info.framebuffer[3 * i + 1] = 0xff;
                 tlcd->info.framebuffer[3 * i + 2] = 0x00;
             }
             tlcd->device.control(&tlcd->device, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
             rt_thread_mdelay(1000);
             /* blue */
             for (int i = 0; i < LCD_BUF_SIZE / 3; i++)
             {
                 tlcd->info.framebuffer[3 * i] = 0xff;
                 tlcd->info.framebuffer[3 * i + 1] = 0x00;
                 tlcd->info.framebuffer[3 * i + 2] = 0x00;
             }
         }
         else if (tlcd->info.pixel_format == RTGRAPHIC_PIXEL_FORMAT_ARGB888)
         {
             /* red */
             for (int i = 0; i < LCD_BUF_SIZE / 4; i++)
             {
                 tlcd->info.framebuffer[3 * i] = 0x00;
                 tlcd->info.framebuffer[3 * i + 1] = 0x00;
                 tlcd->info.framebuffer[3 * i + 2] = 0xff;
                 tlcd->info.framebuffer[3 * i + 3] = 0xff;
             }
             tlcd->device.control(&tlcd->device, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
             rt_thread_mdelay(1000);
             /* green */
             for (int i = 0; i < LCD_BUF_SIZE / 4; i++)
             {
                 tlcd->info.framebuffer[3 * i] = 0x00;
                 tlcd->info.framebuffer[3 * i + 1] = 0xff;
                 tlcd->info.framebuffer[3 * i + 2] = 0x00;
                 tlcd->info.framebuffer[3 * i + 3] = 0xff;
             }
             tlcd->device.control(&tlcd->device, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
             rt_thread_mdelay(1000);
             /* blue */
             for (int i = 0; i < LCD_BUF_SIZE / 4; i++)
             {
                 tlcd->info.framebuffer[3 * i] = 0xff;
                 tlcd->info.framebuffer[3 * i + 1] = 0x00;
                 tlcd->info.framebuffer[3 * i + 2] = 0x00;
                 tlcd->info.framebuffer[3 * i + 3] = 0xff;
             }
         }

        tlcd->device.control(&tlcd->device, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(1000);
    }
}
MSH_CMD_EXPORT(lcd_test, lcd_test);
