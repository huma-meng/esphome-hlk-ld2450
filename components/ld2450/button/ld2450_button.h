#pragma once

#include "../ld2450.h"
#include "esphome/components/button/button.h"


namespace esphome::ld2450
{

class SensorRestartButton : public button::Button, public Parented<LD2450>
{
public:
    SensorRestartButton() = default;

protected:
    void press_action() override;
};

class SensorFactoryResetButton : public button::Button, public Parented<LD2450>
{
public:
    SensorFactoryResetButton() = default;

protected:
    void press_action() override;
};

}  // namespace esphome::ld2420
