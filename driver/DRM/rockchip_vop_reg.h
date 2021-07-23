#ifndef __ROCKCHIP_VOP_REG_H__
#define __ROCKCHIP_VOP_REG_H__

/* rk3326 vopb register base */
#define VOPB_BASE                    0xFF460000

#define VOPB_REG_CFG_DONE            VOPB_BASE + 0x0000 // 0x00000000 // W 0x00000000 Register config done flag 寄存器配置完成标位置, 寄存器配置会在下一帧开始时生效
#define VOPB_VERSION                 VOPB_BASE + 0x0004 // 0x00000000 // W 0x00000000 Version for vop VOP 版本号
#define VOPB_DSP_BG                  VOPB_BASE + 0x0008 // 0x00000000 // W 0x00000000 Display control register 背景纯色图层颜色配置寄存器 
#define VOPB_MCU                     VOPB_BASE + 0x000c // 0x00711c08 // W 0x00000000 MCU control register TODO
#define VOPB_SYS_CTRL0               VOPB_BASE + 0x0010 // 0x00000000 // W 0x00000000 System control register 保留位
#define VOPB_SYS_CTRL1               VOPB_BASE + 0x0014 // 0x00000000 // W 0x00000000 System control register
#define VOPB_SYS_CTRL2               VOPB_BASE + 0x0018 // 0x00006000 // W 0x00006000 System control register
#define VOPB_POST_LB_CTRL            VOPB_BASE + 0x001c // 0x00000000 // W 0x00000000 Post lb control register
#define VOPB_DSP_CTRL0               VOPB_BASE + 0x0020 // 0x03010001 // W 0x00000101 Display register
#define VOPB_DSP_CTRL1               VOPB_BASE + 0x0024 // 0x00000000 // W 0x00000000 Display register 保留位
#define VOPB_DSP_CTRL2               VOPB_BASE + 0x0028 // 0x09000040 // W 0x00004000 Display register
#define VOPB_VOP_STATUS              VOPB_BASE + 0x002c // 0x00000012 // W 0x00000000 Some vop module status 
#define VOPB_LINE_FLAG               VOPB_BASE + 0x0030 // 0x000001e0 // W 0x00000000 Line flag config register
#define VOPB_INTR_EN                 VOPB_BASE + 0x0034 // 0x00000000 // W 0x00000000 Interrupt enable register
#define VOPB_INTR_CLEAR              VOPB_BASE + 0x0038 // 0x00000000 // W 0x00000000 Interrupt clear register
#define VOPB_INTR_STATUS             VOPB_BASE + 0x003c // 0x301d0000 // W 0x00000000 Interrupt raw status and interruptstatus

/* win0 register */
#define VOPB_WIN0_CTRL0              VOPB_BASE + 0x0050 // 0x00000000 // W 0x00000000 Win0 ctrl register
#define VOPB_WIN0_CTRL1              VOPB_BASE + 0x0054 // 0x00021220 // W 0x00021220 Win0 ctrl register
#define VOPB_WIN0_COLOR_KEY          VOPB_BASE + 0x0058 // 0x00000000 // W 0x00000000 Win0 color key register
#define VOPB_WIN0_VIR                VOPB_BASE + 0x005c // 0x01400140 // W 0x01400140 Win0 virtual stride
#define VOPB_WIN0_YRGB_MST0          VOPB_BASE + 0x0060 // 0x00000000 // W 0x00000000 Win0 YRGB memory start address 0
#define VOPB_WIN0_CBR_MST0           VOPB_BASE + 0x0064 // 0x00000000 // W 0x00000000 Win0 Cbr memory start address 0
#define VOPB_WIN0_ACT_INFO           VOPB_BASE + 0x0068 // 0x00ef013f // W 0x00ef013f Win0 active window width/height
#define VOPB_WIN0_DSP_INFO           VOPB_BASE + 0x006c // 0x00ef013f // W 0x00ef013f Win0 display width/height on panel
#define VOPB_WIN0_DSP_ST             VOPB_BASE + 0x0070 // 0x000a000a // W 0x0000000a Win0 display start point on panel
#define VOPB_WIN0_SCL_FACTOR_YRGB    VOPB_BASE + 0x0074 // 0x10001000 // W 0x10001000 Win0 YRGB scaling factor
#define VOPB_WIN0_SCL_FACTOR_CBR     VOPB_BASE + 0x0078 // 0x10001000 // W 0x10001000 Win0 CBR scaling factor
#define VOPB_WIN0_SCL_OFFSET         VOPB_BASE + 0x007c // 0x00000000 // W 0x00000000 Win0 scaling start point offset
#define VOPB_WIN0_ALPHA_CTRL         VOPB_BASE + 0x0080 // 0x00000000 // W 0x00000000 Blending control register

