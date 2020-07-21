#ifndef __MAIN
#define __MAIN

int processCommand(int argc, char** argv);

int processLdCommand(int argc, char** argv);
int processLdReadCommand(int argc, char** argv);
int processLdWriteCommand(int argc, char** argv);
int processLdVerifyCommand(int argc, char** argv);

int processDinfoCommand(int argc, char** argv);
int processDinfoCidCommand(int argc, char** argv);
int processDinfoDidCommand(int argc, char** argv);
int processDinfoPidCommand(int argc, char** argv);

int processCbytesCommand(int argc, char** argv);
int processCbytesReadCommand(int argc, char** argv);
int processCbytesWriteCommand(int argc, char** argv);

void printUsage();
void printLdUsage();
void printDinfoUsage();
void printCbytesUsage();

int readConfigBytes(int isOutputHexadecimal);
int writeConfigBytes(uint8_t* configBytes);
int readCid(int isOutputHexadecimal);
int readDid(int isOutputHexadecimal);
int readPid(int isOutputHexadecimal);

#endif
