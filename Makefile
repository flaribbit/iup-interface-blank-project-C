STATIC_LIBS=-liup -lgdi32 -lcomdlg32 -lcomctl32 -luuid -loleaut32 -lole32

a.exe: a.cpp resource.o
	g++ a.cpp -I include resource.o iup.dll -s -O2 -DNDEBUG #-mwindows

as.exe: a.cpp resource.o
	g++ a.cpp -I include -L iup resource.o $(STATIC_LIBS) -s -O2 -DNDEBUG -mwindows -o as.exe

resource.o: resource.rc manifest.xml
	windres resource.rc -o resource.o
