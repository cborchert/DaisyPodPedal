#pragma once
#include "daisy_pod.h"
#include "dev/oled_ssd130x.h"

using namespace daisy;

class Display
{
public:
  // The top zone (yellow) is 15 pixels high; the blue zone starts after that
  static constexpr int TOP_ZONE_HEIGHT = 15;

  // LEARNING: we are NOT using static here because we want each Display object to have its own instance of the oled display driver
  OledDisplay<SSD130xI2c128x64Driver> display;

  void Init()
  {
    // LEARNING: `::` => class / struct / enum / namespace scope
    //           `.` => instance member
    //           `->` => instance pointer member

    // Set up the oled display config
    OledDisplay<SSD130xI2c128x64Driver>::Config display_config;
    // LEARNING: depending on the OLED board, i2c_address may be 0x3C or 0x3D; this tells daisy how to communicate with the display
    display_config.driver_config.transport_config.i2c_address = 0x3C;
    // LEARNING: auto is type inference. it allows the compiler to figure out the type
    // LEARNING: &i2c means that i2c is a reference to the existing object/struct rather than a copy whichC++ does by default

    auto &i2c = display_config.driver_config.transport_config.i2c_config;
    // set the hardware block and set it up so that daisy initiates communication at an acceptable speed
    // I2C_1 maps to D11/D12 on the pinout sheet
    i2c.periph = I2CHandle::Config::Peripheral::I2C_1;
    // daisy specific pins for outputs wired to the i2c_1 board
    i2c.pin_config.scl = seed::D11;
    i2c.pin_config.sda = seed::D12;
    // LEARNING: IC2_MASTER => the daisy (mcu) drives the clock and initiates communication
    i2c.mode = I2CHandle::Config::Mode::I2C_MASTER;
    // NOTE: most SSD1306OLEDs support 100khz; it's a safe, reliable value.
    //   check the datasheet for the device to see what you can use.
    // i2c.speed = I2CHandle::Config::Speed::I2C_100KHZ;
    // i2c.speed = I2CHandle::Config::Speed::I2C_400KHZ;
    i2c.speed = I2CHandle::Config::Speed::I2C_1MHZ;

    // wait until the hardware is ready to listen
    System::Delay(50);
    // init the device
    display.Init(display_config);
  }

  uint16_t Width() const { return display.Width(); }
  uint16_t Height() const { return display.Height(); }
};
