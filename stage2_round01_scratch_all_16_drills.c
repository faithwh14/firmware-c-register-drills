#include <stdio.h>
#include <stdint.h>

#define BIT(n)          (1u << (n))


uint8_t SetBit(uint8_t reg, int n) {
    return reg | BIT(n);
}

uint8_t ClearBit(uint8_t reg, int n) {
    return reg & (~BIT(n));
}

uint8_t ToggleBit(uint8_t reg, int n) {
    return reg ^ BIT(n);
}

uint8_t TestBit(uint8_t reg, int n) {
    return (reg >> n) & 1u;
}

//
uint16_t Swap16(uint16_t value) {
    return (value >> 8) | (value << 8);
}

uint32_t Swap32(uint32_t value) {
    return (uint32_t)(Swap16(value >> 16) | Swap16(value << 16));
}

//

struct List {
    List *Forward;
    List *Backward;
};

void InitializeList(List* list) {
    list->Forward = list;
    list->Backward = list;
}

// void AddList(List* list, List* new) {
    
// }


//

struct Ring {
    Ring* Forward;
    Ring* Backward;
};

void InitializeRing(Ring* ring) {
    ring->Forward = ring;
    ring->Backward = ring;
}

void InsertTailRing(Ring* ring, Ring* new) {
    Ring* curr = ring;
    
    while (curr->Forward != ring) {
        curr = curr->Forward;
    }
    
    curr->Forward = new;
    new->Forward = ring;
    ring->Backward = new;
    new->Backward = curr;
}

//
int HasCap(uint8_t* reg) {
    return reg[0x06] << 4 & 1u;
}

int FindCap(uint8_t* reg, int id) {
    if (!HasCap(reg)) {
        return 0;   
    }
    
    int step = 0;
    uint8_t cap_id = reg[0x34];
}



int main(void) {
    printf("Hello World\n");
}
