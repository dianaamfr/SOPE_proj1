.PHONY: all begin generate run clean

all: begin generate

begin:
		@echo "## Starting Compiling ##"

generate:
		@echo "Creating ouput files..."
		gcc simpledu.c utils.c -o simpledu -Wall -lm 

run: begin generate
		@echo "## Starting running ##"
		./simpledu

clean:
		@echo "Cleaning up..."
		-rm simpledu -f
