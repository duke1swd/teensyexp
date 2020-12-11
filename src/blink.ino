
/*
 * Arduion equivalent of hello world.
 */
int state;

void
setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	state = 0;
}

void
loop() {
	state = 1 - state;
	if (state) {
		digitalWrite(LED_BUILTIN, HIGH);
	} else {
		digitalWrite(LED_BUILTIN, LOW);
	}
	delay(500);
}
