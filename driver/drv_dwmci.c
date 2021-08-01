/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "drv_dwmci.h" 
#include "drv_clk.h" 
#include "rk3326.h"
#include "bouncebuf.h"
#include "mmu.h"
#include "memalign.h"
#include "stdbool.h"

#define DBG_TAG "drv.dwmci"
#define BSP_ENBALE_DWMCI_DEBUG 1

#define DBG_ENABLE
#if (BSP_ENBALE_DWMCI_DEBUG == 1)
#define DBG_LVL DBG_LOG
#else
#define DBG_LVL DBG_INFO
#endif 
#define DBG_COLOR
#include <rtdbg.h>

#define PAGE_SIZE                    4096
#define DWMCI_MSIZE                  0x6

#define MMC_DATA_READ                DATA_DIR_READ
#define MMC_DATA_WRITE               DATA_DIR_WRITE

#define MMC_CMD_GO_IDLE_STATE        0
#define MMC_CMD_SEND_OP_COND         1
#define MMC_CMD_ALL_SEND_CID         2
#define MMC_CMD_SET_RELATIVE_ADDR    3
#define MMC_CMD_SET_DSR              4
#define MMC_CMD_SWITCH               6
#define MMC_CMD_SELECT_CARD          7
#define MMC_CMD_SEND_EXT_CSD         8
#define MMC_CMD_SEND_CSD             9
#define MMC_CMD_SEND_CID             10
#define MMC_CMD_STOP_TRANSMISSION    12
#define MMC_CMD_SEND_STATUS          13
#define MMC_CMD_SET_BLOCKLEN         16
#define MMC_CMD_READ_SINGLE_BLOCK    17
#define MMC_CMD_READ_MULTIPLE_BLOCK  18
#define MMC_SEND_TUNING_BLOCK        19
#define MMC_SEND_TUNING_BLOCK_HS200  21
#define MMC_CMD_SET_BLOCK_COUNT      23
#define MMC_CMD_WRITE_SINGLE_BLOCK   24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK 25
#define MMC_CMD_ERASE_GROUP_START    35
#define MMC_CMD_ERASE_GROUP_END      36
#define MMC_CMD_ERASE                38
#define MMC_CMD_APP_CMD              55
#define MMC_CMD_SPI_READ_OCR         58
#define MMC_CMD_SPI_CRC_ON_OFF       59
#define MMC_CMD_RES_MAN              62

#define SD_CMD_SEND_RELATIVE_ADDR    3
#define SD_CMD_SWITCH_FUNC           6
#define SD_CMD_SEND_IF_COND          8
#define SD_CMD_SWITCH_UHS18V         11
 
#define SD_CMD_APP_SET_BUS_WIDTH     6
#define SD_CMD_APP_SD_STATUS         13
#define SD_CMD_ERASE_WR_BLK_START    32
#define SD_CMD_ERASE_WR_BLK_END      33
#define SD_CMD_APP_SEND_OP_COND      41
#define SD_CMD_APP_SEND_SCR          51

#define SD_IO_SEND_OP_COND           5  
#define SD_IO_RW_DIRECT              52 
#define SD_IO_RW_EXTENDED            53 

#define MMC_RSP_PRESENT              (1 << 0)
#define MMC_RSP_136                  (1 << 1) /* 136 bit response */
#define MMC_RSP_CRC                  (1 << 2) /* expect valid crc */
#define MMC_RSP_BUSY                 (1 << 3) /* card may send busy */
#define MMC_RSP_OPCODE               (1 << 4) /* response contains opcode */
 
#define MMC_RSP_NONE	             (0)
#define MMC_RSP_R1                   (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1b                  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE| MMC_RSP_BUSY)
#define MMC_RSP_R2                   (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3                   (MMC_RSP_PRESENT)
#define MMC_RSP_R4                   (MMC_RSP_PRESENT)
#define MMC_RSP_R5                   (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R6                   (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R7                   (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define min(x, y) ({				 \
	typeof(x) _min1 = (x);			 \
	typeof(y) _min2 = (y);			 \
	(void) (&_min1 == &_min2);		 \
	_min1 < _min2 ? _min1 : _min2; })

#define max(x, y) ({				 \
	typeof(x) _max1 = (x);			 \
	typeof(y) _max2 = (y);			 \
	(void) (&_max1 == &_max2);		 \
	_max1 > _max2 ? _max1 : _max2; })

#define min3(x, y, z) min((typeof(x))min(x, y), z)
#define max3(x, y, z) max((typeof(x))max(x, y), z)

