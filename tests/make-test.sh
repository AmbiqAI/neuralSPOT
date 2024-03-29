#!/bin/bash
set -e


echo "Running all makes for Apollo4 Lite"
for PL in "apollo4l_evb" "apollo4l_blue_evb"; do
    for TF in "d5f819d_Aug_10_2023" "0264234_Nov_15_2023" "ce72f7b8_Feb_17_2024" "fecdd5d"; do
        for AS in "R4.4.1" ; do
            make -f Makefile PLATFORM=${PL} clean > /dev/null
            make -f Makefile PLATFORM=${PL} AS_VERSION=${AS} TF_VERSION=${TF} -j > /dev/null && echo "Make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF failed"
            make -f Makefile PLATFORM=${PL} AS_VERSION=${AS} TF_VERSION=${TF} nestall > /dev/null > /dev/null && echo "Make Nestall PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL Nestall AS_VERSION=$AS TF_VERSION=$TF failed"
            cd nest
            make -f Makefile PLATFORM=${PL}  AS_VERSION=${AS} TF_VERSION=${TF}  > /dev/null > /dev/null && echo "Make nest PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make nest PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF failed"
            cd ..
            test -f "build/${PL}/arm-none-eabi/examples/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL basic TF stub doesn't exist"; exit 1; }
            test -f "nest/build/${PL}/arm-none-eabi/basic_tf_stub.bin" || { echo "FAIL Nests basic TF stub doesn't exist"; exit 1; }
        done
    done
done

echo "Running all makes for Apollo4p"
for PL in "apollo4p_evb" "apollo4p_blue_kbr_evb" "apollo4p_blue_kxr_evb"; do
    for TF in "d5f819d_Aug_10_2023" "0264234_Nov_15_2023" "ce72f7b8_Feb_17_2024" "fecdd5d"; do
        for AS in "R4.4.1" "R4.3.0"; do
        # for AS in "R4.3.0" "R4.2.0" "R4.1.0"; do
            make -f Makefile clean > /dev/null
            make -f Makefile PLATFORM=${PL} AS_VERSION=${AS} TF_VERSION=${TF} -j > /dev/null && echo "Make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF failed"
            make -f Makefile PLATFORM=${PL} AS_VERSION=${AS} TF_VERSION=${TF} nestall > /dev/null > /dev/null && echo "Make Nestall PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make Nestall PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF failed"
            cd nest
            make -f Makefile PLATFORM=${PL} AS_VERSION=${AS} TF_VERSION=${TF} > /dev/null > /dev/null && echo "Make nest PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make nest PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF failed"
            cd ..
            test -f "build/${PL}/arm-none-eabi/examples/rpc_client/rpc_client.bin" || { echo "FAIL rpc client doesn't exist"; exit 1; }
            test -f "build/${PL}/arm-none-eabi/examples/rpc_server/rpc_server.bin" || { echo "FAIL rpc server doesn't exist"; exit 1; }
            test -f "build/${PL}/arm-none-eabi/examples/mpu_data_collection/mpu_data_collection.bin" || { echo "FAIL mpu data collection doesn't exist"; exit 1; }
            test -f "build/${PL}/arm-none-eabi/examples/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL basic TF stub doesn't exist"; exit 1; }
            test -f "nest/build/${PL}/arm-none-eabi/basic_tf_stub.bin" || { echo "FAIL Nests basic TF stub doesn't exist"; exit 1; }
        done
    done
done


echo "Running all makes for Apollo3p"
for PL in "apollo3p_evb"; do
    for TF in "d5f819d_Aug_10_2023" "0264234_Nov_15_2023" "ce72f7b8_Feb_17_2024" "fecdd5d"; do
        for AS in "R3.1.1"; do
            make -f Makefile clean > /dev/null
            make -f Makefile PLATFORM=${PL} AS_VERSION=${AS} TF_VERSION=${TF} -j > /dev/null && echo "Make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF failed"
            make -f Makefile PLATFORM=${PL} AS_VERSION=${AS} TF_VERSION=${TF} nestall > /dev/null > /dev/null && echo "Make Nestall PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make Nestall PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF failed"
            cd nest
            make -f Makefile PLATFORM=${PL} AS_VERSION=${AS} TF_VERSION=${TF} > /dev/null > /dev/null && echo "Make nest PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make nest PLATFORM=$PL AS_VERSION=$AS TF_VERSION=$TF failed"
            cd ..
            test -f "build/${PL}/arm-none-eabi/examples/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL basic TF stub doesn't exist"; exit 1; }
            test -f "nest/build/${PL}/arm-none-eabi/basic_tf_stub.bin" || { echo "FAIL Nests basic TF stub doesn't exist"; exit 1; }
        done
    done
done
