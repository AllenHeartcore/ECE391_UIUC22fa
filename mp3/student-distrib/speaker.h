#ifndef SPEAKER_H
#define SPEAKER_H

#define PIT_FREQ_MAX		1193180

// the speaker is connected to channel 2
#define PIT_MODE_CH2		0xb6
#define PIT_DATA_PORT_CH2	0x42
#define SPK_DATA_PORT		0x61

#define FREQ_C3		131
#define FREQ_CS3	139
#define FREQ_D3		147
#define FREQ_DS3	156
#define FREQ_E3		165
#define FREQ_F3		175
#define FREQ_FS3	185
#define FREQ_G3		196
#define FREQ_GS3	208
#define FREQ_A3		220
#define FREQ_AS3	233
#define FREQ_B3		247
#define FREQ_C4		262
#define FREQ_CS4	277
#define FREQ_D4		294
#define FREQ_DS4	311
#define FREQ_E4		330
#define FREQ_F4		349
#define FREQ_FS4	370
#define FREQ_G4		392
#define FREQ_GS4	415
#define FREQ_A4		440
#define FREQ_AS4	466
#define FREQ_B4		494
#define FREQ_C5		523
#define FREQ_CS5	554
#define FREQ_D5		587
#define FREQ_DS5	622
#define FREQ_E5		659
#define FREQ_F5		698
#define FREQ_FS5	740
#define FREQ_G5		784
#define FREQ_GS5	831
#define FREQ_A5		880
#define FREQ_AS5	932
#define FREQ_B5		988
#define FREQ_C6		1047

void speaker_play(uint32_t freq);
void speaker_stop(void);

#endif
