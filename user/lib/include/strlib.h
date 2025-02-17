
int str_len(const char* str);

int str_cmp(const char* str1, const char* str2);

int str_cmp_n(const char* str1, const char* str2, int n);

const char* str_reverse(char* str);

const char* int_to_str(unsigned long val, char* out_str);

const char* int_to_hex_str(unsigned long val, char* out_str);

unsigned char str_start_with(const char* str, const char* prefix);

void str_copy(const char* from, char* to);

void str_copy_n(const char* from, char* to, unsigned int n);

void str_split(const char* str, char sep, char** splits, char* out_str);

#define sint(x) int_to_str(x, 0)
#define sxint(x) int_to_hex_str(x, 0)