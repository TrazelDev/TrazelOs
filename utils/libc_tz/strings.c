#include <strings.h>

uint64_t strlen(char* str) {
	int count = 0;
	while (*str) {
		count++;
		str++;
	}

	return count;
}

int64_t strcmp(const char* str1, const char* str2) {
	while (*str1 && (*str1 == *str2)) {
		str1++;
		str2++;
	}

	return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

int64_t strncmp(const char* str1, const char* str2, size_t n) {
	for (size_t i = 0; i < n; i++) {
		if (str1[i] != str2[i]) {
			return (unsigned char)str1[i] - (unsigned char)str2[i];
		}

		if (str1[i] == '\0') {
			return 0;
		}
	}

	return 0;
}

void string_reverse(char* str) {
	int str_len = (int)strlen(str);
	char temp = 0;

	for (int i = 0; i < str_len / 2; i++) {
		temp = str[i];
		str[i] = str[str_len - i - 1];
		str[str_len - i - 1] = temp;
	}
}
