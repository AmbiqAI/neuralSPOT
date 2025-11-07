import configparser
import importlib
from typing import Any, Dict, Tuple
import ctypes
import os

def readPluginConfig(configFile: str, **kwargs):

    # Parse the plugin config file and create a instance of the specified plugin
    config = configparser.ConfigParser()

    # Ensure Config files exists
    with open(configFile) as f:
        config.read(configFile)

    # Grab the required fields: plugin name and path
    sectionName = config.sections()[0]

    # Grab all of the other values within the config file
    for key in config.options(sectionName):
        kwargs.update({key: config.get(sectionName, key)})

    return kwargs

def load_plugin(configFile: str, modulePath: str) -> Tuple[object, Dict[str, Any]]:

    pluginParams = readPluginConfig(configFile)

    try:
        moduleName, pluginClassName = modulePath.rsplit(".", 1)
        module = importlib.import_module(moduleName)
        pluginClass = getattr(module, pluginClassName)
        pluginInstance = pluginClass(**pluginParams)
        return pluginInstance

    except (ImportError, AttributeError) as e:
        print(f"Error loading plugin: {e}\n")
        raise