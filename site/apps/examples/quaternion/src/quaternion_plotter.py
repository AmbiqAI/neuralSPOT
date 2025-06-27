'''
@file quaternion_plotter.py

@brief quaternion plotter using neuralSPOT

quaternion_plotter is a quaternion visualizer using the scripts found here:
https://github.com/scottshambaugh/mpl_quaternion_views.git 
''' 
# import argparse
import struct
import sys
# import time
import erpc
sys.path.append("../../../../neuralspot/ns-rpc/python/ns-rpc-genericdata/")
import GenericDataOperations_EvbToPc
import numpy as np
import serial.tools.list_ports
import quaternion
from collections import defaultdict
import datetime
import matplotlib.pyplot as plt
from itertools import product, combinations

input_fn = raw_input if sys.version_info[:2] <= (2, 7) else input

data_fields = {'quat_w', 'quat_x', 'quat_y', 'quat_z', 'accel_x', 'accel_y', 'accel_z',
               'gyro_x', 'gyro_y', 'gyro_z', 'compass_x', 'compass_y', 'compass_z'}


def wrap_angle(ang):
    ang = (ang + 180) % 360 - 180
    return ang


def quat_to_elev_azim_roll(q, angle_offsets=(0, 0, 0)):
    # See Diebel, James "Representing Attitude: Euler Angles, Unit Quaternions, and Rotation Vectors" (2006)
    # https://www.astro.rug.nl/software/kapteyn-beta/_downloads/attitude.pdf
    # Sequence (3, 2, 1), Eqn. 452
    q0, q1, q2, q3 = q.w, q.x, q.y, q.z
    phi = np.arctan2(-2*q1*q2 + 2*q0*q3, q1**2 + q0**2 - q3**2 - q2**2)
    theta = np.arcsin(2*q1*q3 + 2*q0*q2)
    psi = np.arctan2(-2*q2*q3 + 2*q0*q1, q3**2 - q2**2 - q1**2 + q0**2)
    azim = np.rad2deg(phi) + angle_offsets[0]
    elev = -np.rad2deg(theta) + angle_offsets[1]
    roll = np.rad2deg(psi) + angle_offsets[2]
    return elev, azim, roll


def elev_azim_roll_to_quat(elev, azim, roll, angle_offsets=(0, 0, 0)):
    # See Diebel, James "Representing Attitude: Euler Angles, Unit Quaternions, and Rotation Vectors" (2006)
    # https://www.astro.rug.nl/software/kapteyn-beta/_downloads/attitude.pdf
    # Sequence (3, 2, 1), Eqn. 459
    phi = np.deg2rad(azim) - angle_offsets[0]
    theta = np.deg2rad(-elev) - angle_offsets[1]
    psi = np.deg2rad(roll) - angle_offsets[2]
    q0 = np.cos(phi/2)*np.cos(theta/2)*np.cos(psi/2) - np.sin(phi/2)*np.sin(theta/2)*np.sin(psi/2)
    q1 = np.cos(phi/2)*np.cos(theta/2)*np.sin(psi/2) + np.sin(phi/2)*np.sin(theta/2)*np.cos(psi/2)
    q2 = np.cos(phi/2)*np.sin(theta/2)*np.cos(psi/2) - np.sin(phi/2)*np.cos(theta/2)*np.sin(psi/2)
    q3 = np.cos(phi/2)*np.sin(theta/2)*np.sin(psi/2) + np.sin(phi/2)*np.cos(theta/2)*np.cos(psi/2)
    q = np.quaternion(q0, q1, q2, q3)
    return q


