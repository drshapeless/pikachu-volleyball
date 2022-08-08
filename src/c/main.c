#include "pikavolley.h"

#include <time.h>

int main(int argc, char *argv[])
{
	srand(time(NULL));
	struct PikaVolley *v = NewPikaVolley();
	StartPikaVolley(v);

	return 0;
}
