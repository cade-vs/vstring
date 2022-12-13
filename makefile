
### MAKEMAKE STARTS HERE #######################################################


### Created by makemake.pl on Tue Dec 13 00:29:34 2022 #########################


### GLOBAL TARGETS #############################################################

default: mm_update all

re: mm_update rebuild

li: mm_update link

all: mm_update vstring.a wstring.a test wtest 

clean: mm_update clean-vstring.a clean-wstring.a clean-test clean-wtest 

rebuild: mm_update rebuild-vstring.a rebuild-wstring.a rebuild-test rebuild-wtest 

link: mm_update link-vstring.a link-wstring.a link-test link-wtest 

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
RANLIB_1   = $(RANLIB)
CCFLAGS_1  = -I. -O2 $(CFLAGS) $(CPPFLAGS) $(CCDEF)  
LDFLAGS_1  = $(LDFLAGS) $(LDDEF) 
DEPFLAGS_1 = 
ARFLAGS_1  = 
TARGET_1   = libvstring.a

### SOURCES FOR TARGET 1: libvstring.a #########################################

SRC_1= \
     vstring.cpp \
     vstrlib.cpp \
     vref.cpp \

#### OBJECTS FOR TARGET 1: libvstring.a ########################################

OBJ_1= \
     .OBJ.vstring.a/vstring.o \
     .OBJ.vstring.a/vstrlib.o \
     .OBJ.vstring.a/vref.o \

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

.OBJ.vstring.a/vstring.o: vstring.cpp  vstring.cpp vref.h vdef.h vstring_internal.cpp \
 vstring_internal.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c vstring.cpp          -o .OBJ.vstring.a/vstring.o
.OBJ.vstring.a/vstrlib.o: vstrlib.cpp  vstrlib.cpp vdef.h vref.h vstring_internal.cpp \
 vstring_internal.h vstrlib_internal.cpp vstrlib_internal.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c vstrlib.cpp          -o .OBJ.vstring.a/vstrlib.o
.OBJ.vstring.a/vref.o: vref.cpp  vref.cpp vdef.h vref.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c vref.cpp             -o .OBJ.vstring.a/vref.o


### TARGET 2: libwstring.a #####################################################

CC_2       = $(CXX)
LD_2       = $(CXX)
AR_2       = ar rv
RANLIB_2   = $(RANLIB)
CCFLAGS_2  = -I. -O2 $(CFLAGS) $(CPPFLAGS) $(CCDEF)  
LDFLAGS_2  = $(LDFLAGS) $(LDDEF) 
DEPFLAGS_2 = 
ARFLAGS_2  = 
TARGET_2   = libwstring.a

### SOURCES FOR TARGET 2: libwstring.a #########################################

SRC_2= \
     wstring.cpp \
     wstrlib.cpp \
     vref.cpp \

#### OBJECTS FOR TARGET 2: libwstring.a ########################################

OBJ_2= \
     .OBJ.wstring.a/wstring.o \
     .OBJ.wstring.a/wstrlib.o \
     .OBJ.wstring.a/vref.o \

### TARGET DEFINITION FOR TARGET 2: libwstring.a ###############################

.OBJ.wstring.a: 
	$(MKDIR) .OBJ.wstring.a

wstring.a:   .OBJ.wstring.a $(OBJ_2)
	$(AR_2) $(ARFLAGS_2) $(TARGET_2) $(OBJ_2)
	$(RANLIB_2) $(TARGET_2)

clean-wstring.a: 
	$(RMFILE) $(TARGET_2)
	$(RMDIR) .OBJ.wstring.a

rebuild-wstring.a: clean-wstring.a wstring.a

re-wstring.a: rebuild-wstring.a

link-wstring.a: .OBJ.wstring.a $(OBJ_2)
	$(RMFILE) libwstring.a
	$(AR_2) $(ARFLAGS_2) $(TARGET_2) $(OBJ_2)
	$(RANLIB_2) $(TARGET_2)


### TARGET OBJECTS FOR TARGET 2: libwstring.a ##################################

.OBJ.wstring.a/wstring.o: wstring.cpp  wstring.cpp vdef.h vref.h vstring_internal.cpp \
 vstring_internal.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c wstring.cpp          -o .OBJ.wstring.a/wstring.o
.OBJ.wstring.a/wstrlib.o: wstrlib.cpp  wstrlib.cpp vdef.h vref.h vstring_internal.cpp \
 vstring_internal.h vstrlib_internal.cpp vstrlib_internal.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c wstrlib.cpp          -o .OBJ.wstring.a/wstrlib.o
.OBJ.wstring.a/vref.o: vref.cpp  vref.cpp vdef.h vref.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c vref.cpp             -o .OBJ.wstring.a/vref.o


### TARGET 3: test #############################################################

CC_3       = $(CXX)
LD_3       = $(CXX)
AR_3       = ar rv
RANLIB_3   = $(RANLIB)
CCFLAGS_3  = -I. -O2 -g $(CFLAGS) $(CPPFLAGS) $(CCDEF)  
LDFLAGS_3  = -L. -lpcre2-8 -lpcre2-32 -lvstring $(LDFLAGS) $(LDDEF) 
DEPFLAGS_3 = 
ARFLAGS_3  = 
TARGET_3   = test

### SOURCES FOR TARGET 3: test #################################################

