
OS=$(shell uname -s)

# Si le paramètre est un répertoire existant alors on l'ajoute au PATH
define a2p
  $(eval PATH=$(shell [ -d "${1}" ] && echo ${1}:)${PATH})
endef

CFLAGS += -Wall -Wextra -std=c18 -pedantic
CXXFLAGS += -Wall -Wextra -std=c++20 -pedantic
#LDFLAGS += -static-libgcc -static-libstdc++
#LDFLAGS  += -static
LDFLAGS  += -g -Os

PREFIX=wintail
SRCS=${PREFIX}.cpp
ifneq ($(MSBUILD),)
OBJS=$(SRCS:.cpp=.o)
endif

# If not linux then assume that it is windows
ifneq (${OS},Linux)
#MSYSTEM=UCRT64
#MSYSTEM=MINGW64

MSYSTEM=CLANG64
#MSYSTEM=MSBUILD
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
OBJS += ${PREFIX}_res.o

ECHO=echo -e
TARGETS+=${PREFIX}${EXEXT}
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
TARGETS +=  mywc${EXEXT} mytail${EXEXT}


all : ${TARGETS}

ifneq ($(MSBUILD),)
${PREFIX}${EXEXT} : wintail.cpp
	${MSBUILD} wintail.sln -p:Configuration=Release
	cp x64/Release/${PREFIX}${EXEXT} .	
endif


mywc${EXEXT} : mywc.cpp
	${CXX} ${CXXFLAGS} mywc.cpp  -o mywc

mytail${EXEXT} : mytail.cpp
	${CXX} ${CXXFLAGS} mytail.cpp  -o mytail

ifneq ($(MSBUILD),)
${PREFIX}${EXEXT} : ${OBJS}
endif

${PREFIX}_res.o : ${PREFIX}.ico

strip : $(TARGETS)
	@file ${TARGETS} | grep stripped >/dev/null || ( $(STRIP) $(TARGETS) && echo "Strip OK" )

upx : $(TARGETS)
	$(STRIP) $(TARGETS) 2>/dev/null || true
	$(UPX) -q $(TARGETS) 2>/dev/null || true

cfg :
	@echo "${PATH}"
	@type strip upx convert inkscape iscc
ifneq ($(MSYSTEM),MSBUILD)
	@type cc c++ gcc g++ ld windres gdb convert inkscape iscc
	@${ECHO} "CPPFLAGS=${CPPFLAGS}\nCXXFLAGS=${CXXFLAGS}\nLDFLAGS=${LDFLAGS}\nLDLIBS=${LDLIBS}"
endif
	@${ECHO} "SRCS=${SRCS}\nOBJS=${OBJS}\nTARGETS=${TARGETS}"

clean :
	rm -f *~ *.o $(OBJS) ${PREFIX}.ico

rclean :
	rm -f *~ *.d *.o $(OBJS) $(TARGETS) ${PREFIX}.ico *.exe


# Ces régles implicites ne sont pas utiles quand on fait 'make rclean' (voir même make clean ...)
ifneq ($(MAKECMDGOALS),rclean)
ifneq ($(MSBUILD),)
%.exe: %.o
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.exe: %.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.exe: %.cpp
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.ico : %.png
	${MAGICK} convert -background none $< $@

%.ico : %.svg
	${MAGICK} convert -background none $< $@

# Régles pour construire les fichier objet d'après les .rc
%.o : %.rc
	$(RC) $(CPPFLAGS) $< --include-dir . $(OUTPUT_OPTION)

%.d: %.c
	@echo Checking header dependencies from $<
	@$(COMPILE.c) -isystem /usr/include -MM $< >> $@

#	@echo "Building "$@" from "$<
%.d: %.cpp
	@echo Checking header dependencies from $<
	@$(COMPILE.cpp) -isystem /usr/include -MM $< >> $@

# Inclusion des fichiers de dépendance .d
ifdef OBJS
-include $(OBJS:.o=.d)
endif
endif
endif

