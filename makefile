TARGET = analyse

CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm 

all: $(TARGET)

main.o: donnee.h matrice.h main.c 
	$(CC) -c $(CFLAGS) main.c

donnee.o: donnee.h donnee.c
	$(CC) -c $(CFLAGS) donnee.c

matrice.o: matrice.h matrice.c
	$(CC) -c $(CFLAGS) matrice.c

$(TARGET): main.o donnee.o matrice.o
		@echo "Building $(TARGET)"
		$(CC) -o $(TARGET) main.o donnee.o matrice.o $(CFLAGS) $(LDFLAGS)

clean: 
	-rm *.o -f analyse