.PHONY: all begin generate run clean

all: begin generate

begin:
		@echo "## Starting Compiling ##"

generate:
		@echo "Creating ouput files..."
		gcc searchDir.c aux.c -o searchDir -Wall -lm 
		gcc simpledu.c aux.c -o simpledu -Wall -lm 

run: begin generate
		@echo "## Starting running ##"
		./simpledu

clean:
		@echo "Cleaning up..."
		-rm simpledu -f
