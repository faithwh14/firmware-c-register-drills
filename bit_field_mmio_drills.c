#include <stdint.h>
#include <stdio.h>

#define BIT(n)      (1u << (n))


uint32_t MmioRead32(uintptr_t reg) {
    return *(volatile uint32_t*)reg;
}

void MmioWrite32(uintptr_t reg, uint32_t v) {
    *(volatile uint32_t*)reg = v;
}


uint8_t SetBit8(uint8_t reg, int n) {
    return reg | BIT(n);
}

uint8_t ClearBit8(uint8_t reg, int n) {
    return reg & ~BIT(n);
}

uint8_t ToggleBit8(uint8_t reg, int n) {
    return reg ^ BIT(n);
}

uint8_t TestBit8(uint8_t reg, int n) {
    return (reg >> n) & 1u;
}

void PrintBit8(uint8_t reg) {
    printf("Testing: 0b");
    
    for (int it = sizeof(reg) * 8 - 1; it >= 0; it--) {
        putchar(TestBit8(reg, it) ? '1' : '0');
    }
    printf("\n");
}

uint8_t GenMask8(int lo, int hi) {
    int diff = hi - lo + 1;
    
    return (uint8_t)(BIT(diff) - 1u) << lo;
}

uint8_t GetField8(int8_t reg, int lo, int hi) {
    uint8_t BitMask = GenMask8(lo, hi);
    
    return (reg & BitMask) >> lo;
}

uint8_t SetField8(uint8_t reg, int lo, int hi, uint8_t val) {
    uint8_t BitMask = GenMask8(lo, hi);
    
    reg &= ~BitMask;
    
    return (uint8_t)(reg | (val << lo) & BitMask);
}

int main(void) {
    printf("hello world\n");
    
    uint8_t reg = 0x32;
    PrintBit8(reg);
    
    uint8_t a = GetField8(reg, 3, 6);
    PrintBit8(a);
    
    reg = SetField8(reg, 3, 6, 10);
    PrintBit8(reg);

    return 0;
}
