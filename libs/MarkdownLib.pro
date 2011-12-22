TARGET   = markdown
TEMPLATE = lib
CONFIG   = staticlib
HEADERS  = amalloc.h     \
           config.h      \
           cstring.h     \
           markdowncxx.h \
           markdown.h

SOURCES  = docheader.c \   
           dumptree.c  \
           generate.c  \
           markdown.c  \
           mkdio.c     \ 
           resource.c  \
           version.c   \
           markdowncxx.cpp

MOC_DIR     = .obj
OBJECTS_DIR = .obj
