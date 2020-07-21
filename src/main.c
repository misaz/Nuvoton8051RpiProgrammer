#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "main.h"
#include "Nuvoton8051.h"
#include "Nuvoton8051PlatformSpecific.h"

int main(int argc, char** argv) {
	if (argc < 2) {
		printUsage();
		return 1;
	}

	Nuvoton8051_Init();
	int result = processCommand(argc, argv);
	Nuvoton8051_RaspberryPiUninit();

	return result;
}

int processCommand(int argc, char** argv) {
	if (strcmp(argv[1], "ld") == 0) {
		return processLdCommand(argc, argv);
	} else if (strcmp(argv[1], "dinfo") == 0) {
		return processDinfoCommand(argc, argv);
	} else if (strcmp(argv[1], "cbytes") == 0) {
		return processCbytesCommand(argc, argv);
	} else if (strcmp(argv[1], "help") == 0) {
		printUsage();
		return 0;
	} else {
		printUsage();
		return 1;
	}
}

int processLdCommand(int argc, char** argv) {
	if (argc < 3) {
		printLdUsage();
		return 1;
	}

	if (strcmp(argv[2], "help") == 0) {
		printLdUsage();
		return 0;
	} else if (strcmp(argv[2], "read") == 0) {
		return processLdReadCommand(argc, argv);
	} else if (strcmp(argv[2], "write") == 0) {
		return processLdWriteCommand(argc, argv);
	} else if (strcmp(argv[2], "verify") == 0) {
		return processLdVerifyCommand(argc, argv);
	} else {
		printLdUsage();
		return 1;
	}
}

