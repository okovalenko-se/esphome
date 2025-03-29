import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID, CONF_NUM_ATTEMPTS

DEPENDENCIES = ["i2c"]
MULTI_CONF = True

evse_ns = cg.esphome_ns.namespace("evse")
EVSEComponent = evse_ns.class_("EVSEComponent", cg.PollingComponent, i2c.I2CDevice)

CONF_EVSE_ID = "evse_id"

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(EVSEComponent),
            cv.Optional(CONF_NUM_ATTEMPTS, default=5): cv.All(
                cv.positive_not_null_int,
                cv.Range(min=1, max=100),
            ),
        }
    )
    .extend(cv.polling_component_schema("5s"))
    .extend(i2c.i2c_device_schema(0x5E))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    cg.add(var.set_num_attempts(config[CONF_NUM_ATTEMPTS]))
