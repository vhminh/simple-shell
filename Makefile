.PHONY: all, build, clean

TARGET=simple-shell

HEADERDIR=./src
HEADERS=func.h

SRCDIR=./src
SOURCES=history.c parse.c execute.c main.c

OBJDIR=./build
OBJS=$(patsubst %.c, $(OBJDIR)/%.o, $(SOURCES))

CC=gcc

all: build

build: $(TARGET)

$(TARGET): $(OBJS)
	@echo " [LINK] $@"
	@mkdir -p $(shell dirname $@)
	@$(CC) $(OBJS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERDIR)/$(HEADERS)
	@echo " [CC] $@"
	@mkdir -p $(shell dirname $@)
	@$(CC) -c $< -o $@

clean:
	rm -rf $(OBJDIR)/*.o
	rm -rf $(TARGET)

