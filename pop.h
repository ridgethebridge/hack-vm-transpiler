#ifndef _POP_H_
#define _POP_H_


#define POP_STATIC	"//  pop static\n"\
			"@%s.%hu\n"\
			 "D=A\n"\
			 "@13\n"\
			 "M=D\n"\
			 "@SP\n"\
			 "M=M-1\n"\
			 "A=M\n"\
			 "D=M\n"\
			 "@13\n"\
			 "A=M\n"\
			 "M=D\n"

#define POP_POINTER	"// pop pointer\n"\
			"@%hu\n"\
			"D=A\n"\
			"@%s\n"\
			"A=M\n"\
			"A=D+A\n"\
			 "D=A\n"\
			 "@13\n"\
			 "M=D\n"\
			 "@SP\n"\
			 "M=M-1\n"\
			 "A=M\n"\
			 "D=M\n"\
			 "@13\n"\
			 "A=M\n"\
			 "M=D\n"

#define POP_REG 	"// pop pointer\n"\
			"@%hu\n"\
			"D=A\n"\
			"@%s\n"\
			"A=D+A\n"\
			 "D=A\n"\
			 "@13\n"\
			 "M=D\n"\
			 "@SP\n"\
			 "M=M-1\n"\
			 "A=M\n"\
			 "D=M\n"\
			 "@13\n"\
			 "A=M\n"\
			 "M=D\n"
#endif

