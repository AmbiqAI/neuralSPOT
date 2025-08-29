"""
Audio Viewer for the audio data from EVB
"""
import os
import argparse
import sys
import wave
import multiprocessing
from multiprocessing import Process, Array, Lock
import time
import erpc
import GenericDataOperations_EvbToPc
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Button, Slider
import scipy.io.wavfile as wavfile
import pyaudio
# Define the RPC service handlers - one for each EVB-to-PC RPC function

FRAMES_TO_SHOW  = 500
SAMPLING_RATE   = 8000
HOP_SIZE        = 80
PLAY_AUDIO      = True

class DataServiceClass:
    """
    Capture Audio data: EVB->PC
    """
    def __init__(
            self,
            databuf_raw,
            databuf_enhance,
            wavout,
            lock,
            is_record,
            cyc_count,
            ch_select,
            gain_play_db,
            playback=0):

        self.sp_types=["raw", "enhance"]
        self.cyc_count      = cyc_count
        self.wavefile       = None
        self.wavename       = wavout
        self.databuf = {}
        self.databuf["raw"]    = databuf_raw
        self.databuf['enhance']= databuf_enhance
        self.lock           = lock
        self.is_record      = is_record
        self.ch_select      = ch_select
        self.gain_play_db   = gain_play_db
        self.playback       = playback

    def wavefile_init(self, wavename):
        """
        wavefile initialization
        """
        fldr = 'audio_result'
        os.makedirs(fldr, exist_ok=True)
        wavefile = wave.open(f'{fldr}/{wavename}', 'wb')
        wavefile.setnchannels(2)
        wavefile.setsampwidth(2)
        wavefile.setframerate(SAMPLING_RATE)
        return wavefile

    def ns_rpc_data_sendBlockToPC(self, pcmBlock): # pylint: disable=invalid-name
        """
        pcmBlock: real, real,..., imag, imag, imag,...
        callback function that data sent from EVB to PC.
        """
        self.lock.acquire()
        is_record = self.is_record[0]
        self.lock.release()
        if is_record == 0:
            if self.wavefile:
                self.wavefile.close()

                samplerate, sig = wavfile.read("audio_result/audio.wav")
                sig1 = sig[:,0].flatten()
                wavfile.write("audio_result/audio_raw.wav", samplerate, sig1.astype(np.int16))

                sig2 = sig[:,1].flatten()
                wavfile.write("audio_result/audio_se.wav", samplerate, sig2.astype(np.int16))

                self.wavefile = None
                print('Stop recording')

                if self.playback:
                    self.stream.close()
                    self.pau.terminate()
        else:
            # The data 'block' (in C) is defined below:
            # static char msg_store[30] = "Audio16bPCM_to_WAV";

            # // Block sent to PC
            # static dataBlock outBlock = {
            #     .length = SAMPLES_IN_FRAME * sizeof(int16_t),
            #     .dType = uint8_e,
            #     .description = msg_store,
            #     .cmd = write_cmd,
            #     .buffer = {.data = (uint8_t *)in16AudioDataBuffer, // point this to audio buffer # pylint: disable=line-too-long
            #             .dataLength = SAMPLES_IN_FRAME * sizeof(int16_t)}};

            if self.wavefile:
                self.lock.acquire()
                cyc_count = self.cyc_count[0]
                self.lock.release()
            else:
                print('Start recording')
                cyc_count = 0

                self.lock.acquire()
                self.cyc_count[0] = cyc_count
                self.lock.release()

                self.wavefile = self.wavefile_init(self.wavename)

                if self.playback:
                    self.pau = pyaudio.PyAudio()
                    self.stream = self.pau.open(
                                    format = self.pau.get_format_from_width(2),
                                    channels = 1,
                                    rate = SAMPLING_RATE,
                                    output = True)

            if (pcmBlock.cmd == GenericDataOperations_EvbToPc.common.command.write_cmd) \
                     and (pcmBlock.description == "Audio16bPCM_to_WAV"):

                self.lock.acquire()
                data = np.frombuffer(pcmBlock.buffer, dtype=np.int16).copy()
                ch = self.ch_select[0]
                self.lock.release()

                data_reshape = data.reshape((2, HOP_SIZE)).copy()
                data_flatten = data_reshape.copy().T.flatten()
                self.wavefile.writeframesraw(data_flatten.tobytes())

                if self.playback:
                    self.stream.write(data_reshape[ch].tobytes())
                start = cyc_count * HOP_SIZE

                self.lock.acquire()
                for i, sp_type in enumerate(self.sp_types):
                    self.databuf[sp_type][start:start+HOP_SIZE] = data_reshape[i]/32768.0

                self.lock.release()

                cyc_count = (cyc_count+1) % FRAMES_TO_SHOW

                self.lock.acquire()
                self.cyc_count[0] = cyc_count
                self.lock.release()

        sys.stdout.flush()

        return 0

    def ns_rpc_data_fetchBlockFromPC(self, block): # pylint: disable=invalid-name, unused-argument
        """
        callback function that Data fetching
        """
        sys.stdout.flush()
        return 0

    def ns_rpc_data_computeOnPC( # pylint: disable=invalid-name
            self,
            in_block,       # like a request block from EVB
            IsRecordBlock):  # send the result_block to EVB
        """
        callback function that sending result_block to EVB
            that indicating to record or stop
        """
        self.lock.acquire()
        is_record = self.is_record[0]
        gain_play = self.gain_play_db[0]
        self.lock.release()
        gain_play = np.int8(10**(gain_play / 20)) * 2 # in Q7.1 format

        if (in_block.cmd == GenericDataOperations_EvbToPc.common.command.extract_cmd) and (
            in_block.description == "CalculateMFCC_Please"):

            data2pc = [is_record, gain_play]
            IsRecordBlock.value = GenericDataOperations_EvbToPc.common.dataBlock(
                description ="*\0",
                dType       = GenericDataOperations_EvbToPc.common.dataType.uint8_e,
                cmd         = GenericDataOperations_EvbToPc.common.command.generic_cmd,
                buffer      = bytearray(data2pc),
                length      = len(bytearray(data2pc)),
            )
        sys.stdout.flush()
        return 0

