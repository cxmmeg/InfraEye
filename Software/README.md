# InfraEye

InfraEye - low cost thermal image camera project, uses SoC ESP32. Software is developed using [ESP-IDF (Espressif IoT Development Framework) Toolchain](<https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#introduction>).

## Build Configuration using ESP-IDF Toolchain

#### Prerequisites

* ESP-IDF Toolchain is set up on the machine according to [*Get Started* Guide](<https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html#introduction>)
* ESP-IDF Toolchain is integrated into Eclipse IDE according to [*Build and Flash with Eclipse IDE* Guide](<https://docs.espressif.com/projects/esp-idf/en/latest/get-started/eclipse-setup.html>)

You should be able to build and flash LED blink example (present in ESP-IDF Toolchain) using Eclipse IDE.

#### Creating project in Eclipse IDE

Process of creating project is very similar to the process described in the [guide](<https://docs.espressif.com/projects/esp-idf/en/latest/get-started/eclipse-setup.html>).

In step **Import New Project**, for *Existing Code Location*, put path to directory *InfraEye* of your repository working copy. For example: *C:/Git/InfraEye/Software/InfraEye/*.

In step **Project Properties**, in editing *PATH* variable, paste only *C:\msys32\mingw32\bin;C:\msys32\opt\xtensa-esp32-elf\bin;C:\msys32\usr\bin* everything else should be deleted.

Follow the guide and finish creating project. 

### Building the project

In step **Building in Eclipse**, *make menuconfig* needs to be run outside Eclipse using command line prompt.

1. Run *mingw32.exe* from *msys32* directory.
2. Use command *make -C /c/Git/InfraEye/Software/InfraEye menuconfig*. Using *-C* option specify path of InfraEye directory in repository working copy. 

Configuration window shows up, if everything is correct. Continue according the guide. 

Now, you should be able to build and flash InfraEye project using Eclipse IDE.