
# Test push all
OS=$(shell uname -s)

# Si le paramètre est un répertoire existant alors on l'ajoute au PATH
define a2p
  $(eval PATH=$(shell [ -d "${1}" ] && echo ${1}:)${PATH})
endef

WSDIR=/usr/local/wsserver
#CPPFLAGS += -U VERBOSE_MODE
CPPFLAGS += -I $(WSDIR)/include -I $(WSDIR)/include/wsserver
CFLAGS += -Wall -Wextra -std=c18 -pedantic
CXXFLAGS += -Wall -Wextra -std=c++20 -pedantic
#LDFLAGS += -static-libgcc -static-libstdc++
LDFLAGS  += -static
LDFLAGS  +=  -L $(WSDIR)/lib
LDFLAGS  += -g -Os
LDLIBS   +=  -lws

PREFIX=wintail
SRCS=${PREFIX}.cpp
OBJS=$(SRCS:.cpp=.o)

# If not linux then assume that it is windows
ifneq (${OS},Linux)
#MSYSTEM=UCRT64
#MSYSTEM=CLANG64
MSYSTEM=MINGW64
MAGICK=magick
RC=windres

ifeq (${MSYSTEM},MINGW64)
$(call a2p,/mingw64/bin)
endif

ifeq (${MSYSTEM},UCRT64)
$(call a2p,/ucrt64/bin)
LDFLAGS += -static
endif

# CLANG n'accepte pas les fichiers en ISO-8859
# Pour utiliser clang il faut passer par le shell clang64.exe de msys2
ifeq (${MSYSTEM},CLANG64)
$(call a2p,/clang64/bin)
CC=clang++
CXX=clang++
LDFLAGS += -pthread -static
else
CC=gcc
CXX=g++
CPPFLAGS += -I /usr/local/include/wsserver
LDFLAGS += -L /usr/local/lib64
endif

EXEXT=.exe
LDFLAGS += -mwindows
LDLIBS  +=  -lwsock32 -lws2_32

$(call a2p,/C/Program Files \(x86\)/Inno Setup 6)
$(call a2p,${HOME}/Progs/Inno Setup 6)
$(call a2p,/C/Program Files/ImageMagick-7.1.0-Q16)
$(call a2p,${HOME}/Progs/ImageMagick-7.1.0-portable-Q16-x64)

LDLIBS   += -lurlmon
LDLIBS   += -lwsock32 -lole32 -luuid -lcomctl32 -loleaut32 -lgdi32
UPX=upx
OBJS += ${PREFIX}_res.o

ECHO=echo -e
TARGETS+=${PREFIX}${EXEXT}
else
CPPFLAGS += -I /usr/local/include/wsserver
#LDFLAGS += -L /usr/local/lib64
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

mywc${EXEXT} : mywc.cpp
	${CXX} ${CXXFLAGS} mywc.cpp  -o mywc

mytail${EXEXT} : mytail.cpp
	${CXX} ${CXXFLAGS} mytail.cpp  -o mytail

${PREFIX}${EXEXT} : ${OBJS}

${PREFIX}_res.o : ${PREFIX}.ico

strip : $(TARGETS)
	$(STRIP) $(TARGETS)

upx : strip
	$(UPX) $(TARGETS)

cfg :
	@type ${CC} ${CXX} ${LD} ${RC} ${STRIP} ${GDB} ${UPX} ${MAGICK}
	@${ECHO} "CPPFLAGS=${CPPFLAGS}\nCXXFLAGS=${CXXFLAGS}\nLDFLAGS=${LDFLAGS}\nLDLIBS=${LDLIBS}"
	@${ECHO} "SRCS=${SRCS}\nOBJS=${OBJS}\nTARGETS=${TARGETS}"

clean :
	rm -f *~ *.o $(OBJS) ${PREFIX}.ico

rclean :
	rm -f *~ *.d *.o $(OBJS) $(TARGETS) ${PREFIX}.ico *.exe


# Ces régles implicites ne sont pas utiles quand on fait 'make rclean' (voir même make clean ...)
ifneq ($(MAKECMDGOALS),rclean)
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

