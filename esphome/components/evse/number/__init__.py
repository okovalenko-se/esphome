import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import DEVICE_CLASS_CURRENT, UNIT_AMPERE
from .. import CONF_EVSE_ID, EVSEComponent, evse_ns

CurrentLimitNumber = evse_ns.class_("CurrentLimitNumber", number.Number)

CONF_CURRENT_LIMIT = "current_limit"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_EVSE_ID): cv.use_id(EVSEComponent),
        cv.Optional(CONF_CURRENT_LIMIT): number.number_schema(
            CurrentLimitNumber,
            device_class=DEVICE_CLASS_CURRENT,
            unit_of_measurement=UNIT_AMPERE,
        ),
    }
)


async def to_code(config):
    evse_component = await cg.get_variable(config[CONF_EVSE_ID])
    if current_limit_config := config.get(CONF_CURRENT_LIMIT):
        n = await number.new_number(
            current_limit_config, min_value=6, max_value=32, step=0.01)
        await cg.register_parented(n, config[CONF_EVSE_ID])
        cg.add(evse_component.set_current_limit_number(n))
