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

struct Target
{
    int16_t x;
    int16_t y;
    int speed;
    uint16_t resolution;
};


// TODO room
// n points / hysteresis / room rounter


// TODO zones
// n point / hysteresis / zone counter


class LD2450 : public uart::UARTDevice, public Component
{
    
public:
    LD2450();

    void setup() override;
    void loop() override;


private:
    // LD2450 specific
    void uart_send(uint8_t *cmd, const uint8_t *cmd_value = nullptr);
    void uart_receive(const std::vector<uint8_t> &frame);

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
    void process_data();

    void get_sensor_infos();


    // Debug
    void uart_print(bool data_send, const std::vector<uint8_t> &data);

private:
    // LD2450 specific
    uint8_t targets_max = 3;
    bool sensor_connected = false;
    bool sensor_config_mode = false;

    uint16_t uart_timeout = 500;

    uint8_t firmware_version_ = 123;

    std::vector<Target> target;


    // Outputs
    bool uart_log_output = true;

    bool sensor_presence = false;
    uint8_t sensor_targets = 0;

    bool room_presence = false;
    uint8_t room_targets = 0;

    // TODO dynamic zones
    uint8_t zone_0_targets = 0;


    // Debug
    unsigned long last_time = 0;


};


} // namespace esphome::ld2450
