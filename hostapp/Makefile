EXECUTABLE=nai

all: $(EXECUTABLE)

$(EXECUTABLE): *.c
	gcc -O4 -Wall *.c -o $(EXECUTABLE) `pkg-config --cflags --libs glib-2.0` -lusb-1.0

clean:
	rm -f *.o $(EXECUTABLE)
