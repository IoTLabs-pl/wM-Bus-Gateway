import esphome.config_validation as cv
from esphome.const import SOURCE_FILE_EXTENSIONS, CONF_ID
from esphome.git import run_git_command
from esphome.loader import get_component, ComponentManifest
from esphome import codegen as cg
from pathlib import Path

CODEOWNERS = ["@kubasaw"]
CONF_DRIVERS = "drivers"

wmbus_common_ns = cg.esphome_ns.namespace("wmbus_common")
WMBusCommon = wmbus_common_ns.class_("WMBusCommon", cg.Component)

SOURCE_FILE_EXTENSIONS.add(".cc")

_ALWAYS_EXCLUDED = {"auto", "unknown"}

AVAILABLE_DRIVERS = sorted(
    name
    for f in Path(__file__).parent.glob("driver_*.cc")
    if (name := f.stem.removeprefix("driver_")) not in _ALWAYS_EXCLUDED
)

_registered_drivers = set()


validate_driver = cv.All(
    cv.one_of(*AVAILABLE_DRIVERS, lower=True, space="_"),
    lambda driver: _registered_drivers.add(driver) or driver,
)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(WMBusCommon),
        cv.Optional(CONF_DRIVERS): cv.All(
            lambda x: AVAILABLE_DRIVERS if x == "all" else x,
            [validate_driver],
        ),
    }
)


class WMBusComponentManifest(ComponentManifest):
    @property
    def resources(self):
        exclude_drivers = set(AVAILABLE_DRIVERS) - _registered_drivers
        if _registered_drivers:
            exclude_drivers |= _ALWAYS_EXCLUDED
        else:
            exclude_drivers |= _ALWAYS_EXCLUDED - {"unknown"}

        exclude_files = {f"driver_{name}.cc" for name in exclude_drivers}
        resources = [fr for fr in super().resources if fr.resource not in exclude_files]
        return resources


async def to_code(config):
    wmbusmeters_tag = (
        run_git_command(
            [
                "git",
                "log",
                "--oneline",
                "--grep=^Merge wmbusmeters",
                ".",
            ],
            Path(__file__).parent,
        )
        .splitlines()[0]
        .split()[-1]
    )

    cg.add_define("WMBUSMETERS_TAG", wmbusmeters_tag)

    get_component("wmbus_common").__class__ = WMBusComponentManifest

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
