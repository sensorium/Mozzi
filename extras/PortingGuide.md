# Porting Guide

This is a quick checklist of what needs to be done to support a new processor architecture:

1. Add the logic into __hardware_defines.h__ which identifies your new implementation
2. Implement a new AudioConfig\<PLATFORM\>.h
3. Implement a new hardware_imp_\<PLATFORM\>.cpp (I propose to use the arm_mbed as prototype)
4. Implement or activate the following methods in mozzi_pgmspace.h for your environement
    - FLASH_OR_RAM_READ
    - CONSTTABLE_STORAGE
5. Include your AudioConfig\<PLATFORM.h\> to AduioConfigAll.h
5. Activate the Standard Constructor for __MozziControl__ in Mozzi.cpp or implement your custom logic in hardware_imp__\<PLATFORM\>.cpp


Before you start the excercise I recommend to get a proper feeling for the timings

| Function             | Nano 33 Sense  | ESP32     | Pico MBed     | Nano AVR |
|----------------------|----------------|-----------|---------------|----------|
| aSin(SIN2048_DATA)   |  27'000        | 780'500   |   27'000      | 115'000  |              
| Output of 1 Pin      |  53'000        | NA        |   53'500      |          |
| Output of 2 Pins     |  38'000        | NA        |   44'500      | 153'000  |

- The values are in samples per second - rounded by 500) 