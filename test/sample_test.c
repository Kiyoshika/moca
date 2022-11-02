#include <assert.h>

int main()
{
	int x = 4;
	int y = 4;
	
	assert(x == y);
	if (x == y)
		return 0;

	return -1;
}
