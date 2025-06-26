# NeuralSPOT Applications Overview

The `apps` directory is the **showroom** for NeuralSPOT SDK.  Every sub‑folder contains a ready‑to‑flash application that highlights one or more SDK features.  The layout is intentionally simple:

* **basic\_tf\_stub/** – A kitchen‑sink reference application that combines many NeuralSPOT building blocks in one place.
* **ai/** – End‑to‑end AI reference apps (stand‑alone KWS, HAR, …).
* **demos/** – Polished demos that pair on‑device code with BLE or WebUSB dashboards.
* **examples/** – Minimal, focused examples that illustrate a single concept or peripheral.

Each application carries its own README with full build/run instructions.  The table below is a bird’s‑eye view and lists the main functionality each app **demonstrates**.

| App                                                 | Description                                                  | Demonstrates                              | AP5  | AP4  | AP4 Lite | AP3  |
| --------------------------------------------------- | ------------------------------------------------------------ | ----------------------------------------- | :--: | :--: | :------: | :--: |
| [basic\_tf\_stub](basic_tf_stub)                    | Kitchen‑sink audio KWS example that integrates audio capture, MFCC, TensorFlow, RPC/IPC, power management and USB data dump. | AUDADC, MFCC, TFLM, RPC, IPC, Power Mgmt  |  Y   |  Y   |    Y     |  Y   |
| **ai**                                              |                                                              |                                           |      |      |          |      |
| [ai/kws](ai/kws)                                    | Keyword spotting model based on MLCommons KWS.               | Real‑time audio capture, TFLM inference   |  Y   |  Y   |    Y     |  Y   |
| [ai/har](ai/har)                                    | Human Activity Recognition using IMU data.                   | IMU driver, sliding‑window classification |  Y   |  Y   |    Y     |  Y   |
| **demos**                                           |                                                              |                                           |      |      |          |      |
| [demos/ic](demos/ic)                                | Image classification with WebUSB UI.                         | Camera capture, WebUSB dashboard, TFLM    |  Y   |  Y   |    Y     |  N   |
| [demos/nnid](demos/nnid)                            | Speaker identification with WebBLE UI                        | BLE audio, VAD, Web BLE UI                |  Y   |  Y   |    Y     |  Y   |
| [demos/nnse](demos/nnse)                            | Speech‑enhancing WebBLE microphone                           | NNSE denoiser, Opus codec, BLE streaming  |  Y   |  Y   |    Y     |  N   |
| [demos/nnse\_usb](demos/nnse_usb)                   | Speech‑enhancing WebUSB microphone                           | NNSE denoiser, Opus codec, WebUSB audio   |  Y   |  Y   |    Y     |  N   |
| **examples**                                        |                                                              |                                           |      |      |          |      |
| [examples/icm](examples/icm)                        | ICM‑45605 sensor capture                                     | ns‑imu driver, interrupt frame buffer     |  Y   |  Y   |    Y     |  Y   |
| [examples/pmu\_profiling](examples/pmu_profiling)   | PMU counter collection utility                               | Tick Timer, basic/full PMU, MFCC workload |  Y   |  N   |    N     |  N   |
| [examples/quaternion](examples/quaternion)          | Real‑time quaternion visualizer                              | ns‑features, RPC streaming, matplotlib UI |  Y   |  Y   |    Y     |  Y   |
| [examples/rpc\_evb\_to\_pc](examples/rpc_evb_to_pc) | eRPC EVB→PC server example                                   | Remote compute on EVB, USB transport      |  Y   |  Y   |    N     |  N   |
| [examples/rpc\_pc\_to\_evb](examples/rpc_pc_to_evb) | eRPC PC→EVB server example                                   | Remote compute on PC, USB transport       |  Y   |  Y   |    N     |  N   |
| [examples/tflm\_profiling](examples/tflm_profiling) | TFLM per‑layer profiler                                      | microProfiler, GPIO pulse, SWO dump       |  Y   |  Y   |    Y     |  Y   |
| [examples/uart](examples/uart)                      | UART round‑trip test                                         | ns‑uart driver, blocking/non‑blocking I/O |  Y   |  Y   |    Y     |  Y   |
| [examples/vision](examples/vision)                  | SPI camera capture & WebUSB view                             | ns‑camera, WebUSB image stream            |  Y   |  Y   |    Y     |  N   |
