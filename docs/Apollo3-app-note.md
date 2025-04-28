# AI on Apollo3 with neuralSPOT

[Apollo3](https://ambiq.com/apollo3-blue-plus/) and Apollo4 share many similarities, but there are enough differences pertaining to AI workloads to merit a short app note. In summary, the main application-visible differences are:

1. Clock Speeds - Apollo4 CPU clocks (96Mhz/192Mhz) are 2x faster than Apollo3's (48Mhz/96Mhz)
2. TCM (tightly coupled memory) differs in two ways
   1. It is smaller (64KB on AP3 vs 384KB on AP4)
   2. The linker locates data in SRAM by default on AP3 (it uses TCM by default on AP4)

Taken together, this means that applications that take full advantage of AP4's performance will not work AP3, at least not without modification. For example, the "de-noising BLE microphone" example (NNSE) uses about 50% of AP4's cycles for the neural network, and another 20% for audio encoding in order to transmit the result over BLE. While it may be possible to modify this example to not use an encoder and send uncompressed PCM instead, this experiment hasn't been tried yet.

Most of neuralSPOT's work just fine on AP3, just a little slower - because of peripheral assists such as DMA, in most cases this slowdown can be absorbed without problem.

As in other platforms, AP3 offers a different mix of peripherals, some of which are currently relied on by some neuralSPOT functionality. For example, the lack of USB means that neuralSPOT's autodeploy scripts must use the `--transport UART` option to use the UART interface instead.

See the [Feature](https://github.com/AmbiqAI/neuralSPOT/blob/main/docs/features.md) document for compatibility and supported hardware versions.

See the [Example README](https://github.com/AmbiqAI/neuralSPOT/blob/main/examples/README.md) for details about which examples work on which platforms.