
### MAKEMAKE STARTS HERE #######################################################


### Created by makemake.pl on Thu Jul 30 22:21:57 2020 #########################


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

CC_1       = $(CC)
LD_1       = $(LD)
AR_1       = $(AR)
RANLIB_1   = $(RANLIB)
CCFLAGS_1  = -I. -O2 $(CCDEF) -DHAVE_CONFIG_H  
LDFLAGS_1  = $(LDDEF) 
DEPFLAGS_1 = 
ARFLAGS_1  = 
TARGET_1   = libpcre.a

### SOURCES FOR TARGET 1: libpcre.a ############################################

SRC_1= \
     dftables.c \
     pcre_byte_order.c \
     pcre_chartables.c \
     pcre_compile.c \
     pcre_config.c \
     pcre_dfa_exec.c \
     pcre_exec.c \
     pcre_fullinfo.c \
     pcre_get.c \
     pcre_globals.c \
     pcre_maketables.c \
     pcre_newline.c \
     pcre_ord2utf8.c \
     pcre_printint.c \
     pcre_refcount.c \
     pcre_string_utils.c \
     pcre_study.c \
     pcre_tables.c \
     pcre_ucd.c \
     pcre_valid_utf8.c \
     pcre_version.c \
     pcre_xclass.c \

#### OBJECTS FOR TARGET 1: libpcre.a ###########################################

OBJ_1= \
     .OBJ.libpcre.a/dftables.o \
     .OBJ.libpcre.a/pcre_byte_order.o \
     .OBJ.libpcre.a/pcre_chartables.o \
     .OBJ.libpcre.a/pcre_compile.o \
     .OBJ.libpcre.a/pcre_config.o \
     .OBJ.libpcre.a/pcre_dfa_exec.o \
     .OBJ.libpcre.a/pcre_exec.o \
     .OBJ.libpcre.a/pcre_fullinfo.o \
     .OBJ.libpcre.a/pcre_get.o \
     .OBJ.libpcre.a/pcre_globals.o \
     .OBJ.libpcre.a/pcre_maketables.o \
     .OBJ.libpcre.a/pcre_newline.o \
     .OBJ.libpcre.a/pcre_ord2utf8.o \
     .OBJ.libpcre.a/pcre_printint.o \
     .OBJ.libpcre.a/pcre_refcount.o \
     .OBJ.libpcre.a/pcre_string_utils.o \
     .OBJ.libpcre.a/pcre_study.o \
     .OBJ.libpcre.a/pcre_tables.o \
     .OBJ.libpcre.a/pcre_ucd.o \
     .OBJ.libpcre.a/pcre_valid_utf8.o \
     .OBJ.libpcre.a/pcre_version.o \
     .OBJ.libpcre.a/pcre_xclass.o \

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

.OBJ.libpcre.a/dftables.o: dftables.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c dftables.c           -o .OBJ.libpcre.a/dftables.o
.OBJ.libpcre.a/pcre_byte_order.o: pcre_byte_order.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_byte_order.c    -o .OBJ.libpcre.a/pcre_byte_order.o
.OBJ.libpcre.a/pcre_chartables.o: pcre_chartables.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_chartables.c    -o .OBJ.libpcre.a/pcre_chartables.o
.OBJ.libpcre.a/pcre_compile.o: pcre_compile.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_compile.c       -o .OBJ.libpcre.a/pcre_compile.o
.OBJ.libpcre.a/pcre_config.o: pcre_config.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_config.c        -o .OBJ.libpcre.a/pcre_config.o
.OBJ.libpcre.a/pcre_dfa_exec.o: pcre_dfa_exec.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_dfa_exec.c      -o .OBJ.libpcre.a/pcre_dfa_exec.o
.OBJ.libpcre.a/pcre_exec.o: pcre_exec.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_exec.c          -o .OBJ.libpcre.a/pcre_exec.o
.OBJ.libpcre.a/pcre_fullinfo.o: pcre_fullinfo.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_fullinfo.c      -o .OBJ.libpcre.a/pcre_fullinfo.o
.OBJ.libpcre.a/pcre_get.o: pcre_get.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_get.c           -o .OBJ.libpcre.a/pcre_get.o
.OBJ.libpcre.a/pcre_globals.o: pcre_globals.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_globals.c       -o .OBJ.libpcre.a/pcre_globals.o
.OBJ.libpcre.a/pcre_maketables.o: pcre_maketables.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_maketables.c    -o .OBJ.libpcre.a/pcre_maketables.o
.OBJ.libpcre.a/pcre_newline.o: pcre_newline.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_newline.c       -o .OBJ.libpcre.a/pcre_newline.o
.OBJ.libpcre.a/pcre_ord2utf8.o: pcre_ord2utf8.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_ord2utf8.c      -o .OBJ.libpcre.a/pcre_ord2utf8.o
.OBJ.libpcre.a/pcre_printint.o: pcre_printint.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_printint.c      -o .OBJ.libpcre.a/pcre_printint.o
.OBJ.libpcre.a/pcre_refcount.o: pcre_refcount.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_refcount.c      -o .OBJ.libpcre.a/pcre_refcount.o
.OBJ.libpcre.a/pcre_string_utils.o: pcre_string_utils.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_string_utils.c  -o .OBJ.libpcre.a/pcre_string_utils.o
.OBJ.libpcre.a/pcre_study.o: pcre_study.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_study.c         -o .OBJ.libpcre.a/pcre_study.o
.OBJ.libpcre.a/pcre_tables.o: pcre_tables.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_tables.c        -o .OBJ.libpcre.a/pcre_tables.o
.OBJ.libpcre.a/pcre_ucd.o: pcre_ucd.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_ucd.c           -o .OBJ.libpcre.a/pcre_ucd.o
.OBJ.libpcre.a/pcre_valid_utf8.o: pcre_valid_utf8.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_valid_utf8.c    -o .OBJ.libpcre.a/pcre_valid_utf8.o
.OBJ.libpcre.a/pcre_version.o: pcre_version.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_version.c       -o .OBJ.libpcre.a/pcre_version.o
.OBJ.libpcre.a/pcre_xclass.o: pcre_xclass.c 
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre_xclass.c        -o .OBJ.libpcre.a/pcre_xclass.o


mm_update:
	


### MAKEMAKE ENDS HERE #########################################################

