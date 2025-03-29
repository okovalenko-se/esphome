import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from . import CONF_EVSE_ID, EVSEComponent

DEPENDENCIES = ["evse"]

CONF_EVSE_STATE = "evse_state"
ICON_EVSE = "mdi:ev-station"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_EVSE_ID): cv.use_id(EVSEComponent),
    cv.Optional(CONF_EVSE_STATE): text_sensor.text_sensor_schema(
        icon=ICON_EVSE,
    ),
}


async def to_code(config):
    evse_component = await cg.get_variable(config[CONF_EVSE_ID])
    if evse_state_config := config.get(CONF_EVSE_STATE):
        evse_state_sensor = await text_sensor.new_text_sensor(evse_state_config)
        cg.add(evse_component.set_evse_state_text_sensor(evse_state_sensor))