/* win1 register */
#define VOPB_WIN1_CTRL0              VOPB_BASE + 0x0090 // 0x00000011 // W 0x00000000 Win1 ctrl register 使能窗口功能，窗口数据格式(ARGB888、RGB888、RGB565、ABGR888、BGR888、BGR565、RGBA888、BGRA888) 
#define VOPB_WIN1_CTRL1              VOPB_BASE + 0x0094 // 0x00000580 // W 0x00000500 Win1 ctrl register 看起来和AXI总线有关，也就是从内存取数据相关的，axi的gather是什么玩意? 
#define VOPB_WIN1_VIR                VOPB_BASE + 0x0098 // 0x000000f0 // W 0x00000000 Win1 virtual stride 240 
#define VOPB_WIN1_MST                VOPB_BASE + 0x00a0 // 0x3de00000 // W 0x00000000 Win1 memory start address 窗口1 FrameBuffer地址 
#define VOPB_WIN1_DSP_INFO           VOPB_BASE + 0x00a4 // 0x01df013f // W 0x00ef013f Win1 display width/height on panel 窗口1 宽度和高度 高度 419 宽度 319
#define VOPB_WIN1_DSP_ST             VOPB_BASE + 0x00a8 // 0x00030086 // W 0x0000000a Win1 display start point on panel 窗口1 在面板上显示的开始点
#define VOPB_WIN1_COLOR_KEY          VOPB_BASE + 0x00ac // 0x00000000 // W 0x00000000 Win1 color key register
#define VOPB_WIN1_ALPHA_CTRL         VOPB_BASE + 0x00bc // 0x00000000 // W 0x00000000 Blending control register

/* HWC register */
#define VOPB_HWC_CTRL0               VOPB_BASE + 0x00e0 // 0x00000060 // W 0x00000000 Hwc ctrl register
#define VOPB_HWC_MST                 VOPB_BASE + 0x00e8 // 0x00000000 // W 0x00000000 Hwc memory start address
#define VOPB_HWC_DSP_ST              VOPB_BASE + 0x00ec // 0x000a000a // W 0x00000000 Hwc display start point on panel
#define VOPB_HWC_ALPHA_CTRL          VOPB_BASE + 0x00f0 // 0x00000000 // W 0x00000000 Blending control register

