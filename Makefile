all:
	mkdir -p bin
	gcc -Wall -pedantic src/main.c src/Nuvoton8051.c src/Nuvoton8051PlatformSpecific.c -o bin/nu8051

install: all
	cp bin/nu8051 /usr/bin

uninstall:
	rm -f /usr/bin/nu8051

clean:
	rm -f bin/nu8051