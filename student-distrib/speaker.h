#ifndef SPEAKER_H
#define SPEAKER_H

#define PIT_FREQ_MAX		1193180

// the speaker is connected to channel 2
#define PIT_MODE_CH2		0xb6
#define PIT_DATA_PORT_CH2	0x42
#define SPK_DATA_PORT		0x61

#define FREQ_C4		523
#define FREQ_D4		587
#define FREQ_E4		659
#define FREQ_F4		698
#define FREQ_G4		784
#define FREQ_A4		880
#define FREQ_B4		988
#define FREQ_C5		1047
#define FREQ_D5		1175
#define FREQ_E5		1319
#define FREQ_F5		1397
#define FREQ_G5		1568

void speaker_play(uint32_t freq);
void speaker_stop(void);

#endif
