cmake_minimum_required(VERSION 3.7)

# Create compile_commands.json for clang
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Input parameters
set(ARM_GCC_PATH "" CACHE STRING "Path to ARM GCC toolchain")
set(MCU_TYPE "NRF52840" CACHE STRING "MCU type (default: NRF52840)")
set(NRF_SDK_PATH "" CACHE STRING "Path to nRF SDK")
set(LVGL_PATH "" CACHE STRING "Path to LVGL library")

# Validate required parameters
if(ARM_GCC_PATH STREQUAL "")
    message(FATAL_ERROR "ARM_GCC_PATH must be specified")
endif()

if(NRF_SDK_PATH STREQUAL "")
    message(FATAL_ERROR "NRF_SDK_PATH must be specified")
endif()

# if(LVGL_PATH STREQUAL "")
#     message(FATAL_ERROR "LVGL_PATH must be specified")
# endif()

# Configure for cross-compilation
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Set toolchain
set(CMAKE_C_COMPILER "${ARM_GCC_PATH}/bin/arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "${ARM_GCC_PATH}/bin/arm-none-eabi-g++")
set(CMAKE_ASM_COMPILER "${ARM_GCC_PATH}/bin/arm-none-eabi-gcc")
set(CMAKE_OBJCOPY "${ARM_GCC_PATH}/bin/arm-none-eabi-objcopy")
set(CMAKE_SIZE "${ARM_GCC_PATH}/bin/arm-none-eabi-size")

# Skip compiler tests for cross-compilation
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Project configuration
project(nrf52-lcd-tester-fw C ASM)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# MCU-specific definitions
if(MCU_TYPE STREQUAL "NRF52840")
    set(MCU_DEFINE "-DNRF52840_XXAA")
    set(MCU_FLAGS "-mcpu=cortex-m4")
    set(MCU_LINKER_SCRIPT "nrf52_lcd_tester.ld")
else()
    message(FATAL_ERROR "Unsupported MCU type: ${MCU_TYPE}")
endif()

# Compiler flags
set(COMMON_FLAGS
  "-Wshadow -Wdouble-promotion -fno-common"
  "-mthumb"
  "-mabi=aapcs"
  "-mfloat-abi=hard"
  "-mfpu=fpv4-sp-d16"

  # keep every function in a separate section, this allows linker to discard unused ones
  "-fmessage-length=0 -fdiagnostics-color=never -fdiagnostics-show-caret -std=gnu11 -mcpu=cortex-m4 -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb  -mtp=soft -mfp16-format=ieee"
  "-ffunction-sections"
  "-fdata-sections"
  "-fno-strict-aliasing"
  "-fno-builtin"
  "-fshort-enums"
    # ${MCU_FLAGS}
)

set(COMPILE_DEFINITIONS
  "-DNRF52840_XXAA"
  "-DCONFIG_NFCT_PINS_AS_GPIOS"
  "-DCONFIG_GPIO_AS_PINRESET"
  "-DFLOAT_ABI_HARD"
  "-DUSE_MCU_NRF"
  # Commercial license key for ANT+
  "-DINITIALIZE_USER_SECTIONS"
  "-DNO_VTOR_CONFIG"
  "-DLV_CONF_INCLUDE_SIMPLE"
  "-DS340"
  "-DNRF_LOG_USES_RTT=1"
  "-DHPR_DISP_VEN_TQ"
  "-D__HEAP_SIZE__=32768"
  "-D__GNU_LINKER"
)

# Build type specific flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND MY_COMPILER_FLAGS
    "-O0"
    "-g3"
    "-gdwarf-4"
    "-gpubnames"
    "-DDEBUG"
    "-D DEBUG_NRF"
    "-fomit-frame-pointer -fno-dwarf2-cfi-asm -ffunction-sections -fdata-sections -fshort-enums -fno-common"
    )
else()
    list(APPEND MY_COMPILER_FLAGS
    "-O2"
    "-g3"
    "-fomit-frame-pointer -fno-dwarf2-cfi-asm -ffunction-sections -fdata-sections -fshort-enums -fno-common"
    )
endif()

# Convert lists to strings
string(REPLACE ";" " " CMAKE_C_FLAGS "${COMMON_FLAGS}")
string(REPLACE ";" " " CMAKE_ASM_FLAGS "${COMMON_FLAGS}")
string(REPLACE ";" " " COMPILE_DEFINITIONS_STR "${COMPILE_DEFINITIONS}")

# Add compile definitions
add_definitions(${COMPILE_DEFINITIONS})

# Source files
set(PROJECT_SRCS
    src/main.c
    src/spi.c
    src/syscalls.c
)

