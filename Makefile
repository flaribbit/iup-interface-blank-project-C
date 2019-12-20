STATIC_LIBS=-liup -lgdi32 -lcomdlg32 -lcomctl32 -luuid -loleaut32 -lole32

a.exe: a.cpp resource.o
	g++ a.cpp -I include resource.o iup.dll -s -O2 -DNDEBUG #-mwindows

resource.o: resource.rc manifest.xml
	windres resource.rc -o resource.o