class VisualDataClass:
    """
    Visual the audio data from EVB
    """
    def __init__(
            self,
            databuf_raw,
            databuf_enhance,
            lock, is_record,
            event_stop,
            cyc_count,
            ch_select,
            gain_play_db):

        self.sp_types = ["raw", "enhance"]
        self.databuf = {}
        self.databuf["raw"]        = databuf_raw
        self.databuf["enhance"]    = databuf_enhance
        self.lock    = lock
        self.is_record = is_record
        self.event_stop = event_stop
        self.cyc_count = cyc_count
        self.ch_select = ch_select
        self.gain_play_db = gain_play_db
        secs2show = FRAMES_TO_SHOW * HOP_SIZE/SAMPLING_RATE
        self.xdata = np.arange(FRAMES_TO_SHOW * HOP_SIZE) / SAMPLING_RATE
        self.fig, self.ax_handle = plt.subplots(2,1)
        self.ax_handle[0].title.set_text("VAD Viewer")
        self.fig.canvas.mpl_connect(
            'close_event',
            self.handle_close)
        self.fig.subplots_adjust(
            bottom=0.35,
            hspace=0.1)
        self.ax_handle[0].text(-0.1,0,"Raw",     ha='right')
        self.ax_handle[1].text(-0.1,0,"VAD Info", ha='right')

        self.lock.acquire()
        np_databuf={}
        for sp_type in self.sp_types:
            np_databuf[sp_type] = self.databuf[sp_type][0:]
        ch_select = self.ch_select[0]
        self.lock.release()

        self.line_data = {}
        for i, sp_type in enumerate(self.sp_types):
            self.ax_handle[i].yaxis.tick_right()
            self.line_data[sp_type], = self.ax_handle[i].plot(
                self.xdata, np_databuf[sp_type], lw=0.5, color = 'blue')
            self.ax_handle[i].set_ylim([-1.0,1.0])
            self.ax_handle[i].set_xlim((0, secs2show))
            self.set_plot_color(ch_select, i, self.line_data[sp_type])

        self.ax_handle[0].tick_params(bottom = False)
        self.ax_handle[0].set_xticklabels([])
        self.ax_handle[-1].set_xlabel('Time (Seconds)')
        # making buttons
        def make_button(pos, name, callback_func):
            ax_button = plt.axes(pos)
            button = Button(
                        ax_button,
                        name,
                        color = 'w',
                        hovercolor = 'aliceblue')
            button.label.set_fontsize(16)
            button.on_clicked(callback_func)
            return button

        self.wavfile = None
        self.button_stop = make_button(
                            [0.35, 0.15, 0.14, 0.075],
                            'stop',
                            self.callback_recordstop)

        self.button_record = make_button(
                            [0.5, 0.15, 0.14, 0.075],
                            'record',
                            self.callback_recordstart)

        self.button_enhance = make_button(
                            [0.75, 0.05, 0.05, 0.075],
                            'E',
                            self.callback_enhance)

        self.button_raw = make_button(
                            [0.81, 0.05, 0.05, 0.075],
                            'R',
                            self.callback_raw)
        self.text_info = plt.text(1.2,0.4, "enhance")
        self.text_info.set_text("enhance")

        # Make a horizontal slider to control the frequency.
        ax_play_db_slider = self.fig.add_axes([0.25, 0.1, 0.2, 0.03])
        self.lock.acquire()
        gain_play = self.gain_play_db[0]
        self.lock.release()
        self.gain_play_db_slider = Slider(
            ax=ax_play_db_slider,
            label='gain [dB]  ',
            valmin=0,
            valmax=40,
            valinit=gain_play,
        )
        if ch_select==1:
            self.set_button_color(
                    self.button_enhance,
                    [self.button_raw])
        else:
            self.set_button_color(
                self.button_raw,
                [self.button_enhance])
        plt.show()

    def set_plot_color(self, ch_select, indx, ax_handle):
        """ 
        set the color of the plot
        """
        if ch_select==indx:
            color = "blue"
        else:
            color = "gray"
        ax_handle.set_color(color)

    def stop_processes(self):
        """
        some necessary things when you stop the process 
        """
        self.lock.acquire()
        self.is_record[0] = 0
        for sp_type in self.sp_types:
            self.databuf[sp_type][0:] = np.zeros((FRAMES_TO_SHOW * HOP_SIZE,))
        self.cyc_count[0] = 0
        self.lock.release()

    def handle_close(self, event): # pylint: disable=unused-argument
        """
        Finish everything when you close your plot
        """
        self.lock.acquire()
        is_record = self.is_record[0]
        self.lock.release()
        print('Window close')
        if is_record == 1:
            self.stop_processes()
            time.sleep(0.05)
        self.event_stop.set() # let main function know program should be terminated now

    def callback_recordstop(self, event):
        """
        for stop button
        """
        self.set_button_color(
                self.button_stop,
                [self.button_record])
        self.stop_processes()
        if event.inaxes is not None:
            event.inaxes.figure.canvas.draw_idle()

    def callback_enhance(self, event):
        """
        for enhance button
        """
        self.set_button_color(
                self.button_enhance,
                [self.button_raw])
        self.lock.acquire()
        self.ch_select[0] = 1
        self.lock.release()
        self.text_info.set_text("enhance")
        if event.inaxes is not None:
            event.inaxes.figure.canvas.draw_idle()

    def callback_raw(self, event):
        """
        for raw button
        """
        self.set_button_color(
                self.button_raw,
                [self.button_enhance])
        self.lock.acquire()
        self.ch_select[0] = 0
        self.lock.release()
        self.text_info.set_text("raw")
        if event.inaxes is not None:
            event.inaxes.figure.canvas.draw_idle()

    def set_button_color(self, button_t, buttons_all):
        """ 
        set the color of the button
        """
        for button in buttons_all:
            button.color="w"
        button_t.color="aliceblue"

    def callback_recordstart(self, event):
        """
        for record button
        """
        self.lock.acquire()
        is_record = self.is_record[0]
        self.lock.release()
        if is_record == 0:
            self.set_button_color(
                self.button_record,
                [self.button_stop])
            self.lock.acquire()
            self.is_record[0] = 1
            self.lock.release()
            while 1:
                self.lock.acquire()
                cyc_count = self.cyc_count[0]
                ch_select = self.ch_select[0]
                self.gain_play_db[0] = self.gain_play_db_slider.val
                np_databuf = {}
                for sp_type in self.sp_types:
                    np_databuf[sp_type] = self.databuf[sp_type][0:].copy()
                self.lock.release()

                zeros_tail = [0.0] * (HOP_SIZE * (FRAMES_TO_SHOW - cyc_count))
                for i, sp_type in enumerate(self.sp_types):
                    np_databuf[sp_type] = np_databuf[sp_type][:HOP_SIZE*cyc_count] + zeros_tail.copy()
                    self.line_data[sp_type].set_data(self.xdata, np_databuf[sp_type])
                    self.set_plot_color(ch_select, i, self.line_data[sp_type])
                plt.pause(0.05)
                self.lock.acquire()
                is_record = self.is_record[0]
                self.lock.release()
                if is_record == 0:
                    break
        if event.inaxes is not None:
            event.inaxes.figure.canvas.draw_idle()

