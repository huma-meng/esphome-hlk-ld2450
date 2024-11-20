#include "ld2450.h"

namespace esphome::ld2450
{

    LD2450::LD2450() :
    target(targets_max)
    {

    }


    void LD2450::setup()
    {
        this->set_timeout(1000, [this]() { this->sensor_reboot(); });
    }


    // TODO Check if sensor is online
    void LD2450::loop()
    {
        // Receive UART frame
        if (this->available()) 
        {
            std::vector<uint8_t> uart_buffer;
            while (this->available()) 
            {
                uint8_t byte = this->read();
                uart_buffer.push_back(byte);
            }
            this->uart_receive(uart_buffer);
        }


        /*
        if (millis() - last_time >= 10000)
        {
            if (sensor_config_mode == false)
            {
                this->set_config_mode(true);
            }
            else if (sensor_config_mode == true)
            {
                this->set_config_mode(false);
            }
            
            last_time = millis();
        }
        */


        // process_data();
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
        this->uart_print(true, uart_buffer);
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
            // Check config mode
            constexpr uint8_t config_mode_enable[2] = { 0xFF, 0x00 };
            constexpr uint8_t config_mode_disable[2] = { 0xFE, 0x00 };

            switch (frame[6])
            {
            // Config mode enable
            case config_mode_enable[0]:
                if (frame[7] == 0x00)
                {
                    sensor_config_mode = false;
                    ESP_LOGD("LD2450", "Config mode: ENABLE FAILED");
                }
                else if (frame[7] == 0x01)
                {
                    sensor_config_mode = true;
                    ESP_LOGD("LD2450", "Config mode: ENABLE SUCCESS");
                }
                break;

            // Config mode disable
            case config_mode_disable[0]:
                if (frame[7] == 0x00)
                {
                    sensor_config_mode = true;
                    ESP_LOGD("LD2450", "Config mode: DISABLE FAILED");
                }
                else if (frame[7] == 0x01)
                {
                    sensor_config_mode = false;
                    ESP_LOGD("LD2450", "Config mode: DISABLE SUCCESS");
                }
            default:
                break;
            }

            this->uart_print(false, frame);
        }

        // Data frame
        // X to the front, Y left/right (left = negative)
        else if (frame[0] == header_data[0])
        {
            // Process the target data
            // for (int i = 0; i < targets_max; ++i) {
            for (int i = 0; i < 1; ++i) {
                int offset = i * 8;

                if (frame[offset + 4] == 0x00 && frame[offset + 5] == 0x00 && frame[offset + 6] == 0x00 && frame[offset + 7] == 0x00) {
                    target[i].x = 0;
                    target[i].y = 0;
                    target[i].speed = 0;
                    target[i].resolution = 0;
                } else {
                    target[i].x = static_cast<int16_t>((frame[offset + 5] << 8) | frame[offset + 4]) - ((frame[offset + 5] & 0x80) ? 0x8000 : 0);
                    target[i].y = static_cast<int16_t>((frame[offset + 7] << 8) | frame[offset + 6]) - (frame[offset + 7] & 0x80 ? 0x8000 : 0);
                    // target[i].speed = static_cast<int16_t>((frame[offset + 9] << 8) | frame[offset + 8]) - ((frame[offset + 9] & 0x80) ? 0x8000 : 0);
                    target[i].speed = static_cast<int16_t>((frame[offset + 9] << 8 | frame[offset + 8]) & 0x7FFF) * (frame[offset + 9] & 0x80 ? 1 : -1);
                    target[i].resolution = static_cast<uint16_t>((frame[offset + 11] << 8) | frame[offset + 10]);

                    // target[i].x = 0 -(frame[offset + 4] + frame[offset + 5] * 256);
                    // target[i].x = static_cast<int16_t>((frame[offset + 5] << 8) | frame[offset + 4]);

                    // target[i].y = (frame[offset + 6] + frame[offset + 7] * 256) - 32768;
                    // target[i].y = static_cast<int16_t>((frame[offset + 7] << 8) | frame[offset + 6]) - (frame[offset + 7] & 0x80 ? 0x8000 : 0);

                    // target[i].speed = 0 -(frame[offset + 8] + frame[offset + 9] * 256);
                    // target[i].speed = (uint16_t)(frame[offset + 9] << 8) + frame[offset + 8];
                    // target[i].speed = static_cast<int16_t>((frame[offset + 9] << 8) | frame[offset + 8]);

                    // target[i].resolution = static_cast<uint16_t>((frame[offset + 11] << 8) | frame[offset + 10]);


                    // DEBUG
                    target[i + 1].x = frame[offset + 5] << 8 | frame[offset + 4];
                    if (frame[offset + 5] & 0x80)
                        target[i + 1].x = -target[i + 1].x + 0x8000;
                    
                    target[i + 1].y = (frame[offset + 7] << 8 | frame[offset + 6]);
                    if (target[i + 1].y != 0)
                        target[i + 1].y -= 0x8000;
                    
                    target[i + 1].speed = frame[offset + 9] << 8 | frame[offset + 8];
                    if (frame[offset + 9] & 0x80)
                        target[i + 1].speed = -target[i + 1].speed + 0x8000;
                    
                    target[i + 1].resolution = frame[offset + 11]  << 8 | frame[offset + 10] ;
                }
            }

            // --- UART LOG OUTPUT -------------------------------------------------------------------------------------------------------------------
            if (uart_log_output == true)
            {
                ESP_LOGD("LD2450", "T0: X %d Y %d S %d R %d --- T1: X %d Y %d S %d R %d --- T2: X %d Y %d S %d R %d",
                target[0].x, target[0].y, target[0].speed, target[0].resolution,
                target[1].x, target[1].y, target[1].speed, target[1].resolution,
                target[2].x, target[2].y, target[2].speed, target[2].resolution); 

                this->uart_print(false, frame);
            }
            // ---------------------------------------------------------------------------------------------------------------------------------------
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



    void LD2450::process_data()
    {
        // Precence variables
        if (target[0].x == 0 && target[0].y == 0)
        {
            sensor_presence = false;
            sensor_targets = 0;
        }
        else
        {
            sensor_presence = true;
            sensor_targets = 1;
            if (target[1].x != 0 && target[1].y != 0)
            {
                sensor_targets += sensor_targets;
            }
            if (target[2].x != 0 && target[2].y != 0)
            {
                sensor_targets += sensor_targets;
            }           
        }
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
