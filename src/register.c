#include <stdio.h>
#include "register.h"

struct reg_t _R;

void printRegisters()
{
    printf("[ A=%04X F=%04X ][ B=%04X C=%04X ]\n", _R.A, _R.FLAGS, _R.B, _R.C);
    printf("[ D=%04X E=%04X ][ H=%04X L=%04X ]\n", _R.D, _R.E, _R.H, _R.L);
    printf("[ SP=%08X ][ PC=%08X ]\n", _R.SP, _R.PC);
}
