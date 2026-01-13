# PodPedal

## Description

A personal learning project to get me learning DSP, C++, and the Daisy ecosystem, using the Daisy Pod and some peripherals.

Oh, and to make a kick-ass custom guitar pedal :D

### Required hardware

The project currently uses:

- [Daisy Pod](https://daisy.audio/product/Daisy-Pod/)
- A cheap 0.96in 127x64 pixel OLED module similar to [this](https://www.amazon.fr/dp/B07J2QWF43?ref=cm_sw_r_cso_fm_apan_dp_2B1QBJP3VCRGF9CK1TB4&social_share=cm_sw_r_cso_fm_apan_dp_2B1QBJP3VCRGF9CK1TB4). 

#### OLED Wiring

I have the OLED hooked up to my DaisyPod using the pins available on the right hand side of this [pinout diagram](https://daisy.audio/product/Daisy-Pod/#pinout). To make things semi permanent I soldered a 1x12 female pin header to the exposed pins so that I can run jumper wires from the DaisyPod to a breadboard to connect additional modules such as the OLED display.

The OLED module is wired up as follows:
  - Ground: DaisyPod GND    -> OLED GND
  - Power:  DaisyPod 3V3D   -> OLED VDD 
  - Data:   DaisyPod SDA    -> OLED SDA
  - Clock:  DaisyPod SLK    -> OLED SCK 

## Installation 

### 1. Clone the repo

**NOTE**: Make sure that you include the submodules using `git clone --recurse-submodules <REPO_URL> <INSTALLATION_PATH>`. If you forget to use the `--recurse-submodules` then the DaisySP and libDaisy dependencies will not be installed. If this is the case, you can run `git submodule update --init --recursive` after cloning to install the submodules.

### 2. Install the Daisy toolchain

Follow the instructions on the Daisy Docs for [installing the C++ toolchain](https://daisy.audio/tutorials/cpp-dev-env/#1-install-the-toolchain) depending on your OS. 

## Build and send to device

**NOTE**: All the valid tasks are in `.vscode/tasks.json`

- Put your DaisyPod in flashable mode by pressing and holding the BOOT button and pressing the RESET button (indicated on the [Seed](https://daisy.audio/hardware/Seed/#pinout))
  - NOTE: then you can let go of both
- Run the task `build_and_program_dfu`
- You should see a process start in the terminal with an output which ends in

```
Downloading element to address = 0x08000000, size = 94000
Erase           [=========================] 100%        94000 bytes
Erase    done.
Download        [=========================] 100%        94000 bytes
Download done.
File downloaded successfully
Submitting leave request...
dfu-util: Error during download get_status
make: *** [program-dfu] Error 74
```

*Note that the output ends in an error. This is normal, just don't ask me why!*

The program should now be running on your DaisyPod! If you encounter any errors, try running the tasks `build_all` first. 

## A note about LLM usage / code generation

This is a learning project. The goal is not to crank out features as fast as possible. 

I am **not** vibe coding. 

However, if I get stuck, and there are no easy answers that I can find using my debugging skills and GoogleFu, I am not depriving myself of asking ChatGPT or Claude.

For this project the rules I've put in place for myself are:
- All code and documentation is written by hand
- Any LLM code completion functionality is snoozed or off while coding in this project
- If I don't understand something, start by checking human sources of knowledge
  - Docs
  - Code
  - Community posts
- If I still don't understand, free feel to ask an LLM with the following constraints
  - Do not copy and paste code -- type it out
  - Don't use code you don't understand; if you don't understand, continue "chatting" or digging until you do
- When you learn something essential and new, you should probably document it (I'm often using `// LEARNING: ` comments that would often be uninteresting in a real project but serve as a reminder for me).

It's too easy to be tempted into using an agent to go quickly, but I don't want a product. I want to learn, and I want to build something that I understand and is mine. 