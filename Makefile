1.exe: 1.c iup.o
	gcc 1.c iup.o iup.dll -s -O2 -DNDEBUG -mwindows -o 1.exe

iup.o: iup.rc manifest.xml
	windres iup.rc -o iup.o
