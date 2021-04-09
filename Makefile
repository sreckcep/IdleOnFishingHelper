CC = gcc
CFLAGS = -Wall -I include
LDFLAGS = -L lib -lmingw32 -lSDL2main -lSDL2 #-mwindows

Programme : main.o
	$(CC) main.o -o FishingHelper $(LDFLAGS)

main.o : src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o main.o

# Suppression des fichiers temporaires.
clean :
	del -rf *.o

# Suppression de tous les fichiers, sauf les sources,
# en vue d’une reconstruction complète.
mrproper : clean
	del Programme