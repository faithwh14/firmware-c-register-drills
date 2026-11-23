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
    return (uint32_t)((Swap16(value >> 16) | Swap16(value << 16)));
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// LIST ENTRY
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
typedef struct _List List;

typedef enum _ListType {
    INT_TYPE    = 0x00,
    STR_TYPE    = 0x02,
    JSON_TYPE   = 0x04
} ListType;

struct _List {
    List* Forward;
    List* Backward;
    
    uint8_t Type;
    const char* Name;
};

typedef _Entry {
    uint8_t Signature[4];
    uint8_t Id;
    List* list;
} Entry;

void InitializeList(List* list) {
    list->Forward = list;
    list->Backward = list;
    
    list->Type = INT_TYPE;
    list->Name = "Testing";
}

void InsertTailList(List* list, List* new) {
    List* curr = list;
    
    while (curr->Forward != ring) {
        curr = curr->Forward;
    }
    
    curr->Forward = new;
    new->Forward = ring;
    ring->Backward = new;
    new->Backward = curr;
}

int RemoveHeadList(List* list, List* target) {
    List* FirstList = list;
    
    // if (ring == target) {
    //     ring->Backward->Forward = ring->Forward;
    //     ring->Forward->Backward = ring->Backward;
    // }
    
    while (list->Forward != FirstList) {
        // ring = ring->Forward;
        
        if (list == target) {
            list->Backward->Forward = list->Forward;
            list->Forward->Backward = list->Backward;
            return 1;
        }
        
        list = list->Forward;
    }
    
    return 0;
}

int CountList(List* list) {
    int count = 0;
    
    while (list->Forward != list) {
        count++;
    }
    
    return count;
}
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// RING BUFFER
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define RING_SIZE 12

typedef struct _Ring {
    // struct _Ring* Forward;
    // struct _Ring* Backward;
    uint8_t buffer[RING_SIZE];
    int head;
    int tail;
} Ring;


void InitializeRing(Ring* ring) {
    ring->head = 0;
    ring->tail = 0;
}

void PushHead(Ring* ring, uint8_t value) {
    if (ring->head <= RING_SIZE) {
        ring->head ++;
    } else {
        ring->head = 0;
    }
    
    ring->buffer[ring->head] = value;
}

uint8_t PopTail(Ring* ring) {
    // ring->tail ++;
    uint8_t value;
    
    value = ring->buffer[ring->tail];
    
    if (ring->tail <= RING_SIZE) {
        ring->tail ++;
    } else {
        ring->tail = 0;
    }
    
    return value;
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

int FindExtCap(uint8_t* reg) {
    int step = 0;
    uint8_t off = reg[0x100] & 0xFFC;
    
    while (off > 0x100 || step < 480) {
        //
    }
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// MMIO Read & Write
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

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
#define STATUS_TIMEOUT  0x02

typedef struct _Object {
    uint8_t Status;
    uint32_t Data;
} Object;

void InitObject(Object* obj) {
    obj->Status = STATUS_NOTREADY;
    obj->Status = 0;
}

int FakeHwTick(Object obj, int timeout) {
    if (hw_delay > 0) {
        if (hw_delay == timeout) {
            obj.Status = STATUS_TIMEOUT;
            return 0;
        }
        
        hw_delay ++;
    }
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
#define BASE_CR(Record, Type, Field) \
    ((Type*)(uint8_t)Record) - offsetby(Type, Field))

typedef void (Shared*)(uint32_t data);

typedef struct _Protocol {
    uint32_t Signature;
    uint32_t Id;
    uint32_t BarSize;
    Shared shared;
} Protocol;

void ShardFn(uint32_t data) {
    printf("This is for testing\n");
}

void ProtocolGet(Shared* shared) {
    Protocol* proto;
    
    proto = BASE_CR(shared, Protocol, shared);
    
    return proto;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// Endian
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
uint32_t ReadLe32(uint8_t* reg) {
    return (uint32_t)(reg[0] | reg[1] << 8 | reg[2] << 16 | reg[3] << 24);
}

uint32_t ReadBe32(uint8_t* reg) {
    return (uint32_t)(reg[3] | reg[2] << 8 | reg[1] << 16 | reg[0] << 24);
}

void WriteBe32(uint8_t* reg, uint32_t value) {
    *(volatile uint32_t*)reg = value;
}

// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// ECAM vs Legacy Mmio?
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>> Main Function >>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

int main(void) {
    printf("Hello World\n");
    
    // Test Bit
}

