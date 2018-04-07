#include "xc.h"
#include "audio.h"
#include "music.h"

void config_timer(void) 
{
	PR1 = 0xFF;
	_T1IP = 5;	// set interrupt priority
	_TON  = 1;	// turn on the timer
	_T1IF = 0;	// reset interrupt flag
	_T1IE = 1;	// turn on the timer1 interrupt
}

//_T1Interrupt() is the T1 interrupt service routine (ISR).
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    // Song index
	static unsigned int idx = 0;

	static unsigned short duration = 0;
    static unsigned int ch1_ncount = 0;
    static unsigned short ch1_val = 0;

    // Do Channel 1 Stuff
    if (ch1_ncount < 0x7F80) {
        ch1_ncount += song_ch1f[idx];
    }
    else
    {
        ch1_ncount = 0;
    }
    ch1_val = sinetable[ch1_ncount>>6];
    
	// DURATION
	if(duration < 0x7A1)
	{
	 	duration++;
	}
    else
    {
	 	idx++;

		if(idx == sizeof(song_ch1f) / sizeof(song_ch1f[0]) ) /* loop it! */
		{
			idx = 0;
		}

		duration = 0;
	}
	
	// MIX AND SET AUDIO OUTPUT
    PORTB = (ch1_val<<8);

	_T1IF = 0;
}

