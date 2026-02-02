# Application Note: neuralSPOT and Windows

The neuralSPOT development environment is based on GCC and Python - compiling and deploying has been tested on MacOS 12+, Windows 11, and Ubuntu Linux. Most developers have a preferred environment, and there are thousands of combinations, so neuralSPOT aims to be as agnostic as possible, as long as the following tools are available on the platform:

1. ARM's GCC-based crosscompiler toolchain
2. GNU Make or a compatible equivalent
3. Python 3.10+ with pip support
4. SEGGER Jlink

However, neuralSPOT was developed on Linux and Mac, so there are some toolchain and requirement differences for Windows. This document describes the 'bare minimum' installation and compilation process for neuralSPOT, including eRPC and RPC-based neuralSPOT tools.

> **NOTE** neuralSPOT has experimental support for Arm's proprietary compiler, armclang. Armclang requires an Arm license to run, and has only been tested on Windows.

# Compiling neuralSPOT

First we need to prepare the compile toolchain and environment. In summary:

1. Install a Terminal application (we've tested [Microsoft Terminal](https://apps.microsoft.com/store/detail/windows-terminal/9N0DX20HK701?hl=en-us&gl=us))
2. Install ARM tools
3. Install GNU Make (ironically, installing this simple tool is the most convoluted step of the process)
4. Install git
5. Configuring shells and paths
6. Compile

### Installing ARM Toolchain

The ARM toolchain is needed to compile and link neuralSPOT binaries for ARM processors. Installation is via a Windows Installer located [here](https://developer.arm.com/downloads/-/gnu-rm). We've tested 10.3-2021.10, which is the latest stable release.

The installer will add the tools to your PATH environment variable as an option (this makes things simpler).

### Installing GNU Make

GNU Make is a version of the venerable Unix make tool. neuralSPOT's toolchain is based on this version of make. There are many ways of installing this on Windows. We found that using Chocolatey (a Windows tool package manager) is straightforward. The trickiest part of the process is installing Chocolatey itself.

The [instructions for installing Chocolatey](https://chocolatey.org/install) are a bit convoluted, since it is a shell-based install (on the other hand, once you have that running you can use it to install many other tools easily). What we found worked was:

1. Download the script linked to from their instructions ([here](https://community.chocolatey.org/install.ps1)) to somewhere you can access from the Windows Terminal.
2. Start a Terminal in "Administrative Mode"
3. Set execution permissions as per Chocolatey's instructions (linked above)
4. Execute the script you download in step 1

Once you have Chocolatey installed, installing GNU Make is easy. From the Terminal, run:

```bash
choco install make
```

### Installing Git

Git is a source code management tool, and the simplest way to access projects on GitHub - it isn't strictly needed since GitHub allows you to download projects as Zip or Tar packages, but installing git will make it easier to pull updates, track any changes you make to the code locally, and submit code back to GitHub.

Installing git is straightforward via their installer, located [here](https://gitforwindows.org).

This robust installer will give you many configuration options as it progresses - the default options will work, but if you are an experienced windows and/or git user feel free to modify. We found the following settings helpful:

1. "Add a Git Bash Profile to Windows Terminal" (select this)
2. "Use Git and optional Unix tools from the Command Prompt" (adds stuff to Path, so only do this if you understand the implications).

### Shells and Paths

The neuralSPOT Makefile makes use of a few unix-style commands (such as `mkdir -p` and `cp -R`) which behave differently on Windows, depending on which shell is being used and how the PATH environment variable is configured. We recommend one of:

1. Use git bash - this is installed as part of Git, and is a special CLI that replicates common Unix Bash shell commands. Launching and using git bash is the easiest path to compiling neuralSPOT. See [here for instructions on how to use it](https://www.atlassian.com/git/tutorials/git-bash).
2. Let git modify the Path to point to common unix replacements for windows commands (see option 2 above) when installing Git.

### Compiling neuralSPOT

Now that we have all the essential tools installed, you're ready to download and compile neuralSPOT:

```bash
$> git clone https://github.com/AmbiqAI/neuralSPOT.git
$> cd neuralSPOT
$> make
```

For more compilation options, visit our main [README](https://github.com/AmbiqAI/neuralSPOT).

# Installing eRPC and neuralSPOT RPC

Using RPC in neuralSPOT requires Python and some Python libraries. In summary:

1. Install Python
3. Install neuralSPOT required libraries
4. Run RPC client example

### Installing Python

There are hundreds of ways to install Python on Windows, but we found that using [Window Store's Python](https://apps.microsoft.com/store/detail/python-310/9PJPW5LDXLZ5?hl=en-us&gl=us) worked well and included everything needed.

### Installing neuralSPOT Libraries

neuralSPOT includes a sample Python application which requires some Python libraries to be installed (besides eRPC). The simplest way to install these is using `pip`:

```bash
$> cd .../neuralSPOT
$> pip install .
```
