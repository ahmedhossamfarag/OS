#include "libc.h"


int str_len(const char* str) {
	int len = 0;
	while (str[len] != '\0') {
		len++;
	}
	return len;
}

int str_cmp(const char* str1, const char* str2) {
	while (*str1 && (*str1 == *str2)) {
		str1++;
		str2++;
	}
	return *(unsigned char*)str1 - *(unsigned char*)str2;
}

static char rev_str[10];

const char* str_reverse(const char* str) {
	for (int i = 0; i < 10; i++) rev_str[i] = '\0';
	int len = str_len(str);
	int j = len - 1;
	for (int i = 0; i < len; i++)
	{
		rev_str[j] = str[i];
		j--;
	}
	for (int i = len; i < 10; i++)
	{
		rev_str[i] = '\0';
	}
	return rev_str;
}

static char str[10];

const char* int_to_str(int val)
{
	for (int i = 0; i < 10; i++) str[i] = '\0';
	for (int i = 0; i < 10; i++)
	{
		str[i] = (val % 10) + '0';
		val /= 10;
		if (val == 0) {
			break;
		}
	}
	return str_reverse(str);
}

const char* int_to_hex_str(int val)
{
	for (int i = 0; i < 10; i++) str[i] = '\0';
	for (int i = 0; i < 10; i++)
	{
		int mod = val % 16;
		if (mod < 10)
			str[i] = mod + '0';
		else
			str[i] = mod - 10 + 'A';
		val /= 16;
		if (val == 0) {
			break;
		}
	}
	return str_reverse(str);
}
