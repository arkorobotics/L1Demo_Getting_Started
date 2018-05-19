#include "xc.h"
#include "audio.h"
#include "music.h"

volatile uint32_t time_sec;		                   // Demo Master Clock
volatile uint32_t time_subsec;

volatile unsigned short ch1_val = 0;               // Audio Channel 1
volatile unsigned short ch2_val = 0;               // Audio Channel 2
volatile unsigned short ch3_val = 0;               // Audio Channel 3

void audio_init(void) 
{
    PR1 = 0x100;	// TODO: Add math for calculating PR1 based on sample freq (62.745kHz)
    _T1IP = 5;	// set interrupt priority
    _TON  = 1;	// turn on the timer
    _T1IF = 0;	// reset interrupt flag
    _T1IE = 1;	// turn on the timer1 interrupt

    time_sec = 0;       // Time Epoch
    time_subsec = 0;
}

void audio_isr(void)
{
    // Song index
	static unsigned int idx = 0;

	static unsigned short duration = 0;
    static unsigned int ch1_ncount = 0;
    static unsigned int ch2_ncount = 0;
    static unsigned int ch3_ncount = 0;

    // Do Channel 1 Stuff
    ch1_ncount += song_ch2f[idx] - 1;
    if (ch1_ncount > 0x803f)
    {
        ch1_ncount = 0;
    }
    
    // Do Channel 2 Stuff
    ch2_ncount += song_ch3f[idx] - 1;
    if (ch2_ncount > 0x803f)
    {
        ch2_ncount = 0;
    }
    
    // Do Channel 3 Stuff
    ch3_ncount += song_ch4f[idx] - 1;
    if (ch3_ncount > 0x803f)
    {
        ch3_ncount = 0;
    }

    ch1_val = sinetable[ch1_ncount>>6];
    ch2_val = sinetable[ch2_ncount>>6];
    ch3_val = sinetable[ch3_ncount>>6];
    
	// Duration
	if(duration < 0x7A1)
	{
	 	duration++;
	}
    else
    {
	 	idx++;

        // Loop it!
		if(idx == sizeof(song_ch4f) / sizeof(song_ch4f[0]) ) 
		{
			idx = 0;
		}

		duration = 0;
	}
	
	// Mix and set audio
    PORTB = (ch1_val<<6) + (ch2_val<<6) + (ch3_val<<6);

    // Time Keeping
    time_subsec++;

    if(time_subsec >= 0xf424)
    {
        time_sec++;
        time_subsec = 0;
    }

    // Clear Timer Flag
	_T1IF = 0;
}


//_T1Interrupt() is the T1 interrupt service routine (ISR).
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    audio_isr();
}
