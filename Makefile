#//////////////////////////////////////////
# File: Make file for UPOS Provider
# Author: Fernando Quintero
#
# Modified By:
#
#//////////////////////////////////////////

CXX=g++
OBJECTS =  ucurses.o
MODOBJECTS = ucurses.o ucurses_wrap.o
EXECUTABLE = libucurses.so.1.0
EXTRALIBS = -lncurses -lform -lmenu -lpanel -ltinfo
PERLLDFLAGS=$(shell perl -MConfig -e 'print $$Config{lddlflags}')
PERLCFLAGS=$(shell perl -MConfig -e 'print join(" ", @Config{qw(ccflags optimize cccdlflags)}, "-I$$Config{archlib}/CORE")') 
PERLMODINSTALL=$(shell perl -MConfig -e 'print $$Config{installsitelib}')
PERLLIBINSTALL=$(shell perl -MConfig -e 'print $$Config{archlib}')



all: clean $(OBJECTS) $(EXECUTABLE) module

$(OBJECTS): %.o: %.cpp %.h
	$(CXX) $(PERLCFLAGS) -c ucurses.cpp
	ar -cvq libucurses.a ucurses.o
	$(CXX) -shared $(EXTRALIBS) -o libucurses.so.1.0 ucurses.o

#$(EXECUTABLE): $(OBJECTS)
#	$(CXX) $(LDFLAGS)  $(OBJECTS) $(EXTRALIBS) -o $@


module:clean
	swig  -c++ -perl5 ucurses.i 
	$(CXX) -c $(PERLCFLAGS) ucurses.cpp  ucurses_wrap.cxx
	$(CXX) $(PERLLDFLAGS) $(STGPATH)  $(EXTRALIBS) $(MODOBJECTS) -o ucurses.so
install_module:
	cp ucurses.pm $(PERLMODINSTALL)
	cp ucurses.so $(PERLLIBINSTALL)


install: all install_module
	cp libucurses.so.1.0 /usr/lib
	cp libucurses.a /usr/lib        
	ln -sf /usr/lib/libucurses.so.1.0 /usr/lib/libucurses.so.1
	ln -sf /usr/lib/libucurses.so.1.0 /usr/lib/libucurses.so
	cp ucurses.h /usr/include

clean:
	$(RM) *.o


