#pragma once

#include <array>
#include <cstdint>
#include <random>

class Chip8{
    private:
        //memory allocated to the chip
        std::array<uint8_t,4096> memory{};

        // the data registers
        std::array<uint8_t,16> V{}; 

        // stack and stack Pointer
        std::array<uint16_t, 16> stack{};
        uint8_t sp{};

        // display
        std::array<uint32_t,64*32> display{};

        // draw flag
        bool drawFlag;
        // index register
        uint16_t I;

        // program counter
        uint16_t pc;

        // delay timer
        uint8_t dt;

        // sound timer
        uint8_t st;

        // font start index
        uint8_t fontStart;

        // key array
        std::array<bool, 16> keypad{};

        std::random_device rd; 

        std::mt19937 gen; 

        std::uniform_int_distribution<uint8_t> distr{0, 255}; 

        void storeFont();
        void clearScreen();
        uint8_t getFontAddress(uint16_t f);
        uint8_t getPressedKey();
        uint8_t getRandomNumber();
        void draw(uint8_t x, uint8_t y, uint8_t N);

        uint16_t opcode, NNN;
        uint8_t X, Y, N, NN;

        using Instruction = void (Chip8::*)();
        std::array<Instruction,16> table;
        std::array<Instruction,16> table8;
        std::array<Instruction,256> tableF;

        void initializeTable();
        void initializeTable8();
        void initializeTableF();

        void OP_0x00E0();
        void OP_0x00EE();
        void OP_0x1NNN();
        void OP_0x2NNN();
        void OP_0x3XNN();
        void OP_0x4XNN();
        void OP_0x5XY0();
        void OP_0x6XNN();
        void OP_0x7XNN();
        void OP_0x8XY0();
        void OP_0x8XY1();
        void OP_0x8XY2();
        void OP_0x8XY3();
        void OP_0x8XY4();
        void OP_0x8XY5();
        void OP_0x8XY6();
        void OP_0x8XY7();
        void OP_0x8XYE();
        void OP_0x9XY0();
        void OP_0xANNN();
        void OP_0xBNNN();
        void OP_0xCXNN();
        void OP_0xDXYN();
        void OP_0xEX9E();
        void OP_0xEXA1();
        void OP_0xFX07();
        void OP_0xFX15();
        void OP_0xFX18();
        void OP_0xFX1E();
        void OP_0xFX0A();
        void OP_0xFX29();
        void OP_0xFX33();
        void OP_0xFX55();
        void OP_0xFX65();
        void OP_NULL();
        void OP_0x00EN();
        void OP_0x8XYN();
        void OP_0xEXYN();
        void OP_0x0FNN();
    public:
        Chip8();
        void Cycle();
        void LoadROM(char const* filename);
        void updateTimers();
        bool isDrawFlagSet();
        void unsetDrawFlag();
        void updateKeypad(const uint8_t* keys);
        const uint32_t* getDisplayArray();
};