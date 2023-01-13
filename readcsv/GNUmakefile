
ifneq ($(shell uname -s),Linux)
ifneq (${MSYSTEM},MSYS)
PATH:=/$(shell echo $(MSYSTEM) | tr A-Z a-z)/bin:${PATH}
endif

ifeq (${MSYSTEM},CLANG64)
CC=clang
CXX=clang++
endif

LDFLAGS  += -static
EXEXT=.exe
ECHO=echo -e
else
LD=cxx
ECHO=echo
endif


CPPFLAGS += -D_UNICODE -DUNICODE
CFLAGS   += -Wall -Wextra -std=c18 -pedantic
CXXFLAGS += -Wall -Wextra -std=c++20 -pedantic -Os
#LDFLAGS  += -g
LDFLAGS  += -Os -s
GDB=gdb
STRIP=strip
UPX=upx

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)
TARGET=readcsv${EXEXT}


all : ${TARGET}

${TARGET} : ${OBJS}

strip : $(TARGET)
	@file ${TARGET} | grep stripped >/dev/null || ( $(STRIP) $(TARGET) && echo "Strip OK" )

upx : $(TARGET)
	$(STRIP) $(TARGET) 2>/dev/null || true
	$(UPX) -q $(TARGET) 2>/dev/null || true

cfg :
#	@echo "PATH"
#	@echo "${PATH}" | sed 's/:/\n/g'
#	@echo "END PATH"
	@type ${CXX} ${CXX} ${LD} ${GDB} ${STRIP} ${UPX}
	@${ECHO} "CPPFLAGS=${CPPFLAGS}"
	@${ECHO} "CXXFLAGS=${CXXFLAGS}"
	@${ECHO} "LDFLAGS=${LDFLAGS}"
	@${ECHO} "LDLIBS=${LDLIBS}"
	@${ECHO} "SRCS=${SRCS}"
	@${ECHO} "OBJS=${OBJS}"
	@${ECHO} "TARGET=${TARGET}"

clean :
	rm -f *~ *.o $(OBJS)

rclean :
	rm -f *~ *.d *.o $(OBJS) $(TARGET) *.exe


# Ces régles implicites ne sont pas utiles quand on fait 'make rclean' (voir même make clean ...)
ifneq ($(MAKECMDGOALS),rclean)
%.exe: %.o
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

%.exe: %.cpp
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

#	@echo "Building "$@" from "$<
%.d: %.cpp
	@echo Checking header dependencies from $<
	@$(COMPILE.cpp) -isystem /usr/include -MM -MG $< > $@
# Inclusion des fichiers de dépendance .d
ifdef OBJS
-include $(OBJS:.o=.d)
endif
endif

