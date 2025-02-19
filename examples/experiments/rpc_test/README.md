# RPC Generic Data Operations Server Test
This example is meant to be used in conjuction with ../../tools/rpc_test.py to stress-test the RPC interface.

It won't be compiled by 'make' by default. To compile this example, do the following:

```bash
$> make EXAMPLE=rpc_test
$> make EXAMPLE=rpc_test TARGET=rpc_test deploy
```

Once the test is flashed on an EVB, it can be exercised using the rpc_test.py script located in neuralSPOT/tools:

```bash
$> cd tools
$> python -m rpc_test -t /dev/tty.usbmodem1234561
```
