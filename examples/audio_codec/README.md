# Audio Codec RPC and BLE Example
This example samples audio and sends it to a player, either via RPC-over-USB, or via BLE. Before sending the audio, it encodes it using Opus encoding, which reduces the size of the data by about 75%.

## Defaults
By default, the example:
- Uses PDM microphone 0
- Sends data over BLE if on a BLE-capable EVB, otherwise over USB if the EVB has a USB1 port.

To change these behaviors, modify the audio_codec.cc source code before compile, uncommenting AC_RPC_MODE and/or USE_AUDADC.

## Audio over BLE
This example implements a dead-simple audio-over-BLE protocol: it creates a "notify read" characteristic and starts dumping Opus frames into it. Frame size is hardcoded at 320 PCM samples, and the Opus frame size is 80 bytes.

This example pairs with a web [dashboard](https://github.com/AmbiqAI/web-ble-dashboards/blob/main/audio/index.html) which is configured to look for the service created by this example. Once paired, the web dashboard will play the audio and show corresponding waveforms and spectrograms. This dashboard relies on WebBLE and WebAudio, which are not supported by every browser - it has been tested on recent Chrome browser versions.

![image-20231106120630313](../../docs/images/audio-ble-dashboard.png)



## Audio over RPC
When configured for RPC, this example acts as an RPC client, sending  Opus packets over USB to an RPC server using neuralSPOT's generic data interface. To receive, decode, and record this audio to a WAV file, you can use the opus_receive.py script, found in ../../tools.

```bash
python3 -m opus_receive -t /dev/tty.usbmodem1234561 -o myaudio.wav
```
