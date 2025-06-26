# Speaker Identification Demo
This example shows how to use Ambiq's [NNID and NNVAD](https://github.com/AmbiqAI/nnid) to create a speaker identification application.

Speaker identification is the process of identifying speech as matching that of an enrolled speaker - for each captured speech snippet over about 2 seconds, NNID will calculate how closely it correlates with enrolled speakers and assign it a score between 0 and 1 - the closer to '1' the better the correlation. For purposes of the demo, we consider any score over .7 to be a positive correlation. To detect speech snippets (aka 'utterances') of sufficient length, a neural network VAD (voice activity detection) model is used alongside the NNID model

The demo includes two parts: enrollment and identification. It starts with no enrolled speakers, and at least one speaker must be enrolled for identification to be attempted.

This example is meant to be used with a Web BLE client such as this one: https://ambiqai.github.io/web-ble-dashboards/nnid/

The web BLE client serves as the user interface. It offers buttons to enroll and identify users, and visualizes the audio and VAD state. Note that while audio is being encoded and transmitted to the client, this is only for visualization purposes.

## Running the Demo

The demo needs a BLE-enabled device (Apollo4 Plus KXR/KBR Blue, Apollo4 Lite Blue, or Apollo3P Blue), a microphone, and a PC or laptop running Chrome. It supports both PDM and AUDADC microphones, but is configured for PDM by default. To switch to AUDADC, uncomment this line in nnse/src/nnse.cc:

```c
// #define USE_AUDADC // Uncomment this to use the AUDADC instead of the PDM
``````


First, flash
```bash
$> make clean
$> make -j
$> make TARGET=nnse deploy
```

With the firmware deployed, follow this link on a PC using the Chrome browser: [Audio WebBLE Demo](https://ambiqai.github.io/web-ble-dashboards/nnid/)

Click on the 'Pair and start listening...' button to bring up a list of compatible bluetooth devices - one should be labeled 'Packet': that is the BLE device created by the NNSE firmware. Once paired, the web dashboard will display the messages described above.

**DEPRECATED**
The following instructions are for a previous version of this demo that used the physical EVB buttons rather than the web dashboard. The physical button code is still in the source code, but isn't used. The web client is self guided, and a better way to interact with this demo.

## Enrollment
Press Button 1 on the EVB to trigger enrollement. Enrollment consists of capturing 4 utterances from a single speaker. These don't have to be any particular phrase or word: the only requirements are that they are at least 2 seconds long, and that they're fairly constant, with no long pauses in between. Once enrollment is started for a speaker, all 4 utterances must be captured to move on to the next speaker, or to switch to identification.

1. Press Button 1 - enrollment commences
2. Speak until the application indicates an enrollment has been captured
3. Press Button 1 again when ready to speak the next utterance
4. Continue until 4 utterances have been captured.

This can be done for up to 5 unique speakers.

## Idenfication
Press Button 0 to trigger identification. The EVB will listen for voice activity, and once an utterance over 2 seconds is detected, it will attempt to match it to one of the enrolled speakers. If a match of over 70% is detected, it will print out the speaker ID of the matching speaker. Otherwise, it will indicate that a match failed.

## Running the Demo

The demo needs a BLE-enabled device (Apollo4 Plus KXR/KBR Blue, or Apollo4 Lite Blue), a microphone, and a PC or laptop running Chrome. It supports both PDM and AUDADC microphones, but is configured for PDM by default. To switch to AUDADC, uncomment this line in nnse/src/nnse.cc:

```c
// #define USE_AUDADC // Uncomment this to use the AUDADC instead of the PDM
``````


First, flash
```bash
$> make clean
$> make -j
$> make TARGET=nnse deploy
```

With the firmware deployed, follow this link on a PC using the Chrome browser: [Audio WebBLE Demo](https://ambiqai.github.io/web-ble-dashboards/nnid/)

Click on the 'Pair and start listening...' button to bring up a list of compatible bluetooth devices - one should be labeled 'Packet': that is the BLE device created by the NNSE firmware. Once paired, the web dashboard will display the messages described above.
