import esphome.codegen as cg
from esphome.cpp_generator import AssignmentExpression
import esphome.config_validation as cv
from esphome.core import CORE
from esphome.components.binary_sensor import BinarySensor
from esphome.components.ssd1306_base import SSD1306
from esphome.components.image import Image_
from esphome.components.font import Font
from esphome.components.wmbus_meter.base_sensor import BaseSensor
from esphome.const import CONF_DISPLAY_ID, CONF_ID, CONF_PAGES

gui_ns = cg.esphome_ns.namespace("wmbus_gateway")
resources_ns = gui_ns.namespace("resources")
DisplayManager = gui_ns.class_("DisplayManager", cg.PollingComponent)

CONF_BUTTON_ID = "button_id"

CONF_RESOURCES = "resources"
CONF_LOGO = "logo"
CONF_QRCODE = "qrcode"
CONF_FONT_SMALL = "font_small"
CONF_FONT_LARGE = "font_large"

CODEOWNERS = ["@kubasaw"]
AUTO_LOAD = ["network"]


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(DisplayManager),
        cv.GenerateID(CONF_DISPLAY_ID): cv.use_id(SSD1306),
        cv.GenerateID(CONF_BUTTON_ID): cv.use_id(BinarySensor),
        cv.Required(CONF_RESOURCES): cv.Schema(
            {
                cv.Required(CONF_LOGO): cv.use_id(Image_),
                cv.Required(CONF_QRCODE): cv.use_id(Image_),
                cv.Required(CONF_FONT_SMALL): cv.use_id(Font),
                cv.Required(CONF_FONT_LARGE): cv.use_id(Font),
            }
        ),
        cv.Optional(CONF_PAGES, default="all"): cv.Any(
            "all",
            [cv.use_id(BaseSensor)],
        ),
    }
)


async def to_code(config):
    for var_name in (CONF_LOGO, CONF_QRCODE, CONF_FONT_SMALL, CONF_FONT_LARGE):
        cg.add(
            AssignmentExpression(
                None,
                "",
                getattr(resources_ns, var_name),
                await cg.get_variable(config[CONF_RESOURCES][var_name]),
            )
        )

    manager = cg.new_Pvariable(
        config[CONF_ID],
        await cg.get_variable(config[CONF_DISPLAY_ID]),
        await cg.get_variable(config[CONF_BUTTON_ID]),
    )

    sensors = (
        [
            sensor["id"]
            for sensor in CORE.config.get("sensor", [])
            if sensor.get("platform") == "wmbus_meter"
        ]
        if config[CONF_PAGES] == "all"
        else config[CONF_PAGES]
    )

    for sensor in sensors:
        cg.add(manager.add_sensor(await cg.get_variable(sensor)))

    await cg.register_component(manager, config)
