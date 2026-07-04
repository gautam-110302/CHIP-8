#include "../Chip8.h"

void Chip8::OP_0x8XY0(){
    V[X] = V[Y];
}

void Chip8::OP_0x8XY1(){
    V[X] = V[X] | V[Y];
}

void Chip8::OP_0x8XY2(){
    V[X] = V[X] & V[Y];
}

void Chip8::OP_0x8XY3(){
    V[X] = V[X] ^ V[Y];
}

void Chip8::OP_0x8XY4(){
    uint16_t sum = V[X] + V[Y];
    if(sum > 255U){
        V[0xF] = 1;
    }
    else{
        V[0xF] = 0;
    }
    V[X] = sum & 0xFF;
}

void Chip8::OP_0x8XY5(){
    if(V[X] >= V[Y]){
        V[0xF] = 1;
    }
    else{
        V[0xF] = 0;
    }
    V[X] = V[X] - V[Y];
}

void Chip8::OP_0x8XY6(){
    //V[X] = V[Y]; //configurable according to which version of games are being used
    // you just need to comment the above line for newer version
    V[0xF] = V[X] & 0x1;
    V[X] = V[X] >> 1;
}

void Chip8::OP_0x8XY7(){
    if(V[Y] >= V[X]){
        V[0xF] = 1;
    }
    else{
        V[0xF] = 0;
    }
    V[X] = V[Y] - V[X];
}

void Chip8::OP_0x8XYE(){
    //V[X] = V[Y]; //configurable according to which version of games are being used
    // you just need to comment the above line for newer version
    V[0xF] = (V[X] >> 7) & 1;
    V[X] = V[X] << 1;
}
