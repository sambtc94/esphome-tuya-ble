import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.components.tuya_ble_node import (
    CONF_TUYA_BLE_NODE_ID,
    TUYA_BLE_NODE_SCHEMA,
)
from esphome.const import CONF_ID

DEPENDENCIES = ["tuya_ble_node"]
CODEOWNERS = []

CONF_DP_ID = "dp_id"
CONF_SCALE = "scale"

CONFIG_SCHEMA = TUYA_BLE_NODE_SCHEMA.extend(
    sensor.sensor_schema().extend(
        {
            cv.Required(CONF_DP_ID): cv.int_range(min=1, max=255),
            cv.Optional(CONF_SCALE, default=1.0): cv.float_,
        }
    )
)


async def to_code(config):
    node = await cg.get_variable(config[CONF_TUYA_BLE_NODE_ID])
    sens = await sensor.new_sensor(config)
    cg.add(node.add_dp_sensor(config[CONF_DP_ID], config[CONF_SCALE], sens))