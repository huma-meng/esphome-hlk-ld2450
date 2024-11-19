#include "ld2450.h"

namespace esphome::ld2450
{

    void LD2450::setup()
    {
        this->set_timeout(1000, [this]() { this->sensor_reboot(); });
    }


    // TODO Check if sensor is online
    void LD2450::loop()
    {
        if (millis() - last_time >= 10000)
        {
            this->set_config_mode(true);
            last_time = millis();
        }

        if (this->available()) 
        {
            std::vector<uint8_t> uart_buffer;
            while (this->available()) 
            {
                uint8_t byte = this->read();
                uart_buffer.push_back(byte);
            }
            this->uart_receive(uart_buffer);

            // --- DEBUG ----------------------------------------------------------------------------
            // this->print_uart(false, uart_buffer);
            // --------------------------------------------------------------------------------------
        }
    }


    void LD2450::uart_send(uint8_t *cmd, const uint8_t *cmd_value)
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

        // --- DEBUG ----------------------------------------------------------------------------
        // this->print_uart(true, uart_buffer);
        // --------------------------------------------------------------------------------------
    }




    // TODO Check if sensor is online
    void LD2450::uart_receive(const std::vector<uint8_t> &frame)
    {
        // Check frame header (ack or data)
        const uint8_t header_ack[4] = { 0xFD, 0xFC, 0xFB, 0xFA };
        const uint8_t header_data[4] = { 0xAA, 0xFF, 0x03, 0x00 };

        // ACK frame
        if (frame[0] == header_ack[0])
        {
            ESP_LOGD("LD2450", "Received --ACK-- frame");

            // Check config mode
            constexpr uint8_t config_mode_enable[2] = { 0xFF, 0x00 };
            constexpr uint8_t config_mode_disable[2] = { 0xFE, 0x00 };

            switch (frame[6])
            {
            // Config mode enable
            case config_mode_enable[0]:
                if (frame[7] == 0x00)
                {
                    ESP_LOGD("LD2450", "Config mode: ENABLE SUCCESS");
                }
                else if (frame[7] == 0x01)
                {
                    ESP_LOGD("LD2450", "Config mode: ENABLE FAILED");
                }
                break;

            // Config mode disable
            case config_mode_disable[0]:
                if (frame[7] == 0x00)
                {
                    ESP_LOGD("LD2450", "Config mode: DISABLE SUCCESS");
                }
                else if (frame[7] == 0x01)
                {
                    ESP_LOGD("LD2450", "Config mode: DISABLE FAILED");
                }
            default:
                break;
            }

            this->uart_print(false, frame);
        }

        // Data frame
        else if (frame[0] == header_data[0])
        {
            ESP_LOGD("LD2450", "Received --DATA-- frame");
        }
    }




    void LD2450::set_config_mode(bool enable)
    {
        uint8_t cmd[2] = { static_cast<uint8_t>(enable ? 0xFF : 0xFE), 0x00 };
        uint8_t cmd_value[2] = { 0x01, 0x00 };
        this->uart_send(cmd, enable ? cmd_value : nullptr);       
    }

    // TODO Select baud rate 
    void LD2450::set_baud_rate(BaudRate baud_rate)
    {
        uint8_t cmd[2] = { 0xA1, 0x00 };
        uint8_t cmd_value[2] = { 0x07, 0x00 };  // Baud rate = 256000
        this->uart_send(cmd, cmd_value);
    }

    void LD2450::set_multi_target_tracking(bool enable)
    {
        uint8_t cmd[2] = { static_cast<uint8_t>(enable ? 0x90 : 0x80), 0x00 };
        this->uart_send(cmd);
    }

    void LD2450::set_bluetooth(bool enable)
    {
        uint8_t cmd[2] = { 0xA4, 0x00 };
        uint8_t cmd_value[2] = { static_cast<uint8_t>(enable ? 0x01 : 0x00), 0x00 };
        this->uart_send(cmd, cmd_value);
    }



    void LD2450::get_tracking_mode()
    {
        uint8_t cmd[2] = { 0x91, 0x00 };
        this->uart_send(cmd);
    }

    void LD2450::get_firmware_version()
    {
        uint8_t cmd[2] = { 0xA0, 0x00 };
        this->uart_send(cmd);
    }

    void LD2450::get_bluetooth_mac()
    {
        uint8_t cmd[2] = { 0xA5, 0x00 };
        uint8_t cmd_value[2] = { 0x01, 0x00 };
        this->uart_send(cmd, cmd_value);
    }



    void LD2450::sensor_reboot()
    {
        uint8_t cmd[2] = { 0xA3, 0x00 };
        this->uart_send(cmd);
    }

    void LD2450::sensor_factory_reset()
    {
        uint8_t cmd[2] = { 0xA2, 0x00 };
        this->uart_send(cmd);
        this->sensor_reboot();
    }



    void LD2450::get_sensor_infos()
    {
        this->set_config_mode(true);
        this->get_tracking_mode();
        this->get_firmware_version();
        this->get_bluetooth_mac();
        this->set_config_mode(false);
    }







    // ----- DEBUG: UART data send debug --------------------------------------------------
    void LD2450::uart_print(bool data_send, const std::vector<uint8_t> &uart_data)
    {
        std::string uart_data_str;
        for (size_t i = 0; i < uart_data.size(); ++i)
        {
            char byte_str[5];  // "0x" + 2 hex digits + null terminator
            snprintf(byte_str, sizeof(byte_str), "0x%02X", uart_data[i]);
            uart_data_str += byte_str;
            if (i < uart_data.size() - 1)
            {
                uart_data_str += " ";
            }
        }
        if (data_send)
        {
            ESP_LOGD("LD2450", "Sending UART frame: %s", uart_data_str.c_str());
        }
        else
        {
            ESP_LOGD("LD2450", "Receiving UART frame: %s", uart_data_str.c_str());
        }
    }
    // ------------------------------------------------------------------------------------


} // namespace esphome::ld2450
