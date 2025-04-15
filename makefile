
CC=gcc
BINARY=VMtranslator.exe
CODEDIRS=. ./libs
INCDIRS= . ./libs
DEPFLAGS= -MP -MD
CFILES=$(foreach D,$(CODEDIRS),$(wildcard $(D)/*.c))
OBJECTS= $(patsubst %.c, %.o, $(CFILES))
DEPFILES=$(patsubst %.c, %.d,$(CFILES))
CFLAGS =-g $(foreach D, $(INCDIRS),-I$(D)) $(DEPFLAGS)

all:	$(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) -o $@ $^ -g

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm $(OBJECTS) $(BINARY) $(DEPFILES)
	
