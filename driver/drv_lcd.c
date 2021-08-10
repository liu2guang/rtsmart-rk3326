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

#include <board.h>

// LCD 复位引脚 &gpio3 RK_PC0 GPIO_ACTIVE_LOW

#define RK3326_DUMP_REG(addr)             \
do {                                    \
    rt_uint32_t val = 0x00000000;       \
    val = readl(addr);                  \
    rt_kprintf(#addr ":\n");            \
    rt_kprintf("\t0x%.8x\n", val);      \
} while(0); 

#define MIPI_BASE                           0xFF450000
#define MIPI_DSI_HOST_VERSION               MIPI_BASE + 0x0000  // 0x3133302a
#define MIPI_DSI_HOST_PWR_UP                MIPI_BASE + 0x0004  // 0x00000001 mipi dsi控制器上电, 默认0, 上电1
#define MIPI_DSI_HOST_CLKMGR_CFG            MIPI_BASE + 0x0008  // 0x00000a03 时钟管理 to_clk_division = 10, tx_esc_clk_division = 3
#define MIPI_DSI_HOST_DPI_VCID              MIPI_BASE + 0x000c  // 0x00000000 
#define MIPI_DSI_HOST_DPI_COLOR_CODING      MIPI_BASE + 0x0010  // 0x00000005 24-bit    
#define MIPI_DSI_HOST_DPI_CF                MIPI_BASE + 0x0014  // 0x00000006 colorm_active_low = 0, shutd_active_low = 0, hsync_active_low = 1, vsync_active_low = 1, dataen_active_low = 0
#define MIPI_DSI_HOST_DPI_LP_CMD_TIM        MIPI_BASE + 0x0018  // 0x00040004 outvact_lpcmd_time = 4, invact_lpcmd_time = 4
#define MIPI_DSI_HOST_PCKHDL_CFG            MIPI_BASE + 0x002c  // 0x0000001c crc_rx_en = 1, ecc_rx_en = 1, bta_en = 1, eotp_rx_en = 0, eotp_tx_en = 0
#define MIPI_DSI_HOST_GEN_VCID              MIPI_BASE + 0x0030  // 0x00000000 gen_vcid_rx = 0
#define MIPI_DSI_HOST_MODE_CFG              MIPI_BASE + 0x0034  // 0x00000000
#define MIPI_DSI_HOST_VID_MODE_CFG          MIPI_BASE + 0x0038  // 0x00000000
#define MIPI_DSI_HOST_VID_PKT_SIZE          MIPI_BASE + 0x003c  // 0x00000140
#define MIPI_DSI_HOST_VID_NUM_CHUNKS        MIPI_BASE + 0x0040  // 0x00000000
#define MIPI_DSI_HOST_VID_NULL_SIZE         MIPI_BASE + 0x0044  // 0x00000000
#define MIPI_DSI_HOST_VID_HSA_TIME          MIPI_BASE + 0x0048  // 0x0000000d
#define MIPI_DSI_HOST_VID_HBP_TIME          MIPI_BASE + 0x004c  // 0x000001ae
#define MIPI_DSI_HOST_VID_HLINE_TIME        MIPI_BASE + 0x0050  // 0x0000078c
#define MIPI_DSI_HOST_VID_VSA_LINES         MIPI_BASE + 0x0054  // 0x00000001
#define MIPI_DSI_HOST_VID_VBP_LINES         MIPI_BASE + 0x0058  // 0x00000002
#define MIPI_DSI_HOST_VID_VFP_LINES         MIPI_BASE + 0x005c  // 0x00000002
#define MIPI_DSI_HOST_VID_VACTIVE_LINES     MIPI_BASE + 0x0060  // 0x000001e0
#define MIPI_DSI_HOST_EDPI_CMD_SIZE         MIPI_BASE + 0x0064  // 0x00000000
#define MIPI_DSI_HOST_CMD_MODE_CFG          MIPI_BASE + 0x0068  // 0x000b0000
#define MIPI_DSI_HOST_GEN_HDR               MIPI_BASE + 0x006c  // 0x00002905
#define MIPI_DSI_HOST_GEN_PLD_DATA          MIPI_BASE + 0x0070  // 0x00000000
#define MIPI_DSI_HOST_CMD_PKT_STATUS        MIPI_BASE + 0x0074  // 0x00000015
#define MIPI_DSI_HOST_TO_CNT_CFG            MIPI_BASE + 0x0078  // 0x03e803e8
#define MIPI_DSI_HOST_HS_RD_TO_CNT          MIPI_BASE + 0x007c  // 0x00000000
#define MIPI_DSI_HOST_LP_RD_TO_CNT          MIPI_BASE + 0x0080  // 0x00000000
#define MIPI_DSI_HOST_HS_WR_TO_CNT          MIPI_BASE + 0x0084  // 0x00000000
#define MIPI_DSI_HOST_LP_WR_TO_CNT          MIPI_BASE + 0x0088  // 0x00000000
#define MIPI_DSI_HOST_BTA_TO_CNT            MIPI_BASE + 0x008c  // 0x00000d00
#define MIPI_DSI_HOST_SDF_3D                MIPI_BASE + 0x0090  // 0x00000000
#define MIPI_DSI_HOST_LPCLK_CTRL            MIPI_BASE + 0x0094  // 0x00000001
#define MIPI_DSI_HOST_PHY_TMR_LPCLK_CFG     MIPI_BASE + 0x0098  // 0x00400040
#define MIPI_DSI_HOST_PHY_TMR_CFG           MIPI_BASE + 0x009c  // 0x14102710
#define MIPI_DSI_HOST_PHY_RSTZ              MIPI_BASE + 0x00a0  // 0x00000007
#define MIPI_DSI_HOST_PHY_IF_CFG            MIPI_BASE + 0x00a4  // 0x00002000
#define MIPI_DSI_HOST_PHY_STATUS            MIPI_BASE + 0x00b0  // 0x00001529
#define MIPI_DSI_HOST_INT_ST0               MIPI_BASE + 0x00bc  // 0x00000000
#define MIPI_DSI_HOST_INT_ST1               MIPI_BASE + 0x00c0  // 0x00000880
#define MIPI_DSI_HOST_INT_MSK0              MIPI_BASE + 0x00c4  // 0x00000000
#define MIPI_DSI_HOST_INT_MSK1              MIPI_BASE + 0x00c8  // 0x00000000
#define MIPI_DSI_HOST_INT_FORCE0            MIPI_BASE + 0x00d8  // 0x00000000
#define MIPI_DSI_HOST_INT_FORCE1            MIPI_BASE + 0x00dc  // 0x00000000
#define MIPI_DSI_HOST_VID_SHADOW_CTRL       MIPI_BASE + 0x0100  // 0x00000000
#define MIPI_DSI_HOST_DPI_VCID_ACT          MIPI_BASE + 0x010c  // 0x00000000
#define MIPI_DSI_HOST_DPI_COLOR_CODING_ACT  MIPI_BASE + 0x0110  // 0x00000000
#define MIPI_DSI_HOST_DPI_LP_CMD_TIM_ACT    MIPI_BASE + 0x0118  // 0x00000000
#define MIPI_DSI_HOST_VID_MODE_CFG_ACT      MIPI_BASE + 0x0138  // 0x00000000
#define MIPI_DSI_HOST_VID_PKT_SIZE_ACT      MIPI_BASE + 0x013c  // 0x00000000
#define MIPI_DSI_HOST_VID_NUM_CHUNKS_ACT    MIPI_BASE + 0x0140  // 0x00000000
#define MIPI_DSI_HOST_VID_NULL_SIZE_ACT     MIPI_BASE + 0x0144  // 0x00000000
#define MIPI_DSI_HOST_VID_HSA_TIME_ACT      MIPI_BASE + 0x0148  // 0x00000000
#define MIPI_DSI_HOST_VID_HBP_TIME_ACT      MIPI_BASE + 0x014c  // 0x00000000
#define MIPI_DSI_HOST_VID_HLINE_TIME_ACT    MIPI_BASE + 0x0150  // 0x00000000
#define MIPI_DSI_HOST_VID_VSA_LINES_ACT     MIPI_BASE + 0x0154  // 0x00000000
#define MIPI_DSI_HOST_VID_VBP_LINES_ACT     MIPI_BASE + 0x0158  // 0x00000000
#define MIPI_DSI_HOST_PHY_RSTZ              MIPI_BASE + 0x00a0  // 0x00000007
#define MIPI_DSI_HOST_PHY_IF_CFG            MIPI_BASE + 0x00a4  // 0x00002000
#define MIPI_DSI_HOST_PHY_STATUS            MIPI_BASE + 0x00b0  // 0x00001529
#define MIPI_DSI_HOST_INT_ST0               MIPI_BASE + 0x00bc  // 0x00000000
#define MIPI_DSI_HOST_INT_ST1               MIPI_BASE + 0x00c0  // 0x00000000
#define MIPI_DSI_HOST_INT_MSK0              MIPI_BASE + 0x00c4  // 0x00000000
#define MIPI_DSI_HOST_INT_MSK1              MIPI_BASE + 0x00c8  // 0x00000000
#define MIPI_DSI_HOST_INT_FORCE0            MIPI_BASE + 0x00d8  // 0x00000000
#define MIPI_DSI_HOST_INT_FORCE1            MIPI_BASE + 0x00dc  // 0x00000000
#define MIPI_DSI_HOST_VID_SHADOW_CTRL       MIPI_BASE + 0x0100  // 0x00000000
#define MIPI_DSI_HOST_DPI_VCID_ACT          MIPI_BASE + 0x010c  // 0x00000000
#define MIPI_DSI_HOST_DPI_COLOR_CODING_ACT  MIPI_BASE + 0x0110  // 0x00000000
#define MIPI_DSI_HOST_DPI_LP_CMD_TIM_ACT    MIPI_BASE + 0x0118  // 0x00000000
#define MIPI_DSI_HOST_VID_MODE_CFG_ACT      MIPI_BASE + 0x0138  // 0x00000000
#define MIPI_DSI_HOST_VID_PKT_SIZE_ACT      MIPI_BASE + 0x013c  // 0x00000000
#define MIPI_DSI_HOST_VID_NUM_CHUNKS_ACT    MIPI_BASE + 0x0140  // 0x00000000
#define MIPI_DSI_HOST_VID_NULL_SIZE_ACT     MIPI_BASE + 0x0144  // 0x00000000
#define MIPI_DSI_HOST_VID_HSA_TIME_ACT      MIPI_BASE + 0x0148  // 0x00000000
#define MIPI_DSI_HOST_VID_HBP_TIME_ACT      MIPI_BASE + 0x014c  // 0x00000000
#define MIPI_DSI_HOST_VID_HLINE_TIME_ACT    MIPI_BASE + 0x0150  // 0x00000000
#define MIPI_DSI_HOST_VID_VSA_LINES_ACT     MIPI_BASE + 0x0154  // 0x00000000
#define MIPI_DSI_HOST_VID_VBP_LINES_ACT     MIPI_BASE + 0x0158  // 0x00000000

#define MIPI_DSIPHY_BASE                    0xFF450000
#define MIPI_DSIPHY_CTRL_LANE_ENABLE        MIPI_DSIPHY_BASE + 0x0000  

#define MIPI_DSIPHY_CTRL_PWRCTL             MIPI_DSIPHY_BASE + 0x0004 
#define MIPI_DSIPHY_CTRL_PREDIV             MIPI_DSIPHY_BASE + 0x000c 
#define MIPI_DSIPHY_CTRL_FBDIV              MIPI_DSIPHY_BASE + 0x0010 
#define MIPI_DSIPHY_CTRL_DIG_RST            MIPI_DSIPHY_BASE + 0x0080
#define MIPI_DSIPHY_CTRL_SIG_INV            MIPI_DSIPHY_BASE + 0x0084

#define MIPI_DSIPHY_LVDSTTL_ctrl            MIPI_DSIPHY_BASE + 0x0380 
#define MIPI_DSIPHY_LVDSTTL_digital_en      MIPI_DSIPHY_BASE + 0x0384
#define MIPI_DSIPHY_LVDSTTL_mode_en         MIPI_DSIPHY_BASE + 0x038c
#define MIPI_DSIPHY_LVDSTTL_lane_en         MIPI_DSIPHY_BASE + 0x03ac

int mipi_test(void)
{
    RK3326_DUMP_REG(MIPI_DSI_HOST_VERSION); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_PWR_UP); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_CLKMGR_CFG); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_DPI_VCID); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_DPI_COLOR_CODING); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_DPI_CF); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_DPI_LP_CMD_TIM); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_PCKHDL_CFG); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_GEN_VCID); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_MODE_CFG); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_PKT_SIZE); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_NUM_CHUNKS); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_NULL_SIZE); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_HSA_TIME); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_HBP_TIME); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_HLINE_TIME); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_VSA_LINES); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_VBP_LINES); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_VFP_LINES); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_VACTIVE_LINES); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_EDPI_CMD_SIZE); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_CMD_MODE_CFG); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_GEN_HDR); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_GEN_PLD_DATA); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_CMD_PKT_STATUS); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_TO_CNT_CFG); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_HS_RD_TO_CNT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_LP_RD_TO_CNT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_HS_WR_TO_CNT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_LP_WR_TO_CNT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_BTA_TO_CNT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_SDF_3D); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_LPCLK_CTRL); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_PHY_TMR_LPCLK_CFG); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_PHY_TMR_CFG); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_PHY_RSTZ); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_PHY_IF_CFG); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_PHY_STATUS); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_ST0); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_ST1); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_MSK0); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_MSK1); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_FORCE0); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_FORCE1); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_SHADOW_CTRL); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_DPI_VCID_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_DPI_COLOR_CODING_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_DPI_LP_CMD_TIM_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_MODE_CFG_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_PKT_SIZE_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_NUM_CHUNKS_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_NULL_SIZE_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_HSA_TIME_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_HBP_TIME_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_HLINE_TIME_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_VSA_LINES_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_VBP_LINES_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_PHY_RSTZ); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_PHY_IF_CFG); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_PHY_STATUS); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_ST0); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_ST1); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_MSK0); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_MSK1); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_FORCE0); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_INT_FORCE1); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_SHADOW_CTRL); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_DPI_VCID_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_DPI_COLOR_CODING_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_DPI_LP_CMD_TIM_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_MODE_CFG_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_PKT_SIZE_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_NUM_CHUNKS_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_NULL_SIZE_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_HSA_TIME_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_HBP_TIME_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_HLINE_TIME_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_VSA_LINES_ACT); 
    RK3326_DUMP_REG(MIPI_DSI_HOST_VID_VBP_LINES_ACT); 

    return 0; 
}
MSH_CMD_EXPORT(mipi_test, mipi_test);