# nRF SDK include directories (simplified)
set(NRF_SDK_INCLUDE_DIRS
    "${NRF_SDK_PATH}/components"
    "${NRF_SDK_PATH}/components/libraries/atomic"
    "${NRF_SDK_PATH}/components/libraries/atomic_flags"
    "${NRF_SDK_PATH}/components/libraries/balloc"
    "${NRF_SDK_PATH}/components/libraries/button"
    "${NRF_SDK_PATH}/components/libraries/crypto"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/cc310"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/cc310_bl"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/cifra"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/mbedtls"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/micro_ecc"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/nrf_hw"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/nrf_sw"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/oberon"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/optiga"
    "${NRF_SDK_PATH}/components/libraries/stack_info"
    "${NRF_SDK_PATH}/components/libraries/queue"
    "${NRF_SDK_PATH}/components/libraries/delay"
    "${NRF_SDK_PATH}/components/libraries/experimental_section_vars"
    "${NRF_SDK_PATH}/components/libraries/log"
    "${NRF_SDK_PATH}/components/libraries/fds"
    "${NRF_SDK_PATH}/components/libraries/log/src"
    "${NRF_SDK_PATH}/components/libraries/memobj"
    "${NRF_SDK_PATH}/components/libraries/mutex"
    "${NRF_SDK_PATH}/components/libraries/ringbuf"
    "${NRF_SDK_PATH}/components/libraries/scheduler"
    "${NRF_SDK_PATH}/components/libraries/sortlist"
    "${NRF_SDK_PATH}/components/libraries/strerror"
    "${NRF_SDK_PATH}/components/libraries/util"
    "${NRF_SDK_PATH}/components/libraries/uart"
    "${NRF_SDK_PATH}/components/libraries/fstorage"
    "${NRF_SDK_PATH}/components/libraries/timer"
    "${NRF_SDK_PATH}/components/libraries/sensorsim"
    "${NRF_SDK_PATH}/components/libraries/pwm"
    "${NRF_SDK_PATH}/external/mbedtls/include"
    "${NRF_SDK_PATH}/external/nrf_cc310/include"
    "${NRF_SDK_PATH}/components/toolchain/cmsis/include"
    "${NRF_SDK_PATH}/external/mbedtls/include"
    "${NRF_SDK_PATH}/external/nrf_cc310/include"
    "${NRF_SDK_PATH}/external/nrf_oberon"
    "${NRF_SDK_PATH}/external/nrf_oberon/include"
    "${NRF_SDK_PATH}/external/nrf_tls/mbedtls/nrf_crypto/config"
    "${NRF_SDK_PATH}/external/fprintf"
    "${NRF_SDK_PATH}/external/segger_rtt"
    "${NRF_SDK_PATH}/integration/nrfx"
    "${NRF_SDK_PATH}/integration/nrfx/legacy"
    "${NRF_SDK_PATH}/modules/nrfx"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/include"
    "${NRF_SDK_PATH}/components/drivers_nrf/nrf_soc_nosd"
    "${NRF_SDK_PATH}/modules/nrfx/hal"
    "${NRF_SDK_PATH}/modules/nrfx/mdk"
    ${CMAKE_SOURCE_DIR}/src/config
    ${CMAKE_SOURCE_DIR}/src/
)

