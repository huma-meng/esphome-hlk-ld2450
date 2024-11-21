#include "ld2450_button.h"


namespace esphome::ld2450
{

void SensorRestartButton::press_action() { this->parent_->sensor_reboot(); };
void SensorFactoryResetButton::press_action() { this->parent_->sensor_factory_reset(); };

} // namespace esphome::ld2420
