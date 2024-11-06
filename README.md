[![CI](https://github.com/LiCS-KARPE/libmotioncapture/actions/workflows/CI.yml/badge.svg?branch=main)](https://github.com/LiCS-KARPE/libmotioncapture/actions/workflows/CI.yml)

# libmotioncapture
Interface Abstraction for Motion Capture System API Motion Analysis (Cortex)

This is a fork of https://github.com/IMRCLab/libmotioncapture/ with the following changes:

- Added Motion Analysis SDK dependency
- Removed unused submodules
- Fixed orientation calculation

## Installation

To install Python binary package, go to [release](https://github.com/LiCS-KARPE/libmotioncapture/releases) page, right click the appropriate wheel package (.whl), and copy the download link.
The package can be installed using pip

```bash
pip install <download_link>
```

For example, the following is command to install package on Ubuntu with Python 3.9
```bash
pip install https://github.com/LiCS-KARPE/libmotioncapture/releases/download/1.0/motioncapture-1.0-cp39-cp39-manylinux_2_24_x86_64.manylinux_2_28_x86_64.whl
```

For C++, follow the instructions to install from source below.

## Examples

Example scripts for Python and C++ are in `examples/` folder.

For Python, run it using
```bash
python3 examples/python.py motionanalysis 127.0.0.1
```

For C++, after building run it using
```bash
build/motioncapture_example motionanalysis 127.0.0.1
```

## Build from source

### Prerequisite
```
sudo apt install libboost-system-dev libboost-thread-dev libeigen3-dev ninja-build
```

### C++

```
git submodule init
git submodule update
mkdir build
cd build
cmake ..
make
```

### Python

```
git submodule init
git submodule update
python3 setup.py develop --user
```
