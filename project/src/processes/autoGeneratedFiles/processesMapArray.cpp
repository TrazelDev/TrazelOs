
extern void processTest2();
extern void processTest();
extern void processTest4();
extern void processTest3();


typedef struct
{
    const char *name;
    void (*func)();
} ProcessNameAndFunc;

ProcessNameAndFunc processesInSystem[] = 
{
    { "processTest2", &processTest2},
    { "processTest", &processTest},
    { "processTest4", &processTest4},
    { "processTest3", &processTest3},
};
