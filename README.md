# nRF52 LCD Tester Firmware

A Nordic nRF52840-based firmware project for LCD testing with SPI communication and LVGL graphics library integration.

## Prerequisites

1. **ARM GNU Toolchain**: Download and install the ARM GNU Toolchain for embedded development
   - The LCD GUI Tester application can automatically download this for you
   - Or download manually from: https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads

2. **Nordic nRF5 SDK**: Download the nRF5 SDK v17.1.0
   - The LCD GUI Tester application can automatically download this for you  
   - Or download manually from: https://www.nordicsemi.com/Products/Development-software/nRF5-SDK

3. **LVGL Library**: The LVGL graphics library
   - The LCD GUI Tester application can automatically download this for you
   - Or download manually from: https://github.com/lvgl/lvgl

## Project Structure

```
nrf52-lcd-tester-fw/
├── src/
│   ├── main.c              # Main application (SPI example)
│   └── config/
│       └── sdk_config.h    # Nordic SDK configuration
├── CMakeLists.txt          # Build configuration
└── README.md              # This file
```

## Building with CMake

### Method 1: Using the LCD GUI Tester Application (Recommended)

The LCD GUI Tester application will automatically:
- Download the ARM GNU Toolchain for your platform
- Download the nRF5 SDK v17.1.0
- Download the LVGL library
- Set up the correct paths for building

Simply run the LCD GUI Tester application and it will handle all dependencies.

### Method 2: Manual Build

If you have manually installed the dependencies:

1. **Create build directory:**
   ```bash
   mkdir build
   cd build
   ```
```
cmake -DARM_GCC_PATH=/home/dani/projects/tq/lcd-gui-tester/build/libraries/arm-gnu-toolchain       -DNRF_SDK_PATH=/home/dani/projects/tq/lcd-gui-tester/build/libraries/nrf5_sdk -DLVGL_PATH=/home/dani/projects/tq/lcd-gui-tester/build/libraries/lvgl -DCMAKE_BUILD_TYPE=Release       ..
```

2. **Configure with CMake:**
   ```bash
   cmake -DARM_GCC_PATH=/path/to/arm-gnu-toolchain \
         -DNRF_SDK_PATH=/path/to/nrf5_sdk \
         -DLVGL_PATH=/path/to/lvgl \
         -DCMAKE_BUILD_TYPE=Release \
         ..
   ```

   **Example paths:**
   ```bash
   # Linux example
   cmake -DARM_GCC_PATH=/home/user/libraries/arm-gnu-toolchain \
         -DNRF_SDK_PATH=/home/user/libraries/nrf5_sdk \
         -DLVGL_PATH=/home/user/libraries/lvgl \
         -DCMAKE_BUILD_TYPE=Release \
         ..

   # macOS example  
   cmake -DARM_GCC_PATH=/Users/user/libraries/arm-gnu-toolchain \
         -DNRF_SDK_PATH=/Users/user/libraries/nrf5_sdk \
         -DCMAKE_BUILD_TYPE=Release \
         ..

   # Windows example
   cmake -DARM_GCC_PATH="C:/libraries/arm-gnu-toolchain" \
         -DNRF_SDK_PATH="C:/libraries/nrf5_sdk" \
         -DCMAKE_BUILD_TYPE=Release \
         ..
   ```

3. **Build the project:**
   ```bash
   cmake --build .
   ```

## CMake Configuration Options

| Parameter | Description | Default | Required |
|-----------|-------------|---------|----------|
| `ARM_GCC_PATH` | Path to ARM GNU Toolchain installation | - | ✅ Yes |
| `NRF_SDK_PATH` | Path to Nordic nRF5 SDK installation | - | ✅ Yes |
| `MCU_TYPE` | Target MCU type | `NRF52840` | No |
| `CMAKE_BUILD_TYPE` | Build type (`Debug`/`Release`) | `Release` | No |

## Output Files

After successful build, you'll find these files in the `build/` directory:

- `nrf52-lcd-tester-fw.elf` - ELF executable file
- `nrf52-lcd-tester-fw.hex` - Intel HEX file (for programming)
- `nrf52-lcd-tester-fw.bin` - Binary file
- `nrf52-lcd-tester-fw.map` - Memory map file

## Programming the Device

Use the `.hex` file to program your nRF52840 device with tools like:

- **nRF Connect for Desktop** (Programmer app)
- **nrfjprog**: 
  ```bash
  nrfjprog -f nrf52 --program nrf52-lcd-tester-fw.hex --chiperase --verify --reset
  ```
- **OpenOCD**
- **J-Link**

## Features

- **SPI Communication**: Based on Nordic's SPI example
- **LVGL Graphics**: Integrated LVGL library for LCD graphics
- **Nordic nRF52840**: Optimized for nRF52840 microcontroller
- **RTT Logging**: Real-time logging via SEGGER RTT
- **LED Control**: Basic LED blinking functionality

## Troubleshooting

### Common Issues

1. **"ARM_GCC_PATH must be specified"**
   - Ensure you provide the correct path to your ARM GNU Toolchain installation
   - The path should point to the folder containing the `bin/` directory

2. **"NRF_SDK_PATH must be specified"**
   - Ensure you provide the correct path to your nRF5 SDK installation
   - The path should point to the root SDK folder

3. **LVGL not found warning**
   - The LVGL library should be automatically available if using the LCD GUI Tester application
   - For manual builds, ensure LVGL is available at `../libraries/lvgl/`

4. **Linker script not found**
   - Verify your nRF5 SDK installation is complete
   - Check that `components/toolchain/gcc/nrf52840_xxaa.ld` exists in your SDK path

### Getting Help

- Check that all paths use forward slashes (`/`) even on Windows
- Use absolute paths to avoid path resolution issues
- Ensure the ARM GNU Toolchain version is compatible (v13.2.rel1 recommended)
- Verify the nRF5 SDK version is v17.1.0

## License

This project is based on Nordic Semiconductor's nRF5 SDK examples and follows the same BSD-3-Clause license terms.