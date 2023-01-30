# use this to disable flto optimizations:
#   make NO_FLTO=1
# and this to enable verbose mode:
#   make V=1

AR?=gcc-ar
STRIP?=strip
RANLIB?=ranlib
PKG_CONFIG?=pkg-config

PCRE08_CC?=$(shell $(PKG_CONFIG) --cflags libpcre2-8)
PCRE08_LD?=$(shell $(PKG_CONFIG) --libs libpcre2-8)
PCRE32_CC?=$(shell $(PKG_CONFIG) --cflags libpcre2-32)
PCRE32_LD?=$(shell $(PKG_CONFIG) --libs libpcre2-32)

all: libvstring.a test wtest

SRCS:=\
	test.cpp \
	vref.cpp \
	vstring.cpp \
	vstrlib.cpp \
	vstruti.cpp \
	wstring.cpp \
	wstrlib.cpp \
	wtest.cpp
OBJS:=$(SRCS:.cpp=.o)
DEPS:=$(OBJS:.o=.d)

ifndef NO_FLTO
CXXFLAGS?=-O3 -fno-stack-protector -mno-stackrealign
CXXFLAGS+=-flto=auto
else
CXXFLAGS?=-O3 -fno-stack-protector -mno-stackrealign
endif

# some architectures do not have -mno-stackrealign
HAVESREA:=$(shell if $(CXX) -mno-stackrealign -xc -c /dev/null -o /dev/null >/dev/null 2>/dev/null;then echo yes;else echo no;fi)
# old comiplers do not have -Wdate-time
HAVEWDTI:=$(shell if $(CXX) -Wdate-time -xc -c /dev/null -o /dev/null >/dev/null 2>/dev/null;then echo yes;else echo no;fi)

MYCXXFLAGS:=$(CPPFLAGS) $(CXXFLAGS) $(PCRE08_CC) $(PCRE32_CC) -Wall -Wextra -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 -fPIE
ifeq ("$(HAVESREA)","no")
MYCXXFLAGS:=$(filter-out -mno-stackrealign,$(MYCXXFLAGS))
endif
ifeq ("$(HAVEWDTI)","no")
MYCXXFLAGS:=$(filter-out -Wdate-time,$(MYCXXFLAGS))
endif

MYLDFLAGS:=$(MYCXXFLAGS) $(LDFLAGS) -fPIE -pie
MYLIBS:=$(LIBS) $(PCRE08_LD) $(PCRE32_LD)

ifeq ("$(V)","1")
Q:=
E:=@true
else
Q:=@
E:=@echo
endif

LIBOBJ:=$(filter-out test.o,$(OBJS))
LIBOBJ:=$(filter-out wtest.o,$(LIBOBJ))

%.o: %.cpp
	$(E) DE $@
	$(Q)$(CXX) $(MYCXXFLAGS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(E) CXX $@
	$(Q)$(CXX) $(MYCXXFLAGS) -c -o $@ $<

libvstring.a: $(LIBOBJ)
	$(Q)rm -f $@
	$(E) AR $@
	$(Q)$(AR) rv $@ $+
	$(E) RANLIB $@
	$(Q)$(RANLIB) $@

test: test.o libvstring.a
	$(E) LD $@
	$(Q)$(CXX) -o $@ $(MYLDFLAGS) $< $(MYLIBS) -L. -lvstring

wtest: wtest.o libvstring.a
	$(E) LD $@
	$(Q)$(CXX) -o $@ $(MYLDFLAGS) $< $(MYLIBS) -L. -lvstring

clean:
	$(E) CLEAN
	$(Q) rm -f *.a *.o *.d test wtest

re:
	$(Q)$(MAKE) --no-print-directory clean
	$(Q)$(MAKE) --no-print-directory -j

-include $(DEPS)

.PHONY: all clean re
