
OS=$(shell uname -s)

CFLAGS += -Wall -Wextra -std=c18 -pedantic
CXXFLAGS += -Wall -Wextra -std=c++20 -pedantic
#LDFLAGS += -static-libgcc -static-libstdc++
#LDFLAGS  += -static
LDFLAGS  += -g -Os

PFX0=wintail
PFX0_SRCS=${PFX0}.cpp reghandle.cpp util.cpp

# If not linux then assume that it is windows
ifneq (${OS},Linux)
#MSYSTEM=UCRT64
#MSYSTEM=MINGW64

#MSYSTEM=CLANG64
MSYSTEM=MSBUILD
MAGICK=/mingw64/bin/magick
RC=windres

ifeq (${MSYSTEM},MINGW64)
PATH:=/mingw64/bin:${PATH}
LDFLAGS  += -static
endif

ifeq (${MSYSTEM},UCRT64)
PATH:=/ucrt64/bin:${PATH}
#LDFLAGS += -static
endif

ifeq (${MSYSTEM},MSBUILD)
MSBUILD='C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe'
endif

SRCS=$(wildcard *.cpp)
ifeq ($(MSBUILD),)
OBJS=$(SRCS:.cpp=.o)
endif


# CLANG n'accepte pas les fichiers en ISO-8859
# Pour utiliser clang il faut passer par le shell clang64.exe de msys2
ifeq (${MSYSTEM},CLANG64)
PATH:=/clang64/bin:${PATH}
CC=clang++
CXX=clang++
CPPFLAGS += -D_UNICODE -DUNICODE
LDFLAGS += -pthread -static
#else
#CC=gcc
#CXX=g++
#LDFLAGS += -L /usr/local/lib64
endif

EXEXT=.exe
LDFLAGS += -mwindows
LDLIBS  +=  -lwsock32 -lws2_32

PGF=$(subst \,/,$(subst C:\,/c/,$(PROGRAMFILES)))
PGF86=${PGF} (x86)
PATH:=${PATH}:${PGF86}/Inno Setup 6
PATH:=${PATH}:${PATH}:${PGF}/ImageMagick-7.1.0-Q16
PATH:=${PATH}:${PGF}/Inkscape/bin
PATH:=${PATH}:${HOME}/Progs/Inno Setup 6
PATH:=${PATH}:${HOME}/Progs/ImageMagick-7.1.0-portable-Q16-x64
PATH:=${PATH}:/c/UnixTools/bin

LDLIBS   += -lurlmon
LDLIBS   += -lwsock32 -lole32 -luuid -lcomctl32 -loleaut32 -lgdi32
UPX=upx
OBJS += ${PFX0}_res.o

ECHO=echo -e
TARGETS+=${PFX0}${EXEXT}
else
MAGICK=
UPX=upx
RC=
CC=g++
LDFLAGS += -pthread -lrt
ECHO=echo
endif

STRIP=strip
GDB=gdb
LD=g++
PFX1=line_count
PFX2=tailf
PFX3=randcsv
PFX4=emojis
TARGETS += ${PFX0}${EXEXT} ${PFX1}${EXEXT} ${PFX2}${EXEXT} ${PFX3}${EXEXT} ${PFX4}${EXEXT}


all : ${TARGETS}

ifeq ($(MSBUILD),)
${PFX0}${EXEXT} : ${OBJS}
else
${PFX0}${EXEXT} : ${PFX0_SRCS}
	${MSBUILD} wintail.sln -p:Configuration=Release
	cp x64/Release/${PFX0}${EXEXT} .	
endif


${PFX1}${EXEXT} : ${PFX1}.o
	${CXX} ${CXXFLAGS} $^  -o $@

${PFX2}${EXEXT} : ${PFX2}.o
	${CXX} ${CXXFLAGS} $^  -o $@

${PFX3}${EXEXT} : ${PFX3}.o
	${CXX} ${CXXFLAGS} $^  -o $@

${PFX4}${EXEXT} : ${PFX4}.o
	${CXX} ${CXXFLAGS} $^  -o $@

${PFX0}_res.o : ${PFX0}.ico

strip : $(TARGETS)
	@file ${TARGETS} | grep stripped >/dev/null || ( $(STRIP) $(TARGETS) && echo "Strip OK" )

upx : $(TARGETS)
	$(STRIP) $(TARGETS) 2>/dev/null || true
	$(UPX) -q $(TARGETS) 2>/dev/null || true

cfg :
	@echo "MSYSTEM ${MSYSTEM}"
	@echo "${PATH}"
	@type strip upx convert inkscape iscc
ifneq ($(MSYSTEM),MSBUILD)
	@type cc c++ gcc g++ ld windres gdb convert inkscape iscc
	@${ECHO} "CPPFLAGS=${CPPFLAGS}\nCXXFLAGS=${CXXFLAGS}\nLDFLAGS=${LDFLAGS}\nLDLIBS=${LDLIBS}"
endif
	@${ECHO} "SRCS=${SRCS}\nOBJS=${OBJS}\nTARGETS=${TARGETS}"

clean :
	rm -f *~ *.o $(OBJS) ${PFX0}.ico

rclean :
	rm -f *~ *.d *.o $(OBJS) $(TARGETS) ${PFX0}.ico *.exe
	rm -rf x64


# Ces régles implicites ne sont pas utiles quand on fait 'make rclean' (voir même make clean ...)
ifneq ($(MAKECMDGOALS),rclean)
%.ico : %.png
	${MAGICK} convert -background none $< $@

%.ico : %.svg
	${MAGICK} convert -background none $< $@

ifeq ($(MSBUILD),)
%.exe: %.o
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.exe: %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.exe: %.cpp
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

# Régles pour construire les fichier objet d'après les .rc
%.o : %.rc
	$(RC) $(CPPFLAGS) $< --include-dir . $(OUTPUT_OPTION)
endif

%.d: %.c
	@echo Checking header dependencies from $<
	@$(COMPILE.c) -isystem /usr/include -MM $< >> $@

#	@echo "Building "$@" from "$<
%.d: %.cpp
	@echo Checking header dependencies from $<
	@$(COMPILE.cpp) -isystem /usr/include -MM $< >> $@

# Inclusion des fichiers de dépendance .d
ifdef SRCS
-include $(SRCS:.cpp=.d)
endif
endif

