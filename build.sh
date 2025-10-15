# rm -r build
# mkdir build
cd build
cmake -DARM_GCC_PATH=/home/dani/projects/tq/lcd-gui-tester/build/libraries/arm-gnu-toolchain -DNRF_SDK_PATH=/home/dani/projects/tq/lcd-gui-tester/build/libraries/nrf5_sdk -DLVGL_PATH=/home/dani/projects/tq/lcd-gui-tester/build/libraries/lvgl -DCMAKE_BUILD_TYPE=Debug ..
make -j8