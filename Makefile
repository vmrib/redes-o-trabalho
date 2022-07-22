CC              = gcc
CFLAGS          = -Wall -std=gnu17 -I$(INCLUDEDIR) -DNDEBUG
CFLAGS_DEBUG    = $(CFLAGS) -g
LDFLAGS			= -L$(LIBDIR)

BUILDDIR        = build
INCLUDEDIR      = include
LIBDIR          = lib
SOURCEDIR       = source
TESTSDIR        = tests

SOURCES         := $(shell find $(SOURCEDIR) -name "*.c")
OBJECTS         := $(patsubst %.c, $(BUILDDIR)/%.o, $(notdir $(SOURCES)))
TARGETS         := client server

.PHONY: all debug run clean test

all: $(TARGETS)

debug: CFLAGS := $(CFLAGS_DEBUG)
debug: $(TARGETS)

run: $(TARGETS)
	./$(TARGETS)

clean:
	rm -rf $(BUILDDIR) $(TARGETS)

$(TARGETS): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR):
	mkdir -p $(BUILDDIR)