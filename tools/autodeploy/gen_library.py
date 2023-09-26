import logging as log
import os
import shutil
import numpy as np
from ns_utils import createFromTemplate, xxd_c_dump


def generateModelLib(params, mc, md):
    n = params.model_name
    d = params.working_directory + "/" + params.model_name
    adds, addsLen = mc.modelStructureDetails.getAddList()
    # arena_size = (arena_size // 1024) + 1
    # Windows sucks
    if os.name == 'posix':
        ws_null = '/dev/null'
        ws_j = '-j'
        ws_and = '&&'
        ws_p = '-p'
    else:
        ws_null = 'NUL'
        ws_j = ''
        ws_and = '&'
        ws_p = ''

    rm = {
        "NS_AD_NAME": n,
        "NS_AD_ARENA_SIZE": mc.arena_size_k + params.arena_size_scratch_buffer_padding,
        "NS_AD_RV_COUNT": mc.rv_count,
        "NS_AD_NUM_OPS": addsLen,
        "NS_AD_RESOLVER_ADDS": adds,
        "NS_AD_NUM_INPUT_VECTORS": md.numInputs,
        "NS_AD_NUM_OUTPUT_VECTORS": md.numOutputs,
    }
    print(f"Generating minimal library at {d}/{n}")

    # Generate a clean (no profiler) version of ns-model.a
    os.system(f"cd .. {ws_and} make clean >{ws_null} 2>&1 {ws_and} make {ws_j} >{ws_null} 2>&1")

    # Make destination directory
    os.makedirs(f"{d}/{n}", exist_ok=True)
    print(f"mkdir {ws_p} {d}/{n}")
    os.makedirs(f"{d}/{n}/tensorflow_headers", exist_ok=True)
    os.makedirs(f"{d}/{n}/lib", exist_ok=True)
    os.makedirs(f"{d}/{n}/src", exist_ok=True)
    os.makedirs(f"{d}/{n}/src/gcc", exist_ok=True)
    os.makedirs(f"{d}/{n}/src/armclang", exist_ok=True)

    # Generate files from template
    createFromTemplate(
        "autodeploy/templates/template.cc", f"{d}/{n}/src/{n}_model.cc", rm
    )
    createFromTemplate(
        "autodeploy/templates/template.h", f"{d}/{n}/src/{n}_model.h", rm
    )
    createFromTemplate(
        "autodeploy/templates/template_api.h", f"{d}/{n}/lib/{n}_api.h", rm
    )
    createFromTemplate(
        "autodeploy/templates/template_example.cc", f"{d}/{n}/src/{n}_example.cc", rm
    )
    createFromTemplate(
        "autodeploy/templates/Makefile.template", f"{d}/{n}/Makefile", rm
    )

    # Copy needed files
    shutil.copy("../neuralspot/ns-core/src/gcc/startup_gcc.c", f"{d}/{n}/src/gcc/")
    shutil.copy("../neuralspot/ns-core/src/armclang/startup_armclang.s", f"{d}/{n}/src/armclang/")
    # os.system(f"cp ../neuralspot/ns-core/src/startup_gcc.c {d}/{n}/src/")

    shutil.copy("autodeploy/templates/linker_script.ld", f"{d}/{n}/src/")
    shutil.copy("autodeploy/templates/ns_model.h", f"{d}/{n}/lib/")

    # os.system(f"cp autodeploy/templates/linker_script.ld {d}/{n}/src/")
    # os.system(f"cp autodeploy/templates/ns_model.h {d}/{n}/lib/")

    # Generate model weight file
    xxd_c_dump(
        src_path=params.tflite_filename,
        dst_path=f"{d}/{n}/src/{n}_model_data.h",
        var_name=f"{n}_model",
        chunk_len=12,
        is_header=True,
    )

    # Generate input/output tensor example data
    # inputs = str([list(i) for i in mc.exampleTensors.inputTensors].flatten()).replace("[","{").replace("]","}")
    # outputs = str([list(i) for i in mc.exampleTensors.outputTensors].flatten).replace("[","{").replace("]","}")

    flatInput = [
        element for sublist in mc.exampleTensors.inputTensors for element in sublist
    ]
    flatOutput = [
        element for sublist in mc.exampleTensors.outputTensors for element in sublist
    ]
    inputs = str(flatInput).replace("[", "{").replace("]", "}")
    outputs = str(flatOutput).replace("[", "{").replace("]", "}")

    typeMap = {"<class 'numpy.float32'>": "float", "<class 'numpy.int8'>": "int8_t"}

    rm["NS_AD_INPUT_TENSORS"] = inputs
    rm["NS_AD_OUTPUT_TENSORS"] = outputs
    rm["NS_AD_INPUT_TENSOR_TYPE"] = typeMap[str(md.inputTensors[0].type)]
    rm["NS_AD_OUTPUT_TENSOR_TYPE"] = typeMap[str(md.inputTensors[0].type)]
    createFromTemplate(
        "autodeploy/templates/template_example_tensors.h",
        f"{d}/{n}/src/{n}_example_tensors.h",
        rm,
    )

    # Generate library and example binary
    if params.verbosity > 3:
        makefile_result = os.system(f"cd {d}/{n} {ws_and} make {ws_j}")
    else:
        makefile_result = os.system(f"cd {d}/{n} {ws_and} make {ws_j} >{ws_null} 2>&1")

    if makefile_result != 0:
        log.error("Makefile failed to build minimal example library")
        exit("Makefile failed to build minimal example library")
