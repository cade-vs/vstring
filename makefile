
### MAKEMAKE STARTS HERE #######################################################


### Created by makemake.pl on Mon Jan 30 00:58:24 2023 #########################


### GLOBAL TARGETS #############################################################

default: mm_update all

re: mm_update rebuild

li: mm_update link

all: mm_update vstring.a test wtest 

clean: mm_update clean-vstring.a clean-test clean-wtest 

rebuild: mm_update rebuild-vstring.a rebuild-test rebuild-wtest 

link: mm_update link-vstring.a link-test link-wtest 

### GLOBAL (AND USER) DEFS #####################################################


AR = ar rv
CC = $(CXX)
LD = $(CXX)
MKDIR = mkdir -p
RANLIB = ranlib
RMDIR = rm -rf
RMFILE = rm -f
SRC = *.c *.cpp *.cc *.cxx


### TARGET 1: libvstring.a #####################################################

CC_1       = $(CXX)
LD_1       = $(CXX)
AR_1       = ar rv
RANLIB_1   = ranlib
CCFLAGS_1  = -I. -O3 -Wpedantic -Wextra -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 $(CFLAGS) $(CPPFLAGS) $(CCDEF)  
LDFLAGS_1  = $(LDFLAGS) $(LDDEF) 
DEPFLAGS_1 = 
ARFLAGS_1  = 
TARGET_1   = libvstring.a

### SOURCES FOR TARGET 1: libvstring.a #########################################

SRC_1= \
     vref.cpp \
     vstring.cpp \
     vstrlib.cpp \
     wstring.cpp \
     wstrlib.cpp \
     vstruti.cpp \

#### OBJECTS FOR TARGET 1: libvstring.a ########################################

OBJ_1= \
     .OBJ.vstring.a/vref.o \
     .OBJ.vstring.a/vstring.o \
     .OBJ.vstring.a/vstrlib.o \
     .OBJ.vstring.a/wstring.o \
     .OBJ.vstring.a/wstrlib.o \
     .OBJ.vstring.a/vstruti.o \

### TARGET DEFINITION FOR TARGET 1: libvstring.a ###############################

.OBJ.vstring.a: 
	$(MKDIR) .OBJ.vstring.a

vstring.a:   .OBJ.vstring.a $(OBJ_1)
	$(AR_1) $(ARFLAGS_1) $(TARGET_1) $(OBJ_1)
	$(RANLIB_1) $(TARGET_1)

clean-vstring.a: 
	$(RMFILE) $(TARGET_1)
	$(RMDIR) .OBJ.vstring.a

rebuild-vstring.a: clean-vstring.a vstring.a

re-vstring.a: rebuild-vstring.a

link-vstring.a: .OBJ.vstring.a $(OBJ_1)
	$(RMFILE) libvstring.a
	$(AR_1) $(ARFLAGS_1) $(TARGET_1) $(OBJ_1)
	$(RANLIB_1) $(TARGET_1)


### TARGET OBJECTS FOR TARGET 1: libvstring.a ##################################

.OBJ.vstring.a/vref.o: vref.cpp  vref.cpp vdef.h vref.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c vref.cpp             -o .OBJ.vstring.a/vref.o
.OBJ.vstring.a/vstring.o: vstring.cpp  vstring.cpp vref.h vdef.h vstring_internal.h \
 vstring_internal.cpp
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c vstring.cpp          -o .OBJ.vstring.a/vstring.o
.OBJ.vstring.a/vstrlib.o: vstrlib.cpp  vstrlib.cpp vdef.h vref.h vstring_internal.h \
 vstrlib_internal.cpp vstrlib_internal.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c vstrlib.cpp          -o .OBJ.vstring.a/vstrlib.o
.OBJ.vstring.a/wstring.o: wstring.cpp  wstring.cpp vref.h vdef.h vstring_internal.h \
 vstring_internal.cpp
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c wstring.cpp          -o .OBJ.vstring.a/wstring.o
.OBJ.vstring.a/wstrlib.o: wstrlib.cpp  wstrlib.cpp vdef.h vref.h vstring_internal.h \
 vstrlib_internal.cpp vstrlib_internal.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c wstrlib.cpp          -o .OBJ.vstring.a/wstrlib.o
.OBJ.vstring.a/vstruti.o: vstruti.cpp  vstruti.cpp vstruti.h vstring.h vdef.h vref.h \
 vstring_internal.h wstring.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c vstruti.cpp          -o .OBJ.vstring.a/vstruti.o


### TARGET 2: test #############################################################

CC_2       = $(CXX)
LD_2       = $(CXX)
AR_2       = ar rv
RANLIB_2   = ranlib
CCFLAGS_2  = -I. -O3 -g -Wpedantic -Wextra -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 $(CFLAGS) $(CPPFLAGS) $(CCDEF)  
LDFLAGS_2  = -L. -lvstring -lpcre2-8 -lpcre2-32 $(LDFLAGS) $(LDDEF) 
DEPFLAGS_2 = 
ARFLAGS_2  = 
TARGET_2   = test

