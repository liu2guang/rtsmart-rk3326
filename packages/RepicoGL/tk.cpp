#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/internal/zgl.h>
#include <GL/glx.h>
#include <MemfbDefs.h>
#include "picotk.h"
#include "screen_config.h"
#include "debugger.h"

void *FrameBuffer = (void *) NULL;

struct TinyFBDevGLXContext {
    GLContext *gl_context; /* base class */
    void *screen;
    ZBuffer *zb; // 绘制用buffer
    int line_length;
};

static FBDevGLXContext *ctx;

rt_device_t opengl_lcd;
static struct rt_device_graphic_info info; 

/**
 * 初始化
 */
static void initialize_fbdev() {
    /* For RT-Thread */ 
    opengl_lcd = rt_device_find("lcd"); 
    if(!opengl_lcd)
    {
        rt_kprintf("Not found RT-Thread LCD Device\n");
        return; 
    }

    rt_device_open(opengl_lcd, RT_DEVICE_OFLAG_RDWR); 
    rt_device_control(opengl_lcd, RTGRAPHIC_CTRL_GET_INFO, &info); 

    FrameBuffer = info.framebuffer; 
    rt_memset(FrameBuffer, 0x00, SCREEN_LINE_BYTES_LENGTH*SCREEN_HEIGHT); 

    rt_kprintf("info: succ create framebuffer");
}

static void shutdown_fbdev(void) {
    // 释放FrameBuffer
    // free(FrameBuffer);
    rt_device_close(opengl_lcd); 
}

void errorcatcher();        /* routine to handle errors */

void tkSwapBuffers(void) {
    fbdev_glXSwapBuffers(ctx);
}

int ui_loop(void *p) {

    int done;
    int i;
    int k;

    // TODO: 增加键盘输入支持

    // 初始化glx上下文
    ctx = fbdev_glXCreateContext();
    
    if (ctx == NULL) {
        return -1;
    }

    initialize_fbdev();
    
    fbdev_glXMakeCurrent(ctx);
    
    rt_kprintf("debug:tk.cpp: succ create cxt at %x, gl_context at %x\n", ctx, ctx->gl_context);
    
    rt_kprintf("debug:tk.cpp: succ create zbuf{%d,%d,%d,%d} at %x\n", 
        ctx->gl_context->zb->xsize, ctx->gl_context->zb->ysize, 
        ctx->gl_context->zb->linesize, 
        ctx->gl_context->zb->mode, ctx->gl_context->zb);

    picotk_init();
    
    rt_kprintf("debug:tk.cpp: toggle reshaping (%d,%d)\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    
    reshape(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    rt_kprintf("debug:tk.cpp: reshaped (%d,%d), start render loop\n", SCREEN_WIDTH, SCREEN_HEIGHT);

    done = 0;

    while (!done) {
        idle();
        rt_thread_mdelay(20);
    }
    shutdown_fbdev();
    return 0;
}


/*
 * Here on an unrecoverable error.
 *
 */
void errorcatcher() {
}

/**
 * 获得帧缓存
 * @return
 */
void *getFrameBuffer() {
    return FrameBuffer;
}

size_t sizeOfFrameBuffer() {
    return (size_t) SCREEN_HEIGHT * SCREEN_LINE_BYTES_LENGTH;
}
