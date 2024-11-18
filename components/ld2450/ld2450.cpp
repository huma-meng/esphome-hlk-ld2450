#include "ld2450.h"

namespace esphome::ld2450
{

    void LD2450::setup()
    {
        this->get_sensor_infos();
    }

    void LD2450::loop()
    {

    }



    void LD2450::send_cmd(uint8_t *cmd, const uint8_t *cmd_value = nullptr)
    {
        // UART buffer
        std::vector<uint8_t> uart_buffer;

        // Header
        const uint8_t frame_header[4] = {0xFD, 0xFC, 0xFB, 0xFA};
        uart_buffer.insert(uart_buffer.end(), std::begin(frame_header), std::end(frame_header));

        // Command length (2 bytes for command + 2 bytes for command value if present)
        uint16_t cmd_length = 2 + (cmd_value ? 2 : 0);
        uart_buffer.push_back(static_cast<uint8_t>(cmd_length & 0xFF));         // LSB
        uart_buffer.push_back(static_cast<uint8_t>((cmd_length >> 8) & 0xFF));  // MSB

        // Command
        uart_buffer.push_back(cmd[0]);
        uart_buffer.push_back(cmd[1]);

        // Command value (if present)
        if (cmd_value)
        {
            uart_buffer.push_back(cmd_value[0]);
            uart_buffer.push_back(cmd_value[1]);
        }

        // End
        const uint8_t frame_end[4] = {0x04, 0x03, 0x02, 0x01};
        uart_buffer.insert(uart_buffer.end(), std::begin(frame_end), std::end(frame_end));

        // Send UART frame byte-by-byte
        for (size_t i = 0; i < uart_buffer.size(); ++i)
        {
            this->write(uart_buffer[i]);
        }
        this->flush();


        // ----- DEBUG: UART data send debug --------------------------------------------------
        std::string uart_frame_str;
        for (size_t i = 0; i < uart_buffer.size(); ++i)
        {
            char byte_str[5];  // "0x" + 2 hex digits + null terminator
            snprintf(byte_str, sizeof(byte_str), "0x%02X", uart_buffer[i]);
            uart_frame_str += byte_str;
            if (i < uart_buffer.size() - 1)
            {
                uart_frame_str += " ";
            }
        }
        ESP_LOGD("LD2450", "Sending UART frame: %s", uart_frame_str.c_str());
        // ------------------------------------------------------------------------------------
    }


    void LD2450::set_config_mode(bool enable)
    {
        uint8_t cmd[2] = { static_cast<uint8_t>(enable ? 0xFF : 0xFE), 0x00 };
        uint8_t cmd_value[2] = { 0x01, 0x00 };
        this->send_cmd(cmd, enable ? cmd_value : nullptr);       
    }

    void LD2450::set_baud_rate(uint16_t baud_rate)
    {
        uint8_t cmd[2] = { 0xA1, 0x00 };
        uint8_t cmd_value[2] = { 0x07, 0x00 };  // Baud rate = 256000
        this->send_cmd(cmd, cmd_value);
    }

    void LD2450::set_multi_target_tracking(bool enable)
    {
        uint8_t cmd[2] = { static_cast<uint8_t>(enable ? 0x90 : 0x80), 0x00 };
        this->send_cmd(cmd);
    }

    void LD2450::set_bluetooth(bool enable)
    {
        uint8_t cmd[2] = { 0xA4, 0x00 };
        uint8_t cmd_value[2] = { static_cast<uint8_t>(enable ? 0x01 : 0x00), 0x00 };
        this->send_cmd(cmd, cmd_value);
    }



    void LD2450::get_tracking_mode()
    {
        uint8_t cmd[2] = { 0x91, 0x00 };
        this->send_cmd(cmd);
        // TODO read back the ACK
    }

    void LD2450::get_firmware_version()
    {
        uint8_t cmd[2] = { 0xA0, 0x00 };
        this->send_cmd(cmd);
        // TODO read back the ACK
    }

    void LD2450::get_bluetooth_mac()
    {
        uint8_t cmd[2] = { 0xA5, 0x00 };
        uint8_t cmd_value[2] = { 0x01, 0x00 };
        this->send_cmd(cmd, cmd_value);
        // TODO read back the ACK
    }



    void LD2450::sensor_reboot()
    {
        uint8_t cmd[2] = { 0xA3, 0x00 };
        this->send_cmd(cmd);
    }

    void LD2450::sensor_factory_reset()
    {
        uint8_t cmd[2] = { 0xA2, 0x00 };
        this->send_cmd(cmd);
        // TODO check ack then reboot
        this->sensor_reboot();
    }






    void LD2450::get_sensor_infos()
    {
        this->get_tracking_mode();
        this->get_firmware_version();
        this->get_bluetooth_mac();
    }

} // namespace esphome::ld2450
