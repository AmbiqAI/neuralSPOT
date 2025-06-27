# NeuralSpot RPC and AutoDeploy on Linux

**Introduction**

This Application Note describes how to prepare a Linux system to work with applications using USB such as neuralSPOT’s AutoDeploy tool. By default, the EVB USB port appears as `/dev/ttyACM1` and is assigned to the `dialout`user group. Users not in this group cannot access the serial device, causing AutoDeploy to fail. This note guides you through granting your user account the required permissions.

**Prerequisites**

- A Linux environment (Debian/Ubuntu, Fedora, or similar) with the AutoDeploy tool installed.
- USB connection between the host and the EVB (Evaluation Board).
- A user account with `sudo` privileges.

**Setup Procedure**

1. **Perform an Initial AutoDeploy Run**
   Execute AutoDeploy without specifying the `--transport` option (which defaults to USB):

   ```bash
   ns_autodeploy --tflite-filename <some model tflite file>
   ```

   This run will flash the USB firmware image and then fail if the permissions are not correct, but it will still mount the EVB device at `/dev/ttyACM1`.

2. **Identify Device Ownership**
   List the device permissions to determine which group owns the port:

   ```bash
   ls -l /dev/ttyACM1
   ```

   The output will look similar to:

   ```
   crw-rw---- 1 root dialout 166, 1 Jun 27 10:15 /dev/ttyACM1
   ```

   In this example, the group is `dialout`.

3. **Check Your Group Membership**
   Verify whether your user is already in the `dialout` group (replace `carlos` with your username):

   ```bash
   groups <your username here>
   ```

   If `dialout` is not listed, you need to add your user to the group.

4. **Add Your User to the `dialout` Group**
   Grant your user permission to access USB serial devices by adding it to the `dialout` group:

   ```bash
   sudo adduser <your username here> dialout
   ```

   This command modifies `/etc/group`, appending your username to the `dialout` entry.

5. **Re-login to Apply Changes**
   Log out of your session and log back in (or reboot) so the new group membership takes effect. This step is crucial—without re-authentication, your shell session will not recognize the updated group.

**Verification**

After logging back in:

```bash
groups <your username here>
```

You should now see `dialout` in the list. Next, re-run AutoDeploy:

```bash
ns_autodeploy --tflite-filename <some model tflite file>
```

This time, the tool will successfully communicate with `/dev/ttyACM1` and complete the deployment.

**Conclusion**

By adding your Linux user to the `dialout` group, you ensure proper permissions for the EVB RPC serial interface. Follow this procedure once per user account to enable seamless use of NeuralSpot AutoDeploy over USB.