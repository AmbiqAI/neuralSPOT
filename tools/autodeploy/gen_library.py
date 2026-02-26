import logging as log
import os
import shutil
import glob

import numpy as np
from neuralspot.tools.ns_utils import (
    createFromTemplate,
    xxd_c_dump,
    resolve_resource_path,
    flatten_tensor_examples,
    tensor_values_to_c_initializer,
    assert_no_python_tokens,
)


def generateModelLib(params, mc, md, ambiqsuite=False):
    template_directory = str(
        resolve_resource_path("templates", package=__package__, file_path=__file__)
    )
    # Get the  base path of neuralSPOT

    if ambiqsuite:
        n = params.model_name + "_ambiqsuite"
    else:
        n = params.model_name

    d = params.destination_rootdir + "/" + params.model_name

    # Calculate the AmbiqSuite system_<mcu>.c source directory from platform.
    # R5.3.0 keeps Apollo5 system source under src/apollo510/system_apollo510.c.
    if params.platform in ["apollo510_evb", "apollo510_eb", "apollo510L_eb", "apollo510b_evb"]:
        mcu = "apollo510"
    else:
        mcu = None


    adds, addsLen = mc.modelStructureDetails.getAddList()
    # arena_size = (arena_size // 1024) + 1
    # Windows sucks
    if os.name == "posix":
        ws_null = "/dev/null"
        ws_j = "-j"
        ws_and = "&&"
        ws_p = "-p"
    else:
        ws_null = "NUL"
        ws_j = "-j"
        ws_and = "&"
        ws_p = ""

    rm = {
        "NS_AD_NAME": n,
        "NS_AD_ARENA_SIZE": mc.arena_size_k + params.arena_size_scratch_buffer_padding,
        "NS_AD_MODEL_LOCATION": f"NS_AD_{params.model_location}",
        "NS_AD_ARENA_LOCATION": f"NS_AD_{params.arena_location}",
        "NS_AD_RV_COUNT": mc.rv_count,
        "NS_AD_NUM_OPS": addsLen,
        "NS_AD_RESOLVER_ADDS": adds,
        "NS_AD_NUM_INPUT_VECTORS": md.numInputs,
        "NS_AD_NUM_OUTPUT_VECTORS": md.numOutputs,
        "NS_AD_TOOLCHAIN": "arm-none-eabi",
    }

    if ambiqsuite:
        print(f"[NS] Generating AmbiqSuite example at {d}/{n}")
        # Make destination directory
        os.makedirs(f"{d}/{n}", exist_ok=True)
        os.makedirs(f"{d}/{n}/src", exist_ok=True)
        os.makedirs(f"{d}/{n}/src/tensorflow_headers", exist_ok=True)
        os.makedirs(f"{d}/{n}/gcc", exist_ok=True)
        os.makedirs(f"{d}/{n}/armclang", exist_ok=True)
        os.makedirs(f"{d}/{n}/keil6", exist_ok=True)
        os.makedirs(f"{d}/{n}/make", exist_ok=True)
    else:
        print(f"[NS] Generating minimal library at {d}/{n}")

        # Generate a clean (no profiler) version of ns-model.a
        # if params.verbosity > 3:
        #     print(f"cd .. {ws_and} make clean {ws_j} {ws_and} make EXAMPLE=basic_tf_stub {ws_j}")
        #     os.system(f"cd .. {ws_and} make clean {ws_j} {ws_and} make EXAMPLE=basic_tf_stub {ws_j}")
        # else:
        #     os.system(
        #         f"cd .. {ws_and} make clean >{ws_null} 2>&1 {ws_and} make {ws_j} EXAMPLE=basic_tf_stub >{ws_null} 2>&1"
        #     )

        # Make destination directory
        os.makedirs(f"{d}/{n}", exist_ok=True)
        # os.makedirs(f"{d}/{n}/tensorflow_headers", exist_ok=True)
        os.makedirs(f"{d}/{n}/lib", exist_ok=True)
        os.makedirs(f"{d}/{n}/src", exist_ok=True)
        # os.makedirs(f"{d}/{n}/src/gcc", exist_ok=True)
        # os.makedirs(f"{d}/{n}/src/armclang", exist_ok=True)

    # Generate files from templates
    createFromTemplate(
       template_directory + "/common/template_model_metadata.h", f"{d}/{n}/src/{n}_model.h", rm
    )

    # Generate target-specific files from templates
    if ambiqsuite:
        createFromTemplate(
            template_directory + "/ambiqsuite_example/src/template_ambiqsuite_model.cc",
            f"{d}/{n}/src/{n}_model.cc",
            rm,
        )
        createFromTemplate(
            template_directory + "/ambiqsuite_example/src/template_ambiqsuite_example.cc",
            f"{d}/{n}/src/{n}_example.cc",
            rm,
        )
        createFromTemplate(
            template_directory + "/ambiqsuite_example/Makefile.template",
            f"{d}/{n}/gcc/Makefile",
            rm,
        )
        rm["NS_AD_TOOLCHAIN"] = "arm"
        createFromTemplate(
            template_directory + "/ambiqsuite_example/Makefile.template",
            f"{d}/{n}/armclang/Makefile",
            rm,
        )
        createFromTemplate(
            template_directory + "/common/template_api.h", f"{d}/{n}/src/{n}_api.h", rm
        )

        createFromTemplate(
            template_directory + "/common/template_ns_model.h", f"{d}/{n}/src/ns_model.h", rm
        )

        shutil.copy(
            template_directory + "/ambiqsuite_example/src/am_resources.c",
            f"{d}/{n}/src/",
        )
        shutil.copy(
            template_directory + "/ambiqsuite_example/src/am_hal_usbregs-override.h",
            f"{d}/{n}/src/",
        )

        shutil.copytree(
            template_directory + "/ambiqsuite_example/src/tensorflow_headers",
            f"{d}/{n}/src/tensorflow_headers/",
            dirs_exist_ok=True,
        )
        shutil.copytree(
            template_directory + "/ambiqsuite_example/make",
            f"{d}/{n}/make",
            dirs_exist_ok=True,
        )

    else:
        createFromTemplate(
            template_directory + "/minimal_example/template_minimal_model.cc", f"{d}/{n}/src/{n}_model.cc", rm
        )
        createFromTemplate(
            template_directory + "/minimal_example/template_example.cc",
            f"{d}/{n}/src/{n}_example.cc",
            rm,
        )
        createFromTemplate(
            template_directory + "/minimal_example/Makefile.template", f"{d}/{n}/Makefile", rm
        )
        createFromTemplate(
            template_directory + "/common/template_api.h", f"{d}/{n}/lib/{n}_api.h", rm
        )

        shutil.copytree(
            params.neuralspot_rootdir + "/neuralspot/ns-core/src/", f"{d}/{n}/src/ns-core/", dirs_exist_ok=True
        )

        shutil.copytree( 
            params.neuralspot_rootdir + f"/extern/tensorflow/{params.tensorflow_version}/", f"{d}/{n}/lib/tensorflow/", dirs_exist_ok=True
        )

        # Delete tensorflow static libs that don't end in "_release.a"
        for file_path in glob.glob(f"{d}/{n}/lib/tensorflow/lib/libtensorflow-*-with-logs.a"):
            os.remove(file_path)

        for file_path in glob.glob(f"{d}/{n}/lib/tensorflow/lib/libtensorflow-*-debug.a"):
            os.remove(file_path)     

        # Copy CMSIS headers
        shutil.copytree(
            params.neuralspot_rootdir + f"/extern/AmbiqSuite/{params.ambiqsuite_version}/CMSIS/", f"{d}/{n}/src/CMSIS/", dirs_exist_ok=True
        )
        if mcu is not None:
            shutil.copy(
                params.neuralspot_rootdir + f"/extern/AmbiqSuite/{params.ambiqsuite_version}/src/{mcu}/system_{mcu}.c", f"{d}/{n}/src/"
            )

        shutil.copy(
            template_directory + "/minimal_example/README.md", f"{d}/{n}/README.md"
        )

        createFromTemplate(
            template_directory + "/common/template_ns_model.h", f"{d}/{n}/lib/ns_model.h", rm
        )

    # Generate model weight file
    xxd_c_dump(
        src_path=params.tflite_filename,
        dst_path=f"{d}/{n}/src/{n}_model_data.h",
        var_name=f"{n}_model",
        chunk_len=12,
        is_header=True,
    )

    # Generate input/output tensor example data
    flatInput = flatten_tensor_examples(mc.exampleTensors.inputTensors)
    flatOutput = flatten_tensor_examples(mc.exampleTensors.outputTensors)
    inputs = tensor_values_to_c_initializer(flatInput)
    outputs = tensor_values_to_c_initializer(flatOutput)
    assert_no_python_tokens(inputs)
    assert_no_python_tokens(outputs)

    typeMap = {
        "<class 'numpy.float32'>": "float",
        "<class 'numpy.float64'>": "double",
        "<class 'numpy.int8'>": "int8_t",
        "<class 'numpy.uint8'>": "uint8_t",
        "<class 'numpy.int16'>": "int16_t",
        "<class 'numpy.int32'>": "int32_t",
        "<class 'numpy.int64'>": "int64_t",
    }
    
    rm["NS_AD_INPUT_TENSORS"] = inputs
    rm["NS_AD_OUTPUT_TENSORS"] = outputs
    rm["NS_AD_INPUT_TENSOR_TYPE"] = typeMap[str(md.inputTensors[0].type)]
    rm["NS_AD_OUTPUT_TENSOR_TYPE"] = typeMap[str(md.outputTensors[0].type)]
    createFromTemplate(
        template_directory + "/common/template_example_tensors.h",
        f"{d}/{n}/src/{n}_example_tensors.h",
        rm,
    )
    ps = f"PLATFORM={params.platform} AS_VERSION={params.ambiqsuite_version} TF_VERSION={params.tensorflow_version}"

    if ambiqsuite:
        print("[NS] AmbiqSuite example generated successfully at " + d + "/" + n)
    else:
        # Generate library and example binary
        if params.verbosity > 3:
            print(f"cd {d}/{n} {ws_and} make ROOTDIR={params.neuralspot_rootdir} {ps} {ws_j}")
            makefile_result = os.system(f"cd {d}/{n} {ws_and} make ROOTDIR={params.neuralspot_rootdir} {ps} {ws_j}")
        else:
            makefile_result = os.system(
                f"cd {d}/{n} {ws_and} make ROOTDIR={params.neuralspot_rootdir} {ws_j} >{ws_null} {ps} 2>&1"
            )

        if makefile_result != 0:
            log.error("Makefile failed to build minimal example library")
            exit("Makefile failed to build minimal example library")