#define VOP_LITE_BASE                0xFF470000
#define VOP_LITE_REG_CFG_DONE        VOP_LITE_BASE + 0x0000 //  W 0x00000000 Register config done flag
#define VOP_LITE_VERSION             VOP_LITE_BASE + 0x0004 //  W 0x00000000 Version for vop
#define VOP_LITE_DSP_BG              VOP_LITE_BASE + 0x0008 //  W 0x00000000 Display control register
#define VOP_LITE_MCU                 VOP_LITE_BASE + 0x000c //  W 0x00000000 MCU control register
#define VOP_LITE_SYS_CTRL0           VOP_LITE_BASE + 0x0010 //  W 0x00000000 System control register
#define VOP_LITE_SYS_CTRL1           VOP_LITE_BASE + 0x0014 //  W 0x00000000 System control register
#define VOP_LITE_SYS_CTRL2           VOP_LITE_BASE + 0x0018 //  W 0x00006000 System control register
#define VOP_LITE_DSP_CTRL0           VOP_LITE_BASE + 0x0020 //  W 0x00000101 Display register
#define VOP_LITE_DSP_CTRL1           VOP_LITE_BASE + 0x0024 //  W 0x00000000 Display register
#define VOP_LITE_DSP_CTRL2           VOP_LITE_BASE + 0x0028 //  W 0x00004000 Display register
#define VOP_LITE_VOP_STATUS          VOP_LITE_BASE + 0x002c //  W 0x00000000 Some vop module status
#define VOP_LITE_LINE_FLAG           VOP_LITE_BASE + 0x0030 //  W 0x00000000 Line flag config register
#define VOP_LITE_INTR_EN             VOP_LITE_BASE + 0x0034 //  W 0x00000000 Interrupt enable register
#define VOP_LITE_INTR_CLEAR          VOP_LITE_BASE + 0x0038 //  W 0x00000000 Interrupt clear register
#define VOP_LITE_INTR_STATUS         VOP_LITE_BASE + 0x003c //  W 0x00000000 Interrupt raw status and interruptstatus
#define VOP_LITE_WIN1_CTRL0          VOP_LITE_BASE + 0x0090 //  W 0x00000000 Win1 ctrl register
#define VOP_LITE_WIN1_CTRL1          VOP_LITE_BASE + 0x0094 //  W 0x00000500 Win1 ctrl register
#define VOP_LITE_WIN1_VIR            VOP_LITE_BASE + 0x0098 //  W 0x00000000 Win1 virtual stride
#define VOP_LITE_WIN1_MST            VOP_LITE_BASE + 0x00a0 //  W 0x00000000 Win1 memory start address
#define VOP_LITE_WIN1_DSP_INFO       VOP_LITE_BASE + 0x00a4 //  W 0x00ef013f Win1 display width/height on panel
#define VOP_LITE_WIN1_DSP_ST         VOP_LITE_BASE + 0x00a8 //  W 0x0000000a Win1 display start point on panel
#define VOP_LITE_WIN1_COLOR_KEY      VOP_LITE_BASE + 0x00ac //  W 0x00000000 Win1 color key register
#define VOP_LITE_WIN1_ALPHA_CTRL     VOP_LITE_BASE + 0x00bc //  W 0x00000000 Blending control register
#define VOP_LITE_HWC_CTRL0           VOP_LITE_BASE + 0x00e0 //  W 0x00000000 Hwc ctrl register
#define VOP_LITE_HWC_MST             VOP_LITE_BASE + 0x00e8 //  W 0x00000000 Hwc memory start address
#define VOP_LITE_HWC_DSP_ST          VOP_LITE_BASE + 0x00ec //  W 0x00000000 Hwc display start point on panel
#define VOP_LITE_HWC_ALPHA_CTRL      VOP_LITE_BASE + 0x00f0 //  W 0x00000000 Blending control register
#define VOP_LITE_DSP_HTOTAL_HS_END   VOP_LITE_BASE + 0x0100 //  W 0x014a000a Panel scanning horizontal width and hsync pulse end point
#define VOP_LITE_DSP_HACT_ST_END     VOP_LITE_BASE + 0x0104 //  W 0x000a0000 Panel active horizontal scanning start point and end point
#define VOP_LITE_DSP_VTOTAL_VS_END   VOP_LITE_BASE + 0x0108 //  W 0x00fa000a Panel scanning vertical height and vsync pulse end point
#define VOP_LITE_DSP_VACT_ST_END     VOP_LITE_BASE + 0x010c //  W 0x000a00fa Panel active vertical scanning start point and end point
#define VOP_LITE_DSP_VS_ST_END_F1    VOP_LITE_BASE + 0x0110 //  W 0x00000000 Vertical scanning start point and vsync pulse end point of even file
#define VOP_LITE_DSP_VACT_ST_END_F1  VOP_LITE_BASE + 0x0114 //  W 0x00000000 Vertical scanning active start point and end point of even filed in interlac
#define VOP_LITE_BCSH_CTRL           VOP_LITE_BASE + 0x0160 //  W 0x00000000 Brightness/Contrast enhancement/Saturation/Hue contrl
#define VOP_LITE_BCSH_COL_BAR        VOP_LITE_BASE + 0x0164 //  W 0x00000000 vedio mode equals 2,then output color bar yuv 24bits value
#define VOP_LITE_BCSH_BCS            VOP_LITE_BASE + 0x0168 //  W 0x00000000 Brightness/Contrast enhancement/Saturation
#define VOP_LITE_BCSH_H              VOP_LITE_BASE + 0x016c //  W 0x00000000 Hue
#define VOP_LITE_FRC_LOWER01_0       VOP_LITE_BASE + 0x0170 //  W 0x12844821 Frc algorithm configuration register
#define VOP_LITE_FRC_LOWER01_1       VOP_LITE_BASE + 0x0174 //  W 0x21488412 Frc algorithm configuration register
#define VOP_LITE_FRC_LOWER10_0       VOP_LITE_BASE + 0x0178 //  W 0xa55a9696 Frc algorithm configuration register
#define VOP_LITE_FRC_LOWER10_1       VOP_LITE_BASE + 0x017c //  W 0x5aa56969 Frc algorithm configuration register
#define VOP_LITE_FRC_LOWER11_0       VOP_LITE_BASE + 0x0180 //  W 0xdeb77bed Frc algorithm configuration register
#define VOP_LITE_FRC_LOWER11_1       VOP_LITE_BASE + 0x0184 //  W 0xed7bb7de Frc algorithm configuration register
#define VOP_LITE_MCU_RW_BYPASS_PORT  VOP_LITE_BASE + 0x018c //  W 0x00000000 MCU panel write data
#define VOP_LITE_DBG_REG_SCAN_LINE   VOP_LITE_BASE + 0x0190 //  W 0x00000000 Current line number of dsp timing
#define VOP_LITE_BLANKING_VALUE      VOP_LITE_BASE + 0x0194 //  W 0x00000000 The value of vsync blanking
#define VOP_LITE_FLAG_REG_FRM_VALID  VOP_LITE_BASE + 0x0198 //  W 0x00000000 Flag reg value after frame valid
#define VOP_LITE_FLAG_REG            VOP_LITE_BASE + 0x019c //  W 0x00000000 Flag reg value before frame valid
#define VOP_LITE_HWC_LUT_ADDR        VOP_LITE_BASE + 0x0600 //  W 0x00000000 note: used for HWC BPP format ,base address: 0x600 -- 0x9FF
#define VOP_LITE_GAMMA_LUT_ADDR      VOP_LITE_BASE + 0x0a00 //  W 0x00000000 note: SIZE: 24X256 used for panel GAMMA adjustment, base address: 0x0a00 -- 0x0dff
#define VOP_LITE_MMU_DTE_ADDR        VOP_LITE_BASE + 0x0f00 //  W 0x00000000 MMU current page Table address
#define VOP_LITE_MMU_STATUS          VOP_LITE_BASE + 0x0f04 //  W 0x00000000 MMU status register
#define VOP_LITE_MMU_COMMAND         VOP_LITE_BASE + 0x0f08 //  W 0x00000000 MMU command register
#define VOP_LITE_MMU_PAGE_FAULT_ADDR VOP_LITE_BASE + 0x0f0c //  W 0x00000000 MMU logical address of last page fault
#define VOP_LITE_MMU_ZAP_ONE_LINE    VOP_LITE_BASE + 0x0f10 //  W 0x00000000 MMU Zap cache line register
#define VOP_LITE_MMU_INT_RAWSTAT     VOP_LITE_BASE + 0x0f14 //  W 0x00000000 MMU raw interrupt status register
#define VOP_LITE_MMU_INT_CLEAR       VOP_LITE_BASE + 0x0f18 //  W 0x00000000 MMU raw interrupt status register
#define VOP_LITE_MMU_INT_MASK        VOP_LITE_BASE + 0x0f1c //  W 0x00000000 MMU raw interrupt status register
#define VOP_LITE_MMU_INT_STATUS      VOP_LITE_BASE + 0x0f20 //  W 0x00000000 MMU raw interrupt status register
#define VOP_LITE_MMU_AUTO_GATING     VOP_LITE_BASE + 0x0f24 //  W 0x00000000 mmu auto gating
#define VOP_LITE_MMU_CFG_DONE        VOP_LITE_BASE + 0x0f28 //  W 0x00000000 mmu config done reg

