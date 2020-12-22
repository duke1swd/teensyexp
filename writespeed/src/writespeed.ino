/*
  SD card test

  Measuring write speed to the SD card.

*/
// include the SD library:
#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
SdFile root;

const int chipSelect = BUILTIN_SDCARD;
#define FILE_NAME "data.dat"

File dataFile;
long startTime;
long delta;

#define	BUFFER_MEMORY	48	// KB
#define	MIN_WRITE_SIZE	512
#define	BUFFER_SIZE	(8*1024)
#define	N_TRAIL		(BUFFER_MEMORY * 1024 / MIN_WRITE_SIZE)
//#define	TEST_TIME	10000ul	// running time in milliseconds == 10 seconds
//#define	TEST_TIME	20000ul	// running time in milliseconds == 20 seconds
#define	TEST_TIME	100000ul	// running time in milliseconds == 100 seconds


char buffer[BUFFER_SIZE];
uint32_t trailing[N_TRAIL];

void setup() {
  short i, j;
  int b;
  int n;
  uint32_t ntrail;
  uint32_t written;
  uint32_t write_start;
  uint32_t write_end;
  uint32_t write_time;
  uint32_t write_max;
  uint32_t multi_write_time;
  uint32_t multi_write_max;
  uint32_t dotPeriod;
  uint32_t dotCounter;

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) ;

  if (!SD.begin(chipSelect)) {
    Serial.println("SD initialization failed.");
    while (1);
  }
  Serial.println("SD Card up");

  // Put some non-zero stuff in the buffer
  for (i = 0; i < BUFFER_SIZE; i++) {
    buffer[i] = ' ';
  }

  for (b = MIN_WRITE_SIZE; b <= BUFFER_SIZE; b *= 2) {

    dotPeriod = 1024 * 1024 / b;
    dotCounter = 0;
    dataFile = SD.open(FILE_NAME, FILE_WRITE);
    if (!dataFile) {
      Serial.println("Cannot open data file for writing.");
      while (1);
    }

    write_max = 0;
    multi_write_max = 0;

    startTime = millis();

    written = 0;
    ntrail = BUFFER_MEMORY * 1024 / b - 1;

    while (millis() - startTime < TEST_TIME) {
      write_start = micros();
      n = dataFile.write(buffer, b);
      write_end = micros();
      if (n != b) {
        Serial.print("write returns ");
        Serial.println(n);
        break;
      }

      // max write time for one buffer;
      write_time = write_end - write_start;
      if (write_time > write_max)
        write_max = write_time;

      // max write time for BUFFER_MEMORY worth of buffers
      j = written % N_TRAIL;
      trailing[j] = write_start;
      if (written >= ntrail) {
	j -= ntrail;
	if (j < 0)
	  j += N_TRAIL;
        multi_write_time = write_end - trailing[j];
	if (multi_write_time > multi_write_max)
	  multi_write_max = multi_write_time;
      }
      written++;
      if (written % dotPeriod == 0) {
	dotCounter++;
        if (dotCounter == 10) {
	  dotCounter = 0;
	  Serial.write(' ');
	} else
      	  Serial.write('0' + dotCounter);
	}
    }
    delta = millis() - startTime;

    if (written >= dotPeriod)
      Serial.println("");


    dataFile.close();
    Serial.print("Wrote ");
    Serial.print(written);
    Serial.print(" buffers in ");
    Serial.print(delta);
    Serial.println(" milliseconds");
    Serial.print("Buffer size is ");
    Serial.println(b);
    Serial.print("Data rate ");
    Serial.print((double)(b * written) /  ((double)delta ));
    Serial.println(" KB/sec");
    Serial.print("Data rate ");
    Serial.print((double)delta / (double) written);
    Serial.println(" millseconds/buffer");
    Serial.print("Max write time: ");
    Serial.print((double)write_max / 1000.);
    Serial.println(" milliseconds");
    Serial.print("N buffers to trail: ");
    Serial.println(ntrail);
    Serial.print("Max N writes: ");
    Serial.print((double)multi_write_max / 1000.);
    Serial.println(" milliseconds");
    Serial.print("Guaranteed throughput: ");
    Serial.print((double)(ntrail * b) * 1000. / (double) multi_write_max);
    Serial.println(" KB/sec");
    Serial.println("");
  }
}

void loop(void) {
}
