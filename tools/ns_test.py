import configparser
import json
import logging as log
import pickle

import pydantic_argparse
from autodeploy.tester import generateTests, parseResults, runTests
from pydantic import BaseModel, Field


class Params(BaseModel):
    # General Parameters
    seed: int = Field(42, description="Random Seed")

    configfile: str = Field(
        "",
        description="Test configuration file ini format",
    )

    test_directory: str = Field(
        "../neuralspot/ns_features/tests",
        description="Test directory",
    )

    working_directory: str = Field(
        "../projects/tests",
        description="Directory where generated test artifacts will be placed",
    )

    # Logging Parameters
    verbosity: int = Field(1, description="Verbosity level (0-4)")


def create_parser():
    """Create CLI argument parser
    Returns:
        ArgumentParser: Arg parser
    """
    return pydantic_argparse.ArgumentParser(
        model=Params,
        prog="Measure performance of TFLite model running on EVB",
        description="Characterize TFLite model",
    )


if __name__ == "__main__":
    # parse cmd parameters
    parser = create_parser()
    cli_params = parser.parse_typed_args()

    # override parameters from config file
    json_params = cli_params.json()
    dict_params = json.loads(json_params)
    if cli_params.configfile:
        with open(cli_params.configfile, "r") as f:
            config = json.load(f)
            dict_params.update(config)

    params = Params(**dict_params)

    # Read the test configuration file.
    test_config = configparser.ConfigParser()
    test_config.optionxform = str
    test_config.read(params.configfile)

    # set logging level
    log.basicConfig(
        level=log.DEBUG
        if params.verbosity > 2
        else log.INFO
        if params.verbosity > 1
        else log.WARNING,
        format="%(levelname)s: %(message)s",
    )

    print("")
    print(f"*** Generate Tests for EVB based on {params.test_directory}")

    generateTests(params, test_config)
    print(f"*** Run Tests on EVB")
    runTests(params, test_config)
    parseResults(params, test_config)
