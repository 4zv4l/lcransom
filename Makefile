CC = gcc
CFLAGS = -I lua
LIBS = -llua -lm -lcrypto # -static -lto
LUASCRIPT = ransom.lua

TARGET = ransom

.PHONY: all clean

all: linux

$(TARGET): launcher.c
	$(CC) $< -o $@ $(CFLAGS) $(LIBS)

clean:
	cd lua  && make clean && cd ..
	rm -f $(TARGET) luac.out tmp.c

linux:
	cd lua && make clean && make linux && cd ..
	lua/luac $(LUASCRIPT)
	xxd -i luac.out > tmp.c
	cat launcher.c >> tmp.c
	$(CC) tmp.c -I lua -L lua -llua -lm -lcrypto -o $(TARGET)
	rm tmp.c
	rm luac.out
