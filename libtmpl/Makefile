CC=@echo "Compiling $<"; gcc

C_SRCS=context.c      \
       varlist.c      \
       nclist.c       \
       staglist.c     \
       tagplist.c     \
       template.c     \
       parser.c       \
       default_tags.c

C_OBJS=${C_SRCS:.c=.o}

HDRS=context.h      \
     default_tags.h \
     nclist.h       \
     staglist.h     \
     tagplist.h     \
     template.h     \
     varlist.h

LIB_HDRS=${HDRS}

MODULE=libtmpl.a

CFLAGS=-O3 -Wall -pedantic -I. -g

all: ${MODULE}

${MODULE}: ${C_OBJS}
	@echo "Building library ${MODULE}"
	@rm -f ${MODULE}
	@ar cr ${MODULE} ${C_OBJS}
	@ranlib ${MODULE}

clean:
	@echo "Cleaning ${MODULE}"
	@rm -f ${C_OBJS} ${MODULE}
