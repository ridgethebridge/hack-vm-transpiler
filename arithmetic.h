#ifndef _ARITHMETIC_H_
#define _ARITHMETIC_H_

// defines assembly output for all arithmetic commands
#define EQ_OP	"// equals\n"\
		"@SP\n"\
		"M=M-1\n"\
                "A=M\n"\
                "D=M\n"\
                "A=A-1\n"\
		"D=M-D\n" \
		"@cond.%lu\n"\
		"D;JEQ\n"\
		"@SP\n"\
		"A=M-1\n"\
		"M=0\n"\
		"@3\n"\
		"D=A\n"\
		"@cond.%lu\n"\
		"A=D+A\n"\
		"0;JMP\n"\
		"(cond.%lu)\n"\
		"@SP\n"\
		"A=M-1\n"\
		"M=-1\n"

#define LT_OP	"// less than\n"\
		"@SP\n"\
		"M=M-1\n"\
                "A=M\n"\
                "D=M\n"\
                "A=A-1\n"\
		"D=M-D\n" \
		"@cond.%lu\n"\
		"D;JLT\n"\
		"@SP\n"\
		"A=M-1\n"\
		"M=0\n"\
		"@3\n"\
		"D=A\n"\
		"@cond.%lu\n"\
		"A=D+A\n"\
		"0;JMP\n"\
		"(cond.%lu)\n"\
		"@SP\n"\
		"A=M-1\n"\
		"M=-1\n"

#define GT_OP	"// greater than\n"\
		"@SP\n"\
		"M=M-1\n"\
                "A=M\n"\
                "D=M\n"\
                "A=A-1\n"\
		"D=M-D\n" \
		"@cond.%lu\n"\
		"D;JGT\n"\
		"@SP\n"\
		"A=M-1\n"\
		"M=0\n"\
		"@3\n"\
		"D=A\n"\
		"@cond.%lu\n"\
		"A=D+A\n"\
		"0;JMP\n"\
		"(cond.%lu)\n"\
		"@SP\n"\
		"A=M-1\n"\
		"M=-1\n"

#define ADD_OP	"// add\n"\
		"@SP\n"\
		"M=M-1\n"\
                "A=M\n"\
                "D=M\n"\
                "A=A-1\n"\
		"M=D+M\n"

#define SUB_OP	"// sub\n"\
		"@SP\n"\
		"M=M-1\n"\
                "A=M\n"\
                "D=M\n"\
                "A=A-1\n"\
		"M=M-D\n"

#define AND_OP	"// and\n"\
		"@SP\n"\
		"M=M-1\n"\
                "A=M\n"\
                "D=M\n"\
                "A=A-1\n"\
		"M=D&M\n"

#define OR_OP	"// or\n"\
		"@SP\n"\
		"M=M-1\n"\
                "A=M\n"\
                "D=M\n"\
                "A=A-1\n"\
		"M=D|M\n"

#define NOT_OP	"// not\n"\
		"@SP\n"\
		"A=M-1\n"\
		"M=!M\n"

#define NEG_OP	"// neg\n"\
		"@SP\n"\
		"A=M-1\n"\
		"M=-M\n"
#endif