int vopl_test(void)
{
    RK3326_DUMP_REG(VOP_LITE_REG_CFG_DONE);        
    RK3326_DUMP_REG(VOP_LITE_VERSION);            
    RK3326_DUMP_REG(VOP_LITE_DSP_BG);             
    RK3326_DUMP_REG(VOP_LITE_MCU);                
    RK3326_DUMP_REG(VOP_LITE_SYS_CTRL0);          
    RK3326_DUMP_REG(VOP_LITE_SYS_CTRL1);          
    RK3326_DUMP_REG(VOP_LITE_SYS_CTRL2);          
    RK3326_DUMP_REG(VOP_LITE_DSP_CTRL0);          
    RK3326_DUMP_REG(VOP_LITE_DSP_CTRL1);          
    RK3326_DUMP_REG(VOP_LITE_DSP_CTRL2);          
    RK3326_DUMP_REG(VOP_LITE_VOP_STATUS);         
    RK3326_DUMP_REG(VOP_LITE_LINE_FLAG);          
    RK3326_DUMP_REG(VOP_LITE_INTR_EN);            
    RK3326_DUMP_REG(VOP_LITE_INTR_CLEAR);         
    RK3326_DUMP_REG(VOP_LITE_INTR_STATUS);        
    RK3326_DUMP_REG(VOP_LITE_WIN1_CTRL0);         
    RK3326_DUMP_REG(VOP_LITE_WIN1_CTRL1);         
    RK3326_DUMP_REG(VOP_LITE_WIN1_VIR);           
    RK3326_DUMP_REG(VOP_LITE_WIN1_MST);           
    RK3326_DUMP_REG(VOP_LITE_WIN1_DSP_INFO);      
    RK3326_DUMP_REG(VOP_LITE_WIN1_DSP_ST);        
    RK3326_DUMP_REG(VOP_LITE_WIN1_COLOR_KEY);     
    RK3326_DUMP_REG(VOP_LITE_WIN1_ALPHA_CTRL);    
    RK3326_DUMP_REG(VOP_LITE_HWC_CTRL0);          
    RK3326_DUMP_REG(VOP_LITE_HWC_MST);            
    RK3326_DUMP_REG(VOP_LITE_HWC_DSP_ST);         
    RK3326_DUMP_REG(VOP_LITE_HWC_ALPHA_CTRL);     
    RK3326_DUMP_REG(VOP_LITE_DSP_HTOTAL_HS_END);   
    RK3326_DUMP_REG(VOP_LITE_DSP_HACT_ST_END);     
    RK3326_DUMP_REG(VOP_LITE_DSP_VTOTAL_VS_END);   
    RK3326_DUMP_REG(VOP_LITE_DSP_VACT_ST_END);     
    RK3326_DUMP_REG(VOP_LITE_DSP_VS_ST_END_F1);    
    RK3326_DUMP_REG(VOP_LITE_DSP_VACT_ST_END_F1);  
    RK3326_DUMP_REG(VOP_LITE_BCSH_CTRL);           
    RK3326_DUMP_REG(VOP_LITE_BCSH_COL_BAR);        
    RK3326_DUMP_REG(VOP_LITE_BCSH_BCS);           
    RK3326_DUMP_REG(VOP_LITE_BCSH_H);             
    RK3326_DUMP_REG(VOP_LITE_FRC_LOWER01_0);      
    RK3326_DUMP_REG(VOP_LITE_FRC_LOWER01_1);      
    RK3326_DUMP_REG(VOP_LITE_FRC_LOWER10_0);      
    RK3326_DUMP_REG(VOP_LITE_FRC_LOWER10_1);      
    RK3326_DUMP_REG(VOP_LITE_FRC_LOWER11_0);      
    RK3326_DUMP_REG(VOP_LITE_FRC_LOWER11_1);      
    RK3326_DUMP_REG(VOP_LITE_MCU_RW_BYPASS_PORT); 
    RK3326_DUMP_REG(VOP_LITE_DBG_REG_SCAN_LINE);  
    RK3326_DUMP_REG(VOP_LITE_BLANKING_VALUE);     
    RK3326_DUMP_REG(VOP_LITE_FLAG_REG_FRM_VALID); 
    RK3326_DUMP_REG(VOP_LITE_FLAG_REG);          
    RK3326_DUMP_REG(VOP_LITE_HWC_LUT_ADDR);        
    RK3326_DUMP_REG(VOP_LITE_GAMMA_LUT_ADDR);      
    RK3326_DUMP_REG(VOP_LITE_MMU_DTE_ADDR);       
    RK3326_DUMP_REG(VOP_LITE_MMU_STATUS);         
    RK3326_DUMP_REG(VOP_LITE_MMU_COMMAND);        
    RK3326_DUMP_REG(VOP_LITE_MMU_PAGE_FAULT_ADDR);
    RK3326_DUMP_REG(VOP_LITE_MMU_ZAP_ONE_LINE);   
    RK3326_DUMP_REG(VOP_LITE_MMU_INT_RAWSTAT);    
    RK3326_DUMP_REG(VOP_LITE_MMU_INT_CLEAR);      
    RK3326_DUMP_REG(VOP_LITE_MMU_INT_MASK);       
    RK3326_DUMP_REG(VOP_LITE_MMU_INT_STATUS);     
    RK3326_DUMP_REG(VOP_LITE_MMU_AUTO_GATING);    
    RK3326_DUMP_REG(VOP_LITE_MMU_CFG_DONE);       

    return 0; 
}
MSH_CMD_EXPORT(vopl_test, vopl_test);

