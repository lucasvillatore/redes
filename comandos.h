#ifndef __COMANDOS__
#define __COMANDOS__

int changeDirectory(char *folder, int client_server);

void listDirectory(char *listItems);

char *getItemsFromDirectory();

int seeContentFileServerInClient(char *file, char **file_return);

int checkIfIsInterval(int current_line, int initial_line, int end_line);

int seeIntervalContentServerInClient(int initial_line, int end_line, char* file, char **buffer);

int seeLineContentServerInClient(int line, char* file, char **buffer);

int editContentFileInServer(char *file_name, int line, char *new_message);

char *getCopyOfEntireFileWithNewMessage(FILE *file, int line, char *new_message);

void createNewFileWithNewMessage(FILE *file, char *entire_file);
#endif