#include "cs43l22.h"

extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;

BEEP_CONFIG bconf;
HEADPHONE_CONFIG hconf;
PCM_CONFIG pconf;
SIN_HANDLE sin_hndl;

uint16_t dummy_buffer;
uint16_t i2s_buff[8];

void write_reg(uint8_t reg_addr, int count, ...) {
    uint8_t buff[10];
    va_list args;
    va_start(args, count);

    buff[0] = reg_addr;
    for (int i = 1; i <= count; i++)
        buff[i] = va_arg(args, int);

    HAL_I2C_Master_Transmit(&hi2c1, CS43L22_write, buff, count+1, HAL_MAX_DELAY);
}

void read_reg(uint8_t reg_addr, int count, uint8_t* buff) {
    buff[0] = reg_addr + 0x80;
    HAL_I2C_Master_Transmit(&hi2c1, CS43L22_write, buff, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, CS43L22_read, buff, count, HAL_MAX_DELAY);
}

void set_dac() {
    HAL_GPIO_WritePin(Audio_RST_GPIO_Port, Audio_RST_Pin, GPIO_PIN_SET);
}

void reset_dac() {
    HAL_GPIO_WritePin(Audio_RST_GPIO_Port, Audio_RST_Pin, GPIO_PIN_RESET);
    sin_hndl.enable = 0;
}

static void gen_MCLK() {
	dummy_buffer = 0;
    HAL_I2S_Transmit_IT(&hi2s3, &dummy_buffer, 1);
}

//required init as written in datasheet.
static void power_up() {
    write_reg(0x00, 1, 0x99);
    write_reg(0x47, 1, 0x80);
    write_reg(0x32, 1, 0xBB);
    write_reg(0x32, 1, 0x3B);
    write_reg(0x00, 1, 0x00);
    gen_MCLK();
    write_reg(PDN, 1, POWER_UP);
}

void config_register_mode() {
    //reset before starting
    set_dac();
    HAL_Delay(100);
    reset_dac();
    set_dac();
}

//0.5dB step level for volume
void master_config(uint8_t volume_LR, int mute_en) {
    uint8_t mute_reg;
    write_reg(MASTR_X_VOL_BURST, 2, volume_LR, volume_LR);
    read_reg(PLAYBACK_CNTR1, 1, &mute_reg);
    mute_reg >>= 2;
    mute_reg <<= 2;
    if (mute_en) write_reg(PLAYBACK_CNTR1, 1, mute_reg+3);
    else write_reg(PLAYBACK_CNTR1, 1, mute_reg);
}

void init_beep() {
    bconf.mode = BEEP_MULTIPLE;
    bconf.frequency = MAX_FREQ;
    bconf.ontime = MAX_ONTIME;
    bconf.offtime = MIN_OFFTIME + 0x3;
    bconf.vol = MAX_BEEP_VOL;
    bconf.TC_en = 0;
}

void beep_config() {
    init_beep();
    uint8_t freq_ontime = (bconf.frequency<<4) + bconf.ontime;
    uint8_t offtime_vol = (bconf.offtime<<5) + (bconf.vol & 0x1F);
    uint8_t mode_TCen = (bconf.mode<<6) + bconf.TC_en;
    write_reg(BEEP_CONFIG_REG_BURST, 3, freq_ontime, offtime_vol, mode_TCen);
}

void init_headphone() {
    hconf.power = HP_ON;
    hconf.mute = HP_UNMUTE;
    hconf.gain = HP_MAX_GAIN;
    hconf.vol = HP_MAX_VOL;
}

//writing a part of a byte, in [s_bit...l_bit] part
void partial_write(uint8_t reg_addr, uint8_t val,uint8_t s_bit, uint8_t l_bit) {
    uint8_t initial_val;
    read_reg(reg_addr, 1, &initial_val);
    uint8_t left_part = (initial_val >> (s_bit+1)) << (s_bit+1);
    uint8_t right_part = (initial_val << (l_bit+1)) >> (l_bit+1);
    val <<= l_bit;
    write_reg(reg_addr, 1, left_part+val+right_part);
}

void headphone_config() {
    init_headphone();
    partial_write(PWR_CNTRL2, hconf.power, 7, 4);
    partial_write(PLAYBACK_CNTR1, hconf.gain, 7, 5);
    partial_write(PLAYBACK_CNTR2, hconf.mute, 7, 6);
    write_reg(HP_X_VOL_BURST, 2, hconf.vol, hconf.vol);
}

//using a config as datasheet , value of CLOCKING  register depends on MCLK and Fs(LRCLK) values 
void clock_config() {
    write_reg(CLOCKING_CONF, 1, 0xA0);
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

void init_PCM() {
    pconf.dac_interface = I2S_INTERFACE;
    pconf.audio_wordlen = WORD_LEN_16BIT;
    pconf.mute = 0;
    pconf.vol = PCM_MAX_VOL;
}

void PCM_config() {
    init_PCM();
    uint8_t pcmx_vol = (pconf.mute<<7) + (pconf.vol & 0x7F);
    partial_write(INTERFACE_CTL_1, pconf.dac_interface, 3, 2);
    partial_write(INTERFACE_CTL_1, pconf.audio_wordlen, 1, 0);
    write_reg(PCMx_VOL_BURST, 2, pcmx_vol, pcmx_vol);
}

void read_flash(uint16_t* data_array, uint32_t flash_addr) {
    uint64_t flash_page = *(uint64_t*)flash_addr;
    data_array[3] = flash_page >> 48;
    data_array[2] = (flash_page << 16) >> 48;
    data_array[1] = (flash_page << 32) >> 48;
    data_array[0] = (flash_page << 48) >> 48;
}

void sin_transmition() {
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

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (sin_hndl.enable)
        sin_transmition();
    else
        HAL_I2S_Transmit_IT(&hi2s3, &dummy_buffer, 1);
}

void sin_player(uint16_t freq, uint16_t ampl) {
    //initial config
    config_register_mode();
    master_config(MASTR_VOL_MIN+0x52, 0);
    headphone_config();
    clock_config();
    PCM_config();
    power_up();
    //sin generation
    sin_hndl.frequency = freq;
    sin_hndl.amplitude = ampl;
    save_sin_on_flash();
    sin_hndl.enable = 1;
}

void generate_beep() {
    config_register_mode();
    master_config(MASTR_VOL_MAX, 0);
    beep_config();
    headphone_config();
    clock_config();
    power_up();
}
