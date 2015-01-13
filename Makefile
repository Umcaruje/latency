
cc = gcc
w32cc = i686-w64-mingw32-gcc
w64cc = x86_64-w64-mingw32-gcc

flags = -std=c99 -Wall -Werror -shared
sources = latency.c

linux32bin = linux32/latency.so
linux64bin = linux64/latency.so
win32bin = win32/latency.dll
win64bin = win64/latency.dll

all: linux32 linux64 win32 win64

linux32:
	mkdir -p linux32
	$(cc) $(flags) $(sources) -m32 -fPIC -DPIC -o $(linux32bin)

linux64:
	mkdir -p linux64
	$(cc) $(flags) $(sources) -m64 -fPIC -DPIC -o $(linux64bin)

win32:
	mkdir -p win32
	$(w32cc) $(flags) $(sources) -o $(win32bin)

win64:
	mkdir -p win64
	$(w64cc) $(flags) $(sources) -o $(win64bin)

clean:
	rm -rf linux32
	rm -rf linux64
	rm -rf win32
	rm -rf win64
