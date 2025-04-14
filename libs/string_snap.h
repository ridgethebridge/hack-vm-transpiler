
#ifndef _STRING_SNAP_H_
#define _STRING_SNAP_H_
#include<string.h>
#include<stdbool.h>

#define SS(str_lit) \
	(String_Snap) {\
	.data = str_lit,\
	.length = sizeof(str_lit)-1\
	}

#define ss_isspace(c) (c == ' ' || c == '\t' || c == '\n')

#define ss_isdigit(c) (c >= '0' && c <='9')

typedef unsigned long long uint64;
typedef struct String_Snap {
	char *data;
	uint64 length;
} String_Snap;

typedef struct Snap_Scanner {
	String_Snap snap;
} Snap_Scanner;


 String_Snap ss_next_word(Snap_Scanner *scan);
 String_Snap ss_trim_left(String_Snap str);
 String_Snap ss_trim_right(String_Snap str);
 String_Snap ss_from_cstr(char *cstr);
 String_Snap ss_trim(String_Snap str);
 String_Snap ss_delim_cstr(char *str, char delim);
 String_Snap ss_delim(String_Snap str, char delim);
 Snap_Scanner ss_create_scanner(String_Snap str);
 bool ss_has_next(Snap_Scanner scan);
 bool ss_are_equal(String_Snap s1, String_Snap s2);
String_Snap ss_strdelim(String_Snap str, String_Snap delim);
String_Snap ss_strdelim_cstr(char *str, char *delim);
 

#endif

