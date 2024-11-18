import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import (
    uart,
)
from esphome.const import (
    CONF_ID,
)

# ld2450
CONF_LD2450_ID = "ld2450_id"

MULTI_CONF = True
DEPENDENCIES = ['uart']


ld2450_ns = cg.esphome_ns.namespace("ld2450")
LD2450 = ld2450_ns.class_("LD2450", cg.Component, uart.UARTDevice)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(LD2450),
        # cv.Required(UART_ID): cv.use_id(UARTComponent),
        # cv.Optional(CONF_NAME, default="LD2450"): cv.string_strict,
    }
).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)


