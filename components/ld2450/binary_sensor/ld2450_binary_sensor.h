#pragma once

#include "../ld2450.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome::ld2450
{

class LD2450BinarySensor : public LD2450Listener, public Component, binary_sensor::BinarySensor
{
public:
    void dump_config() override;
    void set_presence_sensor(binary_sensor::BinarySensor *bsensor) { this->presence_bsensor_ = bsensor; };
    void on_presence(bool presence) override
    {
        if (this->presence_bsensor_ != nullptr)
        {
            if (this->presence_bsensor_->state != presence)
            this->presence_bsensor_->publish_state(presence);
        }
    }

protected:
    binary_sensor::BinarySensor *presence_bsensor_{nullptr};
};

}  // namespace esphome::ld2450
