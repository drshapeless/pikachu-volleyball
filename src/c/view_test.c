#include "view.h"

int main(int argc, char *argv[])
{
	struct PikaView *v = NewPikaView();
	while (1)
		DrawMenu(v, 0);
	return 0;
}
