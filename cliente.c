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

				int codes_accepted[2] = {CONTENT_LIST_DIRECTORY, ERROR};

				communicationBetweenProcess(socket, SERVIDOR, CLIENTE, LIST_DIRECTORY_CODE, "", NOT_SEND_LINES, NOT_SEND_LINES, codes_accepted);
			}

			if (isChangeDirectoryCommand(type)) {
				scanf("%s", directory);

				int codes_accepted[2] = {NOP_CODE_1, ERROR};
				
				communicationBetweenProcess(socket, SERVIDOR, CLIENTE, CHANGE_DIRECTORY_CODE, directory, NOT_SEND_LINES, NOT_SEND_LINES, codes_accepted);
			}

			if (isVerCommand(type)) {
				scanf("%s", file);
				
				int codes_accepted[2] = {CONTENT_SEE_FILE, ERROR};

				communicationBetweenProcess(socket, SERVIDOR, CLIENTE, SEE_FILE_SERVER_CODE, file, NOT_SEND_LINES, NOT_SEND_LINES, codes_accepted);
			}

			if (isLinhaCommand(type)) {
				scanf("%d", &initial_line);
				scanf("%[^\n]s", file);
				
				int codes_accepted[2] = {CONTENT_SEE_FILE, ERROR};

				communicationBetweenProcess(socket, SERVIDOR, CLIENTE, SEE_LINE_FILE_SERVER_CODE, file, initial_line, NOT_SEND_LINES, codes_accepted);
			}

			if (isLinhasCommand(type)) {
				scanf("%d", &initial_line);
				scanf("%d", &final_line);
				scanf("%[^\n]s", file);
				
				int codes_accepted[2] = {CONTENT_SEE_FILE, ERROR};

				communicationBetweenProcess(socket, SERVIDOR, CLIENTE, SEE_LINES_FILE_SERVER_CODE, file, initial_line, final_line, codes_accepted);
			}

			if (isEditCommand(type)) {
				scanf("%d", &initial_line);
				scanf("%s", file);
				scanf("%[^\n]s", file_content);

				strcat(file, file_content);
				char tmp[1024 *4];
				memset(tmp, 0, 1024 * 4);
				memcpy(tmp, " \0", sizeof(" \0"));
				memcpy(tmp + strlen(tmp), file, strlen(file));

				int codes_accepted[2] = {NOP_CODE_1, ERROR};
			
				communicationBetweenProcess(socket, SERVIDOR, CLIENTE, EDIT_LINES_FILE_SERVER_CODE, tmp, initial_line, NOT_SEND_LINES, codes_accepted);
			}			
		}
	}
}