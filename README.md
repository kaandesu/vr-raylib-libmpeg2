## Project Description

This project is about streaming video to Raylib in VR format, using [libmpeg2](https://libmpeg2.sourceforge.io/)
for decoding. The main goal is to stream the PC desktop to mobile VR in a 3D virtual environment.
There are also plans to port the project to Odin, Zig, or Go in the future.

> [!IMPORTANT]
> vr-raylib-libmpeg2 is currently under development.

So far:

- Implemented video playback using Raylib and libmpeg2 to decode MPEG-2 video.
- Managed frame parsing and rendering at ~30 FPS, converting video frames to RGB for display.

### Project Setup for macOS and Linux

This document outlines the steps to set up the development environment for the project on both macOS and Linux.

### Prerequisites

#### macOS

1. **Install Homebrew** (if not already installed):

   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

2. **Install Dependencies**:

   ```bash
   brew install libmpeg2 raylib
   ```

#### Linux

1. **Install Dependencies** (Debian-based distributions):

   ```bash
   sudo apt update
   sudo apt install build-essential libmpeg2-4-dev libgl1-mesa-dev libraylib-dev pkg-config
   ```

### Setting Up the Project

1. **Clone the Repository** (replace with your repository URL):

   ```bash
   git clone https://github.com/kaandesu/vr-raylib-libmpeg2
   cd vr-raylib-libmpeg2
   ```

2. **Compile the Project**:

   ```bash
   make
   ```

3. **Compile and run the application**:

   ```bash
   make run
   ```

### Notes

- Ensure that `pkg-config` is installed on both macOS and Linux, as it is used to manage library compile and link flags.
- On macOS, make sure you have the Xcode command line tools installed:

  ```bash
  xcode-select --install
  ```

### Troubleshooting

- If you encounter issues with libraries not being found, double-check the installation paths and ensure that your environment variables are set correctly.
- For any errors related to compilation, consult the output logs for specific issues and verify that all dependencies are correctly installed.
