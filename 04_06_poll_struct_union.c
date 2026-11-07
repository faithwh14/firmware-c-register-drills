#include <stdint.h>
#include <stdio.h>

#define BIT(n)          (1u << (n))
#define READY_BIT       0
#define STATUS_OK       0
#define STATUS_TIMEOUT  (-1)

static uint32_t fake_mmio[4];
#define MMIO_BASE       ((uintptr_t)fake_mmio)
#define CTRL_STATUS     (MMIO_BASE + 0x0)

// drill 4

static int hw_delay;

uint32_t MmioRead32(uintptr_t a) {
    return *(volatile uint32_t*)a;
}

void MmioWrite32(uintptr_t a, uint32_t v) {
    *(volatile uint32_t*)a = v;
}

void FakeHwTick(uintptr_t addr) {
    if (hw_delay > 0) {
        hw_delay--;
        if (hw_delay == 0) {
            uint32_t v = MmioRead32(addr);
            v |= BIT(READY_BIT);
            MmioWrite32(addr, v);
        }
    }
}

int WaitBitSet(uintptr_t addr, int bit, int timeout) {
    while (1) {
        uint32_t v = MmioRead32(addr);
        if ((v & BIT(bit)) != 0) {
            return STATUS_OK;
        }
        
        if (timeout == 0) {
            return STATUS_TIMEOUT;
        }
        timeout--;
        
        FakeHwTick(addr);
    }
}

// drill 5
#define CTRL_EN_BIT         0
#define CTRL_DIV_LO         4
#define CTRL_DIV_HI         7
#define TX_READY_BIT        0
// #define STATUS_OK           0
// #define STATUS_TIMEOUT      (-1)

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t STATUS;
    volatile uint32_t DATA;
    volatile uint32_t RSVD;
} UART_REGS;

static int tx_delay;

void FakeTxTick(UART_REGS* u) {
    if (tx_delay > 0) {
        tx_delay--;
        if (tx_delay == 0) {
            u->STATUS |= BIT(TX_READY_BIT);
        }
    }
}

void UartEnable(UART_REGS *u) {
    u->CTRL |= BIT(CTRL_EN_BIT);
}

void UartSetDiv(UART_REGS* u, uint32_t div) {
    uint32_t mask = (BIT(CTRL_DIV_HI - CTRL_DIV_LO + 1) -1) << CTRL_DIV_LO;
    
    u->CTRL &= ~mask;
    u->CTRL |= ((div << CTRL_DIV_LO) & mask);
}

int UartSend(UART_REGS *u, uint8_t b, int timeout) {
    while (1) {
        if ((u->STATUS & BIT()) != 0) {
            u->STATUS = b;
            return STATUS_OK;
        }
        if (timeout == 0) {
            return STATUS_TIMEOUT;
        }
        
        timeout--;
        
        FakeTxTick(u);
    }
}

void DumpRegs(const UART_REGS* u) {
    printf("CTRL=0x%08X, STATUS=0x%08X, DATA=0x%08X\n", u->CTRL, u->STATUS, u->DATA);
}


// DRILL 6

typedef union {
    struct {
        uint32_t En     : 1;
        uint32_t Rsvd0  : 3;
        uint32_t Div    : 4;
        uint32_t Rsvd1  : 24;
    } Bits;
    uint32_t Uint32;
} UART_CTRL;

uint32_t CtrlEnable(uint32_t raw) {
    // UART_CTRL* c = (UART_CTRL*)&raw;
    UART_CTRL c;
    c.Uint32 = raw;
    
    c.Bits.En = BIT(CTRL_EN_BIT);
    return c.Uint32;
}

uint32_t CtrlSetDiv(uint32_t raw, uint32_t div) {
    UART_CTRL c;
    
    uint32_t mask = (BIT(CTRL_DIV_HI - CTRL_DIV_LO + 1) -1) << CTRL_DIV_LO;
    c.Bits.Div &= ~mask;  // feel like initial value is empty
    c.Bits.Div |= ((div << CTRL_DIV_LO) & mask);
    
    return c.Uint32;
}

uint32_t CtrlGetDiv(uint32_t raw) {
    UART_CTRL* c = (UART_CTRL*)&raw;
    
    return c->Bits.Div;
}

uint32_t CtrlSetDivMask(uint32_t raw, uint32_t div) {
    UART_CTRL c;
    
    uint32_t mask = (BIT(CTRL_DIV_HI - CTRL_DIV_LO + 1) -1) << CTRL_DIV_LO;
    c.Bits.Div &= ~mask;  // feel like initial value is empty
    c.Bits.Div |= ((div << CTRL_DIV_LO) & mask);
    
    return c.Uint32;
}

void UartSetDivMmio(volatile uint32_t* reg, uint32_t div) {
    
}












