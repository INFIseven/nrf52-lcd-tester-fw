cmake_minimum_required(VERSION 3.7)

# create compile_commands.json for clang
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(SDK_ROOT "" CACHE STRING "SDK root folder")

if(SDK_ROOT MATCHES "" AND DEFINED ENV{SDK_ROOT})
  set(SDK_ROOT "$ENV{SDK_ROOT}")
endif()

# Determine the Git tag prefix based on OEM_TARGET
message(STATUS "OEM_TARGET: ${OEM_TARGET}")
if(${OEM_TARGET} MATCHES "TQ")
  add_definitions(-DOEM_TARGET_TQ)
elseif(${OEM_TARGET} MATCHES "TREK")
  add_definitions(-DOEM_TARGET_TREK)
elseif(${OEM_TARGET} MATCHES "BOOST")
  add_definitions(-DOEM_TARGET_TQ_BOOST)
endif()

include(${CMAKE_SOURCE_DIR}/CMake/toolchain_gcc.cmake)
include(${CMAKE_SOURCE_DIR}/components/CMake/hpr_components.cmake)
include(${CMAKE_SOURCE_DIR}/components/CMake/hpr_lvgl.cmake)
include(${CMAKE_SOURCE_DIR}/nRF5-SDK/CMake/nrf5_sdk.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LVGL_LIBRARY ${install_dir}/lib/liblvgl.a)

# project(hpr-disp-04-05-fw C ASM)
project(hpr-disp-04-05-fw C ASM)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Remove the excluded files from PROJECT_SRCS
list(REMOVE_ITEM PROJECT_SRCS ${EXCLUDE_FILES})

