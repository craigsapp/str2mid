
PROGRAM = str2mid

PREFLAGS=-O3 -Wall
PREFLAGS += -Imidifile/include
PREFLAGS += -Ihumlib/include
PREFLAGS += -std=c++11

POSTFLAGS  = -Lmidifile/lib -lmidifile
POSTFLAGS += -Lhumlib/lib -lhumlib -lpugixml

.PHONY: midifile humlib

all: humlib midifile mkbin program

humlib:
	(cd humlib && make library)
	(cd humlib && make pugixml)

midifile:
	(cd midifile && make library);

mkbin:
	mkdir -p bin

program:
	g++ $(PREFLAGS) -o bin/$(PROGRAM) src/$(PROGRAM).cpp $(POSTFLAGS)

install:
	cp bin/$(PROGRAM) /usr/local/bin/$(PROGRAM)

update:
	git submodule update --init --recursive
	git pull

