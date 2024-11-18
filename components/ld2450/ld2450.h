#pragma once

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"


namespace esphome::ld2450
{

enum BaudRate
{
    BAUD_9600   = 0x01,
    BAUD_19200  = 0x02,
    BAUD_38400  = 0x03,
    BAUD_57600  = 0x04,
    BAUD_115200 = 0x05,
    BAUD_230400 = 0x06,
    BAUD_256000 = 0x07,
    BAUD_460800 = 0x08,
};

class LD2450 : public uart::UARTDevice, public Component
{
    
public:
    void setup() override;
    void loop() override;


private:
    // Debug
    void print_uart(bool data_send, const std::vector<uint8_t> &data);

    // LD2450 specific
    void send_cmd(uint8_t *cmd, const uint8_t *cmd_value = nullptr);
    bool get_ack();

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

    uint16_t uart_timeout = 500;

    uint8_t firmware_version_ = 123;


};


} // namespace esphome::ld2450
