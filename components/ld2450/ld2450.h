#pragma once

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"


namespace esphome::ld2450
{

enum BaudRate
{
    9600   = 0x01,
    19200  = 0x02,
    38400  = 0x03,
    57600  = 0x04,
    115200 = 0x05,
    230400 = 0x06,
    256000 = 0x07,
    460800 = 0x08,
};

class LD2450 : public uart::UARTDevice, public Component
{
    
public:
    void setup() override;
    void loop() override;


private:
    // LD2450 specific
    void send_cmd(uint8_t *cmd, const uint8_t *cmd_value = nullptr);
    void set_config_mode(bool enable);
    void set_baud_rate(BaudRate baud_rate);
    void set_multi_target_tracking(bool enable);
    void set_bluetooth(bool enable);
    void get_tracking_mode();
    void get_firmware_version();
    void get_bluetooth_mac();
    void sensor_reboot();
    void sensor_factory_reset();
    // TODO zone filtering and setting

    // Component
    void get_sensor_infos();


private:
    // Debug
    unsigned long last_time = 0;

    // LD2450 specific
    bool sensor_connected = false;

    uint16_t uart_timeout = 1000;

    uint8_t firmware_version_ = 123;


};


} // namespace esphome::ld2450
