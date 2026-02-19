# Ambiq Platform knowledgebase class
import yaml
import math
import importlib.resources

class AmbiqPlatform:
    def __init__(self, params):
        self.platform = params.platform

        # Load the ns_platform yaml file
        yaml_path = str(importlib.resources.files(__package__) / 'ns_platform.yaml')
        with open(yaml_path, "r") as f:
            self.platform_config = yaml.safe_load(f)

        # print (self.platform_config[self.platform])
        # Check if the platform is in the yaml file
        if self.platform not in self.platform_config:
            print(f"Platform {self.platform} not found in ns_platform.yaml")
            # print all available platforms
            print("Available platforms:")
            for platform in self.platform_config.keys():
                print(f"  - {platform}")
            raise ValueError(f"Platform {self.platform} not found in ns_platform.yaml")
        
        # Check if platform is supported by ns_autodeploy
        if self.platform_config[self.platform]["supported_by_autodeploy"] == False:
            raise ValueError(f"Platform {self.platform} is not supported by ns_autodeploy")

        self.raw_platform_config = self.platform_config[self.platform]
        self.platform_config = dict(self.raw_platform_config)

        # Platform-default holdbacks preserve legacy behavior, but final effective
        # sizes are clamped >= 0 so small memories (e.g. AP3 DTCM) never go negative.
        self._default_reserve_percent = {
            "sram": 20,
            "dtcm": 0,
            "mram": 20,
        }
        mcu = self.raw_platform_config.get("mcu")
        self._default_reserve_kb = {
            "sram": 85 if mcu in ["apollo4p", "apollo4l"] else 0,
            "dtcm": 0 if mcu == "apollo3p" else 150,
            "mram": 0,
        }
        self._user_reserve_percent = {
            "sram": int(getattr(params, "reserve_sram_percent", 0) or 0),
            "dtcm": int(getattr(params, "reserve_dtcm_percent", 0) or 0),
            "mram": int(getattr(params, "reserve_mram_percent", 0) or 0),
        }
        self._user_reserve_kb = {
            "sram": int(getattr(params, "reserve_sram_kb", 0) or 0),
            "dtcm": int(getattr(params, "reserve_dtcm_kb", 0) or 0),
            "mram": int(getattr(params, "reserve_mram_kb", 0) or 0),
        }

        self._effective_memories = {}
        self._applied_reserve_percent = {}
        self._applied_reserve_kb = {}
        for mem in ("sram", "dtcm", "mram"):
            raw_kb = int(self.raw_platform_config.get(mem, 0))
            reserve_percent = self._default_reserve_percent[mem] + self._user_reserve_percent[mem]
            reserve_kb = self._default_reserve_kb[mem] + self._user_reserve_kb[mem]
            reserve_percent = min(max(int(reserve_percent), 0), 95)
            reserve_kb = max(int(reserve_kb), 0)
            self._applied_reserve_percent[mem] = reserve_percent
            self._applied_reserve_kb[mem] = reserve_kb
            self._effective_memories[mem] = self._compute_effective_kb(
                raw_kb=raw_kb,
                reserve_percent=reserve_percent,
                reserve_kb=reserve_kb,
            )
            self.platform_config[mem] = self._effective_memories[mem]

    @staticmethod
    def _compute_effective_kb(raw_kb: int, reserve_percent: int, reserve_kb: int) -> int:
        reserve_percent = min(max(int(reserve_percent), 0), 95)
        reserve_kb = max(int(reserve_kb), 0)
        scaled = math.floor(raw_kb * (1.0 - (reserve_percent / 100.0)))
        return max(0, int(scaled - reserve_kb))

    def GetMemoryReserveSummary(self):
        summary = {}
        for mem in ("sram", "dtcm", "mram"):
            summary[mem] = {
                "raw_kb": int(self.raw_platform_config.get(mem, 0)),
                "effective_kb": int(self._effective_memories.get(mem, 0)),
                "default_percent": int(self._default_reserve_percent[mem]),
                "default_kb": int(self._default_reserve_kb[mem]),
                "user_percent": int(self._user_reserve_percent[mem]),
                "user_kb": int(self._user_reserve_kb[mem]),
                "total_percent": int(self._applied_reserve_percent[mem]),
                "total_kb": int(self._applied_reserve_kb[mem]),
            }
        return summary

    def GetSupportsUsb(self):
        return "usb" in self.platform_config["features"]

    def GetMaxArenaSize(self):
        return self.platform_config["sram"]
    
    def GetMaxPsramArenaSize(self):
        if "psram-arena" in self.platform_config:
            return self.platform_config["psram-arena"]
        else:
            raise ValueError(f"Platform {self.platform} does not have PSRAM arena size defined")
    

    def GetMaxNVMSize(self):
        if "nvm" in self.platform_config:
            return self.platform_config["nvm"]
        else:
            raise ValueError(f"Platform {self.platform} does not have NVM size defined")

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
            
            # PSRAM arean size is called something different
            if location == "psram":
                return size <= self.platform_config["psram-arena"]
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
