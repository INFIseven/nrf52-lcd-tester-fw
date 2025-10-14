# Instructions  

1. In spi/ folder is a reference nrf52 project which uses SPI, get the idea how its structured and which libraries are needed
2. In src/ create main.c which is a copy of the spi/ project. Include also other necesary files.
3. Look into reference-CMakeLists.txt and create a new CMakeLists.txt which is modified for the current project. As input parameters only the arm gcc path is needed and MCU type which is by default NRF52840.
4. In cmake include also LVGL which is in ../libraries/lvgl.