#ifndef __BEEP
#define __BEEP

#ifdef __cplusplus
extern "C" {
#endif
/*--------------library--------------------*/
#include "cs43l22.h"
/*---------------typedef-------------------*/
typedef struct __BEEP_CONFIG {
    uint8_t mode;
    uint8_t frequency;
    uint8_t ontime;
    uint8_t offtime;
    uint8_t vol;
    uint8_t TC_en;
} BEEP_CONFIG;
/*--------------constants------------------*/
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
/*---------------declrations---------------*/
void generate_beep();
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
