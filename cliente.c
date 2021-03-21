#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "comandos.h"
#include "rawSocket.h"
#include "commonFunctions.h"

int initial_line;
int final_line;

char command[15];
char data[15];
char file[256];

char *error_message;
char *device = "lo";

char path[100];
char message[15];

char filename[100];
char directory[100];

int initial_line;
int final_line;
char file_content[1000];

int type;

int received_code;
int sequence;
int sequence_received;

char buffer[15];

char *directory_items;
char *received_string;

	
int main()
{
	char *message_from_another_process;
	system("clear");
	int socket = ConexaoRawSocket(device);
	kermit_protocol_t received_buffer;
	message_from_another_process = (char *)calloc(1024 * 4, sizeof(char));

	while (1)
	{
		sequence_received = 0;
		showCurrentPath(
			getCurrentPath(), 
			CLIENTE
		);

		scanf("%s", command);
		type = getTypeByCommand(command);
		
		if (!thisCommandExists(type)) {
			setColorRed();
			printf("Esse comando não existe\n");
			setColorDefault();

			continue;
		}
		if (isCommandClient(command)) {

			if (isListDirectoryCommand(type)) {
				directory_items = getItemsFromDirectory();
				listDirectory(directory_items);
				free(directory_items);
			}

			if (isChangeDirectoryCommand(type)) {
				scanf("%s", directory);
				changeDirectory(directory, CLIENTE);
			}

		}
		else {
			kermit_protocol_t *send_buffer;
			kermit_protocol_t received_buffer;

			if (isListDirectoryCommand(type)) {
				do {
					sendMessageBiggerThenFifteenBits(socket, SERVIDOR, CLIENTE, LIST_DIRECTORY_CODE, "", NOT_SEND_LINES, NOT_SEND_LINES);
					received_code = receiveMessageFromAnotherProcess(socket, CONTENT_LIST_DIRECTORY, &message_from_another_process, CLIENTE, SERVIDOR);
				} while(isNack(received_code));
				printf("%s\n", message_from_another_process);

			}

			if (isChangeDirectoryCommand(type)) {
				scanf("%s", directory);
				do {
					sendMessageBiggerThenFifteenBits(socket, SERVIDOR, CLIENTE, CHANGE_DIRECTORY_CODE, directory, NOT_SEND_LINES ,NOT_SEND_LINES);
					received_code = receiveMessageFromAnotherProcess(socket, ACK_CODE, &message_from_another_process, CLIENTE, SERVIDOR);
				} while(isNack(received_code));

				printf("%s\n", message_from_another_process);
			}

			if (isVerCommand(type)) {
				scanf("%s", file);
				
				do {
					sendMessageBiggerThenFifteenBits(socket, SERVIDOR, CLIENTE, SEE_FILE_SERVER_CODE, file, NOT_SEND_LINES, NOT_SEND_LINES);
					received_code = receiveMessageFromAnotherProcess(socket, CONTENT_SEE_FILE, &message_from_another_process, CLIENTE, SERVIDOR);
				} while(isNack(received_code));

				printf("%s\n", message_from_another_process);
			}

			if (isLinhaCommand(type)) {
				scanf("%d", &initial_line);
				scanf("%[^\n]s", file);
				
				do {
					sendMessageBiggerThenFifteenBits(socket, SERVIDOR, CLIENTE, SEE_LINE_FILE_SERVER_CODE, file, initial_line, NOT_SEND_LINES);
					received_code = receiveMessageFromAnotherProcess(socket, CONTENT_SEE_FILE, &message_from_another_process, CLIENTE, SERVIDOR);
				} while(isNack(received_code));

				printf("%s\n", message_from_another_process);
			}

			if (isLinhasCommand(type)) {
				scanf("%d", &initial_line);
				scanf("%d", &final_line);
				scanf("%[^\n]s", file);
				do{
					sendMessageBiggerThenFifteenBits(socket, SERVIDOR, CLIENTE, SEE_LINES_FILE_SERVER_CODE, file, initial_line, final_line);
					received_code = receiveMessageFromAnotherProcess(socket, CONTENT_SEE_FILE, &message_from_another_process, CLIENTE, SERVIDOR);
				} while(isNack(received_code));

				printf("%s\n", message_from_another_process);
			}

			if (isEditCommand(type)) {
				scanf("%d", &initial_line);
				scanf("%s", file);
				scanf("%[^\n]s", file_content);

				strcat(file, " ");
				strcat(file, file_content);
				
				do {

					sendMessageBiggerThenFifteenBits(socket, SERVIDOR, CLIENTE, EDIT_LINES_FILE_SERVER_CODE, file, initial_line, NOT_SEND_LINES);
					received_code = receiveMessageFromAnotherProcess(socket, ACK_CODE, &message_from_another_process, CLIENTE, SERVIDOR);
				}while(isNack(received_code));

				printf("%s\n", message_from_another_process);
			}			
		}
	}
}