#include <stdlib.h>
#include <stdio.h>
#include "commonFunctions.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

const char CHANGE_DIRECTORY_SERVER_STRING[] = "cd";

const char CHANGE_DIRECTORY_CLIENT_STRING[] = "lcd";

const char LIST_DIRECTORY_SERVER_STRING[] = "ls";

const char LIST_DIRECTORY_CLIENT_STRING[] = "lls";

const char SEE_FILE_SERVER_STRING[] = "ver";

const char SEE_LINE_FILE_SERVER_STRING[] = "linha";

const char SEE_LINES_FILE_SERVER_STRING[] = "linhas";

const char EDIT_LINES_FILE_SERVER_STRING[] = "edit";

kermit_protocol_t *defineProtocol(
    int destination_address,
    int source_address,
    int type,
    char *message,
    int sequence
){
    kermit_protocol_t *kermit = (kermit_protocol_t *)calloc(1,sizeof(kermit_protocol_t));
    kermit->start_marker = 0b01111110;
    kermit->destination_address = destination_address;
    kermit->source_address = source_address;
    strcpy(kermit->data, message);
    kermit->sequence = sequence;
    kermit->type = type;
    kermit->size = strlen(message);

    return kermit;
}

void debug_kermit(kermit_protocol_t *kermit)
{
    if (kermit->start_marker == START_MARKER) {
        printf("############# DEBUG #############\n");
        printf("start marker = %d\n", kermit->start_marker);

        printf("destination_address = %d\n", kermit->destination_address);
        printf("type = %d\n", kermit->type);
        printf("data = %s\n", kermit->data);
        printf("size = %d\n", kermit->size);
        if (kermit->source_address == CLIENTE) {
            printf("source address = CLIENTE\n");
        }else if (kermit->source_address == SERVIDOR ){
            printf("source address = SERVIDOR\n");

        }
        if (kermit->destination_address == CLIENTE) {
            printf("destination address = CLIENTE\n");
        }else if (kermit->destination_address == SERVIDOR){
            printf("destionation address = SERVIDOR\n");
        }
        printf("sequence = %d\n", kermit->sequence);
        printf("type = %d\n", kermit->type);
        printf("############# DEBUG #############\n");
        printf("\n\n");
    }else {
        // printf("mensagem de outro processo\n");
    }
}

void showCurrentPath(char *current_path, int cliente_servidor)
{

    setColorGreen();
    printf("%s (%s): ", current_path, cliente_servidor == 0b01 ? "cliente" : "servidor");
    setColorDefault();
}

char *getCurrentPath()
{
    return getcwd(NULL, 0);
}

void setColorGreen()
{
    printf("\033[0;32m");
}

void setColorRed()
{
    printf("\033[0;31m");
}

void setColorDefault()
{
    printf("\033[0m");
}

void setColorBlue()
{
    printf("\033[0;36m");
}

int getTypeByCommand(char *command)
{
    if (strcmp(CHANGE_DIRECTORY_SERVER_STRING, command) == 0)
    {
        return CHANGE_DIRECTORY_CODE;
    }

    if (strcmp(CHANGE_DIRECTORY_CLIENT_STRING, command) == 0)
    {
        return CHANGE_DIRECTORY_CODE;
    }

    if (strcmp(LIST_DIRECTORY_SERVER_STRING, command) == 0)
    {
        return LIST_DIRECTORY_CODE;
    }

    if (strcmp(LIST_DIRECTORY_CLIENT_STRING, command) == 0)
    {
        return LIST_DIRECTORY_CODE;
    }

    if (strcmp(SEE_FILE_SERVER_STRING, command) == 0)
    {
        return SEE_FILE_SERVER_CODE;
    }

    if (strcmp(SEE_LINE_FILE_SERVER_STRING, command) == 0)
    {
        return SEE_LINE_FILE_SERVER_CODE;
    }

    if (strcmp(SEE_LINES_FILE_SERVER_STRING, command) == 0)
    {
        return SEE_LINES_FILE_SERVER_CODE;
    }

    if (strcmp(EDIT_LINES_FILE_SERVER_STRING, command) == 0)
    {
        return EDIT_LINES_FILE_SERVER_CODE;
    }

    return ERROR;
}

int isCommandClient(char *command)
{
    if (
        strcmp(CHANGE_DIRECTORY_CLIENT_STRING, command) == 0 || 
        strcmp(LIST_DIRECTORY_CLIENT_STRING, command) == 0
    ){
        return 1;
    }
    return 0;
}

int isListDirectoryCommand(int type)
{
    return type == LIST_DIRECTORY_CODE ? 1 : 0;
}

int isChangeDirectoryCommand(int type)
{
    return type == CHANGE_DIRECTORY_CODE ? 1 : 0;
}

int isVerCommand(int type)
{
    return type == SEE_FILE_SERVER_CODE ? 1 : 0;
}

