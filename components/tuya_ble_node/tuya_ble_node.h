#pragma once

#include <deque>
#include <vector>
#include "esphome/core/log.h"
#include "esphome/core/component.h"
#include "esphome/components/md5/md5.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/tuya_ble_tracker/common.h"
#include "esphome/components/tuya_ble_tracker/tuya_ble_tracker.h"
#include "esphome/core/automation.h"

namespace esphome {
namespace tuya_ble_node {

using namespace esphome::tuya_ble;
using md5::MD5Digest;

struct DPSensorEntry {
  uint8_t dp_id;
  float scale;
  sensor::Sensor *sensor;
};

class TuyaBLENode : public TYBLENode, public Component {
  
  std::deque<struct TYBLECommand> command_queue;
  std::vector<DPSensorEntry> dp_sensors_;

  CallbackManager<void(uint8_t, uint8_t, const std::vector<uint8_t> &)> dp_update_callback_;


  public:
    bool has_command();

    bool has_session_key();

    int get_rssi() {
      return this->rssi;
    }

    void issue_command();

    void set_device_id(std::string device_id);

    void set_local_key(const char *local_key);

    void set_max_queued(uint8_t max);

    void set_uuid(std::string uuid);

    void pair();

    void request_info();

    void request_status();

    void reset_session_key();

    void toggle(bool value);

    void set_dp_bool(uint8_t dp_id, bool value);

    void set_dp_enum(uint8_t dp_id, uint8_t value);

    void set_dp_raw(uint8_t dp_id, const std::vector<uint8_t> &value);

    void add_on_dp_update_callback(
        std::function<void(uint8_t, uint8_t, const std::vector<uint8_t> &)> callback) {
      this->dp_update_callback_.add(std::move(callback));
    }

    void add_dp_sensor(uint8_t dp_id, float scale, sensor::Sensor *s) {
      dp_sensors_.push_back({dp_id, scale, s});
    }

    /**
    void on_dp_received(uint8_t dp_id, uint8_t type, uint16_t len, const unsigned char *value) override {
      if(len == 0 || len > 4) return;
      int32_t raw = 0;
      for(uint16_t i = 0; i < len; i++) raw = (raw << 8) | value[i];
      for(auto &e : dp_sensors_) {
        if(e.dp_id == dp_id) e.sensor->publish_state(raw * e.scale);
      }
    }*/

    void on_dp_received(uint8_t dp_id, uint8_t type, uint16_t len, const unsigned char *value) override {
      std::vector<uint8_t> raw_data(value, value + len);
    
      this->dp_update_callback_.call(dp_id, type, raw_data);
    
      if(len == 0 || len > 4) return;
    
      int32_t raw = 0;
      for(uint16_t i = 0; i < len; i++) {
        raw = (raw << 8) | value[i];
      }
    
      for(auto &e : dp_sensors_) {
        if(e.dp_id == dp_id) {
          e.sensor->publish_state(raw * e.scale);
        }
      }
    }

    void register_client(TYBLEClient *client) {
      ESP_LOGD("tuya_ble_node", "Client registered in node!");
      this->client = client;
      this->has_client = true;
    }

  protected:
    TYBLEClient *client;
    bool has_client = false;
    uint8_t max_queued = 1;

    void enqueue_command(TYBLECommand *command);
};

}  // namespace tuya_ble_node
}  // namespace esphome
