#ifndef REGISTER_H
#define REGISTER_H

#include <stdint.h>

struct reg_t {
    union {
        struct {
            uint8_t A;
            union {
                struct {
                    uint8_t S:1;
                    uint8_t Z:1;
                    uint8_t _pad0:1;
                    uint8_t AC:1;
                    uint8_t _pad1:1;
                    uint8_t P:1;
                    uint8_t _pad2:1;
                    uint8_t CY:1;
                };
                uint8_t FLAGS;
            };
        };
        uint16_t PSW;
    };
    union {
        struct {
            uint8_t C;
            uint8_t B;
        };
        uint16_t BC;
    };
    union {
        struct {
            uint8_t E;
            uint8_t D;
        };
        uint16_t DE;
    };
    union {
        struct {
            uint8_t L;
            uint8_t H;
        };
        uint16_t HL;
    };
    union {
        struct {
            uint8_t SPL;
            uint8_t SPH;
        };
        uint16_t SP;
    };
    uint16_t PC;
};

void printRegisters();

extern struct reg_t _R;

#endif // REGISTER_H
