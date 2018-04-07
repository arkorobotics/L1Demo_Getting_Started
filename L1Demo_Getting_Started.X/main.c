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
 
#define  FCY    16000000UL                                                      // Instruction cycle frequency, Hz
#include <libpic30.h>
 
#pragma config FWDTEN = OFF , GWRP = OFF , GCP = OFF , JTAGEN = OFF
#pragma config POSCMOD = HS , FCKSM = CSDCMD , FNOSC = PRIPLL , PLL96MHZ = ON , PLLDIV = DIV2
#pragma config ALTPMP = ALTPMPEN , SOSCSEL = EC

int main(void) 
{
	ANSB = 0x0000;                                                              // Configuation
	ANSC = 0x0000;
	ANSD = 0x0000;
	ANSF = 0x0000;
	ANSG = 0x0000;
	TRISB = 0x0000;

	_VMRGNIF = 0;
	_VMRGNIE = 1;
	_GFX1IE = 1;

	config_graphics();                                                          // Configure Graphics, 320 x 480
	config_clut();                                                              // Configure Color LookUp Table (CLUT)
	config_chr();                                                               // Configure Font Table
	config_timer();                                                             // Configure Audio ISR
	clearbuffers();                                                             // Clear the screen
 	
 	uint16_t c0 = 0;                                                            // Black
	uint16_t c1 = rgb_2_565(180, 180, 180);                                     // Light Grey
	uint16_t c2 = rgb_2_565(180, 180, 16);                                      // Yellow
	uint16_t c3 = rgb_2_565(16, 180, 180);                                      // Cyan
    uint16_t c4 = rgb_2_565(16,180,16);                                         // Green
    uint16_t c5 = rgb_2_565(180,16,180);                                        // Magenta
    uint16_t c6 = rgb_2_565(180,16,16);                                         // Red
    uint16_t c7 = rgb_2_565(16,16,180);                                         // Blue
    uint16_t c8 = rgb_2_565(235,235,235);                                       // White
    uint16_t c9 = rgb_2_565(16,16,16);                                          // Light Black

 	clut_set(0, c0);                                                            // Load CLUT
 	clut_set(1, c1);
 	clut_set(2, c2);
    clut_set(3, c3);
    clut_set(4, c4);  
 	clut_set(5, c5);
 	clut_set(6, c6);
    clut_set(7, c7);
 	clut_set(8, c8);  
 	clut_set(9, c9);
    clut_set(10, c0);
    clut_set(11, c0);
    clut_set(12, c0);
    clut_set(13, c0);
    clut_set(14, c0);
    clut_set(15, c0);
    
    // Draw
	while (1) 
	{
		#ifdef DOUBLE_BUFFERED
			swapBuffers();                                                      // Before drawing the next frame, we must swap buffers
		#endif

        //----------------------------------------------------------------------
        switch(frames)                                                          // Draw each a color bar on the first 7 frames
        {
            case 0:
                rcc_color(1);
                rcc_draw(1, 0, HOR_RES/7, VER_RES-1);
                break;
            case 1:
                rcc_color(2);
                rcc_draw(1+((HOR_RES/7)*1), 0, HOR_RES/7, VER_RES-1);
                break;
            case 2:
                rcc_color(3);
                rcc_draw(1+((HOR_RES/7)*2), 0, HOR_RES/7, VER_RES-1);
                break;
            case 3:
                rcc_color(4);
                rcc_draw(1+((HOR_RES/7)*3), 0, HOR_RES/7, VER_RES-1);
                break;
            case 4:
                rcc_color(5);
                rcc_draw(1+((HOR_RES/7)*4), 0, HOR_RES/7, VER_RES-1);
                break;
            case 5:
                rcc_color(6);
                rcc_draw(1+((HOR_RES/7)*5), 0, HOR_RES/7, VER_RES-1);
                break;
            case 6:
                rcc_color(7);
                rcc_draw(1+((HOR_RES/7)*6), 0, HOR_RES/7, VER_RES-1);
                break;
        }
        
		drawBorder(0);
        //----------------------------------------------------------------------
        
		cleanup();                                                              // Housekeeping for VGA signaling
		waitForBufferFlip();                                                    // For next Vsync
		frames++;                                                               // Increment frame count
	}

	return 0;
}