class quaternion_plotter():
    def __init__(self, angles_init=(0, 0, 0)):
        self.plot_3d_only = True
        if self.plot_3d_only:
            layout = [['3d']]
        else:
            layout = [['3d', '3d',   'q'],
                      ['3d', '3d', 'ang']]
        self.fig, self.axd = plt.subplot_mosaic(layout)
        ss = self.axd['3d'].get_subplotspec()
        self.axd['3d'].remove()
        self.axd['3d'] = self.fig.add_subplot(ss, projection='3d')
        self.data = defaultdict(float)
        self.n = 0
        self.read_freq = 1e3
        self.plot_freq = 30
        self.maxpoints = 10*self.read_freq

        self.t_start = datetime.datetime.now()
        self.last_plotted = datetime.datetime.now()

        self.t = self.t_start
        self.q = np.quaternion(1, 0, 0, 0)
        self.angles_init = angles_init  # elev, azim, roll (deg)
        self.ang = np.array(angles_init)

        self.ts = [self.t]
        self.qs = [self.q]
        self.angs = [self.ang]

    def process_data(self, data):
        q0 = data[0][0]
        q1 = data[0][1]
        q2 = data[0][2]
        q3 = data[0][3]
        self.q = np.quaternion(q0, q1, q2, q3)

        elev, azim, roll = quat_to_elev_azim_roll(self.q, self.angles_init)
        self.ang = wrap_angle(np.array([elev, azim, roll]))

        self.t = datetime.datetime.now()
        self.n += 1

    def update_timeseries(self):
        self.ts.append(self.t)
        self.qs.append(self.q)
        self.angs.append(self.ang)

    ## Plotting init methods
    def plot_cuboid(self, ax):
        r = [0, 1]
        scale = np.array([8.2, 5.4, 0.9])  # mm
        for start, end in combinations(np.array(list(product(r, r, r))), 2):
            if np.sum(np.abs(start - end)) == r[1] - r[0]:
                ax.plot3D(*zip(start*scale, end*scale))

        ax.view_init(elev=0, azim=0, roll=0)
        ax.set_proj_type('persp')
        ax.set_aspect('equal')
        ax.set_xlabel('x')
        ax.set_ylabel('y')
        ax.set_zlabel('z')


    def plot_q_line(self, ax):
        ax.clear()
        npoints = min(self.n, self.maxpoints)
        timedeltas = [t - self.t_start for t in self.ts[-npoints-1:]]
        times = [t.total_seconds() for t in timedeltas]
        self.q0_line, = ax.plot(times, [q.w for q in self.qs[-npoints-1:]], '-*', label='q0')
        self.q1_line, = ax.plot(times, [q.x for q in self.qs[-npoints-1:]], '-*', label='q1')
        self.q2_line, = ax.plot(times, [q.y for q in self.qs[-npoints-1:]], '-*', label='q2')
        self.q3_line, = ax.plot(times, [q.z for q in self.qs[-npoints-1:]], '-*', label='q3')
        ax.legend()
        ax.yaxis.set_label_position("right")
        ax.yaxis.tick_right()
        ax.set_ylabel('q')
        ax.set_ylim((-1.05, 1.05))


    def plot_ang_line(self, ax):
        ax.clear()
        npoints = min(self.n, self.maxpoints)
        timedeltas = [t - self.t_start for t in self.ts[-npoints-1:]]
        times = [t.total_seconds() for t in timedeltas]
        self.elev_line, = ax.plot(times, [angs[0] for angs in self.angs[-npoints-1:]], '-*', label='elev')
        self.azim_line, = ax.plot(times, [angs[1] for angs in self.angs[-npoints-1:]], '-*', label='azim')
        self.roll_line, = ax.plot(times, [angs[2] for angs in self.angs[-npoints-1:]], '-*', label='roll')
        ax.legend()
        ax.yaxis.set_label_position("right")
        ax.yaxis.tick_right()
        ax.set_ylim((-185, 185))
        ax.set_ylabel('ang (deg)')
        ax.set_xlabel('t (s)')

    ## Plotting update methods
    def update_cuboid_plot(self, ax):
        print(self.t, self.q) #, self.ang)
        elev, azim, roll = self.ang
        ax.view_init(elev, azim, roll)
        ax.set_title(f'elev={elev:0.1f}, azim={azim:0.1f}, roll={roll:0.1f}')


    def update_q_plot(self, ax):
        npoints = min(self.n, self.maxpoints)
        timedeltas = [t - self.t_start for t in self.ts[-npoints-1:]]
        times = [t.total_seconds() for t in timedeltas]
        q0s = [q.w for q in self.qs[-npoints-1:]]
        q1s = [q.x for q in self.qs[-npoints-1:]]
        q2s = [q.y for q in self.qs[-npoints-1:]]
        q3s = [q.z for q in self.qs[-npoints-1:]]
        for line, qs in zip((self.q0_line, self.q1_line, self.q2_line, self.q3_line),
                            (q0s, q1s, q2s, q3s)):
            line.set_xdata(times)
            line.set_ydata(qs)
        ax.set_xlim([times[0], times[-1]])


    def update_ang_plot(self, ax):
        npoints = min(self.n, self.maxpoints)
        timedeltas = [t - self.t_start for t in self.ts[-npoints-1:]]
        times = [t.total_seconds() for t in timedeltas]
        for i, line in enumerate((self.elev_line, self.azim_line, self.roll_line)):
            line.set_xdata(times)
            line.set_ydata([angs[i] for angs in self.angs[-npoints-1:]])
        ax.set_xlim([times[0], times[-1]])


