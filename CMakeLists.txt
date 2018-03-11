CMAKE_MINIMUM_REQUIRED(VERSION 3.2)

PROJECT(E8080 C)

ADD_EXECUTABLE(e8080
    src/main.c
    src/run.h
    src/run.c
    src/disasm.h
    src/disasm.c
    src/register.h
    src/register.c
)
