#include <strings.h>

uint64_t strlen(char* str)
{
	int count = 0;
	while (*str) {
		count++;
		str++;
	}

	return count;
}

void string_reverse(char* str) {
	int str_len = strlen(str);
	char temp = 0;

	for (int i = 0; i < str_len / 2; i++) {
		temp = str[i];
		str[i] = str[str_len - i - 1];
		str[str_len - i - 1] = temp;
	}
}