# Function to get the latest Git tag with the prefix "hpr04" and its hash
function(get_latest_hpr_version output_var commit_count_var commit_hash_var commit_date_var commit_time_var)
    if(${OEM_TARGET} MATCHES "TQ")
      execute_process(
          COMMAND git describe --tags --match "hpr-dis04-color-tq-v*" --long
          OUTPUT_VARIABLE full_tag_description
          OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      string(REGEX REPLACE "^hpr-dis04-color-" "" stripped_tag "${full_tag_description}")
    elseif(${OEM_TARGET} MATCHES "TREK")
      execute_process(
          COMMAND git describe --tags --match "hpr-dis04-color-trek-v*" --long
          OUTPUT_VARIABLE full_tag_description
          OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      string(REGEX REPLACE "^hpr-dis04-color-trek-" "" stripped_tag "${full_tag_description}")
    elseif(${OEM_TARGET} MATCHES "BOOST")
      execute_process(
          COMMAND git describe --tags --match "hpr-dis04-color-boost-v*" --long
          OUTPUT_VARIABLE full_tag_description
          OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      string(REGEX REPLACE "^hpr-dis04-color-boost-" "" stripped_tag "${full_tag_description}")
    endif()

    # Check if the working directory is clean
    execute_process(
        COMMAND git diff --quiet --ignore-submodules HEAD
        RESULT_VARIABLE GIT_DIRTY
    )
    if(GIT_DIRTY EQUAL 0)
        set(HASH_PREFIX "0xc")
    else()
        set(HASH_PREFIX "0xd")
    endif()

    # Extract version, number of commits, and hash
    if(stripped_tag MATCHES "([0-9]+\.[0-9]+\.[0-9]+)-([0-9]+)-g([0-9a-fA-F]+)")
        set(${output_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
        set(${commit_count_var} "${CMAKE_MATCH_2}" PARENT_SCOPE)
        set(${commit_hash_var} "${HASH_PREFIX}${CMAKE_MATCH_3}" PARENT_SCOPE) # Prefixed hash in hex format
    elseif(stripped_tag MATCHES "([0-9]+\.[0-9]+\.[0-9]+)") # Handle clean commits with no additional commits
        set(${output_var} "${CMAKE_MATCH_1}" PARENT_SCOPE)
        set(${commit_count_var} "0" PARENT_SCOPE)
        execute_process(
            COMMAND git rev-parse --short HEAD
            OUTPUT_VARIABLE latest_commit_hash
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(${commit_hash_var} "${HASH_PREFIX}${latest_commit_hash}" PARENT_SCOPE)
    else()
        message(WARNING "Failed to parse version info from Git tag: ${stripped_tag}")
    endif()

    # Get commit date in YYYYMMDD format
    execute_process(
        COMMAND git log -1 --format=%cd --date=format:%Y%m%d
        OUTPUT_VARIABLE commit_date
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(${commit_date_var} "${commit_date}" PARENT_SCOPE)

    # Get commit time in HHMMSS format (as a single string)
    execute_process(
        COMMAND git log -1 --format=%cd --date=format:%H%M%S
        OUTPUT_VARIABLE raw_commit_time
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Remove leading zeros from the full time string
    string(REGEX REPLACE "^0+" "" commit_time "${raw_commit_time}")

    set(${commit_time_var} "${commit_time}" PARENT_SCOPE)
endfunction()

# Determine the Git tag prefix based on OEM_TARGET
if(${OEM_TARGET} MATCHES "TQ")
  set(GIT_TAG_PREFIX "hpr-dis04-color-tq-v")
elseif(${OEM_TARGET} MATCHES "TREK")
  set(GIT_TAG_PREFIX "hpr-dis04-color-trek-v")
elseif(${OEM_TARGET} MATCHES "BOOST")
  set(GIT_TAG_PREFIX "hpr-dis04-color-boost-v")
endif()

# Add option for disabling logs
option(DISABLE_LOGS "Disable NRF logging" OFF)

# Call the function to get the latest tag information
get_latest_hpr_version(LATEST_HPR_VERSION COMMIT_COUNT COMMIT_HASH COMMIT_DATE COMMIT_TIME)

# Check if the tag was found
if(LATEST_HPR_VERSION)
    add_definitions(-DHPR_VERSION_COMPLETE="${LATEST_HPR_VERSION}")
    add_definitions(-DNUM_COMMITS=${COMMIT_COUNT})
    add_definitions(-DCOMMIT_HASH=${COMMIT_HASH})
    add_definitions(-DCOMMIT_DATE=${COMMIT_DATE})
    add_definitions(-DCOMMIT_TIME=${COMMIT_TIME})
    
    message(STATUS "Latest hpr-v tag: ${LATEST_HPR_VERSION}")
    message(STATUS "Number of commits since last tag: ${COMMIT_COUNT}")
    message(STATUS "Commit hash: ${COMMIT_HASH}")
    message(STATUS "Commit date: ${COMMIT_DATE}")
    message(STATUS "Commit time: ${COMMIT_TIME}")
    
    if(LATEST_HPR_VERSION MATCHES "([0-9]+)\.([0-9]+)\.([0-9]+)")
      set(FW_VER_MAJOR ${CMAKE_MATCH_1})
      set(FW_VER_MINOR ${CMAKE_MATCH_2})
      set(FW_VER_PATCH ${CMAKE_MATCH_3})

      add_definitions(-DFW_VER_MAJOR=${FW_VER_MAJOR}U)
      add_definitions(-DFW_VER_MINOR=${FW_VER_MINOR}U)
      add_definitions(-DFW_VER_PATCH=${FW_VER_PATCH}U)

      message(STATUS "Version Major: ${FW_VER_MAJOR}")
      message(STATUS "Version Minor: ${FW_VER_MINOR}")
      message(STATUS "Version Patch: ${FW_VER_PATCH}")
    else()
      message(WARNING "Failed to parse version from Git tag: ${LATEST_HPR_VERSION}")
    endif()
else()
    message(WARNING "No hpr-v tag found in the Git repository.")
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  set(MY_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/hpr-disp-04-05-fw.ld)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
  set(MY_LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/hpr-disp-04-05-fw.ld)
else()
  message(FATAL_ERROR "Unknown build type!")
endif()

set(MY_LINK_FLAGS
  "-Xlinker -Map=${CMAKE_CURRENT_BINARY_DIR}/${target_name}.map"
  "-mthumb -mabi=aapcs -L${SDK_ROOT}/components/toolchain/gcc -L${CMAKE_SOURCE_DIR}"
  "-T${MY_LINKER_SCRIPT}"
  "-Wl,--gc-sections"
  "--specs=nano.specs -lc -lnosys"
)
string(REGEX REPLACE ";" " " MY_LINK_FLAGS "${MY_LINK_FLAGS}")

set(MY_TARGET_COMPILER_FLAGS
  "-DNRF52840_XXAA"
  "-DCONFIG_NFCT_PINS_AS_GPIOS"
  "-DCONFIG_GPIO_AS_PINRESET"
  "-DFLOAT_ABI_HARD"
  "-DUSE_MCU_NRF"

  # "-DANT_PLUS_NETWORK_KEY={0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45}"

  # "-DUSE_APP_CONFIG"
  "-DFREERTOS"
  "-DNRF_SD_BLE_API_VERSION=7"
  "-DSOFTDEVICE_PRESENT"
  "-DINITIALIZE_USER_SECTIONS"
  "-DNO_VTOR_CONFIG"
  "-DLV_CONF_INCLUDE_SIMPLE"
  "-DS340"
  "-DNRF_LOG_USES_RTT=1"
  "-DHPR_DISP_VEN_TQ"
  "-D__HEAP_SIZE__=32768"
  "-D__GNU_LINKER"

  # "-DLV_CONF_PATH=${SDK_ROOT}/../components/include/lv_conf.h"
  "-mcpu=cortex-m4"
  "-Wall -Wextra -Wno-unused-variable -Wno-unused-function -Wno-unused-parameter -Wno-switch -Wno-format -Wno-expansion-to-defined "
  "-fdiagnostics-show-caret"
  "-std=gnu11"
  "-mlittle-endian -munaligned-access -mtp=soft -mfp16-format=ieee"
)

# Add logging disable flag if option is set
if(DISABLE_LOGS)
  list(APPEND MY_TARGET_COMPILER_FLAGS "-DNRF_LOG_ENABLED=0")
endif()

if(${OEM_TARGET} MATCHES "TQ")
  list(APPEND MY_TARGET_COMPILER_FLAGS
    "-DOEM_TQ"
  )
elseif(${OEM_TARGET} MATCHES "BOOST")
  list(APPEND MY_TARGET_COMPILER_FLAGS
    "-DOEM_TQ_BOOST"
  )
elseif(${OEM_TARGET} MATCHES "TREK")
  list(APPEND MY_TARGET_COMPILER_FLAGS
    "-DOEM_TREK"
  )
endif()

# if(${HPR_TARGET} MATCHES "HPR04")
#   list(APPEND MY_TARGET_COMPILER_FLAGS
#     "-DHPR04"
#   )
# endif()
# message(STATUS "HPR_TARGET: ${HPR_TARGET}")

# add_compile_definitions(ANT_PLUS_NETWORK_KEY={0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45})
string(REGEX REPLACE ";" " " MY_TARGET_COMPILER_FLAGS "${MY_TARGET_COMPILER_FLAGS}")

set(MY_COMPILER_FLAGS
  "-Wshadow -Wdouble-promotion -fno-common"
  "-mthumb"
  "-mabi=aapcs"
  "-mfloat-abi=hard"
  "-mfpu=fpv4-sp-d16"

  # keep every function in a separate section, this allows linker to discard unused ones
  "-fmessage-length=0 -fdiagnostics-color=never -fdiagnostics-show-caret -std=gnu11 -mcpu=cortex-m4 -mlittle-endian -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mthumb  -mtp=soft -mfp16-format=ieee"

  # "-D__SIZEOF_WCHAR_T=4 -D__ARM_ARCH_7EM__ -D__SES_ARM -D__ARM_ARCH_FPV4_SP_D16__ -D__HEAP_SIZE__=32768 -D__SES_VERSION=73000 -D__GNU_LINKER -DDEBUG -DCONFIG_GPIO_AS_PINRESET -DFLOAT_ABI_HARD -DINITIALIZE_USER_SECTIONS -DNO_VTOR_CONFIG -DNRF52840_XXAA -DCONFIG_NFCT_PINS_AS_GPIOS -DLV_CONF_INCLUDE_SIMPLE -DUSE_MCU_NRF -DSOFTDEVICE_PRESENT -DS340 -DNRF_SD_BLE_API_VERSION=7 -DFREERTOS -DNRF_LOG_USES_RTT=1 -DHPR_DISP_VEN_TQ \"-DANT_PLUS_NETWORK_KEY = {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45}\""
  "-ffunction-sections"
  "-fdata-sections"
  "-fno-strict-aliasing"
  "-fno-builtin"
  "-fshort-enums"
)

# Add debug flags for Debug build type
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
  list(APPEND MY_COMPILER_FLAGS
    "-O0"
    "-g3"
    "-gdwarf-4"
    "-gpubnames"
    "-DDEBUG"
    "-D DEBUG_NRF"
    "-fomit-frame-pointer -fno-dwarf2-cfi-asm -ffunction-sections -fdata-sections -fshort-enums -fno-common"
  )
endif()

# for release
if(${CMAKE_BUILD_TYPE} MATCHES "Release")
  list(APPEND MY_COMPILER_FLAGS
    "-O2"
    "-g3"
    "-fomit-frame-pointer -fno-dwarf2-cfi-asm -ffunction-sections -fdata-sections -fshort-enums -fno-common"
  )
endif()

string(REGEX REPLACE ";" " " MY_COMPILER_FLAGS "${MY_COMPILER_FLAGS}")

if(CMAKE_SYSTEM_PROCESSOR MATCHES "cortex-m4")
  set(CMAKE_C_FLAGS
    ${MY_TARGET_COMPILER_FLAGS}
    ${MY_COMPILER_FLAGS}
  )
  string(REGEX REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  set(CMAKE_CXX_FLAGS
    ${MY_TARGET_COMPILER_FLAGS}
    ${MY_COMPILER_FLAGS}
  )
  string(REGEX REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

set(BUILD_SHARED_LIBS OFF)

# set(target_name_tq ${PROJECT_NAME}_tq${NAME_POSTFIX})

add_executable(${PROJECT_NAME} "${PROJECT_SRCS}" "${NRF5_SDK_SRCS}")

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "${MY_LINK_FLAGS}")

target_compile_options(${PROJECT_NAME} PUBLIC -DDISPLAY_TQ)

target_include_directories(${PROJECT_NAME} PRIVATE "${HPR_INCLUDE_DIRS}" "${NRF5_SDK_INCLUDE_DIRS}")

add_subdirectory(${CMAKE_SOURCE_DIR}/components/lvgl lvgl)

target_include_directories(lvgl PUBLIC ${INCLUDE_DIRS_LVGL})

target_compile_options(lvgl PRIVATE -O2)

target_link_libraries(${PROJECT_NAME} lvgl)

target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/nRF5-SDK/external/nrf_oberon/lib/cortex-m4/hard-float/liboberon_3.0.8.a)

create_hex(${PROJECT_NAME})
create_hex_complete(${PROJECT_NAME})