import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.components.tuya_ble_node import (
    CONF_TUYA_BLE_NODE_ID,
    TUYA_BLE_NODE_SCHEMA,
    TuyaBLENode,
)
from esphome.const import CONF_ID

DEPENDENCIES = ["tuya_ble_node"]

CONF_DP_ID = "dp_id"
CONF_SCALE = "scale"
CONF_SENSORS = "sensors"

SENSOR_ENTRY_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_DP_ID): cv.int_range(min=1, max=255),
        cv.Optional(CONF_SCALE, default=1.0): cv.float_,
        cv.Required("sensor"): sensor.sensor_schema(),
    }
)

CONFIG_SCHEMA = TUYA_BLE_NODE_SCHEMA.extend(
    {
        cv.Required(CONF_SENSORS): cv.ensure_list(SENSOR_ENTRY_SCHEMA),
    }
)


async def to_code(config):
    node = await cg.get_variable(config[CONF_TUYA_BLE_NODE_ID])
    for entry in config[CONF_SENSORS]:
        sens = await sensor.new_sensor(entry["sensor"])
        cg.add(node.add_dp_sensor(entry[CONF_DP_ID], entry[CONF_SCALE], sens))
