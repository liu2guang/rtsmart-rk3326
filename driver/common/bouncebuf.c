/*
* Generic bounce buffer implementation
*
* Copyright (C) 2012 Marek Vasut <marex@denx.de>
*
* SPDX-License-Identifier:	GPL-2.0+
*/
#include "bouncebuf.h"
#include <string.h>
#include <mmu.h>

#define DBG_TAG "bounce_buffer"
#define BSP_ENBALE_BOUNCE_BUFFER_DEBUG 1

#define DBG_ENABLE
#if (BSP_ENBALE_BOUNCE_BUFFER_DEBUG == 1)
#define DBG_LVL DBG_LOG
#else
#define DBG_LVL DBG_INFO
#endif 
#define DBG_COLOR
#include <rtdbg.h>

#define roundup(x, y) (                  \
    {                                    \
        const typeof(y) __y = y;         \
        (((x) + (__y - 1)) / __y) * __y; \
    })

static int addr_aligned(struct bounce_buffer *state)
{
    const unsigned long align_mask = ARCH_DMA_MINALIGN - 1;

    /* Check if start is aligned */
    if ((unsigned long)state->user_buffer & align_mask)
    {
        // LOG_E("Unaligned buffer address %p", state->user_buffer);
        return 0;
    }

    /* Check if length is aligned */
    if (state->len != state->len_aligned)
    {
        // LOG_E("Unaligned buffer length %u", state->len);
        return 0;
    }

    /* Aligned */
    return 1;
}

int bounce_buffer_start(struct bounce_buffer *state, void *data, size_t len, unsigned int flags)
{
    state->user_buffer = data;
    state->bounce_buffer = data;
    state->len = len;
    state->len_aligned = roundup(len, ARCH_DMA_MINALIGN);
    state->flags = flags;

    if (!addr_aligned(state))
    {
        state->bounce_buffer = rt_malloc_align(state->len_aligned, ARCH_DMA_MINALIGN);
        if (!state->bounce_buffer)
            return (-RT_ENOMEM);

        if (state->flags & GEN_BB_READ)
            memcpy(state->bounce_buffer, state->user_buffer, state->len);
    }

    /*
    * Flush data to RAM so DMA reads can pick it up,
    * and any CPU writebacks don't race with DMA writes
    */
    // flush_dcache_range((unsigned long)state->bounce_buffer, (unsigned long)(state->bounce_buffer) + state->len_aligned);
    rt_hw_dcache_flush_range((unsigned long)state->bounce_buffer, (unsigned long)(state->bounce_buffer) + state->len_aligned); 

    return 0;
}

int bounce_buffer_stop(struct bounce_buffer *state)
{
    if (state->flags & GEN_BB_WRITE)
    {
        /* Invalidate cache so that CPU can see any newly DMA'd data */
        // invalidate_dcache_range((unsigned long)state->bounce_buffer, (unsigned long)(state->bounce_buffer) + state->len_aligned);
        rt_hw_dcache_invalidate_range((unsigned long)state->bounce_buffer, (unsigned long)(state->bounce_buffer) + state->len_aligned);
    }

    if (state->bounce_buffer == state->user_buffer)
        return 0;

    if (state->flags & GEN_BB_WRITE)
        memcpy(state->user_buffer, state->bounce_buffer, state->len);

    rt_free_align(state->bounce_buffer);

    return 0;
}