def target_proc_draw(
        databuf_raw,
        databuf_enhance,
        lock,
        recording,
        event_stop,
        cyc_count,
        ch_select,
        gain_play_db):
    """
    one of multiprocesses: draw
    """
    VisualDataClass(
        databuf_raw,
        databuf_enhance,
        lock,
        recording,
        event_stop,
        cyc_count,
        ch_select,
        gain_play_db)

def target_proc_evb2pc(
        tty,
        baud,
        databuf_raw,
        databuf_enhance,
        wavout,
        lock,
        is_record,
        cyc_count,
        ch_select,
        gain_play_db,
        playback=0):
    """
    one of multiprocesses: EVB sends data to PC
    """
    transport_evb2pc = erpc.transport.SerialTransport(tty, int(baud))
    handler = DataServiceClass(
            databuf_raw,
            databuf_enhance,
            wavout,
            lock,
            is_record,
            cyc_count,
            ch_select,
            gain_play_db,
            playback)
    service = GenericDataOperations_EvbToPc.server.evb_to_pcService(handler)
    server = erpc.simple_server.SimpleServer(transport_evb2pc, erpc.basic_codec.BasicCodec)
    server.add_service(service)
    print("\r\nServer started - waiting for EVB to send an eRPC request")
    sys.stdout.flush()
    server.run()

