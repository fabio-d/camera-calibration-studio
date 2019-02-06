# Camera Calibration Studio (ccs)
GUI for OpenCV's camera calibration routines

## Quickstart (Ubuntu 18.04 LTS)
Open a terminal in this folder and run the following commands:
```shell
# Download prerequisites and source code
sudo apt install cmake g++ git libopencv-dev libv4l-dev pkg-config qtbase5-dev
git clone https://github.com/fabio-d/camera-calibration-studio

# Compile camera-calibration-studio
cd camera-calibration-studio
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=../install ..
make install

# Run camera-calibration-studio
cd ../install
./ccs
```
