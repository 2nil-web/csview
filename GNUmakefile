
OS=$(shell uname -s)

CFLAGS += -Wall -Wextra -std=c18 -pedantic
CXXFLAGS += -Wall -Wextra -std=c++20 -pedantic
#LDFLAGS += -static-libgcc -static-libstdc++
#LDFLAGS  += -static
LDFLAGS  += -g -Os

WINTAIL_PREFIX=wintail
WINTAIL_PREFIX_SRCS=${WINTAIL_PREFIX}.cpp reghandle.cpp util.cpp
WINTAIL_PREFIX_OBJS=$(WINTAIL_PREFIX_SRCS:.cpp=.o)

# If not linux then assume that it is windows
ifneq (${OS},Linux)
#BUILD_SYSTEM=UCRT64
BUILD_SYSTEM=MINGW64
#BUILD_SYSTEM=CLANG64
#BUILD_SYSTEM=MSBUILD

MAGICK=/mingw64/bin/magick
RC=windres

ifeq (${MSYSTEM},MINGW64)
PATH:=/mingw64/bin:${PATH}
CPPFLAGS += -D_UNICODE -DUNICODE
LDFLAGS  += -static
endif

ifeq (${MSYSTEM},UCRT64)
PATH:=/ucrt64/bin:${PATH}
#LDFLAGS += -static
endif

ifeq (${MSYSTEM},MSBUILD)
MSBUILD='C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe'
PATH:=/mingw64/bin:${PATH}
endif

EXEXT=.exe
SRCS=$(wildcard *.cpp)
SINGLE_SRCS=$(filter-out ${WINTAIL_PREFIX_SRCS},${SRCS})
SINGLE_EXES=$(SINGLE_SRCS:.cpp=${EXEXT})

ifeq ($(MSBUILD),)
OBJS=$(SRCS:.cpp=.o)
OBJS += ${WINTAIL_PREFIX}_res.o
endif


# CLANG n'accepte pas les fichiers en ISO-8859
# Pour utiliser clang il faut passer par le shell clang64.exe de msys2
ifeq (${BUILD_SYSTEM},CLANG64)
PATH:=/clang64/bin:${PATH}
CC=clang++
CXX=clang++
GXX=clang++
#CPPFLAGS += -D_UNICODE -DUNICODE
LDFLAGS += -pthread -static
endif

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

ECHO=echo -e
TARGETS+=${WINTAIL_PREFIX}${EXEXT} ${SINGLE_EXES}
else
MAGICK=
UPX=upx
RC=
CC=$(CXX)
LDFLAGS += -pthread -lrt
ECHO=echo
endif

STRIP=strip
GDB=gdb
LD=$(CXX)
LINE_COUNT=line_count
PFX2=tailf
PFX3=randcsv
PFX4=emojis


all : ${WINTAIL_PREFIX}.ico ${TARGETS}

ifeq ($(MSBUILD),)
${WINTAIL_PREFIX}${EXEXT} :  ${OBJS}
else
${WINTAIL_PREFIX}${EXEXT} : ${WINTAIL_PREFIX_SRCS}
	${MSBUILD} wintail.sln -p:Configuration=Release
	cp x64/Release/${WINTAIL_PREFIX}${EXEXT} .	
endif


${PFX4}.o : emoji_map.h

emoji_map.h :
	./gen_emojis.sh

${WINTAIL_PREFIX}_res.o : ${WINTAIL_PREFIX}.ico

strip : $(TARGETS)
	@file ${TARGETS} | grep stripped >/dev/null || ( $(STRIP) $(TARGETS) && echo "Strip OK" )

upx : $(TARGETS)
	$(STRIP) $(TARGETS) 2>/dev/null || true
	$(UPX) -q $(TARGETS) 2>/dev/null || true

cfg :
	@echo "BUILD_SYSTEM ${BUILD_SYSTEM}"
	@echo "${PATH}"
	@type strip upx convert inkscape iscc
ifeq (${MSYSTEM},CLANG64)
	@type clang clang++
endif
	@type cc c++ gcc g++ ld windres gdb convert inkscape iscc
	@${ECHO} "CPPFLAGS=${CPPFLAGS}\nCXXFLAGS=${CXXFLAGS}\nLDFLAGS=${LDFLAGS}\nLDLIBS=${LDLIBS}"
	@${ECHO} "SRCS=${SRCS}\nOBJS=${OBJS}\nTARGETS=${TARGETS}"
	@${ECHO} "Single exes=${SINGLE_EXES}"

clean :
	rm -f *~ *.o $(OBJS) ${WINTAIL_PREFIX}.ico

rclean :
	rm -f *~ *.d *.o $(OBJS) $(TARGETS) ${WINTAIL_PREFIX}.ico *.exe
	rm -rf x64


# Ces régles implicites ne sont pas utiles quand on fait 'make rclean' (voir même make clean ...)
ifneq ($(MAKECMDGOALS),rclean)
%.ico : %.png
	${MAGICK} convert -background none $< $@

%.ico : %.svg
	${MAGICK} convert -background none $< $@

#ifeq ($(MSBUILD),)
%.exe: %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.exe: %.cpp
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.exe: %.o
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

# Régles pour construire les fichier objet d'après les .rc
%.o : %.rc
	$(RC) $(CPPFLAGS) $< --include-dir . $(OUTPUT_OPTION)
#endif

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

