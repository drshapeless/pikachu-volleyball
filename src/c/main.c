#include "pikavolley.h"

#include <time.h>

#ifdef __EMSCRIPTEN__
#include "wasm.h"
#endif

int main(int argc, char *argv[])
{
	srand(time(NULL));
	struct PikaVolley *v = NewPikaVolley();

#ifdef __EMSCRIPTEN__
	StartWasmPikaVolley(v);
#else
	StartPikaVolley(v);
#endif

	return 0;
}
