#include "../Chip8.h"

// FX07 sets VX to the current value of the delay timer
void Chip8::OP_0xFX07(){
    V[X] = dt;
}

// FX15 sets the delay timer to the value in VX
void Chip8::OP_0xFX15(){
    dt = V[X];
}

// FX18 sets the sound timer to the value in VX
void Chip8::OP_0xFX18(){
    st = V[X];
}

//FX1E the index register I will get the value in VX added to it.
void Chip8::OP_0xFX1E(){
    // originally if the som overflowed the VF register was not set to 1
    // but most games use this property
    // if(I + V[X] < I) V[F] = 1;
    I = I + V[X];
}

// FX0A stops executing instructions and waits for key input (or loops forever, unless a key is pressed).
void Chip8::OP_0xFX0A(){
    uint8_t temp = getPressedKey();
    if(temp < 0x10){
        V[X] = getPressedKey();
    }
    else{
        pc -= 2;
    }
}

// FX29 the index register I is set to the address of the hexadecimal character in VX 
void Chip8::OP_0xFX29(){
    I = getFontAddress(V[X]);
}

// FX33 . It takes the number in VX and converts it to three decimal digits,
// storing these digits in memory at the address in the index register I.
void Chip8::OP_0xFX33(){
    uint8_t temp = V[X];
    memory[I+2] = temp%10;
    temp = temp/10;
    memory[I+1] = temp%10;
    temp = temp/10;
    memory[I] = temp%10;
}

// FX55 the value of each variable register from V0 to VX inclusive
// will be stored in successive memory addresses starting from I
void Chip8::OP_0xFX55(){
    for(uint8_t i=0;i<=X;i++){
        memory[I + i] = V[i];
    }
    
}

// FX55 the value of each variable register from V0 to VX inclusive
// will be set to value in successive memory addresses starting from I
void Chip8::OP_0xFX65(){
    for(uint8_t i=0;i<=X;i++){
        V[i] = memory[I + i];
    }
}