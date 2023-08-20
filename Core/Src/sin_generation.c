#include "sin_generation.h"

SIN_HANDLE sin_hndl;

extern uint8_t dac_mode;
extern I2S_HandleTypeDef hi2s3;

uint16_t i2s_buff[8];

void read_flash(uint16_t* data_array, uint32_t flash_addr) {
    uint64_t flash_page = *(uint64_t*)flash_addr;
    data_array[3] = flash_page >> 48;
    data_array[2] = (flash_page << 16) >> 48;
    data_array[1] = (flash_page << 32) >> 48;
    data_array[0] = (flash_page << 48) >> 48;
}

void sin_transmission() {
    static uint16_t mem_cnt = 1;
    uint16_t sin_data[4];
    read_flash(sin_data, FLASH_SECTOR11_START+(mem_cnt-1)*8);
    i2s_buff[0] = sin_data[0];
    i2s_buff[1] = sin_data[0];
    i2s_buff[2] = sin_data[1];
    i2s_buff[3] = sin_data[1];
    i2s_buff[4] = sin_data[2];
    i2s_buff[5] = sin_data[2];
    i2s_buff[6] = sin_data[3];
    i2s_buff[7] = sin_data[3];
    if (mem_cnt >= (int)(Fs/4)){
    	uint8_t rem = Fs % 4;
        if (rem != 0) HAL_I2S_Transmit_IT(&hi2s3, i2s_buff, 2*rem);
        else  HAL_I2S_Transmit_IT(&hi2s3, i2s_buff, 8);
        mem_cnt = 1;
    } else {
    	HAL_I2S_Transmit_IT(&hi2s3, i2s_buff, 8);
    	mem_cnt++;
    }
}

void save_sin_on_flash() {
    uint16_t temp_sin_data;
    //erasing the sector before using it
    uint32_t sector_erase_error = 0;
    FLASH_EraseInitTypeDef f_erase = {0};
    f_erase.TypeErase = FLASH_TYPEERASE_SECTORS;
    f_erase.Sector = FLASH_SECTOR_11;
    f_erase.NbSectors = 1;
    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&f_erase, &sector_erase_error);
    //init flash sector
    if (Fs < 2*sin_hndl.frequency) return;
    for (int n = 0; n < Fs; n++) {
        temp_sin_data = DIGITAL_SIN(n, sin_hndl.amplitude, sin_hndl.frequency);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_SECTOR11_START+(n*2), temp_sin_data);
    }
    HAL_FLASH_Lock();
}

void sin_player(uint16_t freq, uint16_t ampl) {
    //initial config
    config_register_mode();
    master_config(MASTR_VOL_MAX - 0x40, 0);
    headphone_config();
    clock_config();
    PCM_config();
    power_up();
    //sin generation
    sin_hndl.frequency = freq;
    sin_hndl.amplitude = ampl;
    save_sin_on_flash();
    //start_sending sin wave
    dac_mode = TX_SIN;
}