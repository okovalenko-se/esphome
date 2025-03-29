#include "remote_error_switch.h"

namespace esphome {
namespace evse {

void RemoteErrorSwitch::write_state(bool state) {
    this->publish_state(state);
    this->parent_->set_remote_error(state);
}

}  // namespace evse
}  // namespace esphome
