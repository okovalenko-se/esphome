#pragma once

#include "esphome/components/switch/switch.h"
#include "../evse.h"

namespace esphome {
namespace evse {

class ChargeEnabledSwitch : public switch_::Switch, public Parented<EVSEComponent> {
 public:
  ChargeEnabledSwitch() = default;

 protected:
  void write_state(bool state) override;
};

}  // namespace evse
}  // namespace esphome
