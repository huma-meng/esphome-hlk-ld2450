#pragma once

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
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
#include "number/entry_point.h"
#include "number/presence_region.h"
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

    class LD2450 : public uart::UARTDevice, public Component
    {
    #ifdef USE_BUTTON
        SUB_BUTTON(sensor_reboot)
        SUB_BUTTON(sensor_factory_reset)
    #endif

    #ifdef USE_SWITCH
        SUB_SWITCH(multi_target_mode)
        SUB_SWITCH(bluetooth)
    #endif

    #ifdef USE_TEXT_SENSOR
        SUB_TEXT_SENSOR(firmware_version)
        SUB_TEXT_SENSOR(bluetooth_mac)
    #endif


    public:
        // General
        void setup() override;
        void loop() override;
        void update() override;
        void dump_config() override;

        // Sensor config
        bool sensor_online();



        void set_baud_rate();
        void set_tracking_mode();







        void set_flip_axis_x(bool flip);
        void set_flip_axis_y(bool flip);

        // Can be done with a room layout + margin for the room layout
        float set_tilt_angle_min(float angle);
        float set_tilt_angle_max(float angle);
        void set_tilt_angle_hysteresis(float angle);
        float set_distance_max(float distance);
        float set_distance_min(float distance);
        void set_distance_hysteresis(float distance);

        void set_fast_off_detection(bool fast_off_detection);

        bool get_room_presence();

        bool get_sensor_available();





    private:
        enum UART_Constant
        {
            CONFIG_ENABLE = 0xFF,
            CONFIG_DISABLE = 0xFE,
            FH_LAST = 0xFA,
            FE_LAST = 0x01,
            DE_LAST = 0xCC
        };

        uint8_t FRAME_HEADER[4] = { 0xFD, 0xFC, 0xFB, FH_LAST };
        uint8_t FRAME_END[4] = { 0x04, 0x03, 0x02, FE_LAST };
        uint8_t DATA_HEADER[4] = { 0xAA, 0xFF, 0x03, 0x00 };
        uint8_t DATA_END[2] = { 0x55, DE_LAST };

        void send_cmd(uint8_t cmd, const uint8_t cmd_value, int cmd_value_length);
        void set_config_mode(bool enable);
        void set_bluetooth(bool enable);

        void get_sensor_infos();
        void get_firmware_version();
        void get_bluetooth_mac();

        void sensor_reboot();
        void sensor_factory_reset();



    protected:


        const char *name = "LD2450";
        
        uint8_t sensor_state_ = 0;

        bool flip_axis_x_ = false;
        bool flip_axis_y_ = false;

        float tilt_angle_min_ = 60;
        float tilt_angle_max_ = 60;
        float tilt_angle_hysteresis_ = 5;
        float distance_max_ = 6000;
        float distance_min_ = 0;
        float distance_hysteresis_ = 250;

        bool sensor_available_ = false;
        bool sensor_apply_changes_ = false;

        bool config_mode_ = false;
        int config_message_length_ = 0;

        bool fast_off_detection_ = false;

        bool room_presence_ = false;
    }



} // namespace esphome::ld2450
