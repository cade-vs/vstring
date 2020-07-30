
### MAKEMAKE STARTS HERE #######################################################


### Created by makemake.pl on Thu Jul 30 23:37:53 2020 #########################


### GLOBAL TARGETS #############################################################

default: mm_update all

re: mm_update rebuild

li: mm_update link

all: mm_update modules vstring.a test 

clean: mm_update clean-modules clean-vstring.a clean-test 

rebuild: mm_update rebuild-modules rebuild-vstring.a rebuild-test 

link: mm_update link-modules link-vstring.a link-test 

### GLOBAL (AND USER) DEFS #####################################################


AR = ar rv
CC = gcc
CCX = g++
LD = gcc
LDX = g++
MKDIR = mkdir -p
MODULES = pcre
RANLIB = ranlib
RMDIR = rm -rf
RMFILE = rm -f
SRC = *.c *.cpp *.cc *.cxx


### TARGET 1: libvstring.a #####################################################

CC_1       = $(CCX)
LD_1       = $(LDX)
AR_1       = ar rv
RANLIB_1   = ranlib
CCFLAGS_1  = -I. -Ipcre -O2 $(CCDEF) $(DEBUG) 
LDFLAGS_1  = -Lpcre $(LDDEF) 
DEPFLAGS_1 = 
ARFLAGS_1  = 
TARGET_1   = libvstring.a

### SOURCES FOR TARGET 1: libvstring.a #########################################

SRC_1= \
     vstring.cpp \
     vstrlib.cpp \

#### OBJECTS FOR TARGET 1: libvstring.a ########################################

OBJ_1= \
     .OBJ.vstring.a/vstring.o \
     .OBJ.vstring.a/vstrlib.o \

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

link-vstring.a: .OBJ.vstring.a $(OBJ_1)
	$(RMFILE) libvstring.a
	$(AR_1) $(ARFLAGS_1) $(TARGET_1) $(OBJ_1)
	$(RANLIB_1) $(TARGET_1)


### TARGET OBJECTS FOR TARGET 1: libvstring.a ##################################

.OBJ.vstring.a/vstring.o: vstring.cpp  vstring.cpp vstring.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c vstring.cpp          -o .OBJ.vstring.a/vstring.o
.OBJ.vstring.a/vstrlib.o: vstrlib.cpp  vstrlib.cpp vstrlib.h vstring.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c vstrlib.cpp          -o .OBJ.vstring.a/vstrlib.o


### TARGET 2: test #############################################################

CC_2       = $(CCX)
LD_2       = $(LDX)
AR_2       = ar rv
RANLIB_2   = ranlib
CCFLAGS_2  = -I. -O2 -g $(CCDEF) $(DEBUG) 
LDFLAGS_2  = -Lpcre -lpcre $(LDDEF) 
DEPFLAGS_2 = 
ARFLAGS_2  = 
TARGET_2   = test

### SOURCES FOR TARGET 2: test #################################################

SRC_2= \
     vstring.cpp \
     vstrlib.cpp \
     test.cpp \

#### OBJECTS FOR TARGET 2: test ################################################

OBJ_2= \
     .OBJ.test/vstring.o \
     .OBJ.test/vstrlib.o \
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

link-test: .OBJ.test $(OBJ_2)
	$(RMFILE) test
	$(LD_2) $(OBJ_2) $(LDFLAGS_2) -o $(TARGET_2)


### TARGET OBJECTS FOR TARGET 2: test ##########################################

.OBJ.test/vstring.o: vstring.cpp  vstring.cpp vstring.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c vstring.cpp          -o .OBJ.test/vstring.o
.OBJ.test/vstrlib.o: vstrlib.cpp  vstrlib.cpp vstrlib.h vstring.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c vstrlib.cpp          -o .OBJ.test/vstrlib.o
.OBJ.test/test.o: test.cpp  test.cpp vstrlib.h vstring.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c test.cpp             -o .OBJ.test/test.o


### MODULES ####################################################################

modules:
	$(MAKE) -C pcre 

clean-modules:
	$(MAKE) -C pcre clean

rebuild-modules:
	$(MAKE) -C pcre rebuild

link-modules:
	$(MAKE) -C pcre link


mm_update:
	


### MAKEMAKE ENDS HERE #########################################################

