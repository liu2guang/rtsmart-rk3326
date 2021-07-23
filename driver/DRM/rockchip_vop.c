#include <rtthread.h>
#include <rockchip_vop.h>
#include <rockchip_vop_reg.h>

#define ARGB888 0 
#define RGB888 1 
#define RGB565 2

#define RGB 0 
#define BGR 1 

#define ARGB 0 
#define RGBA 1 

union vop_big_win1_ctrl0_reg
{
    struct {
        int win1_en             : 1; 
        int reserved1           : 3; 
        int win1_data_fmt       : 3; 
        int reserved2           : 1; 
        int win1_interlace_read : 1; 
        int win1_no_outstanding : 1; 
        int reserved3           : 2; 
        int win1_rb_swap        : 1;
        int win1_alpha_swap     : 1; 
        int reserved3           : 18; 
    } bits; 
    rt_uint32_t val;
}; 
typedef union vop_big_win1_ctrl0_reg vop_big_win1_ctrl0_reg; 

int rk3326_vopb_win1_init(void)
{
    vop_big_win1_ctrl0_reg reg; 
    reg.bits.win1_en = 1; 
    reg.bits.win1_data_fmt = RGB888; 
    reg.bits.win1_interlace_read = 0; 
    reg.bits.win1_no_outstanding = 0; 
    reg.bits.win1_rb_swap = RGB; 
    reg.bits.win1_alpha_swap = ARGB; 
    writel(VOPB_WIN1_CTRL0, reg.val); 
}
