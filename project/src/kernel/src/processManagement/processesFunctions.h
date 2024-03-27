#pragma once
typedef struct
{
    const char *name;
    void (*func)();
} ProcessNameAndFunc;
extern ProcessNameAndFunc processesInSystem[];