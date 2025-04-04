#!/bin/bash
set -e


echo "Running all makes for Apollo5"
for TC in "arm" "arm-none-eabi"; do
    for PL in "apollo510_evb"; do
        for TF in "ns_tflm_2025_03_19"; do
            for AS in  "R5.2.0" ; do
                echo "Running: make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} clean > /dev/null
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} -j > /dev/null && echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} nestall > /dev/null > /dev/null && echo "Make Nestall TOOLCHAIN=${TC} PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL Nestall TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd nest
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC}  AS_VERSION=${AS} TF_VERSION=${TF}  > /dev/null > /dev/null && echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd ..
                test -f "build/apollo5b_evb/${TC}/examples/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL basic TF stub doesn't exist"; exit 1; }
                test -f "nest/build/apollo5b_evb/${TC}/basic_tf_stub.bin" || { echo "FAIL Nests basic TF stub doesn't exist"; exit 1; }
            done
        done
    done
done

echo "Running all makes for Apollo4"

for TC in "arm" "arm-none-eabi"; do
    for PL in "apollo4l_evb" "apollo4l_blue_evb" "apollo4p_evb" "apollo4p_blue_kbr_evb" "apollo4p_blue_kxr_evb"; do
    # for PL in "apollo4p_evb" "apollo4p_blue_kbr_evb" "apollo4p_blue_kxr_evb"; do
        for TF in "ns_tflm_2025_03_19"; do
            # for AS in "R4.4.1" "R4.5.0" ; do
            for AS in  "R4.5.0" ; do
                echo "Running: make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} clean > /dev/null
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} -j > /dev/null && echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} nestall > /dev/null > /dev/null && echo "Make Nestall TOOLCHAIN=${TC} PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL Nestall TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd nest
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC}  AS_VERSION=${AS} TF_VERSION=${TF}  > /dev/null > /dev/null && echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd ..
                test -f "build/${PL}/${TC}/examples/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL basic TF stub doesn't exist"; exit 1; }
                test -f "nest/build/${PL}/${TC}/basic_tf_stub.bin" || { echo "FAIL Nests basic TF stub doesn't exist"; exit 1; }
            done
        done
    done
done

echo "Running all makes for Apollo3"

for TC in "arm" "arm-none-eabi"; do
    # for PL in "apollo4l_evb" "apollo4l_blue_evb" "apollo4p_evb" "apollo4p_blue_kbr_evb" "apollo4p_blue_kxr_evb"; do
    for PL in "apollo3p_evb"; do
        for TF in "ns_tflm_2025_03_19"; do
            for AS in  "R3.1.1" ; do
                echo "Running: make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} clean > /dev/null
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} -j > /dev/null && echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} nestall > /dev/null > /dev/null && echo "Make Nestall TOOLCHAIN=${TC} PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL Nestall TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd nest
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC}  AS_VERSION=${AS} TF_VERSION=${TF}  > /dev/null > /dev/null && echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd ..
                test -f "build/${PL}/${TC}/examples/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL basic TF stub doesn't exist"; exit 1; }
                test -f "nest/build/${PL}/${TC}/basic_tf_stub.bin" || { echo "FAIL Nests basic TF stub doesn't exist"; exit 1; }
            done
        done
    done
done

