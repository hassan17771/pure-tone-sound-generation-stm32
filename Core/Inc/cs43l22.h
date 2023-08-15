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
#include <stdio.h>
/*---------------typedef-------------------*/
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
    uint16_t frequency;
    uint16_t amplitude;
} SIN_HANDLE;
/*--------------constants------------------*/
//to DAC Tx modes
#define TX_MCLK 0
#define TX_SIN 1 
#define TX_EXTERNAL_MIC 2
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
#define DEVICE_ID 0x01
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
//Digital MIC
//#define RESCALE_24_TO_16(x) ((1<<16)/(1<<24) * x)
/*---------------declrations---------------*/
void write_reg(uint8_t addr, int cnt, ...);
void read_reg(uint8_t addr, int cnt, uint8_t* buff);
void power_up();
void config_register_mode();
void master_config(uint8_t vol, int mute);
//writing a part of a byte, in [s_bit...l_bit] part
void partial_write(uint8_t reg_addr, uint8_t val,uint8_t s_bit, uint8_t l_bit);
void headphone_config();
//using a config as datasheet , value of CLOCKING  register depends on MCLK and Fs(LRCLK) values 
void clock_config();
void PCM_config();
void read_all_regs();


void sin_player(uint16_t freq, uint16_t ampl);
void external_mic();
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
