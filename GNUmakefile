
OS=$(shell uname -s)


# If not linux then assume that it is windows
ifneq (${OS},Linux)

MSBUILD='C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\amd64\MSBuild.exe'

ifeq (${MSYSTEM},MINGW64)
ARCH_PATH=/mingw64/bin
CPPFLAGS += -D_UNICODE -DUNICODE
LDFLAGS  += -static
endif

ifeq (${MSYSTEM},UCRT64)
ARCH_PATH=/ucrt64/bin
#LDFLAGS += -static
endif

# CLANG n'accepte pas les fichiers en ISO-8859
# Pour utiliser clang il faut passer par le shell clang64.exe de msys2
ifeq (${MSYSTEM},CLANG64)
ARCH_PATH=/clang64/bin
CC=clang++
CXX=clang++
#CPPFLAGS += -D_UNICODE -DUNICODE
#LDFLAGS += -pthread -static
endif

PGF=$(subst \,/,$(subst C:\,/c/,$(PROGRAMFILES)))
PGF86=${PGF} (x86)
PATH:=${PATH}:${PGF86}/Inno Setup 6
PATH:=${PATH}:${PGF}/Inkscape/bin
#PATH:=${PATH}:${HOME}/Progs/Inno Setup 6
#PATH:=${PATH}:/c/UnixTools/bin

PATH:=${ARCH_PATH}:${PATH}

CC:=${ARCH_PATH}/${CC}
CXX:=${ARCH_PATH}/${CXX}
GCC:=${CC}
GXX:=${CXX}
RC=${ARCH_PATH}/windres
LD=${ARCH_PATH}/ld
STRIP=${ARCH_PATH}/strip
GDB=${ARCH_PATH}/gdb
UPX=upx

CPPFLAGS += -D_UNICODE -DUNICODE
CFLAGS += -Wall -Wextra -std=c18 -pedantic
CXXFLAGS += -Wall -Wextra -std=c++20 -pedantic
LDFLAGS += -mwindows
LDFLAGS += -static -g -Os
LDLIBS   += -lurlmon -lwsock32 -lws2_32 -lole32 -luuid -lcomctl32 -loleaut32 -lgdi32

WINTAIL_PREFIX=wintail
WINTAIL_PREFIX_SRCS=${WINTAIL_PREFIX}.cpp reghandle.cpp util.cpp
WINTAIL_PREFIX_OBJS=$(WINTAIL_PREFIX_SRCS:.cpp=.o)

MAGICK=/mingw64/bin/magick
RC=windres


EXEXT=.exe
SRCS=$(wildcard *.cpp)
SINGLE_SRCS=$(filter-out ${WINTAIL_PREFIX_SRCS},${SRCS})
SINGLE_EXES=$(SINGLE_SRCS:.cpp=${EXEXT})

ifeq ($(MSBUILD),)
OBJS=$(SRCS:.cpp=.o)
OBJS += ${WINTAIL_PREFIX}_res.o
endif

ECHO=echo -e
TARGETS+=${WINTAIL_PREFIX}${EXEXT} ${SINGLE_EXES}

else

MAGICK=
UPX=upx
RC=
CC=$(CXX)
LDFLAGS += -pthread -lrt
ECHO=echo
STRIP=strip
GDB=gdb
endif

#LD=$(CXX)
LINE_COUNT=line_count

all : ${WINTAIL_PREFIX}.ico ${TARGETS}

ifeq ($(MSBUILD),)
${WINTAIL_PREFIX}${EXEXT} :  ${OBJS}
else
${WINTAIL_PREFIX}${EXEXT} : ${WINTAIL_PREFIX_SRCS}
	${MSBUILD} wintail.sln -p:Configuration=Release
	cp x64/Release/${WINTAIL_PREFIX}${EXEXT} .	
endif

$(SINGLE_EXES): %.exe: %.cpp
	$(CXX)  -Wall -Wextra -std=c++20 -pedantic $< -o $@

emojis.o : emoji_map.h

emoji_map.h :
	./gen_emojis.sh

${WINTAIL_PREFIX}_res.o : ${WINTAIL_PREFIX}.ico

strip : $(TARGETS)
	@file ${TARGETS} | grep stripped >/dev/null || ( $(STRIP) $(TARGETS) && echo "Strip OK" )

upx : $(TARGETS)
	$(STRIP) $(TARGETS) 2>/dev/null || true
	$(UPX) -q $(TARGETS) 2>/dev/null || true

cfg :
	@echo "PATH"
	@echo "${PATH}" | sed 's/:/\n/g'
	@echo "END PATH"
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

