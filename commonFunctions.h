#ifndef __commonFunctions__
#define __commonFunctions__


#define CHANGE_DIRECTORY_CODE 0b0
#define LIST_DIRECTORY_CODE 0b1
#define SEE_FILE_SERVER_CODE 0b10
#define SEE_LINE_FILE_SERVER_CODE 0b11
#define SEE_LINES_FILE_SERVER_CODE 0b100
#define EDIT_LINES_FILE_SERVER_CODE 0b101
#define NOP_CODE_1 0b110
#define NOP_CODE_2 0b110
#define NOP_CODE_3 0b1110
#define ACK_CODE 0b1000
#define NACK_CODE 0b1001
#define INITIAL_AND_END_LINE 0b1010
#define CONTENT_LIST_DIRECTORY 0b1011
#define CONTENT_SEE_FILE 0b1100
#define END_TRANSMISSION 0b1101
#define ERROR 0b1111
#define FLAGS 0
#define START_MARKER 0b01111110
#define NOT_RECEIVED_CODE -1
#define SERVIDOR 0b10
#define CLIENTE 0b01
#define NO_SEQUENCE 0
#define NOT_SEND_LINES -1
typedef struct {
    unsigned int start_marker: 8;
    unsigned int size: 4;
    unsigned int sequence: 4;
    unsigned int destination_address: 2;
    unsigned int source_address: 2;
    unsigned char data[15];
    unsigned int type: 4;
    unsigned int parity: 8;
} kermit_protocol_t;


kermit_protocol_t *defineProtocol(    
    int destination_address,
    int source_address,
    int type,
    char *message, 
    int sequence
);

void debug_kermit(kermit_protocol_t *kermit);

void showCurrentPath(char * current_path, int cliente_servidor);

char *getCurrentPath();

void setColorGreen();

void setColorRed();

void setColorDefault();

void setColorBlue();

int getTypeByCommand(char *command);

int isCommandClient(char *command);

int isListDirectoryCommand(int type);

int isChangeDirectoryCommand(int type);

int isVerCommand(int type);

int isLinhaCommand(int type);

int isLinhasCommand(int type);

int isEditCommand(int type);

int thisCommandExists(int type);

int getMessageFromAnotherProcess(int socket, kermit_protocol_t *received_buffer);

int isMessageFromAnotherProcess(int code, kermit_protocol_t *buffer, int source_address);

int getTypeFromKermit(kermit_protocol_t *buffer);

int isAck(int type);

int isNack(int type);

int sendMessageBiggerThenFifteenBits(int socket, int destination_address, int source_address, int type, char *message, int initial_line, int end_line);

int sendMessage(int socket, int destination_address, int source_address, int type, char *message, int sequence);

void sendEndTransmission(int socket, int destination, int source, int sequence);

char *getMessageBiggerThenFifteenBits(int socket, kermit_protocol_t *received_buffer, int *received_code, int client_server);

int getLineFromString(char **string);

char *getFileFromString(char *string);

int countFileLines(char *file);

char *concatInitialLine(char *message, int initial_line);

char  *concatInitialLineAndEndLine(char *message, int initial_line, int end_line);

int sendAckNackCode(int socket, int destination_address, int source_address, int type, int sequence);

int isOnlyInitialLine(int initial_line, int end_line);

int isInitialLineAndEndLine(int initial_line, int end_line);

int receiveMessageFromAnotherProcess(int socket, int expected_type, char **message_from_another_process,  int source_expected, int destination_expected);

int isEndTransmission(kermit_protocol_t *buffer);

int isTypeExpected(kermit_protocol_t *buffer, int type_expected);

int isSourceExpected(kermit_protocol_t *buffer, int source_expected);

#endif