#include "charge_enabled_switch.h"

namespace esphome {
namespace evse {

void ChargeEnabledSwitch::write_state(bool state) {
    this->publish_state(state);
    this->parent_->set_charge_enabled(state);
}

}  // namespace evse
}  // namespace esphome
