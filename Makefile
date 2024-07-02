# Define the compiler
CC = gcc

# Define the source files
SRC1 = teste15.c
SRC2 = teste30.c
SRC3 = main.c

# Define the output executables
OUT1 = teste15
OUT2 = teste30
OUT3 = main

# # Define the compilation flags (if any)
# CFLAGS = -Wall

all: $(OUT1) $(OUT2) $(OUT3)

$(OUT1): $(SRC1)
	$(CC) $(CFLAGS) -o $(OUT1) $(SRC1)

$(OUT2): $(SRC2)
	$(CC) $(CFLAGS) -o $(OUT2) $(SRC2)

$(OUT3): $(SRC3)
	$(CC) $(CFLAGS) -o $(OUT3) $(SRC3)

# Clean up the build
clean:
	rm -f $(OUT1) $(OUT2) $(OUT3)
