#include <iostream>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

int main() {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	printf("nano second is: %09ld\n", ts.tv_nsec);
	int result = ts.tv_nsec % 200;
	printf("the lucky person is: %d\n", result);
	return result;
}
