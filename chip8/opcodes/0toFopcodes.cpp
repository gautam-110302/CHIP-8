#include "../Chip8.h"

void Chip8::OP_0x00EN(){
    if(NN  == 0xE0){
        OP_0x00E0();
    }
    else if(NN == 0xEE){
        OP_0x00EE();
    }
    else{
        OP_NULL();
    }
}

void Chip8::OP_0x00E0(){
    clearScreen();
}

void Chip8::OP_0x00EE(){
    sp--;
    pc = stack[sp];
}

void Chip8::OP_0x1NNN(){
    pc = NNN;
}

void Chip8::OP_0x2NNN(){
    stack[sp] = pc;
    sp++;
    pc = NNN;
}

void Chip8::OP_0x3XNN(){
    if(V[X] == NN) pc += 2;
}

void Chip8::OP_0x4XNN(){
    if(V[X] != NN) pc += 2;
}

void Chip8::OP_0x5XY0(){
    if(V[X] == V[Y]) pc += 2;
}

void Chip8::OP_0x6XNN(){
    V[X] = NN;
}

void Chip8::OP_0x7XNN(){
    V[X] += NN;
}

void Chip8::OP_0x8XYN(){
    (this->*table8[N])();
}


void Chip8::OP_0x9XY0(){
    if(V[X] != V[Y]) pc += 2;
}

void Chip8::OP_0xANNN(){
    I = NNN;
}

void Chip8::OP_0xBNNN(){
    // for chip8 specifically the instruction BNNN
    // jump to address NNN + V[0]
    pc = NNN + V[0];
    // for chip48 and and superchip it was changed to BXNN 
    // jump to address XNN + V[X]
    // pc = (X << 8 | NN) + V[X];
}

void Chip8::OP_0xCXNN(){
    V[X] = getRandomNumber() & NN;
}

void Chip8::OP_0xDXYN(){
    draw(V[X],V[Y],N);
}

void Chip8::OP_0xEXYN(){
    if(NN  == 0x9E){
        OP_0xEX9E();
    }
    else if(NN == 0xA1){
        OP_0xEXA1();
    }
    else{
        OP_NULL();
    }
}

void Chip8::OP_0xEX9E(){
    if(keypad[V[X]]) pc += 2;
}

void Chip8::OP_0xEXA1(){
    if(!keypad[V[X]]) pc += 2;
}

void Chip8::OP_0x0FNN(){
    (this->*tableF[NN])();
}

// dummy function to handle invalid opcodes
void Chip8::OP_NULL(){
    return;
}