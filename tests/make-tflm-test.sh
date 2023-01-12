#!/bin/bash
set -e
echo "Running all makes"
for TF in "0c46d6e" "fecdd5d"; do
    for FLAG in "MLDEBUG=1" "MLPROFILE=1" "MLPROFILE=1 ML_DEBUG=1"; do
        make -f Makefile clean > /dev/null
        make -f Makefile TF_VERSION=${TF} ${FLAG} -j > /dev/null && echo "Make TF_VERSION=$TF $FLAG success" || echo "Make TF_VERSION=$TF $FLAG failed"
        test -f "build/examples/rpc_client_example/rpc_client.bin" || { echo "FAIL rpc client doesn't exit"; exit 1; }
        test -f "build/examples/rpc_server_example/rpc_server.bin" || { echo "FAIL rpc server doesn't exit"; exit 1; }
        test -f "build/examples/mpu_data_collection/mpu_data_collection.bin" || { echo "FAIL mpu data collection doesn't exist"; exit 1; }
        test -f "build/examples/basic_tf_stub/basic_tf_stub.bin" || { echo "FAIL basic TF stub doesn't exist"; exit 1; }
    done
done