def main(args):
    """
    main
    """
    print(f"To playback: {args.playback}")
    event_stop      = multiprocessing.Event()
    lock            = Lock()
    databuf_raw     = Array('d', FRAMES_TO_SHOW * HOP_SIZE)
    databuf_enhance = Array('d', FRAMES_TO_SHOW * HOP_SIZE)
    record_ind      = Array('i', [0]) # is_record indicator. 'No record' as initialization
    cyc_count       = Array('i', [0])
    gain_play_db    = Array('f', [20]) # gain in dB
    # choose the channel to playback
    ch_select       = Array('i', [1]) # 0: raw data, 1: enhanced data

    # we use two multiprocesses to handle real-time visualization and recording
    # 1. proc_draw   : to visualize
    # 2. proc_evb2pc : to capture data from evb and recording
    proc_draw   = Process(
                    target = target_proc_draw,
                    args   = (databuf_raw,
                              databuf_enhance,
                              lock, record_ind,
                              event_stop,
                              cyc_count,
                              ch_select,
                              gain_play_db))
    proc_evb2pc = Process(
                    target = target_proc_evb2pc,
                    args   = (  args.tty,
                                args.baud,
                                databuf_raw,
                                databuf_enhance,
                                args.out,
                                lock,
                                record_ind,
                                cyc_count,
                                ch_select,
                                gain_play_db,
                                args.playback))
    proc_draw.start()
    proc_evb2pc.start()
    # monitor if program should be terminated
    while True:
        if event_stop.is_set():
            proc_draw.terminate()
            proc_evb2pc.terminate()
            #Terminating main process
            sys.exit(1)
        time.sleep(0.5)

if __name__ == "__main__":

    # parse cmd parameters
    argParser = argparse.ArgumentParser(description="NeuralSPOT GenericData RPC Demo")

    argParser.add_argument(
        "-w",
        "--tty",
        default="/dev/serial/by-id/usb-TinyUSB_TinyUSB_Device_123457-if00",
        help    = "Serial device (default value is None)",
    )

    argParser.add_argument(
        "-pb",
        "--playback",
        default =0,
        type    =int,
        help    = "playback the data",
    )

    argParser.add_argument(
        "-B",
        "--baud",
        default = "115200",
        help    = "Baud (default value is 115200)"
    )

    argParser.add_argument(
        "-o",
        "--out",
        default = "audio.wav",
        help    = "File where data will be written (default is audio.wav",
    )

    main(argParser.parse_args())
