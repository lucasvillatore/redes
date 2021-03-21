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

char *device = "lo";

int received_code;

char path[100];
char current_path[100];
char *mensagem;
int type;

char *directory_items;

char *file;

char *file_content;

int line;
int end_line;
int main()
{
	system("clear");
	setColorGreen();
	printf("INICIANDO SERVIDOR\n");
	setColorDefault();
    int socket = ConexaoRawSocket(device);
    kermit_protocol_t received_buffer;
	kermit_protocol_t *send_buffer;
	int debug = 0;
	
	char *received_string = (char *)calloc(1024 * 4, sizeof(char));
    int sequence;
	int buffer_size;
	char *message_from_another_process = (char *)calloc(1024 * 4, sizeof(char));
	while (1) {

		memset(message_from_another_process, 0, 1024 *4);
		
		buffer_size = 0;
		do{	
			received_code = getMessageFromAnotherProcess(socket, &received_buffer);
			received_code = getMessageFromAnotherProcess(socket, &received_buffer);
			if (
				received_buffer.type != END_TRANSMISSION &&
				isMessageFromAnotherProcess(received_code, &received_buffer, CLIENTE) 
			) {
				type = received_buffer.type;
				memcpy(message_from_another_process + buffer_size, received_buffer.data, received_buffer.size);
				buffer_size += received_buffer.size;
				sequence++;
			}
		} while (received_buffer.type != END_TRANSMISSION || received_buffer.source_address == SERVIDOR);

		if (isListDirectoryCommand(type)) {
			setColorBlue();
			printf("Command \"ls\" received\n");
			setColorDefault();
			directory_items = getItemsFromDirectory();

			sendMessage(socket, CLIENTE, SERVIDOR, ACK_CODE, "", NO_SEQUENCE);
			sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, CONTENT_LIST_DIRECTORY, directory_items, NOT_SEND_LINES , NOT_SEND_LINES);
		}

		if (isChangeDirectoryCommand(type)) {
			setColorBlue();
			printf("Command \"cd\" to \"%s\"received\n", message_from_another_process);
			setColorDefault();
			received_code = changeDirectory(message_from_another_process, SERVIDOR);
			printf("Caminho atual: ");
			showCurrentPath(
				getCurrentPath(),
				SERVIDOR
			);

			if (received_code == 0) {
				sendMessage(socket, CLIENTE, SERVIDOR, ACK_CODE, "", NO_SEQUENCE);
				sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, NOP_CODE_1, "", NOT_SEND_LINES ,NOT_SEND_LINES);
			}else {
				sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, ERROR, "Falha ao mudar de diretório", NOT_SEND_LINES ,NOT_SEND_LINES);
			}
		}

		if (isVerCommand(type)) {
			setColorBlue();
			printf("Command \"ver\" file \"%s\" received\n", message_from_another_process);
			setColorDefault();
			
			char *ver_content;

			received_code = seeContentFileServerInClient(message_from_another_process, &ver_content);
			
			if (received_code == -1) {
				sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, ERROR, "Arquivo não encontrado", NOT_SEND_LINES ,NOT_SEND_LINES);
			}else {
				sendMessage(socket, CLIENTE, SERVIDOR, ACK_CODE, "", NO_SEQUENCE);
				sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, CONTENT_SEE_FILE, ver_content, NOT_SEND_LINES ,NOT_SEND_LINES);
			}
		}

		if (isLinhaCommand(type)) {
			line = getLineFromString(&message_from_another_process);
			getFileFromString(message_from_another_process);

			setColorBlue();
			printf("Command \"linha\" \"%d\" of \"%s\" received\n", line, message_from_another_process);
			setColorDefault();

			char *linha_content;

			int number_file_lines = countFileLines(message_from_another_process);

			if (number_file_lines < line) {
				sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, ERROR, "Arquivo não possui a linha desejada", NOT_SEND_LINES ,NOT_SEND_LINES);
			}else{
				received_code = seeLineContentServerInClient(line, message_from_another_process, &linha_content);
				
				if (received_code != -1) {
					sendMessage(socket, CLIENTE, SERVIDOR, ACK_CODE, "", NO_SEQUENCE);
					sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, CONTENT_SEE_FILE, linha_content, NOT_SEND_LINES ,NOT_SEND_LINES);
				}else {
					sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, ERROR, "Arquivo não encontrado", NOT_SEND_LINES ,NOT_SEND_LINES);
				}
			}

		}

		if (isLinhasCommand(type)) {
			setColorBlue();
			printf("Command \"linhas\" received\n");
			setColorDefault();
			line = getLineFromString(&message_from_another_process);
			end_line = getLineFromString(&message_from_another_process);

			char *linhas_content;

			int number_file_lines = countFileLines(message_from_another_process);

			if (line > number_file_lines || end_line > number_file_lines) {
				sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, ERROR, "Arquivo não possui a linha desejada", NOT_SEND_LINES, NOT_SEND_LINES);
			}
			else if(line > end_line){
				sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, ERROR, "Não é possível mostrar esse intervalo", NOT_SEND_LINES, NOT_SEND_LINES);
			}else{

				received_code = seeIntervalContentServerInClient(line, end_line, message_from_another_process, &linhas_content);
				if (received_code != -1) {
					sendMessage(socket, CLIENTE, SERVIDOR, ACK_CODE, "", NO_SEQUENCE);
					sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, CONTENT_SEE_FILE, linhas_content, NOT_SEND_LINES, NOT_SEND_LINES);
				}else {
					sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, ERROR, "Arquivo não encontrado", NOT_SEND_LINES, NOT_SEND_LINES);
				}
			}
		}

		if (isEditCommand(type)) {
			setColorBlue();
			printf("Command \"edit\" received\n");
			setColorDefault();

			char *file_name;
	
			line = getLineFromString(&message_from_another_process);
			file_content = getFileFromString(message_from_another_process);
			file_name = message_from_another_process;

			int number_file_lines = countFileLines(file_name);


			printf("file = %s\nline = %d\ncontent = %s\n", file_name, line, file_content);
			if (number_file_lines == -1) {
				sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, ERROR, "O arquivo não existe", NOT_SEND_LINES ,NOT_SEND_LINES);
			}else if (line > number_file_lines) {
				sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, ERROR, "O arquivo não possui a linha desejada", NOT_SEND_LINES ,NOT_SEND_LINES);
			}else {
				strcat(file_content, "\n");
				if (editContentFileInServer(file_name, line, file_content) == ERROR) {
					sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, ERROR, "O arquivo não existe", NOT_SEND_LINES ,NOT_SEND_LINES);
				}else {
					sendMessage(socket, CLIENTE, SERVIDOR, ACK_CODE, "", NO_SEQUENCE);
					sendMessageBiggerThenFifteenBits(socket, CLIENTE, SERVIDOR, NOP_CODE_1, "Arquivo editado com sucesso", NOT_SEND_LINES ,NOT_SEND_LINES);

				}
			}
		}
		
		type = NOT_SEND_LINES;
    }
}