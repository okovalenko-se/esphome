import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import DEVICE_CLASS_SWITCH
from .. import CONF_EVSE_ID, EVSEComponent, evse_ns

ChargeEnabledSwitch = evse_ns.class_("ChargeEnabledSwitch", switch.Switch)
RemoteErrorSwitch = evse_ns.class_("RemoteErrorSwitch", switch.Switch)

CONF_CHARGE_ENABLED = "charge_enabled"
CONF_REMOTE_ERROR = "remote_error"

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_EVSE_ID): cv.use_id(EVSEComponent),
    cv.Optional(CONF_CHARGE_ENABLED): switch.switch_schema(
        ChargeEnabledSwitch,
        device_class=DEVICE_CLASS_SWITCH
    ),
    cv.Optional(CONF_REMOTE_ERROR): switch.switch_schema(
        RemoteErrorSwitch,
        device_class=DEVICE_CLASS_SWITCH
    ),
}


async def to_code(config):
    evse_component = await cg.get_variable(config[CONF_EVSE_ID])
    if charge_enabled_config := config.get(CONF_CHARGE_ENABLED):
        s = await switch.new_switch(charge_enabled_config)
        await cg.register_parented(s, config[CONF_EVSE_ID])
        cg.add(evse_component.set_charge_enabled_switch(s))
    if remote_error_config := config.get(CONF_REMOTE_ERROR):
        s = await switch.new_switch(remote_error_config)
        await cg.register_parented(s, config[CONF_EVSE_ID])
        cg.add(evse_component.set_remote_error_switch(s))
