#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void waiting(int leds);
void toBinaryLeds(int i, int leds);

int main() {
	int leds = open("/dev/leds", O_WRONLY | O_CREAT | O_TRUNC );

	waiting(leds);

	int n;
	do {
		printf("Introduce un numero del 0 al 7:\n");
		scanf("%d", &n);

		if (n < 0 || n > 7) printf("Error. Terminando programa.\n");
		else {
			waiting(leds);
			toBinaryLeds(n, leds);
		}

	} while(n >= 0 && n <= 7);
}

void waiting(int leds) {
	for (int i = 0; i < 3; i++) {
		toBinaryLeds(0, leds);
		usleep(100000);
		toBinaryLeds(7, leds);
		usleep(100000);
	}
}

void toBinaryLeds(int i, int leds) {
	char* buff;

	switch(i) {
		case 0: buff = "";
		break;
		case 1: buff = "3";
		break;
		case 2: buff = "2";
		break;
		case 3: buff = "23";
		break;
		case 4: buff = "1";
		break;
		case 5: buff = "13";
		break;
		case 6: buff = "12";
		break;
		case 7: buff = "123";
		break;

		default: 
		printf("Numero erroneo\n");
		break;
	}

	write(leds, buff, strlen(buff));
}
