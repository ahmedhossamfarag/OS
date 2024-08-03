#include "strlib.h"


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

int str_cmp_n(const char* str1, const char* str2, int n){
	for (int i = 0; i < n; i++)
	{
		if(*str1 != *str2){
			return *(unsigned char*)str1 - *(unsigned char*)str2;
		}
		str1++;
		str2++;
	}
	return 0;
}

const char* str_reverse(char* str) {
	int len = str_len(str);
	int j = len - 1;
	for (int i = 0; i < len / 2; i++)
	{
		char c = str[j];
		str[j] = str[i];
		str[i] = c;
		j--;
	}
	return str;
}

const char* int_to_str(unsigned long val, char* str)
{
	int i = 0;
	while (val)
	{
		str[i++] = '0' + (val % 10);
		val /= 10;
	}
	if(!i){
		str[i++] = '0';
	}
	str[i] = '\0';
	return str_reverse(str);
}

const char* int_to_hex_str(unsigned long val, char* str)
{
	int i = 0;
	while (val)
	{
		int mod = val % 16;
		if (mod < 10)
			str[i++] = mod + '0';
		else
			str[i++] = mod - 10 + 'A';
		val /= 16;
	}
	if(!i){
		str[i++] = '0';
	}
	str[i++] = 'x';
	str[i++] = '0';
	str[i] = '\0';
	
	return str_reverse(str);
}

unsigned char str_start_with(const char* str, const char* prefix){
	while(1){
		if(*prefix == '\0') return 1;
		if(*str == '\0') return 0;
		if(*str != *prefix) return 0;
		str ++;
		prefix ++;
	}
}

void str_copy(const char* from, char* to){
	do
	{
		*to = *from;
		to++;
		from++;
	} while (*from);
	*to = *from;
	
}

void str_copy_n(const char* from, char* to, unsigned int n){
	for (int i = 0; i < n-1; i++)
	{
		*to = *from;
		if(!*from) return;
		to++;
		from++;
	}
	*to = '\0';
}

void str_split(const char* str, char sep, char** splits, char* out_str){
	char* start = out_str;
	while (*str)
	{
		if(*str == sep){
			*splits = start;
			*out_str = '\0';
			start = out_str + 1;
			splits++;
		}else{
			*out_str = *str;
		}
		str++;
		out_str++;
	}
	*splits = start;
	*(splits+1) = 0;
	*out_str = '\0';
}