int isLinhaCommand(int type)
{
    return type == SEE_LINE_FILE_SERVER_CODE ? 1 : 0;
}

int isLinhasCommand(int type)
{
    return type == SEE_LINES_FILE_SERVER_CODE ? 1 : 0;
}

int isEditCommand(int type)
{
    return type == EDIT_LINES_FILE_SERVER_CODE ? 1 : 0;
}

int thisCommandExists(int type)
{
    return type != ERROR ? 1 : 0; 
}

int getMessageFromAnotherProcess(int socket, kermit_protocol_t *received_buffer)
{
    int received_code;
    received_code = recv(
        socket, 
        received_buffer,
        sizeof(kermit_protocol_t), 
        FLAGS
    );

    return received_code;

}

int isMessageFromAnotherProcess(int code, kermit_protocol_t *buffer, int source_address)
{
    return code != NOT_RECEIVED_CODE && 
        buffer->start_marker == START_MARKER &&
        buffer->source_address == source_address 
        ? 1 
        : 0;
}

int getTypeFromKermit(kermit_protocol_t *buffer)
{
    return buffer->type;
}

int isAck(int type) 
{
    return type == ACK_CODE ? 1 : 0;
}

int isError(int type)
{
    return type == ERROR ? 1 : 0;
}

int isNack(int type) 
{
    return type == NACK_CODE ? 1 : 0;
}

char *concatInitialLine(char *message, int initial_line)
{
    char *new_message = (char *)calloc(1024 * 4, sizeof(char));
    char initial_line_string[10];
    int size_of_initial_line = sizeof(initial_line_string);
    
    sprintf(initial_line_string, "%d", initial_line);
    size_of_initial_line = sizeof(initial_line_string);
    
    memset(new_message, 0, sizeof(new_message));
    memcpy(new_message , initial_line_string, sizeof(initial_line));

    return new_message;
}

char *concatInitialLineAndEndLine(char *message, int initial_line, int end_line)
{
    char *new_message = (char *)calloc(1024 * 4, sizeof(char));
    char initial_line_string[10];
    char end_line_string[10];
    int size_of_initial_line;
    int size_of_end_line;

    sprintf(initial_line_string, "%d ", initial_line);
    size_of_initial_line = sizeof(initial_line_string);
    
    sprintf(end_line_string, "%d", end_line);
    size_of_end_line = sizeof(end_line_string);
    
    memset(new_message, 0, sizeof(new_message));
    
    memcpy(new_message , initial_line_string, sizeof(initial_line));
    strcat(new_message, end_line_string);
   
   return new_message;
}

int sendMessageBiggerThenFifteenBits(int socket, int destination_address, int source_address, int type, char *message, int initial_line, int end_line)
{
	kermit_protocol_t *send_buffer;

	int sequence = 0;
	char *new_message = (char *)calloc(1024 * 4, sizeof(char));
	int received_code;

    char initial_line_end_line[15];

    if (isOnlyInitialLine(initial_line, end_line)){
        char *initial_line_string = concatInitialLine(message, initial_line);

        received_code = sendMessage(socket, destination_address, source_address, INITIAL_AND_END_LINE, initial_line_string, sequence);
        
        sequence++;

    }
    else if (isInitialLineAndEndLine(initial_line, end_line)) {
        char *initial_line_end_line = concatInitialLineAndEndLine(message, initial_line, end_line);
        
        received_code = sendMessage(socket, destination_address, source_address, INITIAL_AND_END_LINE, initial_line_end_line, sequence);

        sequence++;
    }

	int message_size = strlen(message);
    int cut_message = message_size >= 15 ? 15 : message_size;
    do{
        memset(new_message, 0, 15);
		memcpy(new_message, message, cut_message);

        received_code = sendMessage(socket, destination_address, source_address, type, new_message, sequence);

        message += 15;
        message_size -= 15;
        cut_message = message_size >= 15 ? 15 : message_size;
        sequence++;

	}while(message_size > 0);
	
    sendEndTransmission(socket, destination_address, source_address, sequence);

	return 1;
}

int sendMessage(int socket, int destination_address, int source_address, int type, char *message, int sequence)
{
    kermit_protocol_t *send_buffer;
    int received_code;

    do {
        send_buffer = defineProtocol(destination_address, source_address, type, message, sequence);
        received_code = send(socket, send_buffer, sizeof(kermit_protocol_t), FLAGS);

        free(send_buffer);
    } while (received_code <= 0);

    return received_code;
}

void sendEndTransmission(int socket, int destination, int source, int sequence)
{
    kermit_protocol_t *send_buffer;

    send_buffer = defineProtocol(destination, source, END_TRANSMISSION, "", sequence);
    int isSend = send(socket, send_buffer, sizeof(kermit_protocol_t), FLAGS);

    if (isSend > 0) {
        printf("Fim da transmissão\n");
    }
	
}

