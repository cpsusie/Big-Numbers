CPP = g++
CPPFLAGS = -DNDEBUG -Wall -pipe -O3 -fno-exceptions -fomit-frame-pointer -fno-rtti

BIN = GreKo.exe

OBJ = bitboards.o \
      book.o      \
      commands.o  \
      config.o    \
      eval.o      \
      main.o      \
      moves.o     \
      notation.o  \
      position.o  \
      search.o    \
      utils.o     \
      win32.o

LINKOBJ = $(OBJ)

.PHONY: all clean

all: $(BIN)

clean:
	rm -f $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN)

.o: .cpp 
	$(CPP) $(CPPFLAGS) -c $< -o $@
