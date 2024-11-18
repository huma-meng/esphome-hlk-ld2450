#include "ld2450.h"

namespace esphome::ld2450
{
    void LD2450::setup()
    {
        #ifdef USE_BUTTON
            if (btn_reboot_ != nullptr)
            {
                btn_reboot_->add_on_press_callback([this]()
                {
                    this->sensor_reboot();
                });
            }
            if (btn_factory_reset_ != nullptr)
            {
                btn_factory_reset_-> add_on_press_callback([this]()
                {
                    this->sensor_factory_reset();
                });
            }
        #endif
     

        this->set_timeout(1000, [this]()
        {
            this->get_sensor_infos();
        });
    }



    void LD2450::send_cmd(uint8_t cmd, uint8_t cmd_value, int cmd_value_length)
    {
        uint8_t frame_header[4] = { 0xFD, 0xFC, 0xFB, 0xFA };
        this->write_array(frame_header, 4);
        int len = 2;
        if (cmd_value != nullptr)
        {
            for (int i = 0; i < cmd_value_length; i++)
            {
                this->write_byte(cmd_value[i]);
            }
        }
        uint8_t frame_end[4] = { 0x04, 0x03, 0x02, 0x01 };
        this->write_array(frame_end, 4);
        flush();
    }

    void LD2450::set_config_mode(bool enable)
    {
        uint8_t cmd[2] = { enable ? 0xFF : 0xFE, 0x00 } ;
        uint8_t cmd_value[2] = { 0x01, 0x00 };
        this->send_command(cmd, enable ? cmd_value : nullptr, 2);
    }

    void LD2450::set_bluetooth(bool enable)
    {
        this->set_config_mode(true);
        uint8_t cmd = 0xA4;
        uint8_t cmd_value[2] = { enable ? 0x01 : 0x00, 0x00 };
        this->send_cmd(cmd, cmd_value, 2);
        this->set_config_mode(false);
    }

    void LD2450::get_sensor_infos()
    {
        this->set_config_mode(true);
        this->get_firmware_version();
        this->get_bluetooth_mac();
        this->set_config_mode(false);
    }

    void LD2450::get_firmware_version()
    {
        uint8_t cmd = 0xA0;
        this->send_cmd(cmd, nullptr, 0);
    }

    void LD2450::get_bluetooth_mac()
    {
        uint8_t cmd = 0xA5;
        uint8_t cmd_value[2] = { 0x01, 0x00 };
        this->send_cmd(cmd, cmd_value, 0);
    }

    void LD2450::sensor_reboot()
    {
        uint8_t cmd = 0xA3;
        this->send_cmd(cmd, nullptr, 0);
    }

    void LD2450::sensor_factory_reset()
    {
        uint8_t cmd = 0xA2;
        this->send_cmd(cmd, nullptr, 0);
        this->sensor_reboot();
    }


} // namespace esphome::ld2450
