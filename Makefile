CC      := gcc
CFLAGS  := -std=c99 -O3 -Wall

ARCHIVE := compiler.a
PARSER  := min-bnf-parser/min-bnf-parser.a
REGEX   := min-bnf-parser/min-regex/min-regex.a
TARGET  := compiler.out

ALL_CH  := $(wildcard src/*.c include/*.h)
ALL_O   := $(patsubst src/%.c,object/%.o,$(wildcard src/*.c))
ALL_CH_WITHOUT_MAIN := $(filter-out src/main.c,$(ALL_CH))
ALL_O_WITHOUT_MAIN  := $(filter-out object/main.o,$(ALL_O))

ALL_REGEX_CH  := $(filter-out min-bnf-parser/min-regex/src/main.c,$(wildcard min-bnf-parser/min-regex/src/*.c min-bnf-parser/min-regex/include/*.h))
ALL_PARSER_CH := $(filter-out min-bnf-parser/src/main.c,$(wildcard min-bnf-parser/src/*.c min-bnf-parser/include/*.h))

dummy: $(TARGET)

$(ALL_O_WITHOUT_MAIN): $(ALL_CH_WITHOUT_MAIN)
	cd object && $(MAKE) $(patsubst object/%.o,%.o,$@) "CC=$(CC)" "CFLAGS=$(CFLAGS)"

$(ARCHIVE): $(ALL_O_WITHOUT_MAIN)
	ar -r $@ $^

$(PARSER): $(ALL_PARSER_CH)
	cd min-bnf-parser && $(MAKE) $(patsubst min-bnf-parser/%.a,%.a,$@) "CC=$(CC)" "CFLAGS=$(CFLAGS)"

$(REGEX):  $(ALL_REGEX_CH)
	cd min-bnf-parser/min-regex && $(MAKE) $(patsubst min-bnf-parser/min-regex/%.a,%.a,$@) "CC=$(CC)" "CFLAGS=$(CFLAGS)"

$(TARGET): $(ARCHIVE) $(PARSER) $(REGEX) src/main.c
	cd object && $(MAKE) main.o "CC=$(CC)" "CFLAGS=$(CFLAGS)"
	$(CC) $(CFLAGS) object/main.o $(ARCHIVE) $(PARSER) $(REGEX) -o $@

.PHONY: clean
clean:
	rm -rf *.a *.out *.stackdump *.dot *.png *.asm table.txt token_list.txt GPATH GRTAGS GTAGS
	rm -rf script/*.o script/*.asm
	cd object && rm -rf *.o *.d
	cd min-bnf-parser && $(MAKE) clean
