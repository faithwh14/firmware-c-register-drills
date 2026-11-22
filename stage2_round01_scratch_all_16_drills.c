#include <stdio.h>
#include <stdint.h>

#define BIT(n)          (1u << (n))

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// BIT Mask
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
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

void PrintBit(uint8_t reg) {
    printf("0b");
    
    for (size_t it = (sizeof(reg) * 8 - 1); it >= 0; it--) {
        putchar(TestBit(reg, it) ? '1' : '0');
    };
    printf("\n");
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// SWAP
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
uint16_t Swap16(uint16_t value) {
    return (value >> 8) | (value << 8);
}

uint32_t Swap32(uint32_t value) {
    return (uint32_t)(Swap16(value >> 16) | Swap16(value << 16));
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// LIST ENTRY
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
typedef struct _List List;

struct _List {
    List* Forward;
    List* Backward;
};

typedef _Entry {
    uint8_t Signature[4];
    uint8_t Id;
    List* list;
} Entry;

void InitializeList(List* list) {
    list->Forward = list;
    list->Backward = list;
}

// void AddList(List* list, List* new) {
    
// }

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// RING BUFFER
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

typedef struct _Ring {
    struct _Ring* Forward;
    struct _Ring* Backward;
} Ring;


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

int RemoveHeadRing(Ring* ring, Ring* target) {
    Ring* FirstRing = ring;
    
    // if (ring == target) {
    //     ring->Backward->Forward = ring->Forward;
    //     ring->Forward->Backward = ring->Backward;
    // }
    
    while (ring->Forward != FirstRing) {
        // ring = ring->Forward;
        
        if (ring == target) {
            ring->Backward->Forward = ring->Forward;
            ring->Forward->Backward = ring->Backward;
            return 1;
        }
        
        ring = ring->Forward;
    }
    
    return 0;
}

int CountRing(Ring* ring) {
    int count = 0;
    
    while (ring->Forward != ring) {
        count++;
    }
    
    return count;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Capability Walk
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int HasCap(uint8_t* reg) {
    return reg[0x06] << 4 & 1u;
}

int FindCap(uint8_t* reg, int id) {
    if (!HasCap(reg)) {
        return 0;   
    }
    
    int step = 0;
    uint8_t off = reg[0x34] & 0xFC;

    while (off >= 0x40 & step < 48) {
        if (reg[off] == id) {
            return reg[off];
        }
        off = reg[off + 1u] & 0xFC;
        step++;
    }
    return 0;
}

int CountCap(uint8_t* reg) {
    if (!HasCap(reg)) {
        return 0;
    }
    
    int step = 0;
    uint8_t off = reg[0x34] & 0xFC;
    
    while (off >= 0x40 && step < 48) {
        step++;
        off = reg[off + 1u] & 0xFC;
    }
    
    return step;
}

//

uint32_t MmioRead32(uintptr_t addr) {
    return *(volatile uint32_t*)addr;
}

void MmioWrite32(uintptr_t addr, uint32_t value) {
    *(volatile uint32_t*)addr = value;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Alignment
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int IsPow2(uint32_t reg) {
    return (reg & (reg - 1));
}

uint32_t AlignDown(uint32_t reg, int n) {
    return (reg & (uint32_t)(~(BIT(n) - 1)));
}

uint32_t AlignUp(uint32_t reg, int n) {
    return ((reg + (uint32_t)(BIT(n) -1)) & (uint32_t)(~(BIT(n) - 1)));
    // return (reg & (uint32_t)(~(BIT(n) - 1))) + (uint32_t)(BIT(n));
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Polling + Timeout
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int hw_delay = 0;

#define STATUS_READY    0x01
#define STATUS_NOTREADY 0x00

typedef struct _Object {
    uint8_t Status;
    uint32_t Data;
} Object;

void InitObject(Object* obj) {
    obj->Status = STATUS_NOTREADY;
    obj->Status = 0;
}

int FakeHwTick(Object obj) {
    if (obj.Status != STATUS_READY) {
        hw_delay++;
        return 0;
    } 
    
    return 1;
}

int WaitForBits(Object obj, int timeout) {
    while (hw_delay < timeout) {
        if (FakeHwTick(obj)) {
            return STATUS_READY;
        }
    }
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Protocols
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define CR(Record, Type, Field) \
    ((Type*)((uint8_t*)(Record) + offsetof(Type, Field)))

#define Signature   ({0xCE, 0x21, 0x53, 0x12})

typedef struct _Data Data;

typedef struct _Protocol {
    uint32_t Signature;
    uint32_t Id;
    Data* ShrData;
} Protocol;

struct Data {
    uint8_t Data1;
    uint8_t Data2;
    uint8_t Data3;
    uint8_t Status;
}

typedef int EFI_STATUS;
#define EFI_SUCCESS             0
#define EFI_INVALID_PARAMETER   2
#define NIC_SIGNATURE           0x5043494e

typedef struct _MY_PROTO MY_PROTO;

typedef EFI_STATUS (*MY_GET)(MY_PROTO* This, uint32_t* Value);

struct_ MY_PROTO {
    MY_GET Get;
};

typedef struct {
    uint32_t Signature;
    uint32_t Id;
    uintptr_t BarBase;
    MY_PROTO Proto;
} NIC_PRIVATE;

EFI_STATUS NicGet() {
    
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>> Main Function >>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

int main(void) {
    printf("Hello World\n");
    
    // Test Bit
}


