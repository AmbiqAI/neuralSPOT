from itertools import product, combinations
from collections import defaultdict
from serial import Serial
import matplotlib.pyplot as plt
import numpy as np
import quaternion
import datetime


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
    def __init__(self, angles_init=(0, 0, 0), port='/dev/ttyACM0', baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.running = True
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
        # self.accel = np.array([0, 0, 0])
        # self.gyro = np.array([0, 0, 0])
        # self.mag = np.array([0, 0, 0])

        self.ts = [self.t]
        self.qs = [self.q]
        self.angs = [self.ang]
        # self.accels = [self.accel]
        # self.gyros = [self.gyro]
        # self.mags = [self.mag]


    def update_data(self, line):
        if line:
            remaining_keys = data_fields.copy()
            try:
                string = line.decode()  # convert the byte string to a unicode string
                for datapoint in string.split():
                    key, val = datapoint.split(':')
                    if key in remaining_keys:
                        self.data[key] = float(val)
                        remaining_keys.remove(key)
            except:
                # Bad data
                pass
            # for key in remaining_keys:
            #     self.data[key] = None


    def process_data(self):
        q0 = self.data['quat_w']
        q1 = self.data['quat_x']
        q2 = self.data['quat_y']
        q3 = self.data['quat_z']
        self.q = np.quaternion(q0, q1, q2, q3)

        elev, azim, roll = quat_to_elev_azim_roll(self.q, self.angles_init)
        self.ang = wrap_angle(np.array([elev, azim, roll]))

        self.t = datetime.datetime.now()
        self.n += 1

        # accel = np.array([self.data['accel_x'], self.data['accel_y'], self.data['accel_z']])
        # gyro = np.array([self.data['gyro_x'], self.data['gyro_y'], self.data['gyro_z']])
        # compass = np.array([self.data['compass_x'], self.data['compass_y'], self.data['compass_z']])
        # self.accel = accel * 9.81 / 8192  # gpm4 (m/s)
        # self.gyro = gyro / 65.5  # dps500 (deg/s)
        # self.mag = compass * 0.15  # uT


    def update_timeseries(self):
        self.ts.append(self.t)
        self.qs.append(self.q)
        self.angs.append(self.ang)
        # self.accels.append(self.accel)
        # self.gyros.append(self.gyro)
        # self.mags.append(self.mag)


    def run(self):
        plot_3d_only = True
        if plot_3d_only:
            layout = [['3d']]
        else:
            layout = [['3d', '3d',   'q'],
                      ['3d', '3d', 'ang']]
        fig, axd = plt.subplot_mosaic(layout)
        ss = axd['3d'].get_subplotspec()
        axd['3d'].remove()
        axd['3d'] = fig.add_subplot(ss, projection='3d')

        with Serial(self.port, self.baudrate, timeout=1) as ser:
            while self.running:
                dt = datetime.datetime.now() - self.t
                if dt.total_seconds() >= 1./self.read_freq:
                    try:
                        line = ser.readline()
                        if line:
                            self.update_data(line)
                            self.process_data()
                            self.update_timeseries()
                    except:
                        pass

                # Init plots
                if self.n == 1:
                    self.plot_cuboid(axd['3d'])
                    if not plot_3d_only:
                        self.plot_q_line(axd['q'])
                        self.plot_ang_line(axd['ang'])
                    plt.show(block=False)

                # Update plots
                if self.n > 0:
                    dt = self.t - self.last_plotted
                    if dt.total_seconds() >= 1./self.plot_freq:
                        self.update_cuboid_plot(axd['3d'])
                        if not plot_3d_only:
                            self.update_q_plot(axd['q'])
                            self.update_ang_plot(axd['ang'])
                        fig.canvas.draw()
                        fig.canvas.flush_events()
                        self.last_plotted = datetime.datetime.now()


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


def main():
    qp = quaternion_plotter(port='/dev/ttyACM0', angles_init=(0, 0, 180))
    qp.run()


if __name__ == '__main__':
    main()