# NeuralSPOT and Visual Studio Code

Ambiq's AI tools are CLI-based in order to be IDE and GUI-agnostic, and use freely available and/or open source toolchains where possible. This approaches meshes well the Visual Studio Code's functionality, and this is the code editor that the Ambiq AI team uses in day-to-day development.

The Ambiq AI team uses Visual Studio Code (vscode) to:

- Edit and explore code and documentation
- Manage our [GitHub-based version control](https://code.visualstudio.com/docs/sourcecontrol/intro-to-git)
- Compile and flash applications
- Step through code running on the EVB
- Manage our compilation toolchain
- [Develop code remotely](https://code.visualstudio.com/docs/remote/remote-overview)

Beyond that, our data scientists use vscode to [visualize AI training](https://marketplace.visualstudio.com/items?itemName=ms-toolsai.tensorboard) and results, run [Jupyter notebooks](https://code.visualstudio.com/docs/datascience/jupyter-notebooks), and [plot data](https://www.altcademy.com/blog/how-to-get-matplotlib-in-visual-studio-code/). 

#### In This Document

This document covers three use cases that require neuralSPOT-specific configuration: Compiling & Flashing, Debugging, and managing compilation toolchains via devcontainers.

Visual Studio Code is highly configurable and extensible. See [Configuring VSCode](#configuring-vscode), below.

## Using VSCode

Once configured, VSCode will have 3 new 'tasks' it can run: "Build Task", "Flash Firmware", and "Start Debugging". Building and Debugging are common enough that vscode has [https://code.visualstudio.com/docs/getstarted/keybindings](https://code.visualstudio.com/docs/getstarted/keybindings) to start them. "Flash Firmware" doesn't have a key binding, but vscode lets you [create your own key bindings](https://code.visualstudio.com/docs/getstarted/keybindings).

### Devcontainers

Installing toolchains is often a difficult and fiddly task, highly dependent on OS and toolchain versions. To alleviate some of this complexity we enable the use of [devcontainers](https://code.visualstudio.com/docs/devcontainers/containers). This allows the AI team to quickly set up a new development environment directly from a freshly cloned version of neuralSPOT.

Devcontainers allow us to develop inside a containerized environment. This ensures consistency across different development environments and simplifies dependency management. The neuralSPOT devcontainer will automatically install all necessary dependencies. In order to enable and use devcontainers in VS Code:

1. Install Docker here: https://www.docker.com/products/docker-desktop/
2. Install the Dev Containers extension from Microsoft on VS Code.
3. Type into VS Code command Palette: "Dev Containers: Rebuild and Reopen in Container." This will restart VS Code in the neuralSPOT devcontainer. 
   1. Changes can be made to the devcontainer configuration in the .devcontainer folder of neuralSPOT. 
   2. Note that when making changes to the devcontainer configuration, it must be rebuilt for the changes to take effect. Otherwise, you can simply reopen the container.
4. When VS Code restarts, you will be connected to the devcontainer, as shown by the green remote indicator on the left of the Status bar.

## Configuring VSCode

### Needed Applications

- [Visual Studio Code](https://visualstudio.microsoft.com/downloads/)
- [Segger J-link](https://www.segger.com/downloads/jlink/)
- Remote GDB debugger: we use J-link's included GDB server
- [Docker Engine](https://docs.docker.com/engine/install/) (optional): Enables Devcontainer-based operation. 

### Arm Development Extensions

You'll need the following (install from VS Code's extensions browser):

- [Cortex-Debug](https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug) (by marus25): Enables GDB-based debug from inside vscode. Can use JlinkGDB, OpenOCD, or pyOCD. We use Jlink.
- [Arm Assembly](https://marketplace.visualstudio.com/items?itemName=dan-c-underwood.arm) (by dan-c-underwood): occasionally we need to step through assembly. This extension enables a disassembly view of flashed code.
- [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) (by Microsoft): optional, enables the use of containerized compilation environments.

### Configuration Files

VS Code configuration files live in a `.vscode` directory at the root of your SDK directory structure. We need to create two files: a tasks file that tells VS code how to build Ambiq binaries, and a launch file that tells it how to use Jlink to install and execute the binary on an EVB

This is the `tasks.json` file. The way it is written, you have to launch the build task while the editor is 'focused' on the `main()`.

```json
    "version": "2.0.0",
    "tasks": [
        {
            "type": "shell",
            "label": "Build Project",
            "command": "make EXAMPLE=${fileBasenameNoExtension}",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": {
                "base": "$gcc",
                "fileLocation": ["relative", "${relativeFileDirname}/."]
            }
        },
        {
            "type": "shell",
            "label": "Flash Firmware",
            "command": "make EXAMPLE=${fileBasenameNoExtension} TARGET=${fileBasenameNoExtension} deploy",
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": {
                "base": "$gcc",
                "fileLocation": ["relative", "${relativeFileDirname}/."]
            }
        }
    ]
}
```

The `tasks.json` file defines a 'launch' task. There are several hardcoded paths. There may be a clever way to make them soft paths, but I haven't bothered looking.

```json
    "version": "0.2.0",
    "configurations": [
        {
            "cwd": "${fileDirname}",   
            "executable": "${workspaceRoot}/build/apollo5b_evb/arm-none-eabi/examples/${fileBasenameNoExtension}/${fileBasenameNoExtension}.axf",
            "name": "Debug Microcontroller",
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "jlink",
            "serverpath": "/Applications/SEGGER/JLink_V794a/JLinkGDBServer",
            "armToolchainPath": "/Applications/ArmGNUToolchain/12.2.Rel1/arm-none-eabi/bin",
            "device": "AMAP42KK-KBR",
            "interface": "swd",
            "serialNumber": "", // add J-Link serial number if having multiple attached the same time.
            "runToEntryPoint": "main",
            "svdFile": "${workspaceRoot}/build/apollo5b_eb_revb/arm-none-eabi/board.svd",
        }
    ]
}
```