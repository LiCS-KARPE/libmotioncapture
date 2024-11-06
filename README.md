[![CI](https://github.com/IMRCLab/libmotioncapture/actions/workflows/CI.yml/badge.svg)](https://github.com/IMRCLab/libmotioncapture/actions/workflows/CI.yml)

# libmotioncapture
Interface Abstraction for Motion Capture System APIs such as VICON, OptiTrack, Qualisys, Nokov, FZMotion, or VRPN.

For C++, follow the instructions below.

This is a fork of https://github.com/IMRCLab/libmotioncapture/ with the following changes:

- Added Motion Analysis SDK dependency
- Removed unused submodules
- Fixed orientation calculation

## Prerequisites

```
sudo apt install libboost-system-dev libboost-thread-dev libeigen3-dev ninja-build
```

## C++

```
git submodule init
git submodule update
mkdir build
cd build
cmake ..
make
```

An example application is in `examples/main.cpp`. Run it using

```
./motioncapture_example <mocap type> <ip address>
```

## Python (Development)

```
git submodule init
git submodule update
python3 setup.py develop --user
python3 examples/python.py
```
