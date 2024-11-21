#pragma once

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"


#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif

#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif

#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif


namespace esphome::ld2450
{

// TODO command sets
// TODO command value sets

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

#ifdef USE_BINARY_SENSOR
    SUB_BINARY_SENSOR(sensor_presence)
    SUB_BINARY_SENSOR(room_presence)
    SUB_BINARY_SENSOR(zone_0_presence)
#endif

#ifdef USE_BUTTON
    SUB_BUTTON(sensor_reboot)
    SUB_BUTTON(sensor_factory_reset)
#endif

#ifdef USE_NUMBER
    // sensor mounted rotation x / y / z
#endif

#ifdef USE_SELECT
    SUB_SELECT(baud_rate)
#endif

#ifdef USE_SENSOR
    SUB_SENSOR(sensor_target_count)
    SUB_SENSOR(room_target_count)
    SUB_SENSOR(zone_0_target_count)
#endif

#ifdef USE_SWITCH
    SUB_SWITCH(multi_target_tracking)
    SUB_SWITCH(bluetooth)
#endif

#ifdef USE_TEXT_SENSOR
    SUB_TEXT_SENSOR(firmware_version)
    SUB_TEXT_SENSOR(bluetooth_mac)
#endif

    
public:
    LD2450();

    void setup() override;
    void dump_config() override;
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
    std::vector<Target> targets;
    uint8_t targets_max_count = 3;

    std::vector<int16_t>
    uint8_t targets_smooting_window = 3;



    bool sensor_connected = false;
    bool sensor_config_mode = false;

    uint16_t uart_timeout = 500;

    uint8_t firmware_version_ = 123;




    // AddOn specific
    


    // Outputs
    bool uart_log_output = true;

    // Sensor data
    bool sensor_presence = false;
    uint8_t sensor_target_count = 0;

    // Room data
    bool room_presence = false;
    uint8_t room_target_count = 0;

    // Zone data
    // TODO dynamic zones
    bool zone_0_presence = false;
    uint8_t zone_0_targets = 0;
    uint16_t zone_0_active_delay = 0;   // Time delay for activate zone
    uint16_t zone_0_deactive_delay = 0; // Time delay for deactivate zone


    // Debug
    unsigned long last_time = 0;


};


} // namespace esphome::ld2450
