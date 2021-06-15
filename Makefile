# Makefile

CCFLAGS += -Wall

BIN = rtl_iw_priv
SRC =$(wildcard *.c)
OBJ = $(SRC:.c=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f $(OBJ) $(BIN)
