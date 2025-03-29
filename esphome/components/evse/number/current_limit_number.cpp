#include "current_limit_number.h"

namespace esphome {
namespace evse {

void CurrentLimitNumber::control(float value) {
  this->publish_state(value);
  this->parent_->set_current_limit(value);
}

}  // namespace evse
}  // namespace esphome
