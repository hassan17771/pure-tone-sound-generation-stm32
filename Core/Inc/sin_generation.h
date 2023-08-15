#ifndef __SIN
#define __SIN

#ifdef __cplusplus
extern "C" {
#endif
/*--------------library--------------------*/
#include "cs43l22.h"
/*---------------typedef-------------------*/
typedef struct __SIN_HANDLE {
    uint16_t frequency;
    uint16_t amplitude;
} SIN_HANDLE;
/*--------------constants------------------*/
#define I2S_WORK 16
#define Fs 48780
#define TWO_PI  6.28318530717958647692f
#define DIGITAL_SIN(n, ampl, f) (uint16_t)(ampl + round(ampl * sin(TWO_PI * n * f / Fs)))
#define MAX_AMPLITUDE 32767
#define MIN_AMPLITUDE 10
#define FLASH_SECTOR11_START 0x080E0000
/*---------------declarations---------------*/
void sin_player(uint16_t freq, uint16_t ampl);
void sin_transmission();
#ifdef __cplusplus
}
#endif

#endif /* SIN */
