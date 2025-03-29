#pragma once

#include "esphome/components/switch/switch.h"
#include "../evse.h"

namespace esphome {
namespace evse {

class RemoteErrorSwitch : public switch_::Switch, public Parented<EVSEComponent> {
 public:
  RemoteErrorSwitch() = default;

 protected:
  void write_state(bool state) override;
};

}  // namespace evse
}  // namespace esphome
