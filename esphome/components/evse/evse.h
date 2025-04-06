#pragma once

#include "esphome/core/defines.h"
#include "esphome/core/component.h"
#include "esphome/components/i2c/i2c.h"
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

namespace esphome {
namespace evse {

#define bit_is_set(r, b) (r & (1 << b))

/**
 * I2C commands
 */
#define CMD_SET_CHARGE_ENABLED  0xCE
#define CMD_SET_REMOTE_ERROR    0xE0
#define CMD_SET_CURRENT_LIMIT   0xC1

/**
 * EVSE data registers
 */
#define R_STATE 0
#define R_CP_STATE 1
#define R_CP_CURRENT_LIMIT_L 2
#define R_CP_CURRENT_LIMIT_H 3
#define R_REMOTE_CURRENT_LIMIT_L 4
#define R_REMOTE_CURRENT_LIMIT_H 5
#define R_PP_STATE 6
#define R_SOCKET_TEMP 7

/**
 * R_STATE bits
 */
#define RST 0 // Restart flag (set on first boot, reset on first I2C read)
#define CEN 1 // Charge Enabled
#define RER 2 // Remote Error (error that is set by remote system)
#define SOE 3 // Socket Overheat Error
#define CHA 4 // Charging (relay contacts closed)
#define CDE 5 // Cable Disconnection Error
#define DIE 6 // CP Diode Error

#define PP_DEFAULT_CURRENT_LIMIT 13

typedef enum {
  CP_STATE_A = 'A',
  CP_STATE_B = 'B',
  CP_STATE_C = 'C',
  CP_STATE_D = 'D',
  CP_STATE_E = 'E',
  CP_STATE_F = 'F'
} cp_state_t;

typedef enum {
  PP_NOT_CONNECTED = 0,
  PP_CONNECTED_13A = 13,
  PP_CONNECTED_20A = 20,
  PP_CONNECTED_32A = 32,
  PP_CONNECTED_70A = 70,
  PP_UNKNOWN = 255
} pp_state_t;

/**
 * Utility functions
 */
uint16_t join_bytes(uint8_t l, uint8_t h);
uint16_t pack_float_to_uint16(float value);
float unpack_uint16_to_float(uint16_t value);

class EVSEComponent : public PollingComponent, public i2c::I2CDevice {

#ifdef USE_BINARY_SENSOR
  SUB_BINARY_SENSOR(ev_connected)
  SUB_BINARY_SENSOR(ev_ready)
  SUB_BINARY_SENSOR(ev_charging)
  SUB_BINARY_SENSOR(ev_error)
  SUB_BINARY_SENSOR(ev_diode_error)
  SUB_BINARY_SENSOR(cable_detected)
  SUB_BINARY_SENSOR(socket_overheat)
#endif

#ifdef USE_NUMBER
  SUB_NUMBER(current_limit)
#endif

#ifdef USE_SENSOR
  SUB_SENSOR(cp_current_limit)
  SUB_SENSOR(pp_current_limit)
  SUB_SENSOR(socket_ntc_res)
#endif

#ifdef USE_SWITCH
  SUB_SWITCH(charge_enabled)
  SUB_SWITCH(remote_error)
#endif

#ifdef USE_TEXT_SENSOR
  SUB_TEXT_SENSOR(evse_state)
#endif

 public:
  EVSEComponent();
  void setup() override;
  void dump_config() override;
  void update() override;
  void set_num_attempts(uint16_t value) { this->num_attempts_ = value; };

#ifdef USE_SWITCH
  void set_charge_enabled(bool state);
  void set_remote_error(bool state);
#endif

#ifdef USE_NUMBER
  void set_current_limit(float value);
#endif

 protected:
  uint8_t num_attempts_;
  uint8_t failed_read_attempt_counter_;
  void read_data_();
  float get_socket_ntc_resistance_(uint8_t adc_val);
};

}  // namespace evse
}  // namespace esphome