char *getMessageBiggerThenFifteenBits(int socket, kermit_protocol_t *received_buffer, int *received_code, int client_server)
{
	char *message_from_another_process = (char *)calloc(1024 * 4, sizeof(char));
    int sequence = 0;
	
    do{	
        *received_code = getMessageFromAnotherProcess(socket, received_buffer);
        if (isMessageFromAnotherProcess(*received_code, received_buffer, client_server)  && received_buffer->sequence == sequence && received_buffer->type != END_TRANSMISSION) {
            memcpy(message_from_another_process + sequence * 15, received_buffer->data, 15);
            sequence++;
            
        }
    }while (received_buffer->type != END_TRANSMISSION);
    
	return message_from_another_process;	
}

int getLineFromString(char **string)
{
    char *token = __strtok_r(*string, " ", string);
    
    return atoi(token);
}

char *getFileFromString(char *string)
{
    char *token = strsep(&string, " ");

    return string;
}

int countFileLines(char *file)
{
    FILE *file_content = fopen(file, "r");

    if (!file_content) {
        return -1;
    }

    char file_buffer[256];
    int i = 0; 
    while (fgets(file_buffer, 256, file_content)) {
        i++;
    }
    
    fclose(file_content);
    
    return i;
}

int sendAckNackCode(int socket, int destination_address, int source_address, int type, int sequence)
{
    int received_code;
    kermit_protocol_t *send_buffer = defineProtocol(destination_address, source_address, type, "", sequence);
    
    received_code = send(socket, send_buffer, sizeof(kermit_protocol_t), FLAGS);
    
    return received_code;
}

int isOnlyInitialLine(int initial_line, int end_line) 
{
    return initial_line != NOT_SEND_LINES && end_line == NOT_SEND_LINES ? 1 : 0;
}

int isInitialLineAndEndLine(int initial_line, int end_line)
{
    return initial_line != NOT_SEND_LINES && end_line != NOT_SEND_LINES ? 1 : 0;
}

int receiveMessageFromAnotherProcess(int socket, int *expected_type, char **message_from_another_process, int source_expected, int destionation_expected)
{
    kermit_protocol_t *received_buffer = (kermit_protocol_t *)calloc(1, sizeof(kermit_protocol_t));
    *message_from_another_process = (char *)calloc(1024 * 4, sizeof(char));

    int received_code;
    int buffer_size = 0;
    int isReceived = 0;

    do{
        received_code = getMessageFromAnotherProcess(socket, received_buffer);
        received_code = getMessageFromAnotherProcess(socket, received_buffer);

        if (isNack(received_buffer->type) && isMessageFromAnotherProcess(received_code, received_buffer, SERVIDOR)) {
            return NACK_CODE;
        }

    }while(!isAck(received_buffer->type) && !isError(received_buffer->type));

    do{	
        received_code = getMessageFromAnotherProcess(socket, received_buffer);
        received_code = getMessageFromAnotherProcess(socket, received_buffer);
        
        if (isMessageFromAnotherProcess(received_code, received_buffer, SERVIDOR) && !isEndTransmission(received_buffer) && isTypeExpected(received_buffer, expected_type)){
            memcpy(*message_from_another_process + buffer_size, received_buffer->data, received_buffer->size);
            buffer_size += received_buffer->size;
            isReceived = 1;
        }

    } while (!isEndTransmission(received_buffer) || isSourceExpected(received_buffer, source_expected) || !isReceived);

    return 1;
}

int isEndTransmission(kermit_protocol_t *buffer)
{
    return buffer->type == END_TRANSMISSION ? 1 : 0;
}

int isTypeExpected(kermit_protocol_t *buffer, int *type_expected)
{
    int number_of_types = sizeof(type_expected)/4;

    for (int i = 0; i < number_of_types; i++) {
        if (buffer->type == type_expected[i]) {
            return 1;
        }
    }

    return 0;
}

int isSourceExpected(kermit_protocol_t *buffer, int source_expected)
{
    return buffer->source_address == source_expected ? 1 : 0;
}

void communicationBetweenProcess(int socket, int destination_address, int source_address, int type, char *message, int initial_line, int end_line, int *codes_accepted)
{
    int received_code;
    char *message_from_another_process = (char *)calloc(1024 * 4, sizeof(char));
    do {
        sendMessageBiggerThenFifteenBits(socket, destination_address, source_address, type, message, initial_line, end_line);
        received_code = receiveMessageFromAnotherProcess(socket, codes_accepted, &message_from_another_process, source_address, destination_address);
        
        if (isNack(received_code)) {
            printf("Falha ao transmitir a mensagem. Reenviando a mensagem\n");
        }

    } while(isNack(received_code));

    printf("%s\n", message_from_another_process);
}