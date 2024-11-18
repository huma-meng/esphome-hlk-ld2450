#pragma once

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"


namespace esphome::ld2450
{

class LD2450 : public uart::UARTDevice, public Component
{
    
public:
    void setup() override;
    void loop() override;


private:
    // LD2450 specific
    void send_cmd(uint8_t *cmd, const uint8_t *cmd_value);
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
    // LD2450 specific
    bool sensor_connected = false;

    uint16_t uart_timeout = 1000;

    uint8_t firmware_version_ = "";


};


} // namespace esphome::ld2450
