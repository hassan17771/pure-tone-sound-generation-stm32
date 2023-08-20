#include "cs43l22.h"

extern I2C_HandleTypeDef hi2c1;
extern I2S_HandleTypeDef hi2s3;
extern uint8_t dac_mode;

HEADPHONE_CONFIG hconf;
PCM_CONFIG pconf;

uint16_t dummy_buffer;

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
}

void gen_MCLK() {
    HAL_I2S_Transmit_IT(&hi2s3, &dummy_buffer, 1);
}

//required init as written in datasheet.
void power_up() {
    write_reg(0x00, 1, 0x99);
    write_reg(0x47, 1, 0x80);
    write_reg(0x32, 1, 0xBB);
    write_reg(0x32, 1, 0x3B);
    write_reg(0x00, 1, 0x00);
    gen_MCLK();
    write_reg(PDN, 1, POWER_UP);
}

void config_register_mode() {
    //reset before starting and some initial values
    dummy_buffer = 0;
    dac_mode = TX_MCLK;
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

void init_headphone() {
    hconf.power = HP_ON;
    hconf.mute = HP_UNMUTE;
    hconf.gain = HP_MAX_GAIN;
    hconf.vol = HP_MAX_VOL;
}

void partial_write(uint8_t reg_addr, uint8_t val,uint8_t s_bit, uint8_t l_bit) {
    uint8_t initial_val;
    read_reg(reg_addr, 1, &initial_val);
    // reading what was initially in the register, then keep [MSB...s_bit] and [l_bit...LSB], then combining it with desired value
    uint8_t left_part = initial_val & (0xFF << (s_bit+1));
    uint8_t right_part = initial_val & (0xFF >> (8 - l_bit));
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

void clock_config() {
    write_reg(CLOCKING_CONF, 1, 0xA0);
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

void read_all_regs() {
    uint8_t all_regs[50];
    read_reg(DEVICE_ID, 50, all_regs);
}
