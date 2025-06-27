# Speaker Identification Demo
This example shows how to use Ambiq's [NNID and NNVAD](https://github.com/AmbiqAI/nnid) to create a speaker identification application.

Speaker identification is the process of identifying speech as matching that of an enrolled speaker - for each captured speech snippet over about 2 seconds, NNID will calculate how closely it correlates with enrolled speakers and assign it a score between 0 and 1 - the closer to '1' the better the correlation. For purposes of the demo, we consider any score over .7 to be a positive correlation. To detect speech snippets (aka 'utterances') of sufficient length, a neural network VAD (voice activity detection) model is used alongside the NNID model.

The demo includes two parts: enrollment and identification. It starts with no enrolled speakers, and at least one speaker must be enrolled for identification to be attempted.

This example is meant to be used with a Web BLE client such as this one: https://ambiqai.github.io/web-ble-dashboards/nnid/

The web BLE client serves as the user interface. It offers buttons to enroll and identify users, and visualizes the audio and VAD state. Note that while audio is being encoded and transmitted to the client, this is only for visualization purposes.

## Running the Demo

The demo needs a BLE-enabled EVB (Apollo4 Plus KXR/KBR Blue, Apollo4 Lite Blue, or Apollo3P Blue), a microphone (Audio Shield PDM or analog mic (AP4 only)), and a PC or laptop running Chrome. It supports both PDM and AUDADC microphones, but is configured for PDM by default. To switch to AUDADC, uncomment this line in nnid/src/nnid.cc:

```c
// #define USE_AUDADC // Uncomment this to use the AUDADC instead of the PDM
```

## Running the demo
First, flash
```bash
$> make clean
$> make -j
$> make EXAMPLE=demos/nnid deploy
```

With the firmware deployed, follow this link on a PC using the Chrome browser: [Audio WebBLE Demo](https://ambiqai.github.io/web-ble-dashboards/nnid/)

Click on the 'Pair and start listening...' button to bring up a list of compatible bluetooth devices - one should be labeled 'Packet': that is the BLE device created by the NNID firmware. Once paired, the web dashboard will display the messages described above.