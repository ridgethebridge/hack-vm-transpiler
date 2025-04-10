
#ifndef _STR_TOOLZ_H_
#define  _STR_TOOLZ_H_
#include<stdlib.h>
#include<string.h>
#define str_toolz_equal(s1,s2) (strcmp(s1,s2) == 0)
#define str_toolz_isspace(c1) (c1 == ' ' || c1 == '\t' || c1 == '\n')
char *str_toolz_first_char(char *str);
char *str_toolz_rightmost_blanks(char *str);
char* str_toolz_trim(char *str);
char *str_toolz_first_comment(char *str);
char *str_toolz_trim_comment(char *str);
char *file_wo_path(char *file);
char *str_toolz_eol(char *str);
char *str_toolz_substring(char *start, char *end);

// index of newline or null if no newline
char *str_toolz_eol(char *str)
{
	int index = 0;
	while(str[index] != '\0' && str[index] != '\n')
		++index;
	return str+index;
}

// returns a new string composed of chars from start to end
char * str_toolz_substring(char *start, char *end)
{
	char *sub = malloc(end-start+1);
	memcpy(sub,start,end-start);
	sub[end-start] = '\0';
	return sub;
}
	

// makes new string with local name
char *str_toolz_file_wo_path(char* file)
{
	char *name = file;
	int i = -1;
	int slash_index = 0;
	while(name[i] != '\0')
	{
		if(name[i] == '/' || name[i] == '\\')
			slash_index = i;
		++i;
	}
	char *result = malloc(strlen((name+slash_index+1))+1);
	strcpy(result,name+slash_index+1);
	return result;
}
			

// returns pointer to first non whitespace char
char *str_toolz_first_char(char *str) {
	char *first_char = str;
	while(*first_char == ' ' || *first_char =='\t')
		++first_char;
	return first_char;
}

// returns pointer to first consecutive blanks
char* str_toolz_rightmost_blanks(char *str) {
	char *last_char = str;
	int index = 0;
	int space = -1; // space was last thing found

	while(last_char[index] != '\0')
	{
		if(str_toolz_isspace(last_char[index]) && space == -1)
			space = index;
		else if(!str_toolz_isspace(last_char[index]))
		{
			space = -1;
		}
		++index;
	}
	if(space == -1)
		space = index;
	return last_char + space;
}


// returns first comment, or first string of consecutive blanks
char *str_toolz_first_comment(char *str)
{
	char *result = str;
	int index = 0;
	while(result[index] != '\0')
	{
		if(result[index] =='/')
			if(result[index+1] == '/')
			{
				return result+index;
			}
		++index;
	}
	return str_toolz_rightmost_blanks(result);
}

// returns a new string with leading and trailing blanks removed
char* str_toolz_trim(char *str) {
	char *left = str_toolz_first_char(str);
	char *right = str_toolz_rightmost_blanks(str);
	char *result = malloc(right-left +1);
	memcpy(result,left,right-left);
	result[right-left] = '\0';
	return result;
}

// removes leading and trialing spaces, aswell as comments
char * str_toolz_trim_comment(char *str) {
	char *left = str_toolz_first_char(str);
	char *right = str_toolz_first_comment(str);
	char *result = malloc(right-left +1);
	memcpy(result,left,right-left);
	result[right-left] = '\0';
	return result;
}
#endif	
