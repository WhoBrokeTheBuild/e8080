#ifndef DISASM_H 
#define DISASM_H

#include <stdint.h>

void disasm(uint8_t * program, long len);
int disasmInst(uint8_t * program, long addr);

#endif // DISASM_H
