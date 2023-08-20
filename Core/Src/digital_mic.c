#include "digital_mic.h"

extern I2S_HandleTypeDef hi2s3;
extern I2S_HandleTypeDef hi2s2;
extern USART_HandleTypeDef husart2;
extern uint8_t dac_mode;

uint8_t usart_buff[30];
uint32_t rx_buffer[2];
uint16_t tx_buffer[2] = {0, 0};

void mic_transmit() {
    HAL_I2S_Receive_DMA(&hi2s2, rx_buffer, 2);
    dac_mode = TX_PASS;
}

void HAL_I2S_RxCpltCallback (I2S_HandleTypeDef * hi2s) {
	tx_buffer[0] = rx_buffer[0];
	tx_buffer[1] = rx_buffer[1];
	HAL_I2S_Transmit_DMA(&hi2s3, tx_buffer, 2);
}

void external_mic() {
    config_register_mode();
    master_config(MASTR_VOL_MAX, 0);
    headphone_config();
    clock_config();
    PCM_config();
    power_up();
    mic_transmit();
}
