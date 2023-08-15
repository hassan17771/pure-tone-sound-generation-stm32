#include "beep_generation.h"

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

void generate_beep() {
    config_register_mode();
    master_config(MASTR_VOL_MAX, 0);
    beep_config();
    headphone_config();
    clock_config();
    power_up();
}