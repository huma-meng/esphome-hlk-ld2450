#include "ld2450.h"

namespace esphome::ld2450
{

    void LD2450::setup()
    {
        // this->set_config_mode(true);
    }



    void LD2450::send_cmd(uint8_t *cmd, const uint8_t *cmd_value, int cmd_value_length)
    {

        // Frame header
        uint8_t frame_header[4] = { 0xFD, 0xFC, 0xFB, 0xFA };
        this->write_array(frame_header, 4);

        // In-Frame data length
        int length = 2;
        if (cmd_value != nullptr)
        {
            length += cmd_value_length;
        }
        uint8_t frame_data_length[2] = { lowbyte(length), highbyte(length) };
        this->write_array(frame_data_length, 2);

        // In-Frame data
        if (cmd_value != nullptr)
        {
            for (int i = 0; i < cmd_value_length; i++)
            {
                write_byte(cmd_value[i]);
            }
        }

        // Frame end
        uint8_t frame_end[4] = { 0x04, 0x03, 0x02, 0x01 };
        this->write_array(frame_end, 4);
        
        this->flush();

        /*
        // UART frame data
        std::vector<uint8_t> frame;

        // Header
        frame.insert(frame.end(), { 0xFD, 0xFC, 0xFB, 0xFA });

        // Data length
        int lenght = 2 + (cmd_value != nullptr ? cmd_value_length : 0);
        frame.push_back(length & 0xFF);         // Low byte
        frame.push_back((lenght >> 8) & 0xFF);  // High byte

        // Command
        frame.push_back(*cmd);

        // Command value
        if (cmd_value != nullptr)
        {
            frame.insert(frame.end(), cmd_value, cmd_value_length);
        }
        
        // End
        frame.insert(frame.end(), { 0x04, 0x03, 0x02, 0x01 });
        */

        /*
        // ----- DEBUGGING: UART data send debug ----------------------------------------------
        ESP_LOGD("LD2450", "Sent UART data: ");
        for (size_t i = 0; i < uart_data_lenght; i++)
        {
            ESP_LOGD("LD2450", "0x%02X ", data[i]);
        }
        ESP_LOGD("LD2450", "(%d bytes)", uart_data_lenght);
        // ------------------------------------------------------------------------------------
        */
    }


    void LD2450::set_config_mode(bool enable)
    {
        // uint8_t cmd[2] = { enable ? 0xFF : 0xFE, 0x00 };
        uint8_t cmd[2] = { 0xFF, 0x00 };
        uint8_t cmd_value[2] = { 0x01, 0x00 };
        this->send_cmd(cmd, enable ? cmd_value : nullptr, enable ? 2 : 0);       
    }


} // namespace esphome::ld2450
