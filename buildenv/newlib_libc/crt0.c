#include <fcntl.h>

extern void exit(int code);
extern int main();

void _start() {
	_init_signal();
	int exit_code = main();
	exit(exit_code);
}
