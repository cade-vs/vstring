
### MAKEMAKE STARTS HERE #######################################################


### Created by makemake.pl on Tue Jan  7 23:45:39 2014 #########################


### GLOBAL TARGETS #############################################################

default: mm_update all

re: mm_update rebuild

li: mm_update link

all: mm_update libpcre.a 

clean: mm_update clean-libpcre.a 

rebuild: mm_update rebuild-libpcre.a 

link: mm_update link-libpcre.a 

### GLOBAL (AND USER) DEFS #####################################################


AR = ar rv
CC = gcc
LD = gcc
MKDIR = mkdir -p
RANLIB = ranlib
RMDIR = rm -rf
RMFILE = rm -f
SRC = *.c *.cpp *.cc *.cxx


### TARGET 1: libpcre.a ########################################################

CC_1       = gcc
LD_1       = gcc
AR_1       = ar rv
RANLIB_1   = ranlib
CCFLAGS_1  = -I. -O2   
LDFLAGS_1  =  
DEPFLAGS_1 = 
ARFLAGS_1  = 
TARGET_1   = libpcre.a

### SOURCES FOR TARGET 1: libpcre.a ############################################

SRC_1= \
     pcre_chartables.c \
     pcre_compile.c \
     pcre_config.c \
     pcre_exec.c \
     pcre_globals.c \
     pcre_newline.c \
     pcre_tables.c \
     pcre_try_flipped.c \

#### OBJECTS FOR TARGET 1: libpcre.a ###########################################

OBJ_1= \
     .OBJ.libpcre.a/pcre_chartables.o \
     .OBJ.libpcre.a/pcre_compile.o \
     .OBJ.libpcre.a/pcre_config.o \
     .OBJ.libpcre.a/pcre_exec.o \
     .OBJ.libpcre.a/pcre_globals.o \
     .OBJ.libpcre.a/pcre_newline.o \
     .OBJ.libpcre.a/pcre_tables.o \
     .OBJ.libpcre.a/pcre_try_flipped.o \

### TARGET DEFINITION FOR TARGET 1: libpcre.a ##################################

.OBJ.libpcre.a: 
	$(MKDIR) .OBJ.libpcre.a

libpcre.a:   .OBJ.libpcre.a $(OBJ_1)
	$(AR_1) $(ARFLAGS_1) $(TARGET_1) $(OBJ_1)
	$(RANLIB_1) $(TARGET_1)

clean-libpcre.a: 
	$(RMFILE) $(TARGET_1)
	$(RMDIR) .OBJ.libpcre.a

rebuild-libpcre.a: clean-libpcre.a libpcre.a

link-libpcre.a: .OBJ.libpcre.a $(OBJ_1)
	$(RMFILE) libpcre.a
	$(AR_1) $(ARFLAGS_1) $(TARGET_1) $(OBJ_1)
	$(RANLIB_1) $(TARGET_1)


### TARGET OBJECTS FOR TARGET 1: libpcre.a #####################################

.OBJ.libpcre.a/pcre_chartables.o: pcre_chartables.c  pcre_chartables.c pcre_internal.h pcre.h ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_chartables.c    -o .OBJ.libpcre.a/pcre_chartables.o
.OBJ.libpcre.a/pcre_compile.o: pcre_compile.c  pcre_compile.c pcre_internal.h pcre.h ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_compile.c       -o .OBJ.libpcre.a/pcre_compile.o
.OBJ.libpcre.a/pcre_config.o: pcre_config.c  pcre_config.c pcre_internal.h pcre.h ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_config.c        -o .OBJ.libpcre.a/pcre_config.o
.OBJ.libpcre.a/pcre_exec.o: pcre_exec.c  pcre_exec.c pcre_internal.h pcre.h ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_exec.c          -o .OBJ.libpcre.a/pcre_exec.o
.OBJ.libpcre.a/pcre_globals.o: pcre_globals.c  pcre_globals.c pcre_internal.h pcre.h ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_globals.c       -o .OBJ.libpcre.a/pcre_globals.o
.OBJ.libpcre.a/pcre_newline.o: pcre_newline.c  pcre_newline.c pcre_internal.h pcre.h ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_newline.c       -o .OBJ.libpcre.a/pcre_newline.o
.OBJ.libpcre.a/pcre_tables.o: pcre_tables.c  pcre_tables.c pcre_internal.h pcre.h ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_tables.c        -o .OBJ.libpcre.a/pcre_tables.o
.OBJ.libpcre.a/pcre_try_flipped.o: pcre_try_flipped.c  pcre_try_flipped.c pcre_internal.h pcre.h ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_try_flipped.c   -o .OBJ.libpcre.a/pcre_try_flipped.o


mm_update:
	


### MAKEMAKE ENDS HERE #########################################################

