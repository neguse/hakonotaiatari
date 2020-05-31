
CC = g++
CFLAGS = # -O2
CFLAGS_WINDOWS = $(CFLAGS) $(CFLAGS_DEBUG) -mwindows
CFLAGS_DEBUG = -D_DEBUG -g # -pg
LDFLAGS = 
INCLUDES = -I/c/work/lib/glm/ -I../cdbpp-1.1/include
LIBS = -lwinmm -lgdi32 -static-libgcc -static-libstdc++
TARGET = a.exe
PYTHON = /c/Python32/python

$(TARGET): *.cc *.h data.obj precompile.h.gch 
	g++ -o main.o -c main.cc $(INCLUDES) $(CFLAGS_WINDOWS) $(LDFLAGS) $(LIBS)
	g++ -o $@ main.o data.obj $(INCLUDES) $(CFLAGS_WINDOWS) $(LDFLAGS) $(LIBS)
	
packer.exe: packer.cc
	g++ -o $@ packer.cc $(INCLUDES) $(CFLAGS) $(LDFLAGS) $(LIBS)

precompile.h.gch: precompile.h
	g++ precompile.h $(INCLUDES)

data.obj:packer.exe
	packer.exe
	# rm -fr data/*
	objcopy -I binary -O pe-i386 -B i386 data.dat data.obj

.PHONY: clean profile font
clean: 
	rm $(TARGET) data.dat packer.exe *~ *.obj *.dat *.o *.gch *.out

font:
	$(PYTHON) csf1.py KST32B.TXT

prof: $(TARGET)
	$(TARGET)
	gprof $(TARGET) gmon.out > prof.txt

run: $(TARGET)
	$(TARGET) &

# cl main.cc -MTd -W3 -nologo -Od -I/c/work/lib/glm/ -EHsc -Fe$@ -DWIN32 -D_DEBUG -D_WINDOWS -D_MBCS -Gm -RTC1 -GS -fp:precise -Gd -ZI -analyze winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib
