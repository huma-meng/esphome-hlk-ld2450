import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, DEVICE_CLASS_OCCUPANCY, CONF_HAS_TARGET
from .. import ld2450_ns, LD2450Component, CONF_LD2450_ID

LD2420BinarySensor = ld2450_ns.class_(
    "LD2450BinarySensor", binary_sensor.BinarySensor, cg.Component
)


CONFIG_SCHEMA = cv.All(
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(LD2450BinarySensor),
            cv.GenerateID(CONF_LD2450_ID): cv.use_id(LD2450Component),
            cv.Optional(CONF_HAS_TARGET): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_OCCUPANCY
            ),
        }
    ),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if CONF_HAS_TARGET in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_HAS_TARGET])
        cg.add(var.set_presence_sensor(sens))
    ld2450 = await cg.get_variable(config[CONF_LD2450_ID])
    cg.add(ld2450.register_listener(var))