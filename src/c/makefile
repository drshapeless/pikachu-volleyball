CC = clang
CCFLAGS = $(INCLUDE)
INCLUDE =
LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer
OBJS = main.o pikavolley.o view.o sprite.o game.o pikachu.o	\
pokeball.o userinput.o sound.o keyboard.o wave.o cloud.o

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

.PHONY: clean
clean:
	rm $(OBJS) pikachu-volleyball
