/*
 Event>>  			Layerone Demoboard - Getting Started
 Title>>  			NO TITLE YET
 Group>>			0x24D
 By>>     			Arko & Davo
 Base Libraries>>	Datagram & Hotdogs & Jamis
 ---------------------------------------------------------
 Warning>>			Don't take any of this code seriously
 Danger>>			We're super cereal
 ---------------------------------------------------------
 [ INSERT ASCII ART HERE ]
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include <math.h>
#include "gpu.h"
#include "audio.h"
#include "color.h"
#include "music.h"
#include "sprites.h"
#include "scene.h"
 
#define  FCY    16000000UL                                                      // Instruction cycle frequency, Hz
#include <libpic30.h>
 
#pragma config FWDTEN = OFF , GWRP = OFF , GCP = OFF , JTAGEN = OFF
#pragma config POSCMOD = HS , FCKSM = CSDCMD , FNOSC = PRIPLL , PLL96MHZ = ON , PLLDIV = DIV2
#pragma config ALTPMP = ALTPMPEN , SOSCSEL = EC

int main(void) 
{
	// Initialize IO
    ANSB = 0x0000;                                        
    ANSC = 0x0000;
    ANSD = 0x0000;
    ANSF = 0x0000;
    ANSG = 0x0000;
    TRISB = 0x0000;

    // Initialize GPU
    gpu_init();
    gpu_set_res(RES_160x480, DOUBLEBUFFERED, BPP_4);

    gpu_config_clut();                                    // Configure Color LookUp Table (CLUT)
    gpu_config_chr();                                     // Configure Font Table
    gpu_clear_all_fb();                                   // Clear the screen

    // Load CLUT
 	gpu_clut_set(0, rgb_2_565(0, 0, 0));                                        // Black                                
 	gpu_clut_set(1, rgb_2_565(180, 180, 180));                                  // Light Grey
 	gpu_clut_set(2, rgb_2_565(180, 180, 16));                                   // Yellow
    gpu_clut_set(3, rgb_2_565(16, 180, 180));                                   // Cyan
    gpu_clut_set(4, rgb_2_565(16,180,16));                                      // Green 
 	gpu_clut_set(5, rgb_2_565(180,16,180));                                     // Magenta
 	gpu_clut_set(6, rgb_2_565(180,16,16));                                      // Red
    gpu_clut_set(7, rgb_2_565(16,16,180));                                      // Blue
 	gpu_clut_set(8, rgb_2_565(235,235,235));                                    // White
 	gpu_clut_set(9, rgb_2_565(16,16,16));                                       // Light Black
    gpu_clut_set(10, rgb_2_565(0, 0, 0));                                       // Black   
    gpu_clut_set(11, rgb_2_565(0, 0, 0));                                       // Black   
    gpu_clut_set(12, rgb_2_565(0, 0, 0));                                       // Black   
    gpu_clut_set(13, rgb_2_565(0, 0, 0));                                       // Black   
    gpu_clut_set(14, rgb_2_565(0, 0, 0));                                       // Black   
    gpu_clut_set(15, rgb_2_565(0, 0, 0));                                       // Black   
    
    // Initialize Audio
    audio_init();                                         // Configure Audio ISR

    // Initialize Scene Manager
    scene_init();

    // Draw
	while (1) 
	{
        scene_render_frame();                             // Draw the scene

        gpu_draw_border(0);                               // Draw black border (cleans up VGA frame)
        gpu_flip_fb();                                    // Flip buffers at the next Vsync
        frames++;                                         // Increment frame count
	}

	return 0;
}
