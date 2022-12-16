#include "lib.h"
#include "types.h"
#include "pit.h"
#include "speaker.h"

// code inspired by https://wiki.osdev.org/PC_Speaker

/*
 * speaker_play
 * DESCRIPTION: start playing sound at a given frequency
 * INPUTS: freq
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: preempt Channel 2 of PIT, 
 * 				 changing its mode and frequency
 */
void speaker_play(uint32_t freq)
{
	// tweak PIT to channel 2 & set frequency
	uint32_t temp, div = PIT_FREQ_MAX / freq;
	outb(PIT_MODE_CH2, PIT_MODE_PORT);
	outb((uint8_t)div, PIT_DATA_PORT_CH2);
	outb((uint8_t)(div >> 8), PIT_DATA_PORT_CH2);

	// turn on speaker (set bits 0 and 1 of port 0x61)
	temp = inb(SPK_DATA_PORT);
	if (temp != (temp | 3)) {
		outb(temp | 3, SPK_DATA_PORT);
	}
}

/*
 * speaker_stop
 * DESCRIPTION: stop playing sound
 * INPUTS: none
 * OUTPUTS: none
 * RETURN VALUE: none
 * SIDE EFFECTS: reset PIT to channel 0
 */
void speaker_stop(void)
{
	// turn off speaker (clear bits 0 and 1 of port 0x61)
	uint8_t temp = inb(SPK_DATA_PORT) & 0xFC;
	outb(temp, SPK_DATA_PORT);
}