# Define the RPC service handlers - one for each EVB-to-PC RPC function
class DataServiceHandler(GenericDataOperations_EvbToPc.interface.Ievb_to_pc):
    def ns_rpc_data_sendBlockToPC(self, block):
        # IMU capture handler
        if (block.cmd == GenericDataOperations_EvbToPc.common.command.write_cmd) and (
            block.description == "Quaternion Plotter"
        ):
            ncols = 4
            # Data is a 32bit Float MPU packed sample
            data = struct.unpack(f"<{len(block.buffer)//4}f", block.buffer)
            # Convert data into 2D list with ncols columns. We drop leftover values
            data = [
                data[i : i + ncols]
                for i in range(0, ncols * (len(data) // ncols), ncols)
            ]

            dt = datetime.datetime.now() - quat.t
            if dt.total_seconds() >= 1./quat.read_freq:
                try:
                    quat.process_data(data)
                    quat.update_timeseries()
                except:
                    pass

            # Init plots
            if quat.n == 1:
                quat.plot_cuboid(quat.axd['3d'])
                if not quat.plot_3d_only:
                    quat.plot_q_line(quat.axd['q'])
                    quat.plot_ang_line(quat.axd['ang'])
                plt.show(block=False)

            # Update plots
            if quat.n > 0:
                dt = quat.t - quat.last_plotted
                if dt.total_seconds() >= 1./quat.plot_freq:
                    quat.update_cuboid_plot(quat.axd['3d'])
                    if not quat.plot_3d_only:
                        quat.update_q_plot(quat.axd['q'])
                        quat.update_ang_plot(quat.axd['ang'])
                    quat.fig.canvas.draw()
                    quat.fig.canvas.flush_events()
                    self.last_plotted = datetime.datetime.now()
        sys.stdout.flush()
        return 0

    def ns_rpc_data_fetchBlockFromPC(self, block):
        print("Got a ns_rpc_data_fetchBlockFromPC call.")
        sys.stdout.flush()
        return 0

    def ns_rpc_data_computeOnPC(self, in_block, result_block):
        # Example Computation
        if (
            in_block.cmd == GenericDataOperations_EvbToPc.common.command.extract_cmd
        ) and (in_block.description == "CalculateMFCC_Please"):
            result_block.value = GenericDataOperations_EvbToPc.common.dataBlock(
                description="*\0",
                dType=GenericDataOperations_EvbToPc.common.dataType.uint8_e,
                cmd=GenericDataOperations_EvbToPc.common.command.generic_cmd,
                buffer=bytearray([0, 1, 2, 3]),
                length=4,
            )
        sys.stdout.flush()
        return 0

    def ns_rpc_data_remotePrintOnPC(self, msg):
        print("%s" % msg)
        sys.stdout.flush()
        return 0


def runServer(transport):
    handler = DataServiceHandler()
    service = GenericDataOperations_EvbToPc.server.evb_to_pcService(handler)
    server = erpc.simple_server.SimpleServer(transport, erpc.basic_codec.BasicCodec)
    server.add_service(service)
    print("\r\nServer started - waiting for EVB to send an eRPC request")
    global quat
    quat = quaternion_plotter()
    sys.stdout.flush()
    server.run()


def find_tty(transport):
    # Find the TTY of our device by scanning serial USB or UART devices
    # The VID of our device is alway 0xCAFE
    # This is a hack to find the tty of our device, it will break if there are multiple devices with the same VID
    # or if the VID changes
    
    tty = None
    ports = serial.tools.list_ports.comports()

    if transport == 'USB':
        # Look for USB CDC
        for p in ports:
            # print(p)
            # print(p.vid)
            if p.vid == 51966:
                tty = p.device
                print("Found USB CDC device %s" % tty)
                break
    else:
        # Look for UART
        for p in ports:
            # print(p)
            # print(p.vid)
            if p.vid == 4966:
                tty = p.device
                print("Found UART device %s" % tty)
                break

    return tty

if __name__ == "__main__":
    # parse cmd parameters
    tty = find_tty('USB')
    transport = erpc.transport.SerialTransport(tty, 115200)
    runServer(transport)