static rt_bool_t fifo_mode = RT_FALSE;
static rt_uint32_t fifo_depth = 0x100; 
static rt_uint32_t fifoth_val = 0;
static rt_bool_t stride_pio = RT_FALSE; 

struct dwmci_idmac 
{
    rt_uint32_t flags;
    rt_uint32_t cnt;
    rt_uint32_t addr;
    rt_uint32_t next_addr;
} __aligned(ARCH_DMA_MINALIGN);

struct mmc_cmd 
{
    rt_uint16_t cmdidx;
    rt_uint32_t resp_type;
    rt_uint32_t cmdarg;
    rt_uint32_t response[4];
};

struct mmc_data 
{
    union {
        char *dest;
        const char *src; /* src buffers don't get written to */
    };
    rt_uint32_t flags;
    rt_uint32_t blocks;
    rt_uint32_t blocksize;
};

/*
 * Currently it supports read/write up to 8*8*4 Bytes per
 * stride as a burst mode. Please note that if you change
 * MAX_STRIDE, you should also update dwmci_memcpy_fromio
 * to augment the groups of {ldm, stm}.
 */
#define MAX_STRIDE 64
// #if CONFIG_ARM && CONFIG_CPU_V7
#if 0
void __attribute__((noinline)) dwmci_memcpy_fromio(void *buffer, void *fifo_addr)
{
    __asm__ __volatile__ (
        "push {r2, r3, r4, r5, r6, r7, r8, r9}\n"
        "ldm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "stm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "ldm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "stm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "ldm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "stm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "ldm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "stm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "ldm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "stm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "ldm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "stm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "ldm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "stm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "ldm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "stm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
        "pop {r2, r3, r4, r5, r6,r7,r8,r9}\n"
        :::"memory"
    );
}

void __attribute__((noinline)) dwmci_memcpy_toio(void *buffer, void *fifo_addr)
{
	__asm__ __volatile__ (
		"push {r2, r3, r4, r5, r6, r7, r8, r9}\n"
		"ldm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"stm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"ldm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"stm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"ldm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"stm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"ldm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"stm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"ldm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"stm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"ldm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"stm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"ldm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"stm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"ldm r0!, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"stm r1, {r2,r3,r4,r5,r6,r7,r8,r9}\n"
		"pop {r2, r3, r4, r5, r6,r7,r8,r9}\n"
		:::"memory"
	);
}
#else
void dwmci_memcpy_fromio(void *buffer, void *fifo_addr) 
{
    rt_kprintf("dwmci_memcpy_fromio\n");
}

void dwmci_memcpy_toio(void *buffer, void *fifo_addr) 
{
    rt_kprintf("dwmci_memcpy_toio\n");
}
#endif

static inline rt_uint32_t dwmci_readl(rt_uint32_t base, int reg)
{
	return readl(base + reg); 
}

static inline void dwmci_writel(rt_uint32_t base, rt_uint32_t reg, rt_uint32_t val)
{
	writel(val, base + reg);
}

/* 判断是否卡忙 */
// static rt_bool_t dwmci_card_is_busy(rt_ubase_t dwmci_base)
// {
//     rt_uint32_t status;
//     status = dwmci_readl(dwmci_base, DWMCI_STATUS);
//     return !!(status & DWMCI_BUSY);
// } 

