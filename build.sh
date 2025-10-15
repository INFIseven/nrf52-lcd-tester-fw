rm -r build
mkdir build
cd build
cmake -DARM_GCC_PATH=/path/to/lcd-gui-tester/build/libraries/arm-gnu-toolchain \
      -DNRF_SDK_PATH=/path/to/lcd-gui-tester/build/libraries/nrf5_sdk \
      -DLVGL_PATH=/path/to/lcd-gui-tester/build/libraries/lvgl \
      -DIMAGES_PATH=/path/to/lcd-gui-tester/build/generated \
      -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .