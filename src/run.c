#include <stdio.h>
#include "run.h"
#include "register.h"
#include "disasm.h"

void flags(uint8_t v) {
    _R.S = (v >> 7) | (!!v);
    _R.Z = (v == 0 ? 1 : 0);
    _R.P = __builtin_parity(v);
    // _R.AC
}

#define LXI(P, R0, R1) \
    R1 = P[_R.PC + 1]; \
    R0 = P[_R.PC + 2]; \
    _R.PC += 3

#define STAX(P, R) \
    P[R] = _R.A; \
    ++_R.PC

#define LDAX(P, R) \
    _R.A = P[R]; \
    ++_R.PC

#define SHLD(P, R0, R1) \
    P[_R.PC + 1] = R1; \
    P[_R.PC + 2] = R0; \
    _R.PC += 3

#define LHLD(P, R0, R1) \
    R1 = P[_R.PC + 1]; \
    R0 = P[_R.PC + 2]; \
    _R.PC += 3

#define INX(R) \
    ++R; \
    ++_R.PC

#define DCX(R) \
    --R; \
    ++_R.PC

#define INR(R) \
    ++R; \
    flags(R); \
    ++_R.PC

#define DCR(R) \
    --R; \
    flags(R); \
    ++_R.PC

#define MVI(P, R) \
    R = P[_R.PC + 1]; \
    _R.PC += 2

#define DAD(R) \
    _R.HL += R; \
    ++_R.PC

#define MOV(R0, R1) \
    R0 = R1; \
    ++_R.PC

#define ADD(R) \
    _R.A += R; \
    flags(_R.A); \
    ++_R.PC

#define ADC(R) \
    _R.A = R + _R.CY; \
    flags(_R.A); \
    ++_R.PC

#define SUB(R) \
    _R.A -= R; \
    flags(_R.A); \
    ++_R.PC

#define SBB(R) \
    _R.A = R - _R.CY; \
    flags(_R.A); \
    ++_R.PC

#define ANA(R) \
    _R.A &= R; \
    flags(_R.A); \
    ++_R.PC

#define XRA(R) \
    _R.A ^= R; \
    flags(_R.A); \
    ++_R.PC

#define ORA(R) \
    _R.A |= R; \
    flags(_R.A); \
    ++_R.PC

#define CMP(R) \
    _R.A -= R; \
    flags(_R.A); \
    ++_R.PC

#define PUSH(R0, R1) \
    program[_R.SP - 2] = R0; \
    program[_R.SP - 1] = R1; \
    _R.SP -= 2; \
    ++_R.PC;

#define POP(R0, R1) \
    R0 = program[_R.SP + 1]; \
    R1 = program[_R.SP]; \
    _R.SP += 2; \
    ++_R.PC;

#define JMP() \
    _R.PC = (program[_R.PC + 2] << 8) | program[_R.PC + 1]

#define CALL() \
    program[_R.SP - 1] = (_R.PC & 0xFF00) >> 8; \
    program[_R.SP - 2] = (_R.PC & 0x00FF); \
    _R.SP += 2; \
    _R.PC = (program[_R.PC + 2] << 8) | program[_R.PC + 1]

#define RET() \
    _R.PCL = program[_R.SP]; \
    _R.PCH = program[_R.SP + 1]; \
    _R.SP += 2

void run(uint8_t * program, long programSize)
{
    while (_R.PC < programSize) {
        printRegisters();
        disasmInst(program, _R.PC);
        getc(stdin);

        switch (program[_R.PC]) {
        // NOP
        case 0x00: ++_R.PC; break;

        case 0x01: LXI(program, _R.B, _R.C); break;
        case 0x02: STAX(program, _R.BC); break;
        case 0x03: INX(_R.BC); break;
        case 0x04: INR(_R.B); break;
        case 0x05: DCR(_R.B); break;
        case 0x06: MVI(program, _R.B); break;

        // RLC
        case 0x07:
            _R.CY = (_R.A >> 7) | (!!_R.A);
            _R.A = (_R.A << 1) | _R.CY;
            ++_R.PC;
            break;

        case 0x09: DAD(_R.BC); break;
        case 0x0A: LDAX(program, _R.BC); break;
        case 0x0B: DCX(_R.BC); break;
        case 0x0C: INR(_R.C); break;
        case 0x0D: DCR(_R.C); break;
        case 0x0E: MVI(program, _R.C); break;

        // RRC
        case 0x0F:
            _R.CY = (_R.A & 0x01);
            _R.A = (_R.A >> 1) | (_R.CY << 7);
            ++_R.PC;
            break;

        case 0x11: LXI(program, _R.D, _R.E); break;
        case 0x12: STAX(program, _R.DE); break;
        case 0x13: INX(_R.DE); break;
        case 0x14: INR(_R.D); break;
        case 0x15: DCR(_R.D); break;
        case 0x16: MVI(program, _R.D); break;

        // RAL
        case 0x17:
            _R.CY = (_R.A & 0x01);
            _R.A = (_R.A >> 1) | (_R.CY << 7);
            ++_R.PC;
            break;

        case 0x19: DAD(_R.DE); break;
        case 0x1A: LDAX(program, _R.DE); break;
        case 0x1B: DCX(_R.DE); break;
        case 0x1C: INR(_R.E); break;
        case 0x1D: DCR(_R.E); break;
        case 0x1E: MVI(program, _R.E); break;

        // RAR
        case 0x1F:
            ++_R.PC;
            break;

        case 0x21: LXI(program, _R.H, _R.L); break;
        case 0x22: SHLD(program, _R.H, _R.L); break;
        case 0x23: INX(_R.HL); break;
        case 0x24: INR(_R.H); break;
        case 0x25: DCR(_R.H); break;
        case 0x26: MVI(program, _R.H); break;
        case 0x29: DAD(_R.HL); break;
        case 0x2A: LHLD(program, _R.H, _R.L); break;
        case 0x2B: DCX(_R.HL); break;
        case 0x2C: INR(_R.L); break;
        case 0x2D: DCR(_R.L); break;
        case 0x2E: MVI(program, _R.L); break;

        // CMA
        case 0x2F:
            _R.A = ~_R.A;
            ++_R.PC;
            break;

        case 0x31: LXI(program, _R.SPH, _R.SPL); break;

        // STA
        case 0x32:
            program[(program[_R.PC + 2] << 8) | program[_R.PC + 1]] = _R.A;
            ++_R.PC;
            break;

        case 0x33: INX(_R.SP); break;
        case 0x34: INR(program[_R.HL]); break;
        case 0x35: DCR(program[_R.HL]); break;
        case 0x36: MVI(program, program[_R.HL]); break;

        // STC
        case 0x37:
            _R.CY = 1;
            ++_R.PC;
            break;

        case 0x39: DAD(_R.SP); break;

        // LDA
        case 0x3A:
            _R.A = program[(program[_R.PC + 2] << 8) | program[_R.PC + 1]];
            ++_R.PC;
            break;

        case 0x3B: DCX(_R.SP); break;
        case 0x3C: INR(_R.A); break;
        case 0x3D: DCR(_R.A); break;
        case 0x3E: MVI(program, _R.A); break;

        // CMC
        case 0x3F:
            _R.CY = ~_R.CY;
            ++_R.PC;
            break;

        case 0x40: MOV(_R.B, _R.B); break;
        case 0x41: MOV(_R.B, _R.C); break;
        case 0x42: MOV(_R.B, _R.D); break;
        case 0x43: MOV(_R.B, _R.E); break;
        case 0x44: MOV(_R.B, _R.H); break;
        case 0x45: MOV(_R.B, _R.L); break;
        case 0x46: MOV(_R.B, program[_R.HL]); break;
        case 0x47: MOV(_R.B, _R.A); break;
        case 0x48: MOV(_R.C, _R.B); break;
        case 0x49: MOV(_R.C, _R.C); break;
        case 0x4a: MOV(_R.C, _R.D); break;
        case 0x4b: MOV(_R.C, _R.E); break;
        case 0x4c: MOV(_R.C, _R.H); break;
        case 0x4d: MOV(_R.C, _R.L); break;
        case 0x4e: MOV(_R.C, program[_R.HL]); break;
        case 0x4f: MOV(_R.C, _R.A); break;
        case 0x50: MOV(_R.D, _R.B); break;
        case 0x51: MOV(_R.D, _R.C); break;
        case 0x52: MOV(_R.D, _R.D); break;
        case 0x53: MOV(_R.D, _R.E); break;
        case 0x54: MOV(_R.D, _R.H); break;
        case 0x55: MOV(_R.D, _R.L); break;
        case 0x56: MOV(_R.D, program[_R.HL]); break;
        case 0x57: MOV(_R.D, _R.A); break;
        case 0x58: MOV(_R.E, _R.B); break;
        case 0x59: MOV(_R.E, _R.C); break;
        case 0x5a: MOV(_R.E, _R.D); break;
        case 0x5b: MOV(_R.E, _R.E); break;
        case 0x5c: MOV(_R.E, _R.H); break;
        case 0x5d: MOV(_R.E, _R.L); break;
        case 0x5e: MOV(_R.E, program[_R.HL]); break;
        case 0x5f: MOV(_R.E, _R.A); break;
        case 0x60: MOV(_R.H, _R.B); break;
        case 0x61: MOV(_R.H, _R.C); break;
        case 0x62: MOV(_R.H, _R.D); break;
        case 0x63: MOV(_R.H, _R.E); break;
        case 0x64: MOV(_R.H, _R.H); break;
        case 0x65: MOV(_R.H, _R.L); break;
        case 0x66: MOV(_R.H, program[_R.HL]); break;
        case 0x67: MOV(_R.H, _R.A); break;
        case 0x68: MOV(_R.L, _R.B); break;
        case 0x69: MOV(_R.L, _R.C); break;
        case 0x6a: MOV(_R.L, _R.D); break;
        case 0x6b: MOV(_R.L, _R.E); break;
        case 0x6c: MOV(_R.L, _R.H); break;
        case 0x6d: MOV(_R.L, _R.L); break;
        case 0x6e: MOV(_R.L, program[_R.HL]); break;
        case 0x6f: MOV(_R.L, _R.A); break;
        case 0x70: MOV(program[_R.HL], _R.B); break;
        case 0x71: MOV(program[_R.HL], _R.C); break;
        case 0x72: MOV(program[_R.HL], _R.D); break;
        case 0x73: MOV(program[_R.HL], _R.E); break;
        case 0x74: MOV(program[_R.HL], _R.H); break;
        case 0x75: MOV(program[_R.HL], _R.L); break;
        case 0x77: MOV(program[_R.HL], _R.A); break;
        case 0x78: MOV(_R.A, _R.B); break;
        case 0x79: MOV(_R.A, _R.C); break;
        case 0x7a: MOV(_R.A, _R.D); break;
        case 0x7b: MOV(_R.A, _R.E); break;
        case 0x7c: MOV(_R.A, _R.H); break;
        case 0x7d: MOV(_R.A, _R.L); break;
        case 0x7e: MOV(_R.A, program[_R.HL]); break;
        case 0x7f: MOV(_R.A, _R.A); break;

        case 0x80: ADD(_R.B); break;
        case 0x81: ADD(_R.C); break;
        case 0x82: ADD(_R.D); break;
        case 0x83: ADD(_R.E); break;
        case 0x84: ADD(_R.H); break;
        case 0x85: ADD(_R.L); break;
        case 0x86: ADD(program[_R.HL]); break;
        case 0x87: ADD(_R.A); break;
        case 0x88: ADC(_R.B); break;
        case 0x89: ADC(_R.C); break;
        case 0x8A: ADC(_R.D); break;
        case 0x8B: ADC(_R.E); break;
        case 0x8C: ADC(_R.H); break;
        case 0x8D: ADC(_R.L); break;
        case 0x8E: ADC(program[_R.HL]); break;
        case 0x8F: ADC(_R.A); break;
        case 0x90: SUB(_R.B); break;
        case 0x91: SUB(_R.C); break;
        case 0x92: SUB(_R.D); break;
        case 0x93: SUB(_R.E); break;
        case 0x94: SUB(_R.H); break;
        case 0x95: SUB(_R.L); break;
        case 0x96: SUB(program[_R.HL]); break;
        case 0x97: SUB(_R.A); break;
        case 0x98: SBB(_R.B); break;
        case 0x99: SBB(_R.C); break;
        case 0x9A: SBB(_R.D); break;
        case 0x9B: SBB(_R.E); break;
        case 0x9C: SBB(_R.H); break;
        case 0x9D: SBB(_R.L); break;
        case 0x9E: SBB(program[_R.HL]); break;
        case 0x9F: SBB(_R.A); break;
        case 0xA0: ANA(_R.B); break;
        case 0xA1: ANA(_R.C); break;
        case 0xA2: ANA(_R.D); break;
        case 0xA3: ANA(_R.E); break;
        case 0xA4: ANA(_R.H); break;
        case 0xA5: ANA(_R.L); break;
        case 0xA6: ANA(program[_R.HL]); break;
        case 0xA7: ANA(_R.A); break;
        case 0xA8: XRA(_R.B); break;
        case 0xA9: XRA(_R.C); break;
        case 0xAA: XRA(_R.D); break;
        case 0xAB: XRA(_R.E); break;
        case 0xAC: XRA(_R.H); break;
        case 0xAD: XRA(_R.L); break;
        case 0xAE: XRA(program[_R.HL]); break;
        case 0xAF: XRA(_R.A); break;
        case 0xB0: ORA(_R.B); break;
        case 0xB1: ORA(_R.C); break;
        case 0xB2: ORA(_R.D); break;
        case 0xB3: ORA(_R.E); break;
        case 0xB4: ORA(_R.H); break;
        case 0xB5: ORA(_R.L); break;
        case 0xB6: ORA(program[_R.HL]); break;
        case 0xB7: ORA(_R.A); break;
        case 0xB8: CMP(_R.B); break;
        case 0xB9: CMP(_R.C); break;
        case 0xBA: CMP(_R.D); break;
        case 0xBB: CMP(_R.E); break;
        case 0xBC: CMP(_R.H); break;
        case 0xBD: CMP(_R.L); break;
        case 0xBE: CMP(program[_R.HL]); break;
        case 0xBF: CMP(_R.A); break;

        // RNZ
        case 0xC0:
            if (!_R.Z) RET(); break;

        case 0xC1: POP(_R.B, _R.C); break;

        // JNZ
        case 0xC2:
            if (!_R.Z) JMP(); break;

        // JMP
        case 0xC3: JMP(); break;

        // CNZ
        case 0xC4:
            if (!_R.Z) CALL(); break;

        case 0xC5: PUSH(_R.B, _R.C); break;

        // ADI
        case 0xC6:
            _R.A += program[_R.PC + 1];
            _R.PC += 2;
            break;

        // RST 0
        case 0xC7:
            break;

        // RZ
        case 0xC8:
            if (_R.Z) RET(); break;

        // RET
        case 0xC9: RET(); break;

        // JZ
        case 0xCA:
            if (_R.Z) JMP(); break;

        // CZ
        case 0xCC:
            if (_R.Z) CALL(); break;

        // CALL
        case 0xCD: CALL(); break;

        // JPE
        case 0xEA:
            if (!_R.P) JMP(); break;

        default:
            ++_R.PC;
        }
    }
}
