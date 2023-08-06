# Sine Wave Generation Using STM32F407
This project focuses on generating a pure tone signal using the STM32F4-discovery kit and the internal DAC _CS43L22_. The signal is then converted from digital to analog and played through headphones.

To facilitate this process, the DAC is configured using the __I2C__ (inter IC communication) protocol, while the audio signal transmission is achieved through __I2S__ (inter IC sound) communication.

As a preliminary step, an inbuilt beep generator is set up. This allows us to verify the functionality of the device, test the hardware, and ensure proper software operation.

## Inbuilt Beep Generator
Before generating the sine wave and utilizing the internal DAC _CS43L22_ for playback, a proper power-up procedure and essential configuration steps must be followed. The [_CS43L22_ datasheet](https://www.cirrus.com/products/cs43l22/) provides valuable information on these procedures and outlines the functions responsible for each step. Let's delve into the initialization process:
1. `config_register_mode()`resets and sets the dac using RESET pin. This lets the MCU initiate desired registers in the DAC using _I2C_.
2.  `master_config()` configures master registers, which usually is used to set output volume and mute state of the DAC, neglecting the source of the signal or the destination of it.
3. `beep_config()` initiates beep test related registers. It uses a `BEEP_CONFIG` typedef struct which states the attributes of beep test signal. It should be initiated using `init_beep()`.
4. `headphone_config()` initiates headphone attributes, _e.g._ mute enable or signal gain. It is also uses a `HEADPHONE_CONFIG` typedef struct and should be initiated using `init_headphone()`.
5.  `clock_config()` initiates the clocking registers, it is __important__ to check the _CS43L22_ datasheet (serial port clocking section), set MCLK (master clock which is made by _I2S_ peripheral) and Fs (sample rate or WS of _I2S_) to a desired valid value.

__note__: in STM32F407 reference manual is stated that:
> Master clock may be output to drive an external audio component. Ratio is fixed at 256 × FS (where FS is the audio sampling frequency). <sub>(RM0090 Rev 19, SPI and I2S features)</sub>
6. `power_up()` sets power up and necessary registers to make the DAC work as mentioned in data sheet.  Additionally, the `gen_MCLK()` function utilizes the I2S peripheral to send empty data, resulting in the generation of MCLK and Fs for the CS43L22 (acting as the I2S slave).
## Sine Wave Generation
The `sin_player()` function, uses a procedure same as beep test generation, but this time the _PCM_ registers are also initiated which states how the dac gets input from the master. (use `init_PCM()`)
The sine signal is made by the below equation:

$DG[n] = \text{ampl} + \text{round}(\text{ampl} \cdot \sin(2 \pi \frac{f}{Fs} \cdot n))$

implemented in DIGITAL_SIN micro.
the sine wave attributes are initiated in the `sin_player()`. and `save_sin_on_flash()` makes a complete signal wave, which period is 1 second and has sample count equal to Fs. this would need a large data storage, so the data is written and read from the flash.
the flash and it's section addresses are mentioned in the STM32 datasheet. the eleventh section is used in this project because to save a complete 1 sec sine signal on the memory there will be a $Fs \cdot \text{I2S dataframe bytes}$, which in this project is set to $48k \cdot 2 = 96k$ bytes.

`SIN_HANDLE` typedef is a struct to keep and control sin signal features. by setting `SIN_HANDLE.enable` to 1, __I2S__ complete transfer interrupt callback which used to call another empty __I2S__ to just keep the DAC running, starts to call `sin_transmission()` function. This function is responsible to read the flash memory and send data using I2S to the DAC.
