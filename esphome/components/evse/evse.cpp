#include "evse.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace evse {

static const char *const TAG = "evse";

EVSEComponent::EVSEComponent() {
  this->failed_read_attempt_counter_ = 0;
}

void EVSEComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up EVSE...");
  this->read_data_();
}

void EVSEComponent::update() {
  this->read_data_();
}

void EVSEComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "EVSE:");
#ifdef USE_SWITCH
  LOG_SWITCH("  ", "Charge Enabled Switch", this->charge_enabled_switch_);
  LOG_SWITCH("  ", "Remote Error Switch", this->remote_error_switch_);
#endif
#ifdef USE_NUMBER
  LOG_NUMBER("  ", "Current Limit Number", this->current_limit_number_);
#endif
#ifdef USE_TEXT_SENSOR
  LOG_TEXT_SENSOR("  ", "EVSE State", this->evse_state_text_sensor_);
#endif
#ifdef USE_BINARY_SENSOR
  LOG_BINARY_SENSOR("  ", "EV Connected", this->ev_connected_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "EV Ready", this->ev_ready_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "EV Charging", this->ev_charging_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "EV Error", this->ev_error_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Cable Detected", this->cable_detected_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "EVSE Socket Overheat", this->socket_overheat_binary_sensor_);
#endif
#ifdef USE_SENSOR
  LOG_SENSOR("  ", "CP Current Limit", this->cp_current_limit_sensor_);
  LOG_SENSOR("  ", "PP Current Limit", this->pp_current_limit_sensor_);
  LOG_SENSOR("  ", "EVSE Socket NTC Resistance", this->socket_ntc_res_sensor_);
#endif
}

#ifdef USE_SWITCH
void EVSEComponent::set_charge_enabled(bool state) {
  uint8_t data[2] = {CMD_SET_CHARGE_ENABLED, state};
  if (this->write(data, sizeof(data)) == i2c::NO_ERROR) {
    ESP_LOGV(TAG, "I2C write: %s", format_hex_pretty(data, sizeof(data)).c_str());
  } else {
    ESP_LOGE(TAG, "Failed to write charge enabled state!");
  }
}

void EVSEComponent::set_remote_error(bool state) {
  uint8_t data[2] = {CMD_SET_REMOTE_ERROR, state};
  if (this->write(data, sizeof(data)) == i2c::NO_ERROR) {
    ESP_LOGV(TAG, "I2C write: %s", format_hex_pretty(data, sizeof(data)).c_str());
  } else {
    ESP_LOGE(TAG, "Failed to write remote error state!");
  }
}
#endif

#ifdef USE_NUMBER
void EVSEComponent::set_current_limit(float value) {
  uint16_t packed_value = pack_float_to_uint16(value);
  uint8_t data[3] = {
    CMD_SET_CURRENT_LIMIT,
    (uint8_t)packed_value,
    (uint8_t)(packed_value >> 8)
  };
  if (this->write(data, sizeof(data)) == i2c::NO_ERROR) {
    ESP_LOGV(TAG, "I2C write: %s", format_hex_pretty(data, sizeof(data)).c_str());
  } else {
    ESP_LOGE(TAG, "Failed to write current limit value!");
  }
}
#endif

