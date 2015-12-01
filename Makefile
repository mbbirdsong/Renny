#  List of all programs you want to build
EX=final_project_mbb

#List of other source files
ProjectFiles =  cyl_shapes.c basic_shapes.c sphr_shapes.c cmpst_shapes.c floor.c renny.c terrain.c game_area.c

#  Libraries - Ubuntu Linux (32 and 64)
LIBS=-lglut -lGLU
#  Libraries - Fedora 32-bit Linux
#LIBS=-lglut -lGLU /lib/libm.so.6 /usr/lib/libGL.so.1
#  Libraries - Fedora 64-bit Linux
#LIBS=-lglut -lGLU /lib64/libm.so.6 /usr/lib64/libGL.so.1
#  Libraries - OSX
#LIBS=-framework GLUT -framework OpenGL
#  Libraries - MinGW
#LIBS=-lglut32cu -lglu32 -lopengl32 -Wl,--subsystem,windows

#  Main target
all: $(EX)

#  Generic compile rules
.c.o:
	gcc -c -O $<
#  MinGW version
#	gcc -c -O $< -I"C:\Program Files\Common Files\MinGW\glutcu\include"


#  Generic compile and link
%: %.c CSCIx229.a $(ProjectFiles)
	gcc -O3 -o $@ $^ $(LIBS)
#  MinGW version
#	gcc -O3 -o $@ $^  -L"C:\Program Files\Common Files\MinGW\glutcu\lib" $(LIBS)  -I"C:\Program Files\Common Files\MinGW\glutcu\include"

#  Delete unwanted files - Linux
clean:;rm -f $(EX) *.o *.a
#  Delete unwanted files - MinGW
#clean:;del *.exe *.o *.a

#  Create archive
CSCIx229.a:fatal.o loadtexbmp.o print.o project.o errcheck.o object.o
	ar -rcs CSCIx229.a $^

#  Obligatory UNIX inside jokes
love:
	@echo "not war?"
war:
	@echo "what is it good for?"
