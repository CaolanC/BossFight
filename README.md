# Perfect.

A framework for collaborative work, including 3d model design, audiovisualisation, games dev and an education tool for 3d maths visualisation and interaction.

Built on top of OpenGL and SDL3, in C++. Dependencies managed with cmake.

## First install SDL dependencies

Ubuntu/Linux Mint

```bash
sudo apt-get install build-essential git make \
pkg-config cmake ninja-build gnome-desktop-testing libasound2-dev libpulse-dev \
libaudio-dev libjack-dev libsndio-dev libx11-dev libxext-dev \
libxrandr-dev libxcursor-dev libxfixes-dev libxi-dev libxss-dev libxtst-dev \
libxkbcommon-dev libdrm-dev libgbm-dev libgl1-mesa-dev libgles2-mesa-dev \
libegl1-mesa-dev libdbus-1-dev libibus-1.0-dev libudev-dev
```

## Then in order to boogie:

### To generate build files

`make bootstrap`

### To build

`make build`

### To run

`make run`

## Project Structure

Project build files are genereated by camake. There are three executable targets, SouljaServer, SouljaClient and SouljaTestSandbox. Their main entry points are in apps, the internal headers are in internal and their implementation is in src.

Building the client shorthand:
`make brc`
Stands for build, run, client
`make brcf` <- This is a fast compile, it won't compile our internal libs so only use this if updating code in the apps directory. Changes to src or internal should us the above.
Stands for build, run, client, fast.

Similarly with the server:
`make brs`
`make brcf`

We don't have an include directory as we don't have a public API to expose as of right now.