SRC_3= \
     vstring.cpp \
     vstrlib.cpp \
     vref.cpp \
     test.cpp \

#### OBJECTS FOR TARGET 3: test ################################################

OBJ_3= \
     .OBJ.test/vstring.o \
     .OBJ.test/vstrlib.o \
     .OBJ.test/vref.o \
     .OBJ.test/test.o \

### TARGET DEFINITION FOR TARGET 3: test #######################################

.OBJ.test: 
	$(MKDIR) .OBJ.test

test:   .OBJ.test $(OBJ_3)
	$(LD_3) $(OBJ_3) $(LDFLAGS_3) -o $(TARGET_3)

clean-test: 
	$(RMFILE) $(TARGET_3)
	$(RMDIR) .OBJ.test

rebuild-test: clean-test test

re-test: rebuild-test

link-test: .OBJ.test $(OBJ_3)
	$(RMFILE) test
	$(LD_3) $(OBJ_3) $(LDFLAGS_3) -o $(TARGET_3)


### TARGET OBJECTS FOR TARGET 3: test ##########################################

.OBJ.test/vstring.o: vstring.cpp  vstring.cpp vref.h vdef.h vstring_internal.cpp \
 vstring_internal.h
	$(CC_3) $(CFLAGS_3) $(CCFLAGS_3) -c vstring.cpp          -o .OBJ.test/vstring.o
.OBJ.test/vstrlib.o: vstrlib.cpp  vstrlib.cpp vdef.h vref.h vstring_internal.cpp \
 vstring_internal.h vstrlib_internal.cpp vstrlib_internal.h
	$(CC_3) $(CFLAGS_3) $(CCFLAGS_3) -c vstrlib.cpp          -o .OBJ.test/vstrlib.o
.OBJ.test/vref.o: vref.cpp  vref.cpp vdef.h vref.h
	$(CC_3) $(CFLAGS_3) $(CCFLAGS_3) -c vref.cpp             -o .OBJ.test/vref.o
.OBJ.test/test.o: test.cpp  test.cpp vstring.h vref.h vdef.h vstring_internal.h vstrlib.h \
 vstrlib_internal.h
	$(CC_3) $(CFLAGS_3) $(CCFLAGS_3) -c test.cpp             -o .OBJ.test/test.o


### TARGET 4: wtest ############################################################

CC_4       = $(CXX)
LD_4       = $(CXX)
AR_4       = ar rv
RANLIB_4   = $(RANLIB)
CCFLAGS_4  = -I. -O2 -g $(CFLAGS) $(CPPFLAGS) $(CCDEF)  
LDFLAGS_4  = -L. -lpcre2-8 -lpcre2-32 -lwstring  $(LDFLAGS) $(LDDEF) 
DEPFLAGS_4 = 
ARFLAGS_4  = 
TARGET_4   = wtest

### SOURCES FOR TARGET 4: wtest ################################################

SRC_4= \
     wstring.cpp \
     wstrlib.cpp \
     vref.cpp \
     wtest.cpp \

#### OBJECTS FOR TARGET 4: wtest ###############################################

OBJ_4= \
     .OBJ.wtest/wstring.o \
     .OBJ.wtest/wstrlib.o \
     .OBJ.wtest/vref.o \
     .OBJ.wtest/wtest.o \

### TARGET DEFINITION FOR TARGET 4: wtest ######################################

.OBJ.wtest: 
	$(MKDIR) .OBJ.wtest

wtest:   .OBJ.wtest $(OBJ_4)
	$(LD_4) $(OBJ_4) $(LDFLAGS_4) -o $(TARGET_4)

clean-wtest: 
	$(RMFILE) $(TARGET_4)
	$(RMDIR) .OBJ.wtest

rebuild-wtest: clean-wtest wtest

re-wtest: rebuild-wtest

link-wtest: .OBJ.wtest $(OBJ_4)
	$(RMFILE) wtest
	$(LD_4) $(OBJ_4) $(LDFLAGS_4) -o $(TARGET_4)


### TARGET OBJECTS FOR TARGET 4: wtest #########################################

.OBJ.wtest/wstring.o: wstring.cpp  wstring.cpp vdef.h vref.h vstring_internal.cpp \
 vstring_internal.h
	$(CC_4) $(CFLAGS_4) $(CCFLAGS_4) -c wstring.cpp          -o .OBJ.wtest/wstring.o
.OBJ.wtest/wstrlib.o: wstrlib.cpp  wstrlib.cpp vdef.h vref.h vstring_internal.cpp \
 vstring_internal.h vstrlib_internal.cpp vstrlib_internal.h
	$(CC_4) $(CFLAGS_4) $(CCFLAGS_4) -c wstrlib.cpp          -o .OBJ.wtest/wstrlib.o
.OBJ.wtest/vref.o: vref.cpp  vref.cpp vdef.h vref.h
	$(CC_4) $(CFLAGS_4) $(CCFLAGS_4) -c vref.cpp             -o .OBJ.wtest/vref.o
.OBJ.wtest/wtest.o: wtest.cpp  wtest.cpp wstring.h vref.h vdef.h vstring_internal.h wstrlib.h \
 vstrlib_internal.h
	$(CC_4) $(CFLAGS_4) $(CCFLAGS_4) -c wtest.cpp            -o .OBJ.wtest/wtest.o


mm_update:
	


### MAKEMAKE ENDS HERE #########################################################

