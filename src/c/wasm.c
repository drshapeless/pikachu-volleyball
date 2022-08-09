#include "wasm.h"
#include "pikavolley.h"
#include "config.h"

#include <emscripten.h>

void wasmLoop(void *args_)
{
	struct Args *args = (struct Args *)args_;

	loop(args->v, &args->running);

	if (args->running == 0) {
		emscripten_cancel_main_loop();
	}
}

void StartWasmPikaVolley(struct PikaVolley *v)
{
	int running = 1;
	struct Args args;
	args.v = v;
	args.running = running;

	emscripten_set_main_loop_arg(wasmLoop, &args, FPS, 1);
}
