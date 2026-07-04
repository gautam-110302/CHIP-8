#include "chip8/Chip8.h"
#include "platform/Platform.h"

#include <array>
#include <cstdint>
#include <chrono>
#include <thread>
#include <iostream>

int main(int argc, char* argv[]){
    if (argc != 2) {
        std::cout << "Usage: ./chip8.exe <ROM_File>\n";
        return -1;
    }
    // make an object of the chip
    Chip8 emulator = Chip8();
    emulator.LoadROM(argv[1]);
    Platform window = Platform("CHIP-8 Emulator", 640, 320, 64, 32);
    int videoPitch = sizeof(uint32_t) * 64;
    // record the start time
    auto startTime = std::chrono::steady_clock::now();
    // the time gap between two cycles
    std::chrono::nanoseconds cycleDelay(16666667); //(1e9/60)
    bool quit = false;
    std::array<uint8_t,16> keys{};
    while(!quit){
        quit = window.ProcessInput(keys.data());
        // record the current time
        auto currentTime = std::chrono::steady_clock::now();
        // record total time elapsed since last execution of cycle
        auto delay = currentTime - startTime;
        // check if elapsed time is greater than the cycleDelay
        if(delay > cycleDelay){
            // change start to current time
            startTime = startTime + cycleDelay;
            // update the timers at 60Hz
            emulator.updateTimers();
            // execute the cycle 11 times to get a frequency 0f 660Hz for instruction execution
            emulator.updateKeypad(keys.data());
            for(int i=0;i<11;i++){
                emulator.Cycle();
            }
            if(emulator.isDrawFlagSet()){
                window.Update(emulator.getDisplayArray(),videoPitch);
                emulator.unsetDrawFlag();
            }
        }
        // let the thread sleep for 2ms so that this run loop does not mindlessly keep on running and checking if the delay has been achieved
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    return 0;
}