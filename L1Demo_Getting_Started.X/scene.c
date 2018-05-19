#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include <math.h>
#include <libpic30.h>
#include "xc.h"
#include "alu.h"
#include "gpu.h"
#include "audio.h"
#include "color.h"
#include "music.h"
#include "scene.h"
#include "sprites.h"

struct Scene scene[TOTAL_NUM_SCENES];

void (*scene_func)(void);

void scene_init(void)
{
    // Configure settings for all scenes
    scene[0].scene_id = 0;
    scene[0].start_time = 0;
    scene[0].stop_time = 10;
    scene[0].music_track_id = 0;
    scene[0].res = RES_160x480;
    scene[0].fb_num = SINGLEBUFFERED;
    scene[0].color_depth = BPP_4;

    scene[1].scene_id = 1;
    scene[1].start_time = 10;
    scene[1].stop_time = 2000;
    scene[1].music_track_id = 0;
    scene[1].res = RES_160x480;
    scene[1].fb_num = DOUBLEBUFFERED;
    scene[1].color_depth = BPP_4;

    // Set the current scene to the scene #0
    scene_func = &scene_zero;
    gpu_set_res(scene[0].res, scene[0].fb_num, scene[0].color_depth);
}

void scene_render_frame(void)
{
    static uint16_t scene_index = 0;

    // Change scenes when we reach the stop time for the current scene
    if(time_sec >= scene[scene_index].stop_time)
    {
        // Increment scene index
        if(scene_index < TOTAL_NUM_SCENES)
        {
            scene_index++;
        }
        
        // Change Scenes
        switch(scene_index)
        {
            case 0:
                scene_func = &scene_zero;
                break;
            case 1:
                scene_func = &scene_one;
                break;

            // Add additional Scene functions here
            
            default:
                scene_func = &scene_zero;
                break;
        }
        
        // Configure new graphics settings
        gpu_set_res(scene[scene_index].res, scene[scene_index].fb_num, scene[scene_index].color_depth);
        
        // TODO: Update music track here

    }

    scene_func();
}

void scene_zero(void)
{
    rcc_color(1);                                                           
    rcc_rec(1, 0, gfx.hres/7, gfx.vres-1);                                 

    rcc_color(2);
    rcc_rec(1+((gfx.hres/7)*1), 0, gfx.hres/7, gfx.vres-1);

    rcc_color(3);
    rcc_rec(1+((gfx.hres/7)*2), 0, gfx.hres/7, gfx.vres-1);

    rcc_color(4);
    rcc_rec(1+((gfx.hres/7)*3), 0, gfx.hres/7, gfx.vres-1);

    rcc_color(5);
    rcc_rec(1+((gfx.hres/7)*4), 0, gfx.hres/7, gfx.vres-1);

    rcc_color(6);
    rcc_rec(1+((gfx.hres/7)*5), 0, gfx.hres/7, gfx.vres-1);

    rcc_color(7);
    rcc_rec(1+((gfx.hres/7)*6), 0, gfx.hres/7, gfx.vres-1);
 
}

void scene_one(void)
{
    gpu_clear_fb();

    rcc_color(1); 
    rcc_rec(0,0,gfx.hres-1,gfx.vres-1);
}
