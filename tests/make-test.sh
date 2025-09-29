#!/bin/bash
set -e

echo "Running all makes for Apollo510 and Apollo510b"
for TC in "arm" "arm-none-eabi"; do
    for PL in "apollo510_evb" "apollo510b_evb"; do
        for TF in "helios_rt_v1_3_0" "Oct_08_2024_e86d97b6"; do
            for AS in "R5.1.0_rc27"; do
                echo "Running: make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} clean > /dev/null
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} -j > /dev/null && echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} nestall > /dev/null > /dev/null && echo "Make Nestall TOOLCHAIN=${TC} PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL Nestall TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd nest
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC}  AS_VERSION=${AS} TF_VERSION=${TF}  > /dev/null > /dev/null && echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd ..
                test -f "build/${PL}/${TC}/apps/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL build/${PL}/${TC}/basic_tf_stub.bin doesn't exist"; exit 1; }
                test -f "nest/build/${PL}/${TC}/basic_tf_stub.bin" || { echo "FAIL Nests nest/build/${PL}/${TC}/basic_tf_stub.bin doesn't exist"; exit 1; }
            done
        done
    done
done

echo "Running all makes for Apollo510 R5.3.0"
for TC in "arm" "arm-none-eabi"; do
    for PL in "apollo510_evb"; do
        for TF in "helios_rt_v1_3_0" "Oct_08_2024_e86d97b6"; do
            for AS in "R5.3.0"; do
                echo "Running: make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} clean > /dev/null
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} -j > /dev/null && echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} nestall > /dev/null > /dev/null && echo "Make Nestall TOOLCHAIN=${TC} PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL Nestall TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd nest
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC}  AS_VERSION=${AS} TF_VERSION=${TF}  > /dev/null > /dev/null && echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd ..
                # The directory structure is different for R5.2.0, set a var to make it easier to test
                if [ "$AS" = "R5.2.0" ]; then
                    DIR="apollo5b_evb"
                else
                    DIR="apollo510_evb"
                fi
                test -f "build/${DIR}/${TC}/apps/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL build/${DIR}/${TC}/basic_tf_stub.bin doesn't exist"; exit 1; }
                test -f "nest/build/${DIR}/${TC}/basic_tf_stub.bin" || { echo "FAIL Nests nest/build/${DIR}/${TC}/basic_tf_stub.bin doesn't exist"; exit 1; }
            done
        done
    done
done




echo "Running all makes for Apollo4"

for TC in "arm" "arm-none-eabi"; do
    for PL in "apollo4l_evb" "apollo4l_blue_evb" "apollo4p_evb" "apollo4p_blue_kbr_evb" "apollo4p_blue_kxr_evb"; do
        for TF in "helios_rt_v1_3_0"; do
            for AS in "R4.4.1" "R4.5.0" ; do
            # for AS in  "R4.5.0" ; do
                echo "Running: make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} clean > /dev/null
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} -j > /dev/null && echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} nestall > /dev/null > /dev/null && echo "Make Nestall TOOLCHAIN=${TC} PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL Nestall TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd nest
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC}  AS_VERSION=${AS} TF_VERSION=${TF}  > /dev/null > /dev/null && echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd ..
                test -f "build/${PL}/${TC}/apps/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL basic TF stub doesn't exist"; exit 1; }
                test -f "nest/build/${PL}/${TC}/basic_tf_stub.bin" || { echo "FAIL Nests basic TF stub doesn't exist"; exit 1; }
            done
        done
    done
done

echo "Running all makes for Apollo3"

for TC in "arm" "arm-none-eabi"; do
    for PL in "apollo3p_evb"; do
        for TF in "helios_rt_v1_2_0"; do
            for AS in  "R3.1.1" ; do
                echo "Running: make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} clean > /dev/null
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} -j > /dev/null && echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC} AS_VERSION=${AS} TF_VERSION=${TF} nestall > /dev/null > /dev/null && echo "Make Nestall TOOLCHAIN=${TC} PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL Nestall TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd nest
                make -f Makefile PLATFORM=${PL} TOOLCHAIN=${TC}  AS_VERSION=${AS} TF_VERSION=${TF}  > /dev/null > /dev/null && echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make nest PLATFORM=$PL TOOLCHAIN=${TC} AS_VERSION=$AS TF_VERSION=$TF failed"
                cd ..
                test -f "build/${PL}/${TC}/apps/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL basic TF stub doesn't exist"; exit 1; }
                test -f "nest/build/${PL}/${TC}/basic_tf_stub.bin" || { echo "FAIL Nests basic TF stub doesn't exist"; exit 1; }
            done
        done
    done
done