### SOURCES FOR TARGET 2: test #################################################

SRC_2= \
     vref.cpp \
     vstring.cpp \
     vstrlib.cpp \
     wstring.cpp \
     wstrlib.cpp \
     vstruti.cpp \
     test.cpp \

#### OBJECTS FOR TARGET 2: test ################################################

OBJ_2= \
     .OBJ.test/vref.o \
     .OBJ.test/vstring.o \
     .OBJ.test/vstrlib.o \
     .OBJ.test/wstring.o \
     .OBJ.test/wstrlib.o \
     .OBJ.test/vstruti.o \
     .OBJ.test/test.o \

### TARGET DEFINITION FOR TARGET 2: test #######################################

.OBJ.test: 
	$(MKDIR) .OBJ.test

test:   .OBJ.test $(OBJ_2)
	$(LD_2) $(OBJ_2) $(LDFLAGS_2) -o $(TARGET_2)

clean-test: 
	$(RMFILE) $(TARGET_2)
	$(RMDIR) .OBJ.test

rebuild-test: clean-test test

re-test: rebuild-test

link-test: .OBJ.test $(OBJ_2)
	$(RMFILE) test
	$(LD_2) $(OBJ_2) $(LDFLAGS_2) -o $(TARGET_2)


### TARGET OBJECTS FOR TARGET 2: test ##########################################

.OBJ.test/vref.o: vref.cpp  vref.cpp vdef.h vref.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c vref.cpp             -o .OBJ.test/vref.o
.OBJ.test/vstring.o: vstring.cpp  vstring.cpp vref.h vdef.h vstring_internal.h \
 vstring_internal.cpp
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c vstring.cpp          -o .OBJ.test/vstring.o
.OBJ.test/vstrlib.o: vstrlib.cpp  vstrlib.cpp vdef.h vref.h vstring_internal.h \
 vstrlib_internal.cpp vstrlib_internal.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c vstrlib.cpp          -o .OBJ.test/vstrlib.o
.OBJ.test/wstring.o: wstring.cpp  wstring.cpp vref.h vdef.h vstring_internal.h \
 vstring_internal.cpp
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c wstring.cpp          -o .OBJ.test/wstring.o
.OBJ.test/wstrlib.o: wstrlib.cpp  wstrlib.cpp vdef.h vref.h vstring_internal.h \
 vstrlib_internal.cpp vstrlib_internal.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c wstrlib.cpp          -o .OBJ.test/wstrlib.o
.OBJ.test/vstruti.o: vstruti.cpp  vstruti.cpp vstruti.h vstring.h vdef.h vref.h \
 vstring_internal.h wstring.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c vstruti.cpp          -o .OBJ.test/vstruti.o
.OBJ.test/test.o: test.cpp  test.cpp vstring.h vdef.h vref.h vstring_internal.h vstrlib.h \
 vstrlib_internal.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c test.cpp             -o .OBJ.test/test.o


### TARGET 3: wtest ############################################################

CC_3       = $(CXX)
LD_3       = $(CXX)
AR_3       = 
RANLIB_3   = 
CCFLAGS_3  = -I. -O3 -g -Wpedantic -Wextra -Wformat -Werror=format-security -Wdate-time -D_FORTIFY_SOURCE=2 $(CFLAGS) $(CPPFLAGS) $(CCDEF)  
LDFLAGS_3  = -L. -lvstring -lpcre2-8 -lpcre2-32 $(LDFLAGS) $(LDDEF) 
DEPFLAGS_3 = 
ARFLAGS_3  = 
TARGET_3   = wtest

### SOURCES FOR TARGET 3: wtest ################################################

SRC_3= \
     wtest.cpp \

#### OBJECTS FOR TARGET 3: wtest ###############################################

OBJ_3= \
     .OBJ.wtest/wtest.o \

### TARGET DEFINITION FOR TARGET 3: wtest ######################################

.OBJ.wtest: 
	$(MKDIR) .OBJ.wtest

wtest:   .OBJ.wtest $(OBJ_3)
	$(LD_3) $(OBJ_3) $(LDFLAGS_3) -o $(TARGET_3)

clean-wtest: 
	$(RMFILE) $(TARGET_3)
	$(RMDIR) .OBJ.wtest

rebuild-wtest: clean-wtest wtest

re-wtest: rebuild-wtest

link-wtest: .OBJ.wtest $(OBJ_3)
	$(RMFILE) wtest
	$(LD_3) $(OBJ_3) $(LDFLAGS_3) -o $(TARGET_3)


### TARGET OBJECTS FOR TARGET 3: wtest #########################################

.OBJ.wtest/wtest.o: wtest.cpp  wtest.cpp vstring.h vdef.h vref.h vstring_internal.h wstring.h \
 vstrlib.h vstrlib_internal.h wstrlib.h
	$(CC_3) $(CFLAGS_3) $(CCFLAGS_3) -c wtest.cpp            -o .OBJ.wtest/wtest.o


mm_update:
	


### MAKEMAKE ENDS HERE #########################################################