void EVSEComponent::read_data_() {
  uint8_t data[8];

  if (this->read(data, sizeof(data)) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Failed to read EVSE state!");

    if (!this->status_has_error()) {
      this->failed_read_attempt_counter_++;

      if (this->failed_read_attempt_counter_ >= this->num_attempts_) {
        this->status_set_error("Communication with EVSE failed!");
        this->socket_overheat_binary_sensor_->publish_state(false);
        this->ev_charging_binary_sensor_->publish_state(false);
        this->evse_state_text_sensor_->publish_state("unknown");
        this->ev_connected_binary_sensor_->publish_state(false);
        this->ev_ready_binary_sensor_->publish_state(false);
        this->ev_error_binary_sensor_->publish_state(false);
        this->cp_current_limit_sensor_->publish_state(NAN);
        this->pp_current_limit_sensor_->publish_state(NAN);
        this->cable_detected_binary_sensor_->publish_state(false);
        this->socket_ntc_res_sensor_->publish_state(NAN);
      }
    }
    return;
  }

  if (this->status_has_error()) {
    this->status_clear_error();
    ESP_LOGI(TAG, "Communication with EVSE restored!");
  }
  this->failed_read_attempt_counter_ = 0;

  ESP_LOGV(TAG, "I2C read: %s", format_hex_pretty(data, sizeof(data)).c_str());

  bool is_evse_restarted = bit_is_set(data[R_STATE], RST) ? true : false;

#ifdef USE_SWITCH
  // Charge Enabled
  if (this->charge_enabled_switch_ != nullptr) {
    if (is_evse_restarted) {
      this->set_charge_enabled(this->charge_enabled_switch_->state);
    } else {
      bool charge_enabled = bit_is_set(data[R_STATE], CEN) ? true : false;
      if (charge_enabled != this->charge_enabled_switch_->state) {
        this->charge_enabled_switch_->publish_state(charge_enabled);
      }
    }
  }

  // Remote Error
  if (this->remote_error_switch_ != nullptr) {
    if (is_evse_restarted) {
      this->set_remote_error(this->remote_error_switch_->state);
    } else {
      bool remote_error = bit_is_set(data[R_STATE], RER) ? true : false;
      if (remote_error != this->remote_error_switch_->state) {
        this->remote_error_switch_->publish_state(remote_error);
      }
    }
  }
#endif

#ifdef USE_BINARY_SENSOR
  // Socket Overheat
  if (this->socket_overheat_binary_sensor_ != nullptr) {
    bool socket_overheat = bit_is_set(data[R_STATE], SOE) ? true : false;
    this->socket_overheat_binary_sensor_->publish_state(socket_overheat);
  }

  // EV Charging
  if (this->ev_charging_binary_sensor_ != nullptr) {
    bool is_charging = bit_is_set(data[R_STATE], CHA) ? true : false;
    this->ev_charging_binary_sensor_->publish_state(is_charging);
  }
#endif

  // CP State
  cp_state_t cp_state = (cp_state_t)data[R_CP_STATE];
#ifdef USE_TEXT_SENSOR
  if (this->evse_state_text_sensor_ != nullptr) {
    if (cp_state != this->evse_state_text_sensor_->state[0]) {
      std::string cp_state_str(1, cp_state);
      this->evse_state_text_sensor_->publish_state(cp_state_str);
    }
  }
#endif

#ifdef USE_BINARY_SENSOR
  // EV Connected
  if (this->ev_connected_binary_sensor_ != nullptr) {
    switch (cp_state) {
      case CP_STATE_B:
      case CP_STATE_C:
      case CP_STATE_D:
      case CP_STATE_E:
        this->ev_connected_binary_sensor_->publish_state(true);
        break;

      default:
        this->ev_connected_binary_sensor_->publish_state(false);
        break;
    }
  }

  // EV Ready
  if (this->ev_ready_binary_sensor_ != nullptr) {
    switch (cp_state) {
      case CP_STATE_C:
      case CP_STATE_D:
        this->ev_ready_binary_sensor_->publish_state(true);
        break;

      default:
        this->ev_ready_binary_sensor_->publish_state(false);
        break;
    }
  }

  // EV Error
  if (this->ev_error_binary_sensor_ != nullptr) {
    this->ev_error_binary_sensor_->publish_state(cp_state == CP_STATE_E);
  }
#endif

#ifdef USE_SENSOR
  // CP Current Limit
  if (this->cp_current_limit_sensor_ != nullptr) {
    float cp_current_limit = unpack_uint16_to_float(
      join_bytes(data[R_CP_CURRENT_LIMIT_L], data[R_CP_CURRENT_LIMIT_H])
    );
    if (cp_current_limit != this->cp_current_limit_sensor_->state) {
      this->cp_current_limit_sensor_->publish_state(cp_current_limit);
    }
  }
#endif

#ifdef USE_NUMBER
  // Remote Current Limit
  if (this->current_limit_number_ != nullptr) {
    if (is_evse_restarted && this->current_limit_number_->state != 0) {
      this->set_current_limit(this->current_limit_number_->state);
    } else {
      float remote_current_limit = unpack_uint16_to_float(
        join_bytes(data[R_REMOTE_CURRENT_LIMIT_L], data[R_REMOTE_CURRENT_LIMIT_H])
      );
      if (remote_current_limit != this->current_limit_number_->state) {
        this->current_limit_number_->publish_state(remote_current_limit);
      }
    }
  }
#endif

  // PP State
  pp_state_t pp_state = (pp_state_t)data[R_PP_STATE];
#ifdef USE_SENSOR
  if (this->pp_current_limit_sensor_ != nullptr) {
    uint8_t pp_current_limit = PP_DEFAULT_CURRENT_LIMIT;
    if (pp_state != PP_NOT_CONNECTED && pp_state != PP_UNKNOWN) {
      pp_current_limit = pp_state;
    }
    if (pp_current_limit != this->pp_current_limit_sensor_->state) {
      this->pp_current_limit_sensor_->publish_state(pp_current_limit);
    }
  }
#endif

#ifdef USE_BINARY_SENSOR
  // Cable Detected
  if (this->cable_detected_binary_sensor_ != nullptr) {
    switch (pp_state) {
      case PP_CONNECTED_13A:
      case PP_CONNECTED_20A:
      case PP_CONNECTED_32A:
      case PP_CONNECTED_70A:
        this->cable_detected_binary_sensor_->publish_state(true);
        break;
      
      default:
        this->cable_detected_binary_sensor_->publish_state(false);
        break;
    }
  }
#endif

#ifdef USE_SENSOR
  // Socket NTC Resistance
  if (this->socket_ntc_res_sensor_ != nullptr) {
    uint8_t socket_temp_adc_val = data[R_SOCKET_TEMP];
    if (socket_temp_adc_val > 0) {
      float socket_ntc_res = this->get_socket_ntc_resistance_(socket_temp_adc_val);
      if (socket_ntc_res != this->socket_ntc_res_sensor_->state) {
        this->socket_ntc_res_sensor_->publish_state(socket_ntc_res);
      }
    }
  }
#endif
}

float EVSEComponent::get_socket_ntc_resistance_(uint8_t adc_val) {
  const float vcc = 5;
  const float r1 = 10000;
  const float adc_voltage = (adc_val / 255.0) * vcc;
  return (r1 * adc_voltage) / (vcc - adc_voltage);
}

uint16_t join_bytes(uint8_t l, uint8_t h) {
  return ((uint16_t)h << 8) | l;
}

uint16_t pack_float_to_uint16(float val) {
  return (uint16_t)round(val * 10);
}

float unpack_uint16_to_float(uint16_t val) {
  return (float)val / 10;
}

}  // namespace evse
}  // namespace esphome
