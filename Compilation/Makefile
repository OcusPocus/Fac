CC = gcc

LIBS = -lm 
CCFLAGS = -Wall -ggdb -std=c99

OBJ = analyseur_lexical.o syntabs.o util.o dico.o

all: analyseur_syntaxique

analyseur_syntaxique: analyseur_syntaxique.c $(OBJ)
	$(CC) $(CCFLAGS) -o analyseur_syntaxique analyseur_syntaxique.c $(OBJ)

analyseur_lexical.o: analyseur_lexical.c
	$(CC) $(CCFLAGS) -c $^

syntabs.o: syntabs.c
	$(CC) $(CCFLAGS) -c $^

dico.o: dico.c
	$(CC) $(CCFLAGS) -c $^

.PHONY : clean

clean:
	- rm -f $(OBJ)
	- rm -f analyseur_syntaxique
