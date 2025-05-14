#ifndef _PUSH_H_
#define _PUSH_H_
// defines assembly code for push commands

#define PUSH_STATIC	"// push static\n"\
			"@%s.%hu\n"\
			"D=M\n"\
			"@SP\n"\
			"A=M\n"\
			"M=D\n"\
			"@SP\n"\
			"M=M+1\n"

#define PUSH_CONSTANT	"// push constant\n"\
			"@%hu\n"\
		        "D=A\n"\
                        "@%s\n"\
			"A=M\n"\
			"M=D\n"\
			"@SP\n"\
			"M=M+1\n"

#define PUSH_POINTER	"// push pointer\n"\
			"@%hu\n"\
		        "D=A\n"\
                        "@%s\n"\
			"A=M\n"\
			"A=D+A\n"\
			"D=M\n"\
			"@SP\n"\
			"A=M\n"\
			"M=D\n"\
			"@SP\n"\
			"M=M+1\n"

#define PUSH_REG	"// push register\n"\
			"@%hu\n"\
		        "D=A\n"\
                        "@%s\n"\
			"A=D+A\n"\
			"D=M\n"\
			"@SP\n"\
			"A=M\n"\
			"M=D\n"\
			"@SP\n"\
			"M=M+1\n"
#endif	
