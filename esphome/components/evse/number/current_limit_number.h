#pragma once

#include "esphome/components/number/number.h"
#include "../evse.h"

namespace esphome {
namespace evse {

class CurrentLimitNumber : public number::Number, public Parented<EVSEComponent> {
 public:
  CurrentLimitNumber() = default;

 protected:
  void control(float value) override;
};

}  // namespace evse
}  // namespace esphome
