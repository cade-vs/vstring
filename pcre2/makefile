
### MAKEMAKE STARTS HERE #######################################################


### Created by makemake.pl on Fri Nov 19 01:51:13 2021 #########################


### GLOBAL TARGETS #############################################################

default: mm_update all

re: mm_update rebuild

li: mm_update link

all: mm_update libpcre2.a 

clean: mm_update clean-libpcre2.a 

rebuild: mm_update rebuild-libpcre2.a 

link: mm_update link-libpcre2.a 

### GLOBAL (AND USER) DEFS #####################################################


AR = ar rv
CC = gcc
LD = gcc
MKDIR = mkdir -p
RANLIB = ranlib
RMDIR = rm -rf
RMFILE = rm -f
SRC = *.c *.cpp *.cc *.cxx


### TARGET 1: libpcre2.a #######################################################

CC_1       = gcc
LD_1       = gcc
AR_1       = ar rv
RANLIB_1   = ranlib
CCFLAGS_1  = -DHAVE_CONFIG_H -fPIC -I. -O2 $(CCDEF)  
LDFLAGS_1  = $(LDDEF) 
DEPFLAGS_1 = 
ARFLAGS_1  = 
TARGET_1   = libpcre2.a

### SOURCES FOR TARGET 1: libpcre2.a ###########################################

SRC_1= \
     pcre2_auto_possess.c \
     pcre2_chartables.c \
     pcre2_compile.c \
     pcre2_config.c \
     pcre2_context.c \
     pcre2_convert.c \
     pcre2_dfa_match.c \
     pcre2_error.c \
     pcre2_extuni.c \
     pcre2_find_bracket.c \
     pcre2_jit_compile.c \
     pcre2_maketables.c \
     pcre2_match.c \
     pcre2_match_data.c \
     pcre2_newline.c \
     pcre2_ord2utf.c \
     pcre2_pattern_info.c \
     pcre2_script_run.c \
     pcre2_serialize.c \
     pcre2_string_utils.c \
     pcre2_study.c \
     pcre2_substitute.c \
     pcre2_substring.c \
     pcre2_tables.c \
     pcre2_ucd.c \
     pcre2_valid_utf.c \
     pcre2_xclass.c \

#### OBJECTS FOR TARGET 1: libpcre2.a ##########################################

OBJ_1= \
     .OBJ.libpcre2.a/pcre2_auto_possess.o \
     .OBJ.libpcre2.a/pcre2_chartables.o \
     .OBJ.libpcre2.a/pcre2_compile.o \
     .OBJ.libpcre2.a/pcre2_config.o \
     .OBJ.libpcre2.a/pcre2_context.o \
     .OBJ.libpcre2.a/pcre2_convert.o \
     .OBJ.libpcre2.a/pcre2_dfa_match.o \
     .OBJ.libpcre2.a/pcre2_error.o \
     .OBJ.libpcre2.a/pcre2_extuni.o \
     .OBJ.libpcre2.a/pcre2_find_bracket.o \
     .OBJ.libpcre2.a/pcre2_jit_compile.o \
     .OBJ.libpcre2.a/pcre2_maketables.o \
     .OBJ.libpcre2.a/pcre2_match.o \
     .OBJ.libpcre2.a/pcre2_match_data.o \
     .OBJ.libpcre2.a/pcre2_newline.o \
     .OBJ.libpcre2.a/pcre2_ord2utf.o \
     .OBJ.libpcre2.a/pcre2_pattern_info.o \
     .OBJ.libpcre2.a/pcre2_script_run.o \
     .OBJ.libpcre2.a/pcre2_serialize.o \
     .OBJ.libpcre2.a/pcre2_string_utils.o \
     .OBJ.libpcre2.a/pcre2_study.o \
     .OBJ.libpcre2.a/pcre2_substitute.o \
     .OBJ.libpcre2.a/pcre2_substring.o \
     .OBJ.libpcre2.a/pcre2_tables.o \
     .OBJ.libpcre2.a/pcre2_ucd.o \
     .OBJ.libpcre2.a/pcre2_valid_utf.o \
     .OBJ.libpcre2.a/pcre2_xclass.o \

### TARGET DEFINITION FOR TARGET 1: libpcre2.a #################################

.OBJ.libpcre2.a: 
	$(MKDIR) .OBJ.libpcre2.a

libpcre2.a:   .OBJ.libpcre2.a $(OBJ_1)
	$(AR_1) $(ARFLAGS_1) $(TARGET_1) $(OBJ_1)
	$(RANLIB_1) $(TARGET_1)

clean-libpcre2.a: 
	$(RMFILE) $(TARGET_1)
	$(RMDIR) .OBJ.libpcre2.a

rebuild-libpcre2.a: clean-libpcre2.a libpcre2.a

re-libpcre2.a: rebuild-libpcre2.a

link-libpcre2.a: .OBJ.libpcre2.a $(OBJ_1)
	$(RMFILE) libpcre2.a
	$(AR_1) $(ARFLAGS_1) $(TARGET_1) $(OBJ_1)
	$(RANLIB_1) $(TARGET_1)


### TARGET OBJECTS FOR TARGET 1: libpcre2.a ####################################

.OBJ.libpcre2.a/pcre2_auto_possess.o: pcre2_auto_possess.c  pcre2_auto_possess.c pcre2_internal.h pcre2.h \
 pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_auto_possess.c -o .OBJ.libpcre2.a/pcre2_auto_possess.o
.OBJ.libpcre2.a/pcre2_chartables.o: pcre2_chartables.c  pcre2_chartables.c pcre2_internal.h pcre2.h \
 pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_chartables.c   -o .OBJ.libpcre2.a/pcre2_chartables.o
.OBJ.libpcre2.a/pcre2_compile.o: pcre2_compile.c  pcre2_compile.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_compile.c      -o .OBJ.libpcre2.a/pcre2_compile.o
.OBJ.libpcre2.a/pcre2_config.o: pcre2_config.c  pcre2_config.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_config.c       -o .OBJ.libpcre2.a/pcre2_config.o
.OBJ.libpcre2.a/pcre2_context.o: pcre2_context.c  pcre2_context.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_context.c      -o .OBJ.libpcre2.a/pcre2_context.o
.OBJ.libpcre2.a/pcre2_convert.o: pcre2_convert.c  pcre2_convert.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_convert.c      -o .OBJ.libpcre2.a/pcre2_convert.o
.OBJ.libpcre2.a/pcre2_dfa_match.o: pcre2_dfa_match.c  pcre2_dfa_match.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_dfa_match.c    -o .OBJ.libpcre2.a/pcre2_dfa_match.o
.OBJ.libpcre2.a/pcre2_error.o: pcre2_error.c  pcre2_error.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_error.c        -o .OBJ.libpcre2.a/pcre2_error.o
.OBJ.libpcre2.a/pcre2_extuni.o: pcre2_extuni.c  pcre2_extuni.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_extuni.c       -o .OBJ.libpcre2.a/pcre2_extuni.o
.OBJ.libpcre2.a/pcre2_find_bracket.o: pcre2_find_bracket.c  pcre2_find_bracket.c pcre2_internal.h pcre2.h \
 pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_find_bracket.c -o .OBJ.libpcre2.a/pcre2_find_bracket.o
.OBJ.libpcre2.a/pcre2_jit_compile.o: pcre2_jit_compile.c  pcre2_jit_compile.c pcre2_internal.h pcre2.h \
 pcre2_ucp.h pcre2_jit_match.c pcre2_jit_misc.c
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_jit_compile.c  -o .OBJ.libpcre2.a/pcre2_jit_compile.o
.OBJ.libpcre2.a/pcre2_maketables.o: pcre2_maketables.c  pcre2_maketables.c pcre2_internal.h pcre2.h \
 pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_maketables.c   -o .OBJ.libpcre2.a/pcre2_maketables.o
.OBJ.libpcre2.a/pcre2_match.o: pcre2_match.c  pcre2_match.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_match.c        -o .OBJ.libpcre2.a/pcre2_match.o
.OBJ.libpcre2.a/pcre2_match_data.o: pcre2_match_data.c  pcre2_match_data.c pcre2_internal.h pcre2.h \
 pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_match_data.c   -o .OBJ.libpcre2.a/pcre2_match_data.o
.OBJ.libpcre2.a/pcre2_newline.o: pcre2_newline.c  pcre2_newline.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_newline.c      -o .OBJ.libpcre2.a/pcre2_newline.o
.OBJ.libpcre2.a/pcre2_ord2utf.o: pcre2_ord2utf.c  pcre2_ord2utf.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_ord2utf.c      -o .OBJ.libpcre2.a/pcre2_ord2utf.o
.OBJ.libpcre2.a/pcre2_pattern_info.o: pcre2_pattern_info.c  pcre2_pattern_info.c pcre2_internal.h pcre2.h \
 pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_pattern_info.c -o .OBJ.libpcre2.a/pcre2_pattern_info.o
.OBJ.libpcre2.a/pcre2_script_run.o: pcre2_script_run.c  pcre2_script_run.c pcre2_internal.h pcre2.h \
 pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_script_run.c   -o .OBJ.libpcre2.a/pcre2_script_run.o
.OBJ.libpcre2.a/pcre2_serialize.o: pcre2_serialize.c  pcre2_serialize.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_serialize.c    -o .OBJ.libpcre2.a/pcre2_serialize.o
.OBJ.libpcre2.a/pcre2_string_utils.o: pcre2_string_utils.c  pcre2_string_utils.c pcre2_internal.h pcre2.h \
 pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_string_utils.c -o .OBJ.libpcre2.a/pcre2_string_utils.o
.OBJ.libpcre2.a/pcre2_study.o: pcre2_study.c  pcre2_study.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_study.c        -o .OBJ.libpcre2.a/pcre2_study.o
.OBJ.libpcre2.a/pcre2_substitute.o: pcre2_substitute.c  pcre2_substitute.c pcre2_internal.h pcre2.h \
 pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_substitute.c   -o .OBJ.libpcre2.a/pcre2_substitute.o
.OBJ.libpcre2.a/pcre2_substring.o: pcre2_substring.c  pcre2_substring.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_substring.c    -o .OBJ.libpcre2.a/pcre2_substring.o
.OBJ.libpcre2.a/pcre2_tables.o: pcre2_tables.c  pcre2_tables.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_tables.c       -o .OBJ.libpcre2.a/pcre2_tables.o
.OBJ.libpcre2.a/pcre2_ucd.o: pcre2_ucd.c  pcre2_ucd.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_ucd.c          -o .OBJ.libpcre2.a/pcre2_ucd.o
.OBJ.libpcre2.a/pcre2_valid_utf.o: pcre2_valid_utf.c  pcre2_valid_utf.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_valid_utf.c    -o .OBJ.libpcre2.a/pcre2_valid_utf.o
.OBJ.libpcre2.a/pcre2_xclass.o: pcre2_xclass.c  pcre2_xclass.c pcre2_internal.h pcre2.h pcre2_ucp.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c pcre2_xclass.c       -o .OBJ.libpcre2.a/pcre2_xclass.o


mm_update:
	


### MAKEMAKE ENDS HERE #########################################################

