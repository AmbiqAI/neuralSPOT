import argparse
import os

from ns_utils import createFromTemplate, xxd_c_dump


def generateModelLib(params, arena_size):
    n = params.model_name
    d = params.model_path
    arena_size = (arena_size // 1024) + 1
    rm = {"NS_AD_NAME": n, "NS_ARENA_SIZE": arena_size}
    print(f"[INFO] Generating minimal library at {d}/{n}")

    # Generate a clean (no profiler) version of ns-model.a
    os.system("cd .. && make clean >/dev/null 2>&1 && make -j >/dev/null 2>&1")

    # Make destination directory
    os.system(f"mkdir -p {d}/{n}")
    os.system(f"mkdir -p {d}/{n}/tensorflow_headers")
    os.system(f"mkdir -p {d}/{n}/lib")
    os.system(f"mkdir -p {d}/{n}/src")

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
    os.system(f"cp ../neuralspot/ns-core/src/startup_gcc.c {d}/{n}/src/")
    os.system(f"cp autodeploy/templates/linker_script.ld {d}/{n}/src/")

    # Generate model weight file
    xxd_c_dump(
        src_path=params.tflite_filename,
        dst_path=f"{d}/{n}/src/{n}_model_data.h",
        var_name=f"{n}_model",
        chunk_len=12,
        is_header=True,
    )

    # Generate library and example binary
    os.system(f"cd {d}/{n} && make")
