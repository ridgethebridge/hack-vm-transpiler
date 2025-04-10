
CC=gcc
FLAGS=-Wall -o VMtranslator
C_FLAGS =-Wall -c
OBJECTS= vm_translator.o vm_parser.o vm_writer.o

VMtranslator.exe: $(OBJECTS)
	$(CC) $(OBJECTS) $(FLAGS) 

%.o: %.c
	$(CC) $(C_FLAGS) $<

clean:
	del *.o
	
