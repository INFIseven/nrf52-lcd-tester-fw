# Automatically collect all .c files in this directory
file(GLOB IMAGE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

# Add image sources to parent scope
set(IMAGE_SRCS ${IMAGE_SRCS} PARENT_SCOPE)

message(STATUS "Found ${CMAKE_CURRENT_SOURCE_DIR} image files: ${IMAGE_SRCS}")
