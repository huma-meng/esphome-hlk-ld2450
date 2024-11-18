#include "ld2450.h"

namespace esphome::ld2450
{

    void LD2450::setup()
    {
        this->sensor_reboot();
    }

    void LD2450::loop()
    {
        if (millis() - last_time >= 10000)
        {
            // this->get_sensor_infos();

            // this->set_timeout(1000, [this]() { this->set_config_mode(true); });
            this->set_config_mode(true);

            last_time = millis();
        }


        if (this->available()) 
        {
            std::vector<uint8_t> uart_buffer;
            
            // Solange Daten empfangen werden
            while (this->available()) 
            {
                uint8_t byte = this->read();  // Lese Byte aus UART
                uart_buffer.push_back(byte);  // Füge Byte zum Buffer hinzu
            }

            // Debug-Ausgabe der empfangenen UART-Daten
            // ESP_LOGD("LD2450", "Received UART frame: ");
            this->print_uart(false, uart_buffer);  // Ausgabe des gesamten Buffers
        }
        

    }


    // ----- DEBUG: UART data send debug --------------------------------------------------
    void LD2450::print_uart(bool data_send, const std::vector<uint8_t> &uart_data)
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



    void LD2450::send_cmd(uint8_t *cmd, const uint8_t *cmd_value)
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

        // --- DEBUG -----------------------
        this->print_uart(true, uart_buffer);
        // ---------------------------------
    }

    /*
    // TODO Expected response for true/false readback
    bool LD2450::get_ack()
    {
        unsigned long start_time = millis();
        std::vector<uint8_t> uart_buffer;

        while (millis() - start_time < uart_timeout)
        {
            if (this->available()) 
            {
                uint8_t byte = this->read();
                uart_buffer.push_back(byte);
                
                // --- Debug: Ausgabe des empfangenen Bytes ---
                ESP_LOGD("LD2450", "Received byte: 0x%02X", byte);
                
                // --- Debug: UART-Daten als Puffer anzeigen ---
                this->print_uart(false, uart_buffer);
            }
            else {
                // --- Debug: Keine Daten verfügbar ---
                ESP_LOGD("LD2450", "No data available in UART buffer.");
            }

            // Timeout erreichen
            if (millis() - start_time >= uart_timeout) {
                ESP_LOGW("LD2450", "Sensor timeout... No response received.");
                break;
            }
        }

        // Überprüfen, ob Daten empfangen wurden
        if (uart_buffer.empty()) {
            ESP_LOGW("LD2450", "No UART data received during timeout.");
            return false;
        } else {
            ESP_LOGD("LD2450", "Final received UART data:");
            this->print_uart(false, uart_buffer);
            return true;
        }
    }
    */



    void LD2450::set_config_mode(bool enable)
    {
        uint8_t cmd[2] = { static_cast<uint8_t>(enable ? 0xFF : 0xFE), 0x00 };
        uint8_t cmd_value[2] = { 0x01, 0x00 };
        this->send_cmd(cmd, enable ? cmd_value : nullptr);       
    }


    void LD2450::set_baud_rate(BaudRate baud_rate)
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
        this->get_ack();
    }

    void LD2450::get_firmware_version()
    {
        uint8_t cmd[2] = { 0xA0, 0x00 };
        this->send_cmd(cmd);
        this->get_ack();
    }

    void LD2450::get_bluetooth_mac()
    {
        uint8_t cmd[2] = { 0xA5, 0x00 };
        uint8_t cmd_value[2] = { 0x01, 0x00 };
        this->set_config_mode(true);
        this->send_cmd(cmd, cmd_value);
        this->get_ack();
        this->set_config_mode(false);
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
        this->get_ack();
        this->sensor_reboot();
    }






    void LD2450::get_sensor_infos()
    {
        this->get_tracking_mode();
        this->get_firmware_version();
        this->get_bluetooth_mac();
    }

} // namespace esphome::ld2450