#define VOPB_DSP_HTOTAL_HS_END       VOPB_BASE + 0x0100 // 0x02480004 // W 0x014a000a Panel scanning horizontal width and hsync pulse end point
#define VOPB_DSP_HACT_ST_END         VOPB_BASE + 0x0104 // 0x008601c6 // W 0x000a0000 Panel active horizontal scanning start point and end point
#define VOPB_DSP_VTOTAL_VS_END       VOPB_BASE + 0x0108 // 0x01e50001 // W 0x00fa000a Panel scanning vertical height and vsync pulse end point
#define VOPB_DSP_VACT_ST_END         VOPB_BASE + 0x010c // 0x000301e3 // W 0x000a00fa Panel active vertical scanning start point and end point
#define VOPB_DSP_VS_ST_END_F1        VOPB_BASE + 0x0110 // 0x00000000 // W 0x00000000 Vertical scanning start point and vsync pulse end point of even file
#define VOPB_DSP_VACT_ST_END_F1      VOPB_BASE + 0x0114 // 0x00000000 // W 0x00000000 Vertical scanning active start point and end point of even filed in interlac
#define VOPB_BCSH_CTRL               VOPB_BASE + 0x0160 // 0x00000012 // W 0x00000000 Brightness/Contrast enhancement/Saturation/Hue contrl
#define VOPB_BCSH_COL_BAR            VOPB_BASE + 0x0164 // 0x00000000 // W 0x00000000 vedio mode equals 2,then output color bar yuv 24bits value
#define VOPB_BCSH_BCS                VOPB_BASE + 0x0168 // 0x00000000 // W 0x00000000 Brightness/Contrast enhancement/Saturation
#define VOPB_BCSH_H                  VOPB_BASE + 0x016c // 0x00000000 // W 0x00000000 Hue
#define VOPB_FRC_LOWER01_0           VOPB_BASE + 0x0170 // 0x12844821 // W 0x12844821 Frc algorithm configuration register
#define VOPB_FRC_LOWER01_1           VOPB_BASE + 0x0174 // 0x21488412 // W 0x21488412 Frc algorithm configuration register
#define VOPB_FRC_LOWER10_0           VOPB_BASE + 0x0178 // 0xa55a9696 // W 0xa55a9696 Frc algorithm configuration register
#define VOPB_FRC_LOWER10_1           VOPB_BASE + 0x017c // 0x5aa56969 // W 0x5aa56969 Frc algorithm configuration register
#define VOPB_FRC_LOWER11_0           VOPB_BASE + 0x0180 // 0xdeb77bed // W 0xdeb77bed Frc algorithm configuration register
#define VOPB_FRC_LOWER11_1           VOPB_BASE + 0x0184 // 0xed7bb7de // W 0xed7bb7de Frc algorithm configuration register
#define VOPB_MCU_RW_BYPASS_PORT      VOPB_BASE + 0x018c // 0x00000000 // W 0x00000000 MCU panel write data
#define VOPB_DBG_REG_SCAN_LINE       VOPB_BASE + 0x0190 // 0x00000000 // W 0x00000000 Current line number of dsp timing

/* win2 register */
#define VOPB_WIN2_CTRL0              VOPB_BASE + 0x0190 // 0x00000000 // W 0x00000000 win2 ctrl register
#define VOPB_WIN2_CTRL1              VOPB_BASE + 0x0194 // 0x00000800 // W 0x003a0000 win2 ctrl register
#define VOPB_WIN2_VIR0_1             VOPB_BASE + 0x0198 // 0x00000000 // W 0x01400140 Win2 virtual stride0 and virtaul stride1
#define VOPB_WIN2_VIR2_3             VOPB_BASE + 0x019c // 0x00000000 // W 0x01400140 Win2 virtual stride2 and virtaul stride3
#define VOPB_WIN2_MST0               VOPB_BASE + 0x01a0 // 0x00000000 // W 0x00000000 Win2 memory start address0
#define VOPB_WIN2_DSP_INFO0          VOPB_BASE + 0x01a4 // 0x00000000 // W 0x00ef013f Win2 display width0/height0 on panel
#define VOPB_WIN2_DSP_ST0            VOPB_BASE + 0x01a8 // 0x00000000 // W 0x000a000a Win2 display start point0 on panel
#define VOPB_WIN2_COLOR_KEY          VOPB_BASE + 0x01ac // 0x00000000 // W 0x00000000 Win2 color key register
#define VOPB_WIN2_MST1               VOPB_BASE + 0x01b0 // 0x00000000 // W 0x00000000 Win2 memory start address1
#define VOPB_WIN2_DSP_INFO1          VOPB_BASE + 0x01b4 // 0x00000000 // W 0x00ef013f Win2 display width1/height1 on panel
#define VOPB_WIN2_DSP_ST1            VOPB_BASE + 0x01b8 // 0x00000000 // W 0x000a000a Win2 display start point1 on panel
#define VOPB_WIN2_ALPHA_CTRL         VOPB_BASE + 0x01bc // 0x00000000 // W 0x00000000 Win2 alpha source control register
#define VOPB_WIN2_MST2               VOPB_BASE + 0x01c0 // 0x00000000 // W 0x00000000 Win2 memory start address2
#define VOPB_WIN2_DSP_INFO2          VOPB_BASE + 0x01c4 // 0x00000000 // W 0x00ef013f panel Win2 display width2/height2 on
#define VOPB_WIN2_DSP_ST2            VOPB_BASE + 0x01c8 // 0x00000000 // W 0x000a000a Win2 display start point2 on panel
#define VOPB_WIN2_MST3               VOPB_BASE + 0x01d0 // 0x00000000 // W 0x00000000 Win2 memory start address3
#define VOPB_WIN2_DSP_INFO3          VOPB_BASE + 0x01d4 // 0x00000000 // W 0x00ef013f panel Win2 display width3/height3 on
#define VOPB_WIN2_DSP_ST3            VOPB_BASE + 0x01d8 // 0x00000000 // W 0x000a000a Win2 display start point3 on panel

