
CC= gcc
RM= rm -f

CFLAGS= -W -Wall -std=c99 -pedantic -g
LIBS= 

SOURCES= $(wildcard *.c)
HEADERS= $(wildcard *.h)
BASENAMES= $(basename $(SOURCES)) 
OBJECTS= $(addsuffix .o , $(BASENAMES))
EXEC= main

all:: $(EXEC)

.c.o :
	@echo "=== COMPILING $*.c INTO $@ ==="
	$(CC) $(CFLAGS)  -c  $*.c 

$(EXEC): $(OBJECTS)
	@echo "=== LINKING OBJECTS INTO $(EXEC) ==="
	$(CC) $^   -o $@    $(LIBS)

clean::
	@echo "=== CLEANING OBJECTS ==="
	$(RM) $(OBJECTS)

display::
	@echo "SOURCES:" $(SOURCES)
	@echo "HEADERS:" $(HEADERS)
	@echo "BASENAMES:" $(BASENAMES)
	@echo "OBJECTS:" $(OBJECTS)
	@echo "EXEC:" $(EXEC)

depend: $(SOURCES) $(HEADERS)
	@echo "=== UPDATING DEPENDENCIES ==="
	gcc -MM $(SOURCES) | tee depend

-include depend