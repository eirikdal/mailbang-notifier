TARGET   = mailbang-notifier

CC       = gcc
CFLAGS   = -std=c99 -Wall -I. $(shell pkg-config --cflags libnotify gmime-2.6)
LINKER   = gcc -o 
LFLAGS   = -Wall -I. -lm $(shell pkg-config --libs libnotify gmime-2.6)

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/inc/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
mk		 = mkdir -p 
rm       = rm -rf 

${OBJDIR}:
	@$(mk) $(OBJDIR)

${BINDIR}:
	@$(mk) $(BINDIR)

$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONEY: clean
clean:
	@$(rm) $(OBJECTS) $(OBJDIR) $(BINDIR)

check-syntax:
	@$(CC) $(CLFAGS) -o nul -S ${CHK_SOURCES}

.PHONEY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)