#define VOPB_BLANKING_VALUE          VOPB_BASE + 0x01f4 // 0x01000000 // W 0x00000000 The value of vsync blanking
#define VOPB_FLAG_REG_FRM_VALID      VOPB_BASE + 0x01f8 // 0x00000000 // W 0x00000000 Flag reg value after frame valid
#define VOPB_FLAG_REG                VOPB_BASE + 0x01fc // 0x00000000 // W 0x00000000 Flag reg value before frame valid
#define VOPB_CABC_CTRL0              VOPB_BASE + 0x0200 // 0x00000000 // W 0x00000000 Content Adaptive Backlight Control register0
#define VOPB_CABC_CTRL1              VOPB_BASE + 0x0204 // 0x00000000 // W 0x00000000 Content Adaptive Backlight Control register1
#define VOPB_CABC_CTRL2              VOPB_BASE + 0x0208 // 0x00000000 // W 0x00000000 Content Adaptive Backlight Control register2
#define VOPB_CABC_CTRL3              VOPB_BASE + 0x020c // 0x00000000 // W 0x00000000 Content Adaptive Backlight Control register3
#define VOPB_CABC_GAUSS_LINE0_0      VOPB_BASE + 0x0210 // 0x15110903 // W 0x00000000 CABC gauss line config register
#define VOPB_CABC_GAUSS_LINE0_1      VOPB_BASE + 0x0214 // 0x00030911 // W 0x00000000 CABC gauss line config register
#define VOPB_CABC_GAUSS_LINE1_0      VOPB_BASE + 0x0218 // 0x1a150b04 // W 0x00000000 CABC gauss line config register
#define VOPB_CABC_GAUSS_LINE1_1      VOPB_BASE + 0x021c // 0x00040b15 // W 0x00000000 CABC gauss line config register
#define VOPB_CABC_GAUSS_LINE2_0      VOPB_BASE + 0x0220 // 0x15110903 // W 0x00000000 CABC gauss line config register
#define VOPB_CABC_GAUSS_LINE2_1      VOPB_BASE + 0x0224 // 0x00030911 // W 0x00000000 CABC gauss line config register
#define VOPB_PWM_CTRL                VOPB_BASE + 0x0230 // 0x0000200a // W 0x00000000 PWM Control Register , The register provides the cotrol of PWM
#define VOPB_PWM_PERIOD_HPR          VOPB_BASE + 0x0234 // 0x00000000 // W 0x00000000 PWM Period Register/High Polarity Capture Register
#define VOPB_PWM_DUTY_LPR            VOPB_BASE + 0x0238 // 0x00000000 // W 0x00000000 PWM Duty Register/Low Polarity Capture Register
#define VOPB_PWM_CNT                 VOPB_BASE + 0x023c // 0x00000000 // W 0x00000000 PWM Counter Register

