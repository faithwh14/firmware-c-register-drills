#include <stdio.h>
#include <stdint.h>

#define BIT(n)      (1u << (n))

// #define MMIO_BASE   0x1a1a1a1a
// #define MMIO_A      MMIO_BASE + 0x0

static uint32_t fake_mmio[4];
#define MMIO_BASE           ((uintptr_t)fake_mmio)
#define AER_UNCOR_STATUS    (MMIO_BASE + 0x4)


// Drill 1

uint32_t MmioRead32(uintptr_t addr) {
    return *(volatile uint32_t*)addr;
}

void MmioWrite32(uintptr_t addr, uint32_t v) {
    *(volatile uint32_t*)addr = v;
}

void ClearW1C(uintptr_t addr, int n) {
    // uint32_t a = MmioRead32(addr);
    // a &= ~BIT(n);
    
    // MmioWrite32(addr, a);
    MmioWrite32(addr, BIT(n));
}

void HwW1cWrite(uintptr_t addr, uint32_t v) {
    *(volatile uint32_t*)addr &= ~v;
}

// Drill 2

uint8_t SetBit8(uint8_t v, int n) {
    return (v | BIT(n));
}

uint8_t ClearBit8(uint8_t v, int n) {
    return (v & ~BIT(n));
}

uint8_t ToggleBit8(uint8_t v, int n) {
    return (v ^ BIT(n));
}

uint8_t TestBit8(uint8_t v, int n) {
    return (v >> n) & 1u;
}

void PrintBit8(uint8_t v) {
    printf("0b");
    
    for (int it = (sizeof(uint8_t) * 8 - 1); it >= 0; it--) {
        putchar(TestBit8(v, it) ? '1' : '0');
    }
    printf("\n");
}

// Drill 3
uint8_t GenMask8(int hi, int lo) {
    // uint8_t v;
    // for (int it = lo; it <= hi; it++) {
    //     SetBit8(v, it);
    // }
    
    // return v;
    
    int gap = (hi - lo) + 1u;
    uint8_t c = (uint8_t)(BIT(gap) - 1u);
    return (c << lo);
}

uint8_t GetField(uint8_t v, int hi, int lo) {
    uint8_t mask = GenMask8(hi, lo);
    return (v & mask) >> lo;
}

uint8_t SetField(uint8_t v, int hi, int lo, uint8_t val) {
    // uint8_t mask = GenMask8(hi, lo);
    // // reset to zero
    // HwW1cWrite(v, mask);
    
    // return (v >> lo = val) & 0xff;
    
    uint8_t mask = GenMask8(hi, lo);
    // HwW1cWrite(v, mask);
    v &= ~mask;
    
    uint8_t tar = (val << lo) & mask;
    
    return v | tar;
}


// main
int main(void) {
    printf("Hello Testing\n");
    PrintBit8(MmioRead32(AER_UNCOR_STATUS));
    
    uint32_t result = MmioRead32(AER_UNCOR_STATUS);
    result |= BIT(4);
    MmioWrite32(AER_UNCOR_STATUS, result);
    
    printf("result value is %x\n", MmioRead32(AER_UNCOR_STATUS));
    PrintBit8(MmioRead32(AER_UNCOR_STATUS));
    
    return 0;
}