/* 设置总线SDIO时钟 */ 
static int dwmci_setup_bus_freq(rt_ubase_t dwmci_base, rt_uint32_t *freq)
{
    rt_uint32_t src_clk = 0; 
    rt_uint32_t div = 0; 
    rt_uint32_t status; 
    int timeout = 10000; 

    RT_ASSERT(freq != RT_NULL); 

    if(*freq == 0)
    {
        return RT_EOK; 
    }
    
    /* TODO */ 
    src_clk = rk3326_clk_get_rate_sdmmc(); 
    if(src_clk == (*freq))
    {
        div = 0; 
    }
    else
    {
        div = DIV_ROUND_UP(src_clk, 2 * (*freq));
    }

    /* 关闭SDIO时钟 */ 
	dwmci_writel(dwmci_base, DWMCI_CLKENA, 0);
	dwmci_writel(dwmci_base, DWMCI_CLKSRC, 0); /* TODO */ 

    /* 设置时钟分频系数 */ 
	dwmci_writel(dwmci_base, DWMCI_CLKDIV, div); 

    /* 等待前一个数据传输完成后更新时钟域，开始等待配置完成 */ 
	dwmci_writel(dwmci_base, DWMCI_CMD, DWMCI_CMD_PRV_DAT_WAIT | DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

    /* 等待配置生效 */ 
    timeout = 10000;
    do {
        status = dwmci_readl(dwmci_base, DWMCI_CMD);
        if(timeout-- < 0)
        {
            LOG_E("%s %d: timeout", __func__, __LINE__); 
            return (-RT_ETIMEOUT); 
        }
    } while (status & DWMCI_CMD_START);

    /* 启动时钟 */ 
    dwmci_writel(dwmci_base, DWMCI_CLKENA, DWMCI_CLKEN_ENABLE | DWMCI_CLKEN_LOW_PWR);
    // dwmci_writel(dwmci_base, DWMCI_CLKENA, DWMCI_CLKEN_ENABLE);

    /* 等待前一个数据传输完成后更新时钟域，开始等待配置完成 */ 
    dwmci_writel(dwmci_base, DWMCI_CMD, DWMCI_CMD_PRV_DAT_WAIT | DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

    /* 等待配置生效 */ 
    timeout = 10000;
    do {
        status = dwmci_readl(dwmci_base, DWMCI_CMD);
        if(timeout-- < 0)
        {
            LOG_E("%s %d: timeout", __func__, __LINE__); 
            return (-RT_ETIMEOUT); 
        }
    } while (status & DWMCI_CMD_START);

    *freq = src_clk / (2*div); 
    // LOG_I("setup bus freq %dHz", *freq); 

    return RT_EOK; 
}

/* 设置SDIO总线宽度 */ 
static int dwmci_set_bus_width(rt_ubase_t dwmci_base, rt_uint32_t width)
{
    rt_uint32_t ctype; 

    switch(width)
    {
	case 8:
		ctype = DWMCI_CTYPE_8BIT;
		break;
	case 4:
		ctype = DWMCI_CTYPE_4BIT;
		break;
	default:
		ctype = DWMCI_CTYPE_1BIT;
		break;
    }

    dwmci_writel(dwmci_base, DWMCI_CTYPE, ctype);

    return RT_EOK; 
}

/* 设置时序模式 */ 
static int dwmci_set_ddr_mode(rt_ubase_t dwmci_base, rt_bool_t is_ddr)
{
    rt_uint32_t regs; 

    regs = dwmci_readl(dwmci_base, DWMCI_UHS_REG);

    if(is_ddr)
    {
        regs |= DWMCI_DDR_MODE; 
    }
    else
    {
        regs &= ~DWMCI_DDR_MODE;
    }

    dwmci_writel(dwmci_base, DWMCI_UHS_REG, regs);

    return RT_EOK; 
}

/* 等待各模块复位 */ 
static int dwmci_wait_reset(rt_ubase_t dwmci_base, rt_uint32_t value)
{
	rt_uint32_t timeout = 100000;
	rt_uint32_t ctrl;

	dwmci_writel(dwmci_base, DWMCI_CTRL, value);

	while (timeout--) {
		ctrl = dwmci_readl(dwmci_base, DWMCI_CTRL);
		if (!(ctrl & DWMCI_RESET_ALL))
			return 1;
	}
	return 0;
}

static int dwmci_set_ios(rt_ubase_t dwmci_base, rt_uint32_t clock, rt_uint32_t width, rt_bool_t is_ddr)
{
    dwmci_setup_bus_freq(dwmci_base, &clock); 
    dwmci_set_bus_width(dwmci_base, width); 
    dwmci_set_ddr_mode(dwmci_base, is_ddr); 
    return RT_EOK; 
}

static void dwmci_set_idma_desc(struct dwmci_idmac *idmac, rt_uint32_t desc0, rt_uint32_t desc1, rt_uint32_t desc2)
{
	struct dwmci_idmac *desc = idmac;

	desc->flags = desc0;
	desc->cnt = desc1;
	desc->addr = desc2;
	desc->next_addr = (rt_ubase_t)desc + sizeof(struct dwmci_idmac);
}

/* 准备数据 */ 
static void dwmci_prepare_data(rt_ubase_t dwmci_base, struct mmc_data *data, struct dwmci_idmac *cur_idmac, void *bounce_buffer)
{
    rt_ubase_t ctrl;
    rt_uint32_t i = 0, flags, cnt, blk_cnt;
    rt_ubase_t data_start, data_end;

    blk_cnt = data->blocks;

    /* 复位FIFO */ 
    dwmci_wait_reset(dwmci_base, DWMCI_CTRL_FIFO_RESET);

    /* 配置DMA描述符 */ 
    data_start = (rt_ubase_t)cur_idmac;
    dwmci_writel(dwmci_base, DWMCI_DBADDR, data_start); 

    /* 添加数据到DMA描述符中，每次 */ 
    do{
        flags = DWMCI_IDMAC_OWN | DWMCI_IDMAC_CH; 
        flags |= (i == 0) ? DWMCI_IDMAC_FS : 0; 
		if (blk_cnt <= 8) {
			flags |= DWMCI_IDMAC_LD;
			cnt = data->blocksize * blk_cnt;
		} else
			cnt = data->blocksize * 8;

        dwmci_set_idma_desc(cur_idmac, flags, cnt, (rt_ubase_t)bounce_buffer + (i * PAGE_SIZE)); 

        if (blk_cnt <= 8)
            break;
        blk_cnt -= 8;
        cur_idmac++;
        i++;
    } while(1);

    data_end = (rt_ubase_t)cur_idmac;
    rt_hw_dcache_flush_range(data_start, (data_end+ARCH_DMA_MINALIGN) - data_start);

    /* 使能DMA传输 */ 
    ctrl = dwmci_readl(dwmci_base, DWMCI_CTRL);
    ctrl |= DWMCI_IDMAC_EN | DWMCI_DMA_EN;
    dwmci_writel(dwmci_base, DWMCI_CTRL, ctrl);

    /* 使能IDMAC，突发模式 */ 
    ctrl = dwmci_readl(dwmci_base, DWMCI_BMOD);
    ctrl |= DWMCI_BMOD_IDMAC_FB | DWMCI_BMOD_IDMAC_EN;
    dwmci_writel(dwmci_base, DWMCI_BMOD, ctrl);

    /* 配置DMA描述符块数量和大小字节 */ 
    dwmci_writel(dwmci_base, DWMCI_BLKSIZ, data->blocksize);
    dwmci_writel(dwmci_base, DWMCI_BYTCNT, data->blocksize * data->blocks);
}

static int dwmci_data_transfer(rt_ubase_t dwmci_base, struct mmc_data *data)
{
	int ret = 0;
	int reset_timeout = 100;
	rt_uint32_t timeout = 240000;
	rt_uint32_t status, ctrl, mask, size, i, len = 0;
	rt_uint32_t *buf = NULL;
	rt_tick_t start = rt_tick_get();
	rt_uint32_t fifo_depth = (((fifoth_val & RX_WMARK_MASK) >> RX_WMARK_SHIFT) + 1) * 2;
	bool stride;

    size = data->blocksize * data->blocks / 4;
    /* Still use legacy PIO mode if size < 512(128 * 4) Bytes */
    stride = stride_pio && size > 128;

    if (data->flags == MMC_DATA_READ)
        buf = (unsigned int *)data->dest;
    else
        buf = (unsigned int *)data->src;

    // LOG_I("DATA blocks = %d, blocksize = %d, dir = %d", data->blocks, data->blocksize, data->flags); 

    for (;;) 
    {
        mask = dwmci_readl(dwmci_base, DWMCI_RINTSTS);

        /* Error during data transfer. */
        if (mask & (DWMCI_DATA_ERR | DWMCI_DATA_TOUT)) 
        {
            LOG_E("%s: DATA ERROR! mask = 0x%08x", __func__, mask);

			dwmci_wait_reset(dwmci_base, DWMCI_RESET_ALL);
            dwmci_writel(dwmci_base, DWMCI_CMD, DWMCI_CMD_PRV_DAT_WAIT | DWMCI_CMD_UPD_CLK | DWMCI_CMD_START);

            do {
                status = dwmci_readl(dwmci_base, DWMCI_CMD);
                if (reset_timeout-- < 0)
                    break;
                rt_thread_mdelay(1);
            } while (status & DWMCI_CMD_START);

			if (!fifo_mode) 
            {
				ctrl = dwmci_readl(dwmci_base, DWMCI_BMOD);
				ctrl |= DWMCI_BMOD_IDMAC_RESET;
				dwmci_writel(dwmci_base, DWMCI_BMOD, ctrl);
			}

            ret = -RT_EINVAL; 
            break;
        }

        if (fifo_mode && size) 
        {
            len = 0; 
            if (data->flags == MMC_DATA_READ && (mask & DWMCI_INTMSK_RXDR)) 
            {
                while (size) 
                {
                    len = dwmci_readl(dwmci_base, DWMCI_STATUS);
                    len = (len >> DWMCI_FIFO_SHIFT) & DWMCI_FIFO_MASK;
                    len = min(size, len);

                    if (!stride) 
                    {
                        /* Legacy pio mode */
                        for (i = 0; i < len; i++)
                            *buf++ = dwmci_readl(dwmci_base, DWMCI_FIFO_BASE);
                        goto read_again;
                    }

                    /* dwmci_memcpy_fromio now bursts 256 Bytes once */
                    if (len < MAX_STRIDE)
                        continue;

                    for (i = 0; i < len / MAX_STRIDE; i++) 
                    {
                        dwmci_memcpy_fromio(buf, (void *)(dwmci_base + DWMCI_FIFO_BASE));
                        buf += MAX_STRIDE;
                    }

                    len = i * MAX_STRIDE;
read_again:
                    size = size > len ? (size - len) : 0;
                }
                dwmci_writel(dwmci_base, DWMCI_RINTSTS, DWMCI_INTMSK_RXDR);
            }
            else if (data->flags == MMC_DATA_WRITE &&  (mask & DWMCI_INTMSK_TXDR))
            {
                while (size)
                {
                    len = dwmci_readl(dwmci_base, DWMCI_STATUS);
                    len = fifo_depth - ((len >> DWMCI_FIFO_SHIFT) & DWMCI_FIFO_MASK);
                    len = min(size, len);
                    if (!stride) 
                    {
                        for (i = 0; i < len; i++)
                            dwmci_writel(dwmci_base, DWMCI_FIFO_BASE, *buf++);
                        goto write_again;
                    }

					/* dwmci_memcpy_toio now bursts 256 Bytes once */
					if (len < MAX_STRIDE)
						continue;

                    for (i = 0; i < len / MAX_STRIDE; i++) 
                    {
                        dwmci_memcpy_toio(buf, (void *)(dwmci_base + DWMCI_FIFO_BASE));
                        buf += MAX_STRIDE;
                    }

					len = i * MAX_STRIDE;
write_again:
					size = size > len ? (size - len) : 0;
                }
                dwmci_writel(dwmci_base, DWMCI_RINTSTS, DWMCI_INTMSK_TXDR);
            }
        }

        /* Data arrived correctly. */
        if (mask & DWMCI_INTMSK_DTO) 
        {
            ret = RT_EOK;
            break;
        }

		/* Check for timeout. */
		if ((rt_tick_get()-start) > timeout) 
        {
			LOG_E("%s: Timeout waiting for data!", __func__);
			ret = -RT_ETIMEOUT;
			break;
		}
    }

    dwmci_writel(dwmci_base, DWMCI_RINTSTS, mask);

    return ret; 
}

static int dwmci_set_transfer_mode(struct mmc_data *data)
{
	rt_ubase_t mode;

	mode = DWMCI_CMD_DATA_EXP;
	if (data->flags & MMC_DATA_WRITE)
		mode |= DWMCI_CMD_RW;

	return mode;
}

static int dwmci_send_cmd(rt_ubase_t dwmci_base, struct mmc_cmd *cmd, struct mmc_data *data)
{
    ALLOC_CACHE_ALIGN_BUFFER(struct dwmci_idmac, cur_idmac, data ? DIV_ROUND_UP(data->blocks, 8) : 0);
    int ret = 0, flags = 0, i;
    rt_tick_t timeout = 500;
    rt_uint32_t retry = 100000;
    rt_uint32_t mask, ctrl;
    struct bounce_buffer bbstate;
    
    rt_tick_t start = rt_tick_get_millisecond();

    /* 带超时时间的忙等待 */ 
	while (dwmci_readl(dwmci_base, DWMCI_STATUS) & DWMCI_BUSY) 
    {
		if ((rt_tick_get_millisecond()-start) > timeout) 
        {
            LOG_E("%s %d: timeout on data busy", __func__, __LINE__); 
			return (-RT_ETIMEOUT);
		}
	}

    /* 屏蔽所有中断 */ 
    dwmci_writel(dwmci_base, DWMCI_RINTSTS, DWMCI_INTMSK_ALL); 

    /* 如果命令带有数据 */ 
    if (data)
    {
        if (fifo_mode) 
        {
            dwmci_writel(dwmci_base, DWMCI_BLKSIZ, data->blocksize);
            dwmci_writel(dwmci_base, DWMCI_BYTCNT, data->blocksize * data->blocks);
            dwmci_wait_reset(dwmci_base, DWMCI_CTRL_FIFO_RESET);
        }
        else
        {
            if (data->flags == MMC_DATA_READ) 
                bounce_buffer_start(&bbstate, (void*)data->dest, data->blocksize * data->blocks, GEN_BB_WRITE);
            else
                bounce_buffer_start(&bbstate, (void*)data->src, data->blocksize * data->blocks, GEN_BB_READ);
            dwmci_prepare_data(dwmci_base, data, cur_idmac, bbstate.bounce_buffer);
        }
    }
    
    /* 写入CMD参数 */ 
    dwmci_writel(dwmci_base, DWMCI_CMDARG, cmd->cmdarg); 

    if (data)
        flags = dwmci_set_transfer_mode(data);

    if ((cmd->resp_type & MMC_RSP_136) && (cmd->resp_type & MMC_RSP_BUSY))
        return -1; 

    /* 判断命令是否是终止传输 */ 
    if (cmd->cmdidx == MMC_CMD_STOP_TRANSMISSION)
        flags |= DWMCI_CMD_ABORT_STOP;
    else
        flags |= DWMCI_CMD_PRV_DAT_WAIT;

    if (cmd->resp_type & MMC_RSP_PRESENT) 
    {
        flags |= DWMCI_CMD_RESP_EXP;
        if (cmd->resp_type & MMC_RSP_136)
            flags |= DWMCI_CMD_RESP_LENGTH;
    }

    /* 判断是否检测CRC */ 
    if (cmd->resp_type & MMC_RSP_CRC)
        flags |= DWMCI_CMD_CHECK_CRC;

    flags |= (cmd->cmdidx | DWMCI_CMD_START | DWMCI_CMD_USE_HOLD_REG); 

    // LOG_D("Sending CMD%d", cmd->cmdidx);

    /* 写入需要发送的命令 */ 
    dwmci_writel(dwmci_base, DWMCI_CMD, flags);

    /* 轮询判断 Command done (CD) 命令是否发送完成 */ 
    for (i = 0; i < retry; i++) 
    {
        mask = dwmci_readl(dwmci_base, DWMCI_RINTSTS);
        if (mask & DWMCI_INTMSK_CDONE) 
        {
            if (!data)
                dwmci_writel(dwmci_base, DWMCI_RINTSTS, mask);
            break;
        }
    }

    if (i == retry) 
    {
        LOG_E("%s: Wait CMD done timeout.", __func__);
        return -RT_ETIMEOUT;
    }

    /* 判断是否响应超时 Response timeout (RTO) */ 
    if (mask & DWMCI_INTMSK_RTO)
    {
        /* 这里的超时不一定是致命的。(e)当MMC卡收到CMD55时，它们会在这里飞溅，
           因为它们不支持这个命令，这正是区分它们和SD卡的方法。因此，下面的输
           出应该是debug()。eMMC卡也不支持CMD8，请记住 */
        // LOG_D("%s: Response Timeout.\n", __func__);
        return -RT_ETIMEOUT;
    }
    else if (mask & DWMCI_INTMSK_RE) /* 判断响应是否错误 Response error (RE) */ 
    {
		LOG_E("%s: Response Error", __func__);
		return -RT_EIO;
    }

    if (cmd->resp_type & MMC_RSP_PRESENT) 
    {
        if (cmd->resp_type & MMC_RSP_136) 
        {
            cmd->response[0] = dwmci_readl(dwmci_base, DWMCI_RESP3);
            cmd->response[1] = dwmci_readl(dwmci_base, DWMCI_RESP2);
            cmd->response[2] = dwmci_readl(dwmci_base, DWMCI_RESP1);
            cmd->response[3] = dwmci_readl(dwmci_base, DWMCI_RESP0);
        } 
        else 
        {
            cmd->response[0] = dwmci_readl(dwmci_base, DWMCI_RESP0);
        }
    }

    /* 发送数据 */ 
    if (data)
    {
        ret = dwmci_data_transfer(dwmci_base, data); 

		/* only dma mode need it */
        if (!fifo_mode) 
        {
            ctrl = dwmci_readl(dwmci_base, DWMCI_CTRL);
            ctrl &= ~(DWMCI_DMA_EN);
            dwmci_writel(dwmci_base, DWMCI_CTRL, ctrl);
            bounce_buffer_stop(&bbstate);
        }
    }

    rt_thread_mdelay(1);

    return ret;
}

/* 初始化 */  
static int dwmci_init(rt_ubase_t dwmci_base)
{
    rt_uint32_t use_dma = 0;

    /* 下电再上电 */ 
    dwmci_writel(dwmci_base, DWMCI_PWREN, 0);
    rt_thread_mdelay(10); 
    dwmci_writel(dwmci_base, DWMCI_PWREN, 1);
    rt_thread_mdelay(10); 

    /* 等待全部模块复位 */ 
    if (!dwmci_wait_reset(dwmci_base, DWMCI_RESET_ALL)) 
    {
        LOG_E("%s %d: reset all fail", __func__, __LINE__); 
        return (-RT_EIO);
    }

    /* 读取DMA模式 */ 
    use_dma = DWMCI_GET_TRANS_MODE(dwmci_readl(dwmci_base, DWMCI_HCON)); 
    if (use_dma == DMA_INTERFACE_IDMA) 
        fifo_mode = 0;
    else 
        fifo_mode = 1;

    /* 以100KHz枚举设备 */
    rt_uint32_t clock = 400*1000; 
    dwmci_setup_bus_freq(dwmci_base, &clock); 
    dwmci_set_bus_width(dwmci_base, 1); 
    dwmci_set_ddr_mode(dwmci_base, 0); 

    /* 启动SDIO中断, 数据空闲中断, 清空所有中断状态 */ 
	dwmci_writel(dwmci_base, DWMCI_RINTSTS, 0xFFFFFFFF); 

    /* 取消全部中断屏蔽, 响应所有中断 */ 
	dwmci_writel(dwmci_base, DWMCI_INTMASK, 0); 

    /* 设置最大超时时间 */ 
    dwmci_writel(dwmci_base, DWMCI_TMOUT, 0xFFFFFFFF);

    /* 内部DMAC中断使能全部关闭 */ 
    dwmci_writel(dwmci_base, DWMCI_IDINTEN, 0);

    /* 软件复位。重置DMA控制器的所有内部寄存器 */ 
    dwmci_writel(dwmci_base, DWMCI_BMOD, 1); 

    rt_uint32_t ctrl = dwmci_readl(dwmci_base, DWMCI_BMOD);
    ctrl |= DWMCI_BMOD_IDMAC_RESET;
    dwmci_writel(dwmci_base, DWMCI_BMOD, ctrl);

    rt_thread_mdelay(10); 

    if(!fifoth_val)
    {
        rt_uint32_t fifo_size; 

        fifo_size = dwmci_readl(dwmci_base, DWMCI_FIFOTH);
        fifo_size = ((fifo_size & RX_WMARK_MASK) >> RX_WMARK_SHIFT) + 1; 
        fifoth_val = MSIZE(DWMCI_MSIZE) | RX_WMARK(fifo_size / 2 - 1) | TX_WMARK(fifo_size / 2);
    }
    dwmci_writel(dwmci_base, DWMCI_FIFOTH, fifoth_val); 
    rt_thread_mdelay(10); 
    dwmci_writel(dwmci_base, DWMCI_FIFOTH, fifoth_val); 
    rt_thread_mdelay(10); 

    /* 关闭SDIO时钟 */ 
    dwmci_writel(dwmci_base, DWMCI_CLKENA, 0); 
    dwmci_writel(dwmci_base, DWMCI_CLKSRC, 0); 

    return RT_EOK; 
}
///////////////////////////////////////////////////////////////////////////////
#include <rtthread.h>
#include <drivers/mmcsd_core.h>
#include <drivers/sd.h>
#include <drivers/mmc.h>
#include <drivers/sdio.h>

static struct rt_mmcsd_host *host; 

static void sdio_request_send(struct rt_mmcsd_host *host, struct rt_mmcsd_req *req)
{
    struct mmc_cmd cmd;
    struct mmc_cmd stop;
    struct mmc_data dat;

    rt_memset(&cmd, 0, sizeof(struct mmc_cmd));
    rt_memset(&stop, 0, sizeof(struct mmc_cmd));
    rt_memset(&dat, 0, sizeof(struct mmc_data));

    cmd.cmdidx = req->cmd->cmd_code;
    cmd.cmdarg = req->cmd->arg;
    if (req->cmd->flags & RESP_MASK)
    {
        cmd.resp_type = MMC_RSP_PRESENT; 
        if(resp_type(req->cmd) == RESP_R2)
        {
            cmd.resp_type |= MMC_RSP_136;
        }
    }
    else
    {
        cmd.resp_type = MMC_RSP_NONE;
    } 

    if(req->data)
    {
        dat.dest = (char *)req->data->buf;
        dat.flags = req->data->flags;
        dat.blocksize = req->data->blksize;
        dat.blocks = req->data->blks;

        // LOG_D("dat.flags = %d, dat.blocksize = %d, dat.blocks = %d", dat.flags, dat.blocksize, dat.blocks); 

        req->cmd->err = dwmci_send_cmd(SDMMC_BASE, &cmd, &dat); 
    }
    else
    {
        req->cmd->err = dwmci_send_cmd(SDMMC_BASE, &cmd, RT_NULL); 
    }

    // LOG_D("cmdarg:%d", cmd.cmdarg);
    // LOG_D("cmdidx:%d", cmd.cmdidx);

    // LOG_D("[0]:0x%08x [1]:0x%08x [2]:0x%08x [3]:0x%08x", cmd.response[0], cmd.response[1], cmd.response[2], cmd.response[3]);
    req->cmd->resp[3] = cmd.response[3];
    req->cmd->resp[2] = cmd.response[2];
    req->cmd->resp[1] = cmd.response[1];
    req->cmd->resp[0] = cmd.response[0]; 

    if (req->stop)
    {
        stop.cmdidx = req->stop->cmd_code;
        stop.cmdarg = req->stop->arg;
        if (req->stop->flags & RESP_MASK)
        {
            stop.resp_type = MMC_RSP_PRESENT;
            if (resp_type(req->stop) == RESP_R2)
            {
                stop.resp_type |= MMC_RSP_136;
            }
        }
        else
        {
            cmd.resp_type = MMC_RSP_NONE;
        }

        req->stop->err = dwmci_send_cmd(SDMMC_BASE, &stop, RT_NULL);
    }

    mmcsd_req_complete(host);
}

static void sdio_set_iocfg(struct rt_mmcsd_host *host, struct rt_mmcsd_io_cfg *io_cfg)
{
    dwmci_set_ios(SDMMC_BASE, io_cfg->clock, io_cfg->bus_width, RT_FALSE); 
}

static const struct rt_mmcsd_host_ops ops =
{
    sdio_request_send,
    sdio_set_iocfg,
    RT_NULL,
    RT_NULL,
};

int rt_hw_sdio_init(void)
{
    rt_ubase_t dwmci_base = SDMMC_BASE; 

    host = mmcsd_alloc_host();
    host->ops = &ops;
    host->freq_min = 400 * 1000;
    host->freq_max = 25 * 1000 * 1000;
    host->valid_ocr =  VDD_32_33 | VDD_33_34; 
    // host->flags = MMCSD_MUTBLKWRITE | MMCSD_SUP_SDIO_IRQ | MMCSD_SUP_HIGHSPEED;
    host->flags = MMCSD_SUP_SDIO_IRQ | MMCSD_SUP_HIGHSPEED;
    host->max_seg_size = 2048;
    host->max_dma_segs = 3;
    host->max_blk_size = 512;
    host->max_blk_count = 65535; // 4096

    fifoth_val = MSIZE(DWMCI_MSIZE) | RX_WMARK(fifo_depth / 2 - 1) | TX_WMARK(fifo_depth / 2);

    dwmci_init(dwmci_base); 
    mmcsd_change(host);

    return RT_EOK; 
}
INIT_ENV_EXPORT(rt_hw_sdio_init); 
// MSH_CMD_EXPORT_ALIAS(rt_hw_sdio_init, sdio, rt_hw_sdio_init); 

///////////////////////////////////////////////////////////////////////////////
int dwmci_dump(void)
{
    DUMP_UART_REG(SDMMC_BASE, DWMCI_CTRL);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_PWREN);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_CLKDIV);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_CLKSRC);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_CLKENA);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_TMOUT);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_CTYPE);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_BLKSIZ);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_BYTCNT);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_INTMASK);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_CMDARG);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_CMD);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_RESP0);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_RESP1);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_RESP2);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_RESP3);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_MINTSTS);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_RINTSTS);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_STATUS);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_FIFOTH);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_CDETECT);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_WRTPRT);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_TCBCNT);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_TBBCNT);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_DEBNCE);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_USRID);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_VERID);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_HCON);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_UHS_REG);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_RSTN);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_BMOD);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_PLDMND);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_DBADDR);
    DUMP_UART_REG(SDMMC_BASE, DWMCI_IDSTS);

    return RT_EOK; 
}
MSH_CMD_EXPORT_ALIAS(dwmci_dump, dwmci_dump, dwmci_dump); 