/* afbcd register */
#define VOPB_AFBCD_CTRL              VOPB_BASE + 0x0240 // 0x00000000 // W 0x00000000 AFBCD control register
#define VOPB_AFBCD_HDR_PTR           VOPB_BASE + 0x0244 // 0x00000000 // W 0x00000000 AFBC header memory start address
#define VOPB_AFBCD_PIC_SIZE          VOPB_BASE + 0x0248 // 0x00000000 // W 0x00000000 AFBCD picture size
#define VOPB_AFBCD_PIC_OFFSET        VOPB_BASE + 0x024c // 0x00000000 // W 0x00000000 AFBCD picture offset
#define VOPB_AFBCD_AXI_CTRL          VOPB_BASE + 0x0250 // 0x00000087 // W 0x00000000 AFBCD axi control register
#define VOPB_AFBCD_DBG0              VOPB_BASE + 0x0254 // 0x00000001 // W 0x00000000 AFBCD debug register

/* lut register */
#define VOPB_CABC_LUT_ADDR           VOPB_BASE + 0x0400 // 0x0000053b // W 0x00000000 note: used for CABC , base address: 0x400 -- 0x5FF
#define VOPB_HWC_LUT_ADDR            VOPB_BASE + 0x0600 // 0x0f5325c9 // W 0x00000000 note: used for HWC BPP format , base address: 0x600 -- 0x9FF
#define VOPB_GAMMA_LUT_ADDR          VOPB_BASE + 0x0a00 // 0x005dbbaf // W 0x00000000 note: SIZE: 24X256 used for panel GAMMA adjustment, base address: 0x0a00 -- 0x0dff

/* vopb mmu register */
#define VOPB_MMU_DTE_ADDR            VOPB_BASE + 0x0f00 // 0x00000000 // W 0x00000000 MMU current page Table address
#define VOPB_MMU_STATUS              VOPB_BASE + 0x0f04 // 0x00000018 // W 0x00000000 MMU status register
#define VOPB_MMU_COMMAND             VOPB_BASE + 0x0f08 // 0x00000000 // W 0x00000000 MMU command register
#define VOPB_MMU_PAGE_FAULT_ADDR     VOPB_BASE + 0x0f0c // 0x00000000 // W 0x00000000 MMU logical address of last page fault
#define VOPB_MMU_ZAP_ONE_LINE        VOPB_BASE + 0x0f10 // 0x00000000 // W 0x00000000 MMU Zap cache line register
#define VOPB_MMU_INT_RAWSTAT         VOPB_BASE + 0x0f14 // 0x00000000 // W 0x00000000 MMU raw interrupt status register
#define VOPB_MMU_INT_CLEAR           VOPB_BASE + 0x0f18 // 0x00000000 // W 0x00000000 MMU raw interrupt status register
#define VOPB_MMU_INT_MASK            VOPB_BASE + 0x0f1c // 0x00000000 // W 0x00000000 MMU raw interrupt status register
#define VOPB_MMU_INT_STATUS          VOPB_BASE + 0x0f20 // 0x00000000 // W 0x00000000 MMU raw interrupt status register
#define VOPB_MMU_AUTO_GATING         VOPB_BASE + 0x0f24 // 0x00000003 // W 0x00000000 mmu auto gating
#define VOPB_MMU_CFG_DONE            VOPB_BASE + 0x0f28 // 0x00000000 // W 0x00000000 mmu config done reg

#endif /* __ROCKCHIP_VOP_REG_H__ */
