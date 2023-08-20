#ifndef __DIGITAL_MIC
#define __DIGITAL_MIC

#ifdef __cplusplus
extern "C" {
#endif
/*----------------------------library------------------------------*/
#include "cs43l22.h"
#include <stdio.h>
/*----------------------------typedef------------------------------*/

/*----------------------------constants----------------------------*/

/*-------------------------declarations------------------------------*/
void external_mic();
void mic_transmit();
#ifdef __cplusplus
}
#endif

#endif /* DIGITAL_MIC */
