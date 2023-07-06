#!/bin/bash
set -e
echo "Running all makes"
for TF in "R2.3.1" "0c46d6e" "fecdd5d"; do
    for AS in "R4.3.0" "R4.2.0" "R4.1.0"; do
        make -f Makefile clean > /dev/null
        make -f Makefile AS_VERSION=${AS} TF_VERSION=${TF} -j > /dev/null && echo "Make AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make AS_VERSION=$AS TF_VERSION=$TF failed"
        make -f Makefile nestall > /dev/null > /dev/null && echo "Make Nestall AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make Nestall AS_VERSION=$AS TF_VERSION=$TF failed"
        cd nest
        make -f Makefile > /dev/null > /dev/null && echo "Make nest AS_VERSION=$AS TF_VERSION=$TF success" || echo "Make nest AS_VERSION=$AS TF_VERSION=$TF failed"
        cd ..
        test -f "build/examples/rpc_client/rpc_client.bin" || { echo "FAIL rpc client doesn't exist"; exit 1; }
        test -f "build/examples/rpc_server/rpc_server.bin" || { echo "FAIL rpc server doesn't exist"; exit 1; }
        test -f "build/examples/mpu_data_collection/mpu_data_collection.bin" || { echo "FAIL mpu data collection doesn't exist"; exit 1; }
        test -f "build/examples/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL basic TF stub doesn't exist"; exit 1; }
        test -f "nest/build/basic_tf_stub.bin" || { echo "FAIL Nests basic TF stub doesn't exist"; exit 1; }
    done
done
