#ifndef WASM_H
#define WASM_H

#include "pikavolley.h"

struct Args {
	struct PikaVolley *v;
	int running;
};

void StartWasmPikaVolley(struct PikaVolley *v);

#endif /* WASM_H */
