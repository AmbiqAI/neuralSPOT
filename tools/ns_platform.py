# Ambiq Platform knowledgebase class
import yaml
import math
import pkg_resources

class AmbiqPlatform:
    def __init__(self, params):
        self.platform = params.platform

        # Load the ns_platform yaml file
        yaml_path = pkg_resources.resource_filename(__name__, 'ns_platform.yaml')
        with open(yaml_path, "r") as f:
            self.platform_config = yaml.safe_load(f)

        # print (self.platform_config[self.platform])
        # Check if the platform is in the yaml file
        if self.platform not in self.platform_config:
            raise ValueError(f"Platform {self.platform} not found in ns_platform.yaml")
        
        # Check if platform is supported by ns_autodeploy
        if self.platform_config[self.platform]["supported_by_autodeploy"] == False:
            raise ValueError(f"Platform {self.platform} is not supported by ns_autodeploy")

        self.platform_config = self.platform_config[self.platform]

        # print (f"[INFO] Platform: {self.platform}")
        # print ("Features: ", self.platform_config["features"])

        # reserve memory for non-model stuff (20-25%)
        # Reserve is different for each platform
        if self.platform_config["mcu"] in ["apollo4p", "apollo4l"]:
            self.platform_config["sram"] = int(math.ceil(self.platform_config["sram"] * .8) - 70) # 70K holdback
        else:
            self.platform_config["sram"] = int(math.ceil(self.platform_config["sram"] * .8))
        self.platform_config["dtcm"] = int(math.ceil(self.platform_config["dtcm"] - 60)) # 50K holdback
        self.platform_config["mram"] = int(math.ceil(self.platform_config["mram"] * .8))

    def GetSupportsUsb(self):
        return "usb" in self.platform_config["features"]

    def GetMaxArenaSize(self):
        return self.platform_config["sram"]
    
    def GetDTCMSize(self):
        return self.platform_config["dtcm"]
    
    def CheckArenaSize(self, size, location):
        # if arena location is 'auto', check if size is less than max arena size
        location = location.lower()
        if location == "tcm":
            location = "dtcm"

        if location == "auto":
            return size <= self.GetMaxArenaSize()
        else:
            # Check if platform has that type of arena
            if location not in self.platform_config:
                raise ValueError(f"Platform {self.platform} does not have arena location {location}")
            
            # Check if size is less than specified arena location size
            return size <= self.platform_config[location]
    
    def GetModelLocation(self, size, location):
        # if model location is 'auto', return the best location that can fit the model
        location = location.lower()
        # print(f"[NS] Model size {size}KB, location {location} platform size {self.platform_config["dtcm"]}")
        if location == "tcm":
            location = "dtcm"

        if location == "auto":
            if size <= self.platform_config["dtcm"]:
                return "TCM"
            elif size <= self.platform_config["mram"]:
                return "MRAM"
            elif size <= self.platform_config["sram"]:
                return "SRAM"
            else:
                # Put the model in PSRAM if present on the platform and less than 20MB
                if ("psram" in self.platform_config) and (size <= 20 * 1024):
                    print(f"[NS] Model size {size}KB exceeds all internal memories for platform {self.platform}. Placing in PSRAM, which is slower.")
                    return "PSRAM"
                else:
                    raise ValueError(f"Model size {size}KB exceeds all available memory locations for platform {self.platform}")
        else:
            # Check if platform has that type of model location
            if location not in self.platform_config:
                raise ValueError(f"Platform {self.platform} does not have model location {location}")

    def GetArenaLocation(self, size, location):
        # if model location is 'auto', return the best location that can fit the model
        if location == "auto":
            if size <= self.platform_config["dtcm"]:
                return "TCM"
            elif size <= self.platform_config["sram"]:
                return "SRAM"
            else:
                # Put the model in PSRAM if present on the platform and less than 12MB (32 minus 20)
                if ("psram" in self.platform_config) and (size <= 12 * 1024):
                    print(f"[NS] Tensor Arena size {size}KB exceeds all internal memories for platform {self.platform}. Placing in PSRAM, which is slower.")
                    return "PSRAM"
                else:
                    raise ValueError(f"Model size {size}KB exceeds all available memory locations for platform {self.platform}")
        else:
            # Check if platform has that type of model location
            if location not in self.platform_config:
                raise ValueError(f"Platform {self.platform} does not have model location {location}")

