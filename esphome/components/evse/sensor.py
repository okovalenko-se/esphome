import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_EMPTY,
    ENTITY_CATEGORY_DIAGNOSTIC,
    STATE_CLASS_MEASUREMENT,
    UNIT_AMPERE,
    UNIT_OHM
)
from . import CONF_EVSE_ID, EVSEComponent

DEPENDENCIES = ["evse"]

CONF_CP_CURRENT_LIMIT = "cp_current_limit"
CONF_PP_CURRENT_LIMIT = "pp_current_limit"
CONF_SOCKET_NTC_RES = "socket_ntc_resistance"


CONFIG_SCHEMA = {
    cv.GenerateID(CONF_EVSE_ID): cv.use_id(EVSEComponent),
    cv.Optional(CONF_CP_CURRENT_LIMIT): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_PP_CURRENT_LIMIT): sensor.sensor_schema(
        unit_of_measurement=UNIT_AMPERE,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_CURRENT,
        state_class=STATE_CLASS_MEASUREMENT,
    ),
    cv.Optional(CONF_SOCKET_NTC_RES): sensor.sensor_schema(
        unit_of_measurement=UNIT_OHM,
        accuracy_decimals=0,
        device_class=DEVICE_CLASS_EMPTY,
        state_class=STATE_CLASS_MEASUREMENT,
        entity_category=ENTITY_CATEGORY_DIAGNOSTIC
    ),
}


async def to_code(config):
    evse_component = await cg.get_variable(config[CONF_EVSE_ID])
    if cp_current_limit_config := config.get(CONF_CP_CURRENT_LIMIT):
        cp_current_limit_sensor = await sensor.new_sensor(cp_current_limit_config)
        cg.add(evse_component.set_cp_current_limit_sensor(cp_current_limit_sensor))
    if pp_current_limit_config := config.get(CONF_PP_CURRENT_LIMIT):
        pp_current_limit_sensor = await sensor.new_sensor(pp_current_limit_config)
        cg.add(evse_component.set_pp_current_limit_sensor(pp_current_limit_sensor))
    if socket_ntc_res_config := config.get(CONF_SOCKET_NTC_RES):
        socket_ntc_res_sensor = await sensor.new_sensor(socket_ntc_res_config)
        cg.add(evse_component.set_socket_ntc_res_sensor(socket_ntc_res_sensor))