# nRF SDK source files (simplified)
set(NRF_SDK_SRCS
    "${NRF_SDK_PATH}/modules/nrfx/mdk/gcc_startup_nrf52840.S"
    "${NRF_SDK_PATH}/modules/nrfx/mdk/system_nrf52840.c"
    "${NRF_SDK_PATH}/components/libraries/sensorsim/sensorsim.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_aead.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_aes.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_aes_shared.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_ecc.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_ecdh.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_ecdsa.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_eddsa.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_error.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_hash.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_hkdf.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_hmac.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_init.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_rng.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/nrf_crypto_shared.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_init.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng_mbedtls.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/oberon/oberon_backend_eddsa.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/oberon/oberon_backend_hash.c"
    "${NRF_SDK_PATH}/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c"
    "${NRF_SDK_PATH}/integration/nrfx/legacy/nrf_drv_clock.c"
    "${NRF_SDK_PATH}/integration/nrfx/legacy/nrf_drv_rng.c"
    "${NRF_SDK_PATH}/integration/nrfx/legacy/nrf_drv_spi.c"
    "${NRF_SDK_PATH}/integration/nrfx/legacy/nrf_drv_ppi.c"
    # "${NRF_SDK_PATH}/integration/nrfx/legacy/nrf_drv_twi.c"
    # "${NRF_SDK_PATH}/integration/nrfx/legacy/nrf_drv_uart.c"
    # "${NRF_SDK_PATH}/components/libraries/queue/nrf_queue.c"
    "${NRF_SDK_PATH}/modules/nrfx/soc/nrfx_atomic.c"
    # "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_clock.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_gpiote.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/prs/nrfx_prs.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_pwm.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_saadc.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_spi.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_spim.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_twi.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_twim.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_uart.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_uarte.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_rtc.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_ppi.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_timer.c"
    "${NRF_SDK_PATH}/components/libraries/util/app_error.c"
    "${NRF_SDK_PATH}/components/libraries/util/app_error_handler_gcc.c"
    "${NRF_SDK_PATH}/components/libraries/util/app_error_weak.c"
    "${NRF_SDK_PATH}/components/libraries/pwm/app_pwm.c"
    # "${NRF_SDK_PATH}/components/libraries/button/app_button.c"
    # "${NRF_SDK_PATH}/components/libraries/scheduler/app_scheduler.c"
    # "${NRF_SDK_PATH}/components/libraries/timer/drv_rtc.c"
    "${NRF_SDK_PATH}/components/libraries/util/app_util_platform.c"
    "${NRF_SDK_PATH}/components/libraries/fds/fds.c"
    "${NRF_SDK_PATH}/components/libraries/util/nrf_assert.c"
    "${NRF_SDK_PATH}/components/libraries/atomic_flags/nrf_atflags.c"
    "${NRF_SDK_PATH}/components/libraries/atomic/nrf_atomic.c"
    "${NRF_SDK_PATH}/components/libraries/balloc/nrf_balloc.c"
    "${NRF_SDK_PATH}/external/fprintf/nrf_fprintf.c"
    "${NRF_SDK_PATH}/external/fprintf/nrf_fprintf_format.c"
    "${NRF_SDK_PATH}/components/libraries/fstorage/nrf_fstorage.c"
    "${NRF_SDK_PATH}/components/libraries/fstorage/nrf_fstorage_nvmc.c"
    "${NRF_SDK_PATH}/components/libraries/fstorage/nrf_fstorage_sd.c"
    "${NRF_SDK_PATH}/components/libraries/memobj/nrf_memobj.c"
    "${NRF_SDK_PATH}/components/libraries/ringbuf/nrf_ringbuf.c"
    "${NRF_SDK_PATH}/components/libraries/experimental_section_vars/nrf_section_iter.c"
    "${NRF_SDK_PATH}/components/libraries/sortlist/nrf_sortlist.c"
    "${NRF_SDK_PATH}/components/libraries/strerror/nrf_strerror.c"
    "${NRF_SDK_PATH}/modules/nrfx/drivers/src/nrfx_rng.c"
    # "${NRF_SDK_PATH}/components/libraries/uart/retarget.c"
    "${NRF_SDK_PATH}/components/libraries/log/src/nrf_log_backend_rtt.c"
    "${NRF_SDK_PATH}/components/libraries/log/src/nrf_log_backend_serial.c"
    # "${NRF_SDK_PATH}/components/libraries/log/src/nrf_log_backend_uart.c"
    "${NRF_SDK_PATH}/components/libraries/log/src/nrf_log_default_backends.c"
    "${NRF_SDK_PATH}/components/libraries/log/src/nrf_log_frontend.c"
    "${NRF_SDK_PATH}/components/libraries/log/src/nrf_log_str_formatter.c"
    "${NRF_SDK_PATH}/external/segger_rtt/SEGGER_RTT.c"
    "${NRF_SDK_PATH}/external/segger_rtt/SEGGER_RTT_printf.c"
    "${NRF_SDK_PATH}/external/mbedtls/library/aes.c"
    "${NRF_SDK_PATH}/external/mbedtls/library/ctr_drbg.c"
    "${NRF_SDK_PATH}/external/mbedtls/library/platform_util.c"
)

# LVGL configuration
# LVGL_PATH is now set via command line parameter
if(EXISTS ${LVGL_PATH})
    # Add LVGL subdirectory
    add_subdirectory(${LVGL_PATH} lvgl)
    
    # LVGL include directories
    set(LVGL_INCLUDE_DIRS
        ${LVGL_PATH}
        ${LVGL_PATH}/src
    )
    
    # Enable LVGL
    set(USE_LVGL TRUE)
    message(STATUS "LVGL found and enabled at: ${LVGL_PATH}")
else()
    set(USE_LVGL FALSE)
    message(WARNING "LVGL not found at: ${LVGL_PATH}")
endif()

# Create executable
add_executable(${PROJECT_NAME} ${PROJECT_SRCS} ${NRF_SDK_SRCS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${NRF_SDK_INCLUDE_DIRS})

# Linker options
target_link_options(${PROJECT_NAME} PRIVATE
    -T${CMAKE_SOURCE_DIR}/${MCU_LINKER_SCRIPT}
    -L${CMAKE_SOURCE_DIR}
    -L${NRF_SDK_PATH}/modules/nrfx/mdk
    ${MCU_FLAGS}
    -mthumb
    -mabi=aapcs
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
    -Wl,--gc-sections
    -Wl,--no-warn-rwx-segments
    --specs=nano.specs
)

if(USE_LVGL)
    target_include_directories(${PROJECT_NAME} PRIVATE ${LVGL_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} lvgl)
endif()

# Create .hex file
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMENT "Creating .hex file")

# Create .bin file
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMENT "Creating .bin file")

# Print size
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.elf
    COMMENT "Printing size information")