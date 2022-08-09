CC = clang
CCFLAGS = $(INCLUDE)
INCLUDE =
LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer
OBJS = main.o pikavolley.o view.o sprite.o game.o pikachu.o	\
pokeball.o userinput.o sound.o keyboard.o wave.o cloud.o
WASM_SOURCE = main.c pikavolley.c view.c sprite.c game.c pikachu.c	\
pokeball.c userinput.c sound.c keyboard.c wave.c cloud.c wasm.c

pikachu-volleyball: $(OBJS)
	$(CC) $(CCFLAGS) $(LIBS) $(OBJS) -o pikachu-volleyball

main.o: pikavolley.h

pikavolley.o: pikavolley.h view.h config.h sound.h userinput.h keyboard.h

view.o: view.h config.h sprite.h cloud.h wave.h

sprite.o: sprite.h

game.o: game.h pikachu.h pokeball.h config.h

pikachu.o: pikachu.h config.h

pokeball.o: pokeball.h config.h

userinput.o: userinput.h keyboard.h

sound.o: sound.h

keyboard.o: keyboard.h

wave.o: wave.h

cloud.o: cloud.h

wasm: $(WASM_SOURCE)
	emcc $(WASM_SOURCE) -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=1  -s SDL2_IMAGE_FORMATS='["png"]' -o  pikachu-volleyball.html --preload-file assets/ --shell-file template.html

wasm-test: $(WASM_SOURCE)
	emcc $(WASM_SOURCE) -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_SDL_MIXER=1  -s SDL2_IMAGE_FORMATS='["png"]' -o  pikachu-volleyball.html --preload-file assets/

.PHONY: clean wasm-clean
clean:
	rm $(OBJS) pikachu-volleyball

wasm-clean:
	rm pikachu-volleyball.html pikachu-volleyball.js pikachu-volleyball.wasm pikachu-volleyball.data