int processLdReadCommand(int argc, char** argv) {
	int offset;
	int size;
	int sscanfTest;
	uint8_t* buffer;

	if (argc != 6) {
		printLdUsage();
		return 1;
	}

	if (sscanf(argv[3], "%d%n", &offset, &sscanfTest) != 1 || offset < 0 || sscanfTest != strlen(argv[3])) {
		printf("Invalid offset specified.\n\n");
		printLdUsage();
		return 1;
	}

	if (sscanf(argv[4], "%d%n", &size, &sscanfTest) != 1 || size < 0 || sscanfTest != strlen(argv[4])) {
		printf("Invalid size specified.\n\n");
		printLdUsage();
		return 1;
	}

	buffer = (uint8_t*)malloc(size);
	if (buffer == NULL) {
		printf("Out of memory.\n\n");
		return 1;
	}

	FILE* output = fopen(argv[5], "wb");
	if (output == NULL) {
		printf("I/O error while opening output file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	Nuvoton8051_EntryMode();

	if (Nuvoton8051_ReadFlash(offset, size, buffer) != offset + size) {
		printf("Error while reading LDROM.\n");

		Nuvoton8051_ExitMode();
		return 1;
	}

	Nuvoton8051_ExitMode();

	// write firmware to output file
	if (fwrite(buffer, 1, size, output) != size) {
		printf("I/O error while writing output file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	// close file
	if (fclose(output) != 0) {
		printf("I/O error while closing output file.\n");
		return 1;
	}

	return 0;
}

int processLdWriteCommand(int argc, char** argv) {
	if (argc != 4) {
		printLdUsage();
		return 1;
	}

	// open firmware file
	FILE* input = fopen(argv[3], "rb");
	if (input == NULL) {
		printf("I/O error while opening input file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	// seek to end to determine file size
	if (fseek(input, 0, SEEK_END) != 0) {
		printf("I/O error while seeking input file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	// load length from current position
	long int length;
	if ((length = ftell(input)) == -1L) {
		printf("I/O error while telling position in input file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	// seek back to start of file
	if (fseek(input, 0, SEEK_SET) != 0) {
		printf("I/O error while seeking input file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	// allocated buffer for firmware
	uint8_t* buffer = (uint8_t*)malloc(length);
	if (buffer == NULL) {
		printf("Out of memory.\n");
		return 1;
	}

	// read firmware from file to buffer
	if (fread(buffer, 1, length, input) != length) {
		printf("I/O error while reading input file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	// close file
	if (fclose(input) != 0) {
		printf("I/O error while closing input file.\n");
		return 1;
	}


	Nuvoton8051_EntryMode();

	Nuvoton8051_MassErase();
	if (Nuvoton8051_ProgramFlash(0, length, buffer, 1) != length) {
		printf("Error while programming device flash.\n");

		Nuvoton8051_ExitMode();
		return 1;
	}

	Nuvoton8051_ExitMode();
	return 0;
}

int processLdVerifyCommand(int argc, char** argv) {
	if (argc != 4) {
		printLdUsage();
		return 1;
	}

	// open firmware file
	FILE* input = fopen(argv[3], "rb");
	if (input == NULL) {
		printf("I/O error while opening input file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	// seek to end to determine file size
	if (fseek(input, 0, SEEK_END) != 0) {
		printf("I/O error while seeking input file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	// load length from current position
	long int length;
	if ((length = ftell(input)) == -1L) {
		printf("I/O error while telling position in input file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	// seek back to start of file
	if (fseek(input, 0, SEEK_SET) != 0) {
		printf("I/O error while seeking input file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	// allocated buffer for firmware
	uint8_t* fileBuffer = (uint8_t*)malloc(length);
	uint8_t* mcuBuffer = (uint8_t*)malloc(length);
	if (fileBuffer == NULL) {
		printf("Out of memory.\n");
		return 1;
	}

	// read firmware from file to buffer
	if (fread(fileBuffer, 1, length, input) != length) {
		printf("I/O error while reading input file. Error %d (%s).\n", errno, strerror(errno));
		return 1;
	}

	// close file
	if (fclose(input) != 0) {
		printf("I/O error while closing input file.\n");
		return 1;
	}

	Nuvoton8051_EntryMode();

	if (Nuvoton8051_ReadFlash(0, length, mcuBuffer) != length) {
		printf("Error while reading firmware from device.\n");

		Nuvoton8051_ExitMode();
		return 1;
	}

	Nuvoton8051_ExitMode();

	for (long i = 0; i < length; i++) {
		if (fileBuffer[i] != mcuBuffer[i]) {
			printf("Verify failed at offset 0x%lX.\n", i);
			return 1;
		}
	}

	printf("Verify OK.\n");
	return 0;
}

int processDinfoCommand(int argc, char** argv) {
	if (argc < 3) {
		printDinfoUsage();
		return 1;
	}

	if (strcmp(argv[2], "help") == 0) {
		printDinfoUsage();
		return 0;
	} else if (strcmp(argv[2], "cid") == 0) {
		return processDinfoCidCommand(argc, argv);
	} else if (strcmp(argv[2], "pid") == 0) {
		return processDinfoPidCommand(argc, argv);
	} else if (strcmp(argv[2], "did") == 0) {
		return processDinfoDidCommand(argc, argv);
	} else {
		printDinfoUsage();
		return 1;
	}
}

int processDinfoCidCommand(int argc, char** argv) {
	if (argc == 3) {
		return readCid(1);
	} else if (argc == 4 && strcmp(argv[3], "odecimal") == 0) {
		return readCid(0);
	} else {
		printDinfoUsage();
		return 1;
	}
}

int processDinfoDidCommand(int argc, char** argv) {
	if (argc == 3) {
		return readDid(1);
	} else if (argc == 4 && strcmp(argv[3], "odecimal") == 0) {
		return readDid(0);
	} else {
		printDinfoUsage();
		return 1;
	}
}

int processDinfoPidCommand(int argc, char** argv) {
	if (argc == 3) {
		return readPid(1);
	} else if (argc == 4 && strcmp(argv[3], "odecimal") == 0) {
		return readPid(0);
	} else {
		printDinfoUsage();
		return 1;
	}
}

int processCbytesCommand(int argc, char** argv) {
	if (argc < 3) {
		printCbytesUsage();
		return 1;
	}

	if (strcmp(argv[2], "help") == 0) {
		printCbytesUsage();
		return 0;
	} else if (strcmp(argv[2], "read") == 0) {
		return processCbytesReadCommand(argc, argv);
	} else if (strcmp(argv[2], "write") == 0) {
		return processCbytesWriteCommand(argc, argv);
	} else {
		printCbytesUsage();
		return 1;
	}
}

int processCbytesReadCommand(int argc, char** argv) {
	if (argc == 3) {
		return readConfigBytes(1);
	} else if (argc == 4 && strcmp(argv[3], "odecimal") == 0) {
		return readConfigBytes(0);
	} else {
		printCbytesUsage();
		return 1;
	}
}

int processCbytesWriteCommand(int argc, char** argv) {
	uint8_t bytes[5];
	int sscanfTest;
	int iValueRead;
	unsigned int uValueRead;

	if (argc == 8) {
		for (int i = 0; i < 5; i++) {
			if (sscanf(argv[i + 3], "%x%n", &uValueRead, &sscanfTest) != 1 || uValueRead > 255 || sscanfTest != strlen(argv[i + 3])) {
				printf("Invalid config byte specified.\n\n");
				printLdUsage();
				return 1;
			}

			bytes[i] = (uint8_t)uValueRead;
		}

		return writeConfigBytes(bytes);
	} else if (argc == 9 && strcmp(argv[3], "idecimal") == 0) {
		for (int i = 0; i < 5; i++) {
			if (sscanf(argv[i + 4], "%d%n", &iValueRead, &sscanfTest) != 1 || iValueRead < 0 || iValueRead > 255 || sscanfTest != strlen(argv[i + 4])) {
				printf("Invalid config byte specified.\n\n");
				printLdUsage();
				return 1;
			}

			bytes[i] = (uint8_t)iValueRead;
		}

		return writeConfigBytes(bytes);
	} else {
		printCbytesUsage();
		return 1;
	}
}

void printUsage() {
	printf("Usage: nu8051 help\n");
	printf("Usage: nu8051 ld help\n");
	printf("Usage: nu8051 ld read {offset} {size} {file}\n");
	printf("Usage: nu8051 ld write {file}\n");
	printf("Usage: nu8051 ld verify {file}\n");
	printf("Usage: nu8051 dinfo help\n");
	printf("Usage: nu8051 dinfo cid\n");
	printf("Usage: nu8051 dinfo cid odecimal\n");
	printf("Usage: nu8051 dinfo did\n");
	printf("Usage: nu8051 dinfo did odecimal\n");
	printf("Usage: nu8051 dinfo pid\n");
	printf("Usage: nu8051 dinfo pid odecimal\n");
	printf("Usage: nu8051 cbytes help\n");
	printf("Usage: nu8051 cbytes read\n");
	printf("Usage: nu8051 cbytes read odecimal\n");
	printf("Usage: nu8051 cbytes write {cbyte0} {cbyte1} {cbyte2} {cbyte3} {cbyte4}\n");
	printf("Usage: nu8051 cbytes write idecimal {cbyte0} {cbyte1} {cbyte2} {cbyte3} {cbyte4}\n");
}

void printLdUsage() {
	printf("nu8051 ld command help:\n");
	printf("  read {offset} {size} {file}   reads LDROM of connected MCU starting at {offset} with size {size} to the output file {file}\n");
	printf("  write {file}                  writes LDROM of connected MCU starting by specified {file}\n");
	printf("  verify {file}                 verifies content of LDROM of connected MCU starting against specified {file}\n");
	printf("  help                          Prints this help\n");
}

void printDinfoUsage() {
	printf("nu8051 dinfo command help:\n");
	printf("  cid            reads CID of connected device and prints it in hexadecimal.\n");
	printf("  cid odecimal   reads CID of connected device and prints it in decimal.\n");
	printf("  did            reads DID of connected device and prints it in hexadecimal.\n");
	printf("  did odecimal   reads DID of connected device and prints it in decimal.\n");
	printf("  pid            reads PID of connected device and prints it in hexadecimal.\n");
	printf("  pid odecimal   reads PID of connected device and prints it in decimal.\n");
	printf("  help           Prints this help\n");
}

void printCbytesUsage() {
	printf("nu8051 cbytes command help:\n");
	printf("  read            reads config bytes of device and prints it in hexadecimal.\n");
	printf("  read odecimal   reads config bytes of device and prints it in decimal.\n");
	printf("  write {cbyte0} {cbyte1} {cbyte2} {cbyte3} {cbyte4}           writes specified config bytes as hexadecimal to the device.\n");
	printf("  write idecimal {cbyte0} {cbyte1} {cbyte2} {cbyte3} {cbyte4}  writes specified config bytes as decimal to the device.\n");
	printf("  help            Prints this help\n");
}

int readConfigBytes(int isOutputHexadecimal) {
	uint8_t buffer[5];

	Nuvoton8051_EntryMode();

	if (Nuvoton8051_ReadCFG(buffer) != 0x30005) {
		printf("Error while reading config bytes from device.\n");

		Nuvoton8051_ExitMode();
		return 1;
	}

	for (int i = 0; i < 5; i++) {
		if (i != 0) {
			printf(" ");
		}

		if (isOutputHexadecimal) {
			printf("%02x", buffer[i]);
		} else {
			printf("%d", buffer[i]);
		}
	}
	printf("\n");

	Nuvoton8051_ExitMode();
	return 0;
}

int writeConfigBytes(uint8_t* configBytes) {
	Nuvoton8051_EntryMode();

	Nuvoton8051_EraseCFG();

	if (Nuvoton8051_ProgramCFG(configBytes) != 0x30005) {
		printf("Error while programming config bytes.\n");

		Nuvoton8051_ExitMode();
		return 1;
	}

	Nuvoton8051_ExitMode();
	return 0;
}

int readCid(int isOutputHexadecimal) {
	Nuvoton8051_EntryMode();
	uint8_t cid = Nuvoton8051_ReadCID();
	Nuvoton8051_ExitMode();

	if (isOutputHexadecimal) {
		printf("%02x\n", cid);
	} else {
		printf("%d\n", cid);
	}

	return 0;
}

int readDid(int isOutputHexadecimal) {
	Nuvoton8051_EntryMode();
	uint16_t did = Nuvoton8051_ReadDID();
	Nuvoton8051_ExitMode();

	if (isOutputHexadecimal) {
		printf("%04x\n", did);
	} else {
		printf("%d\n", did);
	}

	return 0;
}

int readPid(int isOutputHexadecimal) {
	Nuvoton8051_EntryMode();
	uint16_t pid = Nuvoton8051_ReadPID();
	Nuvoton8051_ExitMode();

	if (isOutputHexadecimal) {
		printf("%04x\n", pid);
	} else {
		printf("%d\n", pid);
	}

	return 0;
}