#define VOP_BIG_BASE                    0xFF460000
#define VOP_BIG_REG_CFG_DONE            VOP_BIG_BASE + 0x0000 // 0x00000000 // W 0x00000000 Register config done flag 寄存器配置完成标位置, 寄存器配置会在下一帧开始时生效
#define VOP_BIG_VERSION                 VOP_BIG_BASE + 0x0004 // 0x00000000 // W 0x00000000 Version for vop VOP 版本号
#define VOP_BIG_DSP_BG                  VOP_BIG_BASE + 0x0008 // 0x00000000 // W 0x00000000 Display control register 背景纯色图层颜色配置寄存器 
#define VOP_BIG_MCU                     VOP_BIG_BASE + 0x000c // 0x00711c08 // W 0x00000000 MCU control register TODO
#define VOP_BIG_SYS_CTRL0               VOP_BIG_BASE + 0x0010 // 0x00000000 // W 0x00000000 System control register 保留位
#define VOP_BIG_SYS_CTRL1               VOP_BIG_BASE + 0x0014 // 0x00000000 // W 0x00000000 System control register
#define VOP_BIG_SYS_CTRL2               VOP_BIG_BASE + 0x0018 // 0x00006000 // W 0x00006000 System control register
#define VOP_BIG_POST_LB_CTRL            VOP_BIG_BASE + 0x001c // 0x00000000 // W 0x00000000 Post lb control register
#define VOP_BIG_DSP_CTRL0               VOP_BIG_BASE + 0x0020 // 0x03010001 // W 0x00000101 Display register
#define VOP_BIG_DSP_CTRL1               VOP_BIG_BASE + 0x0024 // 0x00000000 // W 0x00000000 Display register 保留位
#define VOP_BIG_DSP_CTRL2               VOP_BIG_BASE + 0x0028 // 0x09000040 // W 0x00004000 Display register
#define VOP_BIG_VOP_STATUS              VOP_BIG_BASE + 0x002c // 0x00000012 // W 0x00000000 Some vop module status 
#define VOP_BIG_LINE_FLAG               VOP_BIG_BASE + 0x0030 // 0x000001e0 // W 0x00000000 Line flag config register
#define VOP_BIG_INTR_EN                 VOP_BIG_BASE + 0x0034 // 0x00000000 // W 0x00000000 Interrupt enable register
#define VOP_BIG_INTR_CLEAR              VOP_BIG_BASE + 0x0038 // 0x00000000 // W 0x00000000 Interrupt clear register
#define VOP_BIG_INTR_STATUS             VOP_BIG_BASE + 0x003c // 0x301d0000 // W 0x00000000 Interrupt raw status and interruptstatus
#define VOP_BIG_WIN0_CTRL0              VOP_BIG_BASE + 0x0050 // 0x00000000 // W 0x00000000 Win0 ctrl register
#define VOP_BIG_WIN0_CTRL1              VOP_BIG_BASE + 0x0054 // 0x00021220 // W 0x00021220 Win0 ctrl register
#define VOP_BIG_WIN0_COLOR_KEY          VOP_BIG_BASE + 0x0058 // 0x00000000 // W 0x00000000 Win0 color key register
#define VOP_BIG_WIN0_VIR                VOP_BIG_BASE + 0x005c // 0x01400140 // W 0x01400140 Win0 virtual stride
#define VOP_BIG_WIN0_YRGB_MST0          VOP_BIG_BASE + 0x0060 // 0x00000000 // W 0x00000000 Win0 YRGB memory start address 0
#define VOP_BIG_WIN0_CBR_MST0           VOP_BIG_BASE + 0x0064 // 0x00000000 // W 0x00000000 Win0 Cbr memory start address 0
#define VOP_BIG_WIN0_ACT_INFO           VOP_BIG_BASE + 0x0068 // 0x00ef013f // W 0x00ef013f Win0 active window width/height
#define VOP_BIG_WIN0_DSP_INFO           VOP_BIG_BASE + 0x006c // 0x00ef013f // W 0x00ef013f Win0 display width/height on panel
#define VOP_BIG_WIN0_DSP_ST             VOP_BIG_BASE + 0x0070 // 0x000a000a // W 0x0000000a Win0 display start point on panel
#define VOP_BIG_WIN0_SCL_FACTOR_YRGB    VOP_BIG_BASE + 0x0074 // 0x10001000 // W 0x10001000 Win0 YRGB scaling factor
#define VOP_BIG_WIN0_SCL_FACTOR_CBR     VOP_BIG_BASE + 0x0078 // 0x10001000 // W 0x10001000 Win0 CBR scaling factor
#define VOP_BIG_WIN0_SCL_OFFSET         VOP_BIG_BASE + 0x007c // 0x00000000 // W 0x00000000 Win0 scaling start point offset
#define VOP_BIG_WIN0_ALPHA_CTRL         VOP_BIG_BASE + 0x0080 // 0x00000000 // W 0x00000000 Blending control register
#define VOP_BIG_WIN1_CTRL0              VOP_BIG_BASE + 0x0090 // 0x00000011 // W 0x00000000 Win1 ctrl register
#define VOP_BIG_WIN1_CTRL1              VOP_BIG_BASE + 0x0094 // 0x00000580 // W 0x00000500 Win1 ctrl register
#define VOP_BIG_WIN1_VIR                VOP_BIG_BASE + 0x0098 // 0x000000f0 // W 0x00000000 Win1 virtual stride
#define VOP_BIG_WIN1_MST                VOP_BIG_BASE + 0x00a0 // 0x3de00000 // W 0x00000000 Win1 memory start address 窗口1基地址 
#define VOP_BIG_WIN1_DSP_INFO           VOP_BIG_BASE + 0x00a4 // 0x01df013f // W 0x00ef013f Win1 display width/height on panel
#define VOP_BIG_WIN1_DSP_ST             VOP_BIG_BASE + 0x00a8 // 0x00030086 // W 0x0000000a Win1 display start point on panel
#define VOP_BIG_WIN1_COLOR_KEY          VOP_BIG_BASE + 0x00ac // 0x00000000 // W 0x00000000 Win1 color key register
#define VOP_BIG_WIN1_ALPHA_CTRL         VOP_BIG_BASE + 0x00bc // 0x00000000 // W 0x00000000 Blending control register
#define VOP_BIG_HWC_CTRL0               VOP_BIG_BASE + 0x00e0 // 0x00000060 // W 0x00000000 Hwc ctrl register
#define VOP_BIG_HWC_MST                 VOP_BIG_BASE + 0x00e8 // 0x00000000 // W 0x00000000 Hwc memory start address
#define VOP_BIG_HWC_DSP_ST              VOP_BIG_BASE + 0x00ec // 0x000a000a // W 0x00000000 Hwc display start point on panel
#define VOP_BIG_HWC_ALPHA_CTRL          VOP_BIG_BASE + 0x00f0 // 0x00000000 // W 0x00000000 Blending control register
#define VOP_BIG_DSP_HTOTAL_HS_END       VOP_BIG_BASE + 0x0100 // 0x02480004 // W 0x014a000a Panel scanning horizontal width and hsync pulse end point
#define VOP_BIG_DSP_HACT_ST_END         VOP_BIG_BASE + 0x0104 // 0x008601c6 // W 0x000a0000 Panel active horizontal scanning start point and end point
#define VOP_BIG_DSP_VTOTAL_VS_END       VOP_BIG_BASE + 0x0108 // 0x01e50001 // W 0x00fa000a Panel scanning vertical height and vsync pulse end point
#define VOP_BIG_DSP_VACT_ST_END         VOP_BIG_BASE + 0x010c // 0x000301e3 // W 0x000a00fa Panel active vertical scanning start point and end point
#define VOP_BIG_DSP_VS_ST_END_F1        VOP_BIG_BASE + 0x0110 // 0x00000000 // W 0x00000000 Vertical scanning start point and vsync pulse end point of even file
#define VOP_BIG_DSP_VACT_ST_END_F1      VOP_BIG_BASE + 0x0114 // 0x00000000 // W 0x00000000 Vertical scanning active start point and end point of even filed in interlac
#define VOP_BIG_BCSH_CTRL               VOP_BIG_BASE + 0x0160 // 0x00000012 // W 0x00000000 Brightness/Contrast enhancement/Saturation/Hue contrl
#define VOP_BIG_BCSH_COL_BAR            VOP_BIG_BASE + 0x0164 // 0x00000000 // W 0x00000000 vedio mode equals 2,then output color bar yuv 24bits value
#define VOP_BIG_BCSH_BCS                VOP_BIG_BASE + 0x0168 // 0x00000000 // W 0x00000000 Brightness/Contrast enhancement/Saturation
#define VOP_BIG_BCSH_H                  VOP_BIG_BASE + 0x016c // 0x00000000 // W 0x00000000 Hue
#define VOP_BIG_FRC_LOWER01_0           VOP_BIG_BASE + 0x0170 // 0x12844821 // W 0x12844821 Frc algorithm configuration register
#define VOP_BIG_FRC_LOWER01_1           VOP_BIG_BASE + 0x0174 // 0x21488412 // W 0x21488412 Frc algorithm configuration register
#define VOP_BIG_FRC_LOWER10_0           VOP_BIG_BASE + 0x0178 // 0xa55a9696 // W 0xa55a9696 Frc algorithm configuration register
#define VOP_BIG_FRC_LOWER10_1           VOP_BIG_BASE + 0x017c // 0x5aa56969 // W 0x5aa56969 Frc algorithm configuration register
#define VOP_BIG_FRC_LOWER11_0           VOP_BIG_BASE + 0x0180 // 0xdeb77bed // W 0xdeb77bed Frc algorithm configuration register
#define VOP_BIG_FRC_LOWER11_1           VOP_BIG_BASE + 0x0184 // 0xed7bb7de // W 0xed7bb7de Frc algorithm configuration register
#define VOP_BIG_MCU_RW_BYPASS_PORT      VOP_BIG_BASE + 0x018c // 0x00000000 // W 0x00000000 MCU panel write data
#define VOP_BIG_DBG_REG_SCAN_LINE       VOP_BIG_BASE + 0x0190 // 0x00000000 // W 0x00000000 Current line number of dsp timing
#define VOP_BIG_WIN2_CTRL0              VOP_BIG_BASE + 0x0190 // 0x00000000 // W 0x00000000 win2 ctrl register
#define VOP_BIG_WIN2_CTRL1              VOP_BIG_BASE + 0x0194 // 0x00000800 // W 0x003a0000 win2 ctrl register
#define VOP_BIG_WIN2_VIR0_1             VOP_BIG_BASE + 0x0198 // 0x00000000 // W 0x01400140 Win2 virtual stride0 and virtaul stride1
#define VOP_BIG_WIN2_VIR2_3             VOP_BIG_BASE + 0x019c // 0x00000000 // W 0x01400140 Win2 virtual stride2 and virtaul stride3
#define VOP_BIG_WIN2_MST0               VOP_BIG_BASE + 0x01a0 // 0x00000000 // W 0x00000000 Win2 memory start address0
#define VOP_BIG_WIN2_DSP_INFO0          VOP_BIG_BASE + 0x01a4 // 0x00000000 // W 0x00ef013f Win2 display width0/height0 on panel
#define VOP_BIG_WIN2_DSP_ST0            VOP_BIG_BASE + 0x01a8 // 0x00000000 // W 0x000a000a Win2 display start point0 on panel
#define VOP_BIG_WIN2_COLOR_KEY          VOP_BIG_BASE + 0x01ac // 0x00000000 // W 0x00000000 Win2 color key register
#define VOP_BIG_WIN2_MST1               VOP_BIG_BASE + 0x01b0 // 0x00000000 // W 0x00000000 Win2 memory start address1
#define VOP_BIG_WIN2_DSP_INFO1          VOP_BIG_BASE + 0x01b4 // 0x00000000 // W 0x00ef013f Win2 display width1/height1 on panel
#define VOP_BIG_WIN2_DSP_ST1            VOP_BIG_BASE + 0x01b8 // 0x00000000 // W 0x000a000a Win2 display start point1 on panel
#define VOP_BIG_WIN2_ALPHA_CTRL         VOP_BIG_BASE + 0x01bc // 0x00000000 // W 0x00000000 Win2 alpha source control register
#define VOP_BIG_WIN2_MST2               VOP_BIG_BASE + 0x01c0 // 0x00000000 // W 0x00000000 Win2 memory start address2
#define VOP_BIG_WIN2_DSP_INFO2          VOP_BIG_BASE + 0x01c4 // 0x00000000 // W 0x00ef013f panel Win2 display width2/height2 on
#define VOP_BIG_WIN2_DSP_ST2            VOP_BIG_BASE + 0x01c8 // 0x00000000 // W 0x000a000a Win2 display start point2 on panel
#define VOP_BIG_WIN2_MST3               VOP_BIG_BASE + 0x01d0 // 0x00000000 // W 0x00000000 Win2 memory start address3
#define VOP_BIG_WIN2_DSP_INFO3          VOP_BIG_BASE + 0x01d4 // 0x00000000 // W 0x00ef013f panel Win2 display width3/height3 on
#define VOP_BIG_WIN2_DSP_ST3            VOP_BIG_BASE + 0x01d8 // 0x00000000 // W 0x000a000a Win2 display start point3 on panel
#define VOP_BIG_BLANKING_VALUE          VOP_BIG_BASE + 0x01f4 // 0x01000000 // W 0x00000000 The value of vsync blanking
#define VOP_BIG_FLAG_REG_FRM_VALID      VOP_BIG_BASE + 0x01f8 // 0x00000000 // W 0x00000000 Flag reg value after frame valid
#define VOP_BIG_FLAG_REG                VOP_BIG_BASE + 0x01fc // 0x00000000 // W 0x00000000 Flag reg value before frame valid
#define VOP_BIG_CABC_CTRL0              VOP_BIG_BASE + 0x0200 // 0x00000000 // W 0x00000000 Content Adaptive Backlight Control register0
#define VOP_BIG_CABC_CTRL1              VOP_BIG_BASE + 0x0204 // 0x00000000 // W 0x00000000 Content Adaptive Backlight Control register1
#define VOP_BIG_CABC_CTRL2              VOP_BIG_BASE + 0x0208 // 0x00000000 // W 0x00000000 Content Adaptive Backlight Control register2
#define VOP_BIG_CABC_CTRL3              VOP_BIG_BASE + 0x020c // 0x00000000 // W 0x00000000 Content Adaptive Backlight Control register3
#define VOP_BIG_CABC_GAUSS_LINE0_0      VOP_BIG_BASE + 0x0210 // 0x15110903 // W 0x00000000 CABC gauss line config register
#define VOP_BIG_CABC_GAUSS_LINE0_1      VOP_BIG_BASE + 0x0214 // 0x00030911 // W 0x00000000 CABC gauss line config register
#define VOP_BIG_CABC_GAUSS_LINE1_0      VOP_BIG_BASE + 0x0218 // 0x1a150b04 // W 0x00000000 CABC gauss line config register
#define VOP_BIG_CABC_GAUSS_LINE1_1      VOP_BIG_BASE + 0x021c // 0x00040b15 // W 0x00000000 CABC gauss line config register
#define VOP_BIG_CABC_GAUSS_LINE2_0      VOP_BIG_BASE + 0x0220 // 0x15110903 // W 0x00000000 CABC gauss line config register
#define VOP_BIG_CABC_GAUSS_LINE2_1      VOP_BIG_BASE + 0x0224 // 0x00030911 // W 0x00000000 CABC gauss line config register
#define VOP_BIG_PWM_CTRL                VOP_BIG_BASE + 0x0230 // 0x0000200a // W 0x00000000 PWM Control Register , The register provides the cotrol of PWM
#define VOP_BIG_PWM_PERIOD_HPR          VOP_BIG_BASE + 0x0234 // 0x00000000 // W 0x00000000 PWM Period Register/High Polarity Capture Register
#define VOP_BIG_PWM_DUTY_LPR            VOP_BIG_BASE + 0x0238 // 0x00000000 // W 0x00000000 PWM Duty Register/Low Polarity Capture Register
#define VOP_BIG_PWM_CNT                 VOP_BIG_BASE + 0x023c // 0x00000000 // W 0x00000000 PWM Counter Register
#define VOP_BIG_AFBCD_CTRL              VOP_BIG_BASE + 0x0240 // 0x00000000 // W 0x00000000 AFBCD control register
#define VOP_BIG_AFBCD_HDR_PTR           VOP_BIG_BASE + 0x0244 // 0x00000000 // W 0x00000000 AFBC header memory start address
#define VOP_BIG_AFBCD_PIC_SIZE          VOP_BIG_BASE + 0x0248 // 0x00000000 // W 0x00000000 AFBCD picture size
#define VOP_BIG_AFBCD_PIC_OFFSET        VOP_BIG_BASE + 0x024c // 0x00000000 // W 0x00000000 AFBCD picture offset
#define VOP_BIG_AFBCD_AXI_CTRL          VOP_BIG_BASE + 0x0250 // 0x00000087 // W 0x00000000 AFBCD axi control register
#define VOP_BIG_AFBCD_DBG0              VOP_BIG_BASE + 0x0254 // 0x00000001 // W 0x00000000 AFBCD debug register
#define VOP_BIG_CABC_LUT_ADDR           VOP_BIG_BASE + 0x0400 // 0x0000053b // W 0x00000000 note: used for CABC , base address: 0x400 -- 0x5FF
#define VOP_BIG_HWC_LUT_ADDR            VOP_BIG_BASE + 0x0600 // 0x0f5325c9 // W 0x00000000 note: used for HWC BPP format , base address: 0x600 -- 0x9FF
#define VOP_BIG_GAMMA_LUT_ADDR          VOP_BIG_BASE + 0x0a00 // 0x005dbbaf // W 0x00000000 note: SIZE: 24X256 used for panel GAMMA adjustment, base address: 0x0a00 -- 0x0dff
#define VOP_BIG_MMU_DTE_ADDR            VOP_BIG_BASE + 0x0f00 // 0x00000000 // W 0x00000000 MMU current page Table address
#define VOP_BIG_MMU_STATUS              VOP_BIG_BASE + 0x0f04 // 0x00000018 // W 0x00000000 MMU status register
#define VOP_BIG_MMU_COMMAND             VOP_BIG_BASE + 0x0f08 // 0x00000000 // W 0x00000000 MMU command register
#define VOP_BIG_MMU_PAGE_FAULT_ADDR     VOP_BIG_BASE + 0x0f0c // 0x00000000 // W 0x00000000 MMU logical address of last page fault
#define VOP_BIG_MMU_ZAP_ONE_LINE        VOP_BIG_BASE + 0x0f10 // 0x00000000 // W 0x00000000 MMU Zap cache line register
#define VOP_BIG_MMU_INT_RAWSTAT         VOP_BIG_BASE + 0x0f14 // 0x00000000 // W 0x00000000 MMU raw interrupt status register
#define VOP_BIG_MMU_INT_CLEAR           VOP_BIG_BASE + 0x0f18 // 0x00000000 // W 0x00000000 MMU raw interrupt status register
#define VOP_BIG_MMU_INT_MASK            VOP_BIG_BASE + 0x0f1c // 0x00000000 // W 0x00000000 MMU raw interrupt status register
#define VOP_BIG_MMU_INT_STATUS          VOP_BIG_BASE + 0x0f20 // 0x00000000 // W 0x00000000 MMU raw interrupt status register
#define VOP_BIG_MMU_AUTO_GATING         VOP_BIG_BASE + 0x0f24 // 0x00000003 // W 0x00000000 mmu auto gating
#define VOP_BIG_MMU_CFG_DONE            VOP_BIG_BASE + 0x0f28 // 0x00000000 // W 0x00000000 mmu config done reg

