EXECUTABLE=nai

all: $(EXECUTABLE)

$(EXECUTABLE): *.c
	gcc -O4 -Wall *.c -o $(EXECUTABLE) -lusb-1.0

clean:
	rm -f *.o $(EXECUTABLE)
