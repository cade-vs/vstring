
### MAKEMAKE STARTS HERE #######################################################


### Created by makemake.pl on Fri Nov 19 02:57:12 2021 #########################


### GLOBAL TARGETS #############################################################

default: mm_update  vstring.a  test 

re: mm_update rebuild

li: mm_update link

all: mm_update vstring.a test 

clean: mm_update clean-vstring.a clean-test 

rebuild: mm_update rebuild-vstring.a rebuild-test 

link: mm_update link-vstring.a link-test 

### GLOBAL (AND USER) DEFS #####################################################


AR ?= ar
LD = $(CXX)
MKDIR = mkdir -p
RANLIB ?= ranlib
RMDIR = rm -rf
RMFILE = rm -f
SRC = *.c *.cpp *.cc *.cxx


### TARGET 1: libvstring.a #####################################################

CC_1       = $(CXX)
LD_1       = $(CXX)
AR_1       = $(AR) rv
RANLIB_1   = $(RANLIB)
CCFLAGS_1  = -I. -Ipcre2 -O2 $(CFLAGS) $(CPPFLAGS) $(CCDEF)  
LDFLAGS_1  = $(LDFLAGS) $(LDDEF) 
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

re-vstring.a: rebuild-vstring.a

link-vstring.a: .OBJ.vstring.a $(OBJ_1)
	$(RMFILE) libvstring.a
	$(AR_1) $(ARFLAGS_1) $(TARGET_1) $(OBJ_1)
	$(RANLIB_1) $(TARGET_1)


### TARGET OBJECTS FOR TARGET 1: libvstring.a ##################################

.OBJ.vstring.a/vstring.o: vstring.cpp 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c vstring.cpp          -o .OBJ.vstring.a/vstring.o
.OBJ.vstring.a/vstrlib.o: vstrlib.cpp 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c vstrlib.cpp          -o .OBJ.vstring.a/vstrlib.o


### TARGET 2: test #############################################################

CC_2       = $(CXX)
LD_2       = $(CXX)
AR_2       = $(AR) rv
RANLIB_2   = $(RANLIB)
CCFLAGS_2  = -I. -Ipcre2 -O2 -g $(CFLAGS) $(CPPFLAGS) $(CCDEF)  
LDFLAGS_2  = -Lpcre2 -lpcre2 $(LDFLAGS) $(LDDEF) 
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

re-test: rebuild-test

link-test: .OBJ.test $(OBJ_2)
	$(RMFILE) test
	$(LD_2) $(OBJ_2) $(LDFLAGS_2) -o $(TARGET_2)


### TARGET OBJECTS FOR TARGET 2: test ##########################################

.OBJ.test/vstring.o: vstring.cpp 
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c vstring.cpp          -o .OBJ.test/vstring.o
.OBJ.test/vstrlib.o: vstrlib.cpp 
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c vstrlib.cpp          -o .OBJ.test/vstrlib.o
.OBJ.test/test.o: test.cpp 
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c test.cpp             -o .OBJ.test/test.o


mm_update:
	


### MAKEMAKE ENDS HERE #########################################################

