import esphome.codegen as cg
import esphome.config_validation as cv

from esphome import automation
from esphome.components import uart
from esphome.components.uart import UARTComponent
from esphome.const import CONF_ID


DEPENDENCIES = ["uart"]


ld2450_ns = cg.esphome_ns.namespace("ld2450")
LD2450 = ld2450_ns.class_("LD2450", cg.Component, uart.UARTDevice)


CONFIG_SCHEMA = cv.All(
    uart.UART_DEVICE_SCHEMA.extend(
        {

        }
    )
)


def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])

    yield cg.register_component(var, config)
    yield uart.register_uart_device(var, config)