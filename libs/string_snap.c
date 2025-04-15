
#include"string_snap.h"

// fix trimming

 bool ss_are_equal(String_Snap s1, String_Snap s2)
{
	if(s1.length != s2.length)
		return false;
	uint64 index = 0;
	while(index < s1.length)
	{
		if(s1.data[index] != s2.data[index])
			return false;
		++index;
	}
	return true;
}

 bool ss_has_next(Snap_Scanner scan)
{
	return scan.snap.length > 0;
}

 Snap_Scanner ss_create_scanner(String_Snap str)
{
	return (Snap_Scanner) {
				.snap = {
					.data = str.data,
					.length = str.length
				}
				};
}
	
String_Snap ss_strdelim_cstr(char *str, char *delim)
{
	uint64 delim_length = strlen(delim);
	String_Snap delim_snap = ss_from_cstr(delim);
	String_Snap result  = ss_from_cstr(str);
	uint64 index = 0;
	while(result.length - index >=delim_length)
	{
		String_Snap temp = {.data = result.data+index, .length = delim_length};
		if(ss_are_equal(temp,delim_snap))
		{
			result.length=index;
			break;
		}
		++index;
	}
	return result;
}
	
String_Snap ss_strdelim(String_Snap str, String_Snap delim)
{
//five//
	uint64 index = 0;
	String_Snap result = str;
	while(result.length - index >=delim.length)
	{
		String_Snap temp = {.data = result.data+index, .length = delim.length};
		if(ss_are_equal(temp,delim))
		{
			result.length = index;
			break;
		}
		++index;
	}
	return result;
}
	

 String_Snap ss_delim_cstr(char *str, char delim) 
{
	uint64 start = 0;
	while(str[start] && str[start] == delim)
		++start;
	uint64 end = start;
	while(str[end] && str[end] != delim)
		++end;
	return (String_Snap) {
				.data = str+start,
				.length =  end-start
			};
}

 String_Snap ss_delim(String_Snap str, char delim)
{
	uint64 start = 0;
	while(start < str.length && str.data[start] == delim)
		++start;
	uint64 end = start;
	while(end < str.length && str.data[end] != delim)
		++end;
	return (String_Snap) {
				.data =	str.data+start,
				.length = end - start
			};


}

 String_Snap ss_next_word(Snap_Scanner *scan)
{
	scan->snap = ss_trim(scan->snap);
	uint64 index = 0;
	while(index < scan->snap.length && !(ss_isspace(scan->snap.data[index])))
		++index;
	String_Snap result = {
				.data = scan->snap.data,
				.length = index
				};
	scan->snap.data +=index+1;
	scan->snap.length -=index;
return result;
}

 String_Snap ss_trim_left(String_Snap str)
{
	uint64 index = 0;
	while(index < str.length && ss_isspace(str.data[index]))
		++index;
	return (String_Snap) {
		.data = str.data +index,
		.length = str.length-index
	};
}

 String_Snap ss_trim_right(String_Snap str)
{
	uint64 index = str.length;
	while(index > 0 && str.data[index-1] && ss_isspace(str.data[index-1]))
		--index;

	return (String_Snap) {
		.data = str.data,
		.length = index
	};
}

 String_Snap ss_from_cstr(char *cstr)
{
	return (String_Snap) {
		.data = cstr,
		.length = strlen(cstr)
		};
}

 String_Snap ss_trim(String_Snap str)
{
	return ss_trim_right(ss_trim_left(str));
}
