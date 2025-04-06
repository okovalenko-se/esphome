import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import DEVICE_CLASS_BATTERY_CHARGING, DEVICE_CLASS_PLUG, DEVICE_CLASS_PROBLEM
from . import CONF_EVSE_ID, EVSEComponent

DEPENDENCIES = ["evse"]

CONF_EV_CONNECTED = "ev_connected"
CONF_EV_READY = "ev_ready"
CONF_EV_CHARGING = "ev_charging"
CONF_EV_ERROR = "ev_error"
CONF_EV_DIODE_ERROR = "ev_diode_error"
CONF_CABLE_DETECTED = "cable_detected"
CONF_SOCKET_OVERHEAT = "socket_overheat"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_EVSE_ID): cv.use_id(EVSEComponent),
    cv.Optional(CONF_EV_CONNECTED): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_PLUG,
    ),
    cv.Optional(CONF_EV_READY): binary_sensor.binary_sensor_schema(),
    cv.Optional(CONF_EV_CHARGING): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_BATTERY_CHARGING,
    ),
    cv.Optional(CONF_EV_ERROR): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_PROBLEM,
    ),
    cv.Optional(CONF_EV_DIODE_ERROR): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_PROBLEM,
    ),
    cv.Optional(CONF_CABLE_DETECTED): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_PLUG,
    ),
    cv.Optional(CONF_SOCKET_OVERHEAT): binary_sensor.binary_sensor_schema(
        device_class=DEVICE_CLASS_PROBLEM,
    ),
}


async def to_code(config):
    evse_component = await cg.get_variable(config[CONF_EVSE_ID])
    if ev_connected_config := config.get(CONF_EV_CONNECTED):
        ev_connected_sensor = await binary_sensor.new_binary_sensor(ev_connected_config)
        cg.add(evse_component.set_ev_connected_binary_sensor(ev_connected_sensor))
    if ev_ready_config := config.get(CONF_EV_READY):
        ev_ready_sensor = await binary_sensor.new_binary_sensor(ev_ready_config)
        cg.add(evse_component.set_ev_ready_binary_sensor(ev_ready_sensor))
    if ev_charging_config := config.get(CONF_EV_CHARGING):
        ev_charging_sensor = await binary_sensor.new_binary_sensor(ev_charging_config)
        cg.add(evse_component.set_ev_charging_binary_sensor(ev_charging_sensor))
    if ev_error_config := config.get(CONF_EV_ERROR):
        ev_error_sensor = await binary_sensor.new_binary_sensor(ev_error_config)
        cg.add(evse_component.set_ev_error_binary_sensor(ev_error_sensor))
    if ev_diode_error_config := config.get(CONF_EV_DIODE_ERROR):
        ev_diode_error_sensor = await binary_sensor.new_binary_sensor(ev_diode_error_config)
        cg.add(evse_component.set_ev_diode_error_binary_sensor(ev_diode_error_sensor))
    if cable_detected_config := config.get(CONF_CABLE_DETECTED):
        cable_detected_sensor = await binary_sensor.new_binary_sensor(cable_detected_config)
        cg.add(evse_component.set_cable_detected_binary_sensor(cable_detected_sensor))
    if socket_overheat_config := config.get(CONF_SOCKET_OVERHEAT):
        socket_overheat_sensor = await binary_sensor.new_binary_sensor(socket_overheat_config)
        cg.add(evse_component.set_socket_overheat_binary_sensor(socket_overheat_sensor))