int vopb_test(void)
{   
    RK3326_DUMP_REG(VOP_BIG_REG_CFG_DONE);            
    RK3326_DUMP_REG(VOP_BIG_VERSION);                 
    RK3326_DUMP_REG(VOP_BIG_DSP_BG);                  
    RK3326_DUMP_REG(VOP_BIG_MCU);                     
    RK3326_DUMP_REG(VOP_BIG_SYS_CTRL0);               
    RK3326_DUMP_REG(VOP_BIG_SYS_CTRL1);               
    RK3326_DUMP_REG(VOP_BIG_SYS_CTRL2);               
    RK3326_DUMP_REG(VOP_BIG_POST_LB_CTRL);            
    RK3326_DUMP_REG(VOP_BIG_DSP_CTRL0);               
    RK3326_DUMP_REG(VOP_BIG_DSP_CTRL1);               
    RK3326_DUMP_REG(VOP_BIG_DSP_CTRL2);               
    RK3326_DUMP_REG(VOP_BIG_VOP_STATUS);              
    RK3326_DUMP_REG(VOP_BIG_LINE_FLAG);               
    RK3326_DUMP_REG(VOP_BIG_INTR_EN);                 
    RK3326_DUMP_REG(VOP_BIG_INTR_CLEAR);              
    RK3326_DUMP_REG(VOP_BIG_INTR_STATUS);             
    RK3326_DUMP_REG(VOP_BIG_WIN0_CTRL0);              
    RK3326_DUMP_REG(VOP_BIG_WIN0_CTRL1);              
    RK3326_DUMP_REG(VOP_BIG_WIN0_COLOR_KEY);          
    RK3326_DUMP_REG(VOP_BIG_WIN0_VIR);                
    RK3326_DUMP_REG(VOP_BIG_WIN0_YRGB_MST0);          
    RK3326_DUMP_REG(VOP_BIG_WIN0_CBR_MST0);           
    RK3326_DUMP_REG(VOP_BIG_WIN0_ACT_INFO);           
    RK3326_DUMP_REG(VOP_BIG_WIN0_DSP_INFO);           
    RK3326_DUMP_REG(VOP_BIG_WIN0_DSP_ST);             
    RK3326_DUMP_REG(VOP_BIG_WIN0_SCL_FACTOR_YRGB);    
    RK3326_DUMP_REG(VOP_BIG_WIN0_SCL_FACTOR_CBR);     
    RK3326_DUMP_REG(VOP_BIG_WIN0_SCL_OFFSET);         
    RK3326_DUMP_REG(VOP_BIG_WIN0_ALPHA_CTRL);         
    RK3326_DUMP_REG(VOP_BIG_WIN1_CTRL0);              
    RK3326_DUMP_REG(VOP_BIG_WIN1_CTRL1);              
    RK3326_DUMP_REG(VOP_BIG_WIN1_VIR);               
    RK3326_DUMP_REG(VOP_BIG_WIN1_MST);               
    RK3326_DUMP_REG(VOP_BIG_WIN1_DSP_INFO);           
    RK3326_DUMP_REG(VOP_BIG_WIN1_DSP_ST);             
    RK3326_DUMP_REG(VOP_BIG_WIN1_COLOR_KEY);          
    RK3326_DUMP_REG(VOP_BIG_WIN1_ALPHA_CTRL);         
    RK3326_DUMP_REG(VOP_BIG_HWC_CTRL0);               
    RK3326_DUMP_REG(VOP_BIG_HWC_MST);                 
    RK3326_DUMP_REG(VOP_BIG_HWC_DSP_ST);              
    RK3326_DUMP_REG(VOP_BIG_HWC_ALPHA_CTRL);          
    RK3326_DUMP_REG(VOP_BIG_DSP_HTOTAL_HS_END);       
    RK3326_DUMP_REG(VOP_BIG_DSP_HACT_ST_END);         
    RK3326_DUMP_REG(VOP_BIG_DSP_VTOTAL_VS_END);       
    RK3326_DUMP_REG(VOP_BIG_DSP_VACT_ST_END);         
    RK3326_DUMP_REG(VOP_BIG_DSP_VS_ST_END_F1);       
    RK3326_DUMP_REG(VOP_BIG_DSP_VACT_ST_END_F1);     
    RK3326_DUMP_REG(VOP_BIG_BCSH_CTRL);               
    RK3326_DUMP_REG(VOP_BIG_BCSH_COL_BAR);            
    RK3326_DUMP_REG(VOP_BIG_BCSH_BCS);                
    RK3326_DUMP_REG(VOP_BIG_BCSH_H);                  
    RK3326_DUMP_REG(VOP_BIG_FRC_LOWER01_0);           
    RK3326_DUMP_REG(VOP_BIG_FRC_LOWER01_1);           
    RK3326_DUMP_REG(VOP_BIG_FRC_LOWER10_0);           
    RK3326_DUMP_REG(VOP_BIG_FRC_LOWER10_1);           
    RK3326_DUMP_REG(VOP_BIG_FRC_LOWER11_0);           
    RK3326_DUMP_REG(VOP_BIG_FRC_LOWER11_1);           
    RK3326_DUMP_REG(VOP_BIG_MCU_RW_BYPASS_PORT);      
    RK3326_DUMP_REG(VOP_BIG_DBG_REG_SCAN_LINE);       
    RK3326_DUMP_REG(VOP_BIG_WIN2_CTRL0);              
    RK3326_DUMP_REG(VOP_BIG_WIN2_CTRL1);              
    RK3326_DUMP_REG(VOP_BIG_WIN2_VIR0_1);             
    RK3326_DUMP_REG(VOP_BIG_WIN2_VIR2_3);             
    RK3326_DUMP_REG(VOP_BIG_WIN2_MST0);               
    RK3326_DUMP_REG(VOP_BIG_WIN2_DSP_INFO0);          
    RK3326_DUMP_REG(VOP_BIG_WIN2_DSP_ST0);            
    RK3326_DUMP_REG(VOP_BIG_WIN2_COLOR_KEY);          
    RK3326_DUMP_REG(VOP_BIG_WIN2_MST1);               
    RK3326_DUMP_REG(VOP_BIG_WIN2_DSP_INFO1);          
    RK3326_DUMP_REG(VOP_BIG_WIN2_DSP_ST1);            
    RK3326_DUMP_REG(VOP_BIG_WIN2_ALPHA_CTRL);         
    RK3326_DUMP_REG(VOP_BIG_WIN2_MST2);               
    RK3326_DUMP_REG(VOP_BIG_WIN2_DSP_INFO2);          
    RK3326_DUMP_REG(VOP_BIG_WIN2_DSP_ST2);            
    RK3326_DUMP_REG(VOP_BIG_WIN2_MST3);               
    RK3326_DUMP_REG(VOP_BIG_WIN2_DSP_INFO3);          
    RK3326_DUMP_REG(VOP_BIG_WIN2_DSP_ST3);            
    RK3326_DUMP_REG(VOP_BIG_BLANKING_VALUE);          
    RK3326_DUMP_REG(VOP_BIG_FLAG_REG_FRM_VALID);      
    RK3326_DUMP_REG(VOP_BIG_FLAG_REG);                
    RK3326_DUMP_REG(VOP_BIG_CABC_CTRL0);              
    RK3326_DUMP_REG(VOP_BIG_CABC_CTRL1);              
    RK3326_DUMP_REG(VOP_BIG_CABC_CTRL2);              
    RK3326_DUMP_REG(VOP_BIG_CABC_CTRL3);              
    RK3326_DUMP_REG(VOP_BIG_CABC_GAUSS_LINE0_0);      
    RK3326_DUMP_REG(VOP_BIG_CABC_GAUSS_LINE0_1);      
    RK3326_DUMP_REG(VOP_BIG_CABC_GAUSS_LINE1_0);      
    RK3326_DUMP_REG(VOP_BIG_CABC_GAUSS_LINE1_1);      
    RK3326_DUMP_REG(VOP_BIG_CABC_GAUSS_LINE2_0);      
    RK3326_DUMP_REG(VOP_BIG_CABC_GAUSS_LINE2_1);      
    RK3326_DUMP_REG(VOP_BIG_PWM_CTRL);                
    RK3326_DUMP_REG(VOP_BIG_PWM_PERIOD_HPR);          
    RK3326_DUMP_REG(VOP_BIG_PWM_DUTY_LPR);            
    RK3326_DUMP_REG(VOP_BIG_PWM_CNT);                 
    RK3326_DUMP_REG(VOP_BIG_AFBCD_CTRL);              
    RK3326_DUMP_REG(VOP_BIG_AFBCD_HDR_PTR);           
    RK3326_DUMP_REG(VOP_BIG_AFBCD_PIC_SIZE);          
    RK3326_DUMP_REG(VOP_BIG_AFBCD_PIC_OFFSET);        
    RK3326_DUMP_REG(VOP_BIG_AFBCD_AXI_CTRL);          
    RK3326_DUMP_REG(VOP_BIG_AFBCD_DBG0);              
    RK3326_DUMP_REG(VOP_BIG_CABC_LUT_ADDR);           
    RK3326_DUMP_REG(VOP_BIG_HWC_LUT_ADDR);            
    RK3326_DUMP_REG(VOP_BIG_GAMMA_LUT_ADDR);           
    RK3326_DUMP_REG(VOP_BIG_MMU_DTE_ADDR);            
    RK3326_DUMP_REG(VOP_BIG_MMU_STATUS);              
    RK3326_DUMP_REG(VOP_BIG_MMU_COMMAND);             
    RK3326_DUMP_REG(VOP_BIG_MMU_PAGE_FAULT_ADDR);     
    RK3326_DUMP_REG(VOP_BIG_MMU_ZAP_ONE_LINE);        
    RK3326_DUMP_REG(VOP_BIG_MMU_INT_RAWSTAT);         
    RK3326_DUMP_REG(VOP_BIG_MMU_INT_CLEAR);           
    RK3326_DUMP_REG(VOP_BIG_MMU_INT_MASK);            
    RK3326_DUMP_REG(VOP_BIG_MMU_INT_STATUS);          
    RK3326_DUMP_REG(VOP_BIG_MMU_AUTO_GATING);         
    RK3326_DUMP_REG(VOP_BIG_MMU_CFG_DONE);            

    return 0; 
}
MSH_CMD_EXPORT(vopb_test, vopb_test);