#ifndef __CS43L22_H
#define __CS43L22_H

#ifdef __cplusplus
extern "C" {
#endif
/*--------------library--------------------*/
#include "main.h"
#include "stdarg.h"
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
/*---------------typedef-------------------*/
typedef struct __BEEP_CONFIG {
    uint8_t mode;
    uint8_t frequency;
    uint8_t ontime;
    uint8_t offtime;
    uint8_t vol;
    uint8_t TC_en;
} BEEP_CONFIG;

typedef struct __HEADPHONE_CONFIG {
    uint8_t power;
    uint8_t gain;
    uint8_t mute;
    uint8_t vol;
} HEADPHONE_CONFIG;

typedef struct __PCM_CONFIG {
    uint8_t dac_interface;
    uint8_t audio_wordlen;
    uint8_t mute;
    uint8_t vol;
} PCM_CONFIG;

typedef struct __SIN_HANDLE {
    uint8_t enable;
    uint16_t frequency;
    uint16_t amplitude;
} SIN_HANDLE;
/*--------------constants------------------*/
//dev addrs
#define CS43L22_write 0x94
#define CS43L22_read 0x95
//regs configs
#define BPFREQ_ONTIME_1K_5S 0x7F
#define OFFTIME_BPVOL 0x06
#define MULTIBP_MIX_TC 0xC1 //no Tone Control
#define MASTR_VOL_MAX 0x18 //12dB
#define MASTR_VOL_MIN 0x34 //-102dB
#define EN_HP_DIS_SPKR 0xAF // headphone always on, speaker always off
#define POWER_UP 0x9E // headphone always on, speaker always off
//regs address
#define PLAYBACK_CNTR1 0x0D
#define PLAYBACK_CNTR2 0x0F
#define INTERFACE_CTL_1 0x06
#define MASTR_X_VOL_BURST 0xA0
#define BEEP_CONFIG_REG_BURST 0x9C
#define PCMx_VOL_BURST 0x9A
#define HP_X_VOL_BURST 0xA2
#define PWR_CNTRL2 0x04
#define PDN 0x02
#define CLOCKING_CONF 0x05
#define STATUS 0x2E
//beep config
#define MIN_FREQ 0x0 //260.87 Hz
#define MAX_FREQ 0xF //2181.82 Hz
#define MIN_ONTIME 0x0 //86ms
#define MAX_ONTIME 0xF //5.25s
#define MIN_OFFTIME 0x0 //1.23s
#define MAX_OFFTIME 0xF //10.8s
#define MIN_BEEP_VOL 0x7 //-56dB step 2dB
#define MAX_BEEP_VOL 0x6 //6dB
#define BEEP_OFF 0x0
#define BEEP_SINGLE 0x1
#define BEEP_MULTIPLE 0x2
#define BEEP_CONTINUOUS 0x3
//headphone
#define HP_MAX_GAIN 0x1 //1.143 step 0.14 
#define HP_MIN_GAIN 0x0 //0.3959 step 0.14
#define HP_ON 0xA //always on
#define HP_OFF 0xF //always off0.000016985
#define HP_MUTE 0x1
#define HP_UNMUTE 0x0
#define HP_MAX_VOL 0x0 //0dB step 0.5
#define HP_MIN_VOL 0x1 //mute
//sin generation
#define I2S_WORK 16
#define Fs 48780
#define Fs_INV 0.00002050020500205f // 1/48780
#define TWO_PI  6.28318530717958647692f
#define DIGITAL_SIN(n, ampl, f) (uint16_t)(ampl + round(ampl * sin(TWO_PI * n * f / Fs)))
#define MAX_AMPLITUDE 32767
#define MIN_AMPLITUDE 10
#define FLASH_SECTOR11_START 0x080E0000
//PCM
#define I2S_INTERFACE 0x1
#define WORD_LEN_16BIT 0x3
#define PCM_MAX_VOL 0x18 //12dB step 0.5dB 
#define PCM_MIN_VOL 0x19 //-51.5dB step 0.5dB
/*---------------declrations---------------*/
void generate_beep();
void sin_player(uint16_t freq, uint16_t ampl);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
