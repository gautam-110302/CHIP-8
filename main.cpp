#include <array>
#include <cstdint>
#include <random>
#include <chrono>
#include <thread>

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
};

// Chip8 Constructor
Chip8::Chip8() : gen(rd()){
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
            if(display[pos] && (memory[I+i] >> (7-j))&1) V[0xF] = 1;
            // finally xor to set or unset the pixel
            display[pos] = display[pos] ^ ((memory[I+i] >> (7-j))&1);
        }
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
    if(V[X] + V[Y] < V[X]){
        V[0xF] = 1;
    }
    else{
        V[0xF] = 0;
    }
    V[X] = V[X] + V[Y];
}

void Chip8::OP_0x8XY5(){
    if(V[X] - V[Y] > V[X]){
        V[0xF] = 0;
    }
    else{
        V[0xF] = 1;
    }
    V[X] = V[X] - V[Y];
}

void Chip8::OP_0x8XY6(){
    V[X] = V[Y]; //configurable according to which version of games are being used
    // you just need to comment the above line for newer version
    V[0xF] = V[X] & 0x1;
    V[X] = V[X] >> 1;
}

void Chip8::OP_0x8XY7(){
    if(V[Y] - V[X] > V[Y]){
        V[0xF] = 0;
    }
    else{
        V[0xF] = 1;
    }
    V[X] = V[Y] - V[X];
}

void Chip8::OP_0x8XYE(){
    V[X] = V[Y]; //configurable according to which version of games are being used
    // you just need to comment the above line for newer version
    V[0xF] = (V[X] >> 7) & 1;
    V[X] = V[X] << 1;
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

void Chip8::OP_0xEX9E(){
    if(keypad[V[X]]) pc += 2;
}

void Chip8::OP_0xEXA1(){
    if(!keypad[V[X]]) pc += 2;
}

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

// dummy function to handle invalid opcodes
void Chip8::OP_NULL(){
    return;
}

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

void Chip8::OP_0x8XYN(){
    (this->*table8[N])();
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

void Chip8::OP_0x0FNN(){
    (this->*tableF[NN])();
}

void Chip8::Cycle(){
    for(int i=0;i<11;i++){
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

    if(dt > 0){
        dt--;
    }

    if(st > 0){
        st--;
    }

}



int main(){

    // make an object of the chip
    Chip8 emulator = Chip8();

    bool run = true;
    // record the start time
    auto startTime = std::chrono::steady_clock::now();
    // the time gap between two cycles
    std::chrono::nanoseconds cycleDelay(16666667); //(1e9/60) 

    while(run){
        // record the current time
        auto currentTime = std::chrono::steady_clock::now();
        // record total time elapsed since last execution of cycle
        auto delay = currentTime - startTime;
        // check if elapsed time is greater than the cycleDelay
        if(delay > cycleDelay){
            // change start to current time
            startTime = startTime + cycleDelay;
            // if yes then start another cycle
            emulator.Cycle();
        }
        // let the thread sleep for 2ms so that this run loop does not mindlessly keep on running and checking if the delay has been achieved
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    return 0;
}