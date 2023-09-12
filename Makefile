CC = gcc
CFLAGS = -I lua
LIBS = -llua -lm -lcrypto # -static -lto

TARGET = ransom

.PHONY: all clean

all: linux

$(TARGET): launcher.c
	$(CC) $< -o $@ $(CFLAGS) $(LIBS)

clean:
	cd lua  && make clean && cd ..
	rm -f $(TARGET) $(TARGET).exe

linux:
	cd lua && make clean && make linux && cd ..
	$(CC) launcher.c -I lua -L lua -llua -lm -lcrypto -o $(TARGET)
