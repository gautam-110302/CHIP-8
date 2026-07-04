#include "chip8.h"

#include <fstream>
#include <iostream>

const uint8_t fontSize = 0x50;

std::array<uint8_t,fontSize> font = 
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// Chip8 Constructor
Chip8::Chip8() : gen(rd()){
    pc = 0x200;
    sp = 0;
    I = 0;

    dt = 0;
    st = 0;
    drawFlag = false;

    stack.fill(0);
    V.fill(0);
    memory.fill(0);

    fontStart = 0x50;
    storeFont();
    initializeTable();
}

void Chip8::storeFont(){
    for(uint8_t i = 0 ; i < fontSize ; i++) {
        memory[fontStart + i] = font[i];
    }
}

void Chip8::initializeTable(){
    table = {
        &Chip8::OP_0x00EN, &Chip8::OP_0x1NNN, &Chip8::OP_0x2NNN, &Chip8::OP_0x3XNN,
        &Chip8::OP_0x4XNN, &Chip8::OP_0x5XY0, &Chip8::OP_0x6XNN, &Chip8::OP_0x7XNN,
        &Chip8::OP_0x8XYN, &Chip8::OP_0x9XY0, &Chip8::OP_0xANNN, &Chip8::OP_0xBNNN,
        &Chip8::OP_0xCXNN, &Chip8::OP_0xDXYN, &Chip8::OP_0xEXYN, &Chip8::OP_0x0FNN
    };
    initializeTable8();
    initializeTableF();
}

void Chip8::initializeTable8(){
    for(uint8_t i=0;i<16;i++){
        table8[i] = &Chip8::OP_NULL;
    }
    table8[0x0] = &Chip8::OP_0x8XY0;
    table8[0x1] = &Chip8::OP_0x8XY1;
    table8[0x2] = &Chip8::OP_0x8XY2;
    table8[0x3] = &Chip8::OP_0x8XY3;
    table8[0x4] = &Chip8::OP_0x8XY4;
    table8[0x5] = &Chip8::OP_0x8XY5;
    table8[0x6] = &Chip8::OP_0x8XY6;
    table8[0x7] = &Chip8::OP_0x8XY7;
    table8[0xE] = &Chip8::OP_0x8XYE;
}

void Chip8::initializeTableF(){
    for(uint16_t i=0;i<256;i++){
        tableF[i] = &Chip8::OP_NULL;
    }
    tableF[0x07] = &Chip8::OP_0xFX07;
    tableF[0x0A] = &Chip8::OP_0xFX0A;
    tableF[0x15] = &Chip8::OP_0xFX15;
    tableF[0x18] = &Chip8::OP_0xFX18;
    tableF[0x1E] = &Chip8::OP_0xFX1E;
    tableF[0x29] = &Chip8::OP_0xFX29;
    tableF[0x33] = &Chip8::OP_0xFX33;
    tableF[0x55] = &Chip8::OP_0xFX55;
    tableF[0x65] = &Chip8::OP_0xFX65;
}

void Chip8::LoadROM(char const* filename) {
    // 1. Open the file as a binary stream, and move the cursor to the end (ate)
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        // get the file size
        std::streamsize size = file.tellg();

        // bring back the cursor to the beginning
        file.seekg(0, std::ios::beg);

        // read bytes directly into yhe memory array starting at index 0x200
        file.read(reinterpret_cast<char*>(&memory[0x200]), size);

        // close the file
        file.close();
    } else {
        std::cout << "Failed to open ROM file: " << filename << "\n";
    }
}

void Chip8::Cycle(){
    opcode = (memory[pc]<<8) | memory[pc+1];
    pc += 2;
    X = (opcode & 0x0F00) >> 8;
    Y = (opcode & 0x00F0) >> 4;
    N = opcode & 0x000F;
    NN = opcode & 0x00FF;
    NNN = opcode & 0x0FFF;
    uint8_t category = opcode >> 12;
    (this->*table[category])();
}

void Chip8::updateKeypad(const uint8_t* keys){
    for(uint8_t i=0;i<0x10;i++){
        keypad[i] = keys[i] == 1 ? true : false;
    }
}

const uint32_t* Chip8::getDisplayArray(){
    return display.data();
}

bool Chip8::isDrawFlagSet(){
    return drawFlag;
}

void Chip8::unsetDrawFlag(){
    drawFlag = false;
}

void Chip8::updateTimers(){
    if(dt > 0){
        dt--;
    }

    if(st > 0){
        st--;
    }
}

// function to clear screen
void Chip8::clearScreen(){
    for(int i=0;i<64*32;i++){
        display[i] = 0;
    }
}

// function to get start address of a character 0 to F
uint8_t Chip8::getFontAddress(uint16_t f){
    uint8_t address = f*5 + fontStart;
    return address;
}

// function to get the pressed key
uint8_t Chip8::getPressedKey(){
    for(uint8_t i=0;i<0x10;i++){
        if(keypad[i]){
            return i;
        }
    }
    return 0x10;
}

// function to get an randdom number in range [0,255]
uint8_t Chip8::getRandomNumber(){
    uint8_t random_num = distr(gen);
    return random_num;
}

// function to draw on screen
void Chip8::draw(uint8_t x, uint8_t y, uint8_t N){
    // wrap X
    x = x & 0x3F;
    // wrap Y
    y = y & 0x1F;
    // unset VF
    V[0xF] = 0;
    // start the drawing process
    for(uint8_t i=0;i<N;i++){
        // if the y coordinate is greater than or equal to 32 break
        if((y + i) >= 32) break;
        for(uint8_t j=0;j<8;j++){
            // if the x coordinate is greater than or equal to 64 break
            if((x + j) >= 64) break;
            // compressing 2D to 1D
            // position shpuld be y*64 + x
            uint16_t pos = ((y + i) << 6) | (x+j);
            // check if there is a collision, if there is set VF to 1
            // since the msb should xor with the first coordinate we right shift according to that
            //if(display[pos] && (memory[I+i] >> (7-j))&1) V[0xF] = 1;
            // finally xor to set or unset the pixel
            //display[pos] = display[pos] ^ ((memory[I+i] >> (7-j))&1);
            // the above is logically correct but if you want to use RGBA8888, then white(ON) has value 0xFFFFFFFF and the value of black is 0 (OFF) 
            // so we first get whether we wan to flip the value or not from the index register
            uint8_t sprite = (memory[I+i] >> (7-j))&1;
            if(sprite && display[pos] != 0){
                V[0xF] = 1;
            }
            if(sprite){
                display[pos] ^= 0xFFFFFFFF;
            }
            drawFlag = true;
        }
    }
}