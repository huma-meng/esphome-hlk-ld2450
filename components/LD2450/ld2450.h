#pragma once

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"


namespace esphome::ld2450
{

class LD2450 : public uart::UARTDevice, public Component
{
public:

    void setup() override;

private:

    void send_cmd(uint8_t *cmd, const uint8_t *cmd_value, int cmd_value_length);  

    void set_config_mode(bool enable);  


private:
    bool cmd_last_success_ = false;



}


} // namespace esphome::ld2450
