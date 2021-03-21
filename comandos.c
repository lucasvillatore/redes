#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "comandos.h"
#include <dirent.h>
#include <sys/stat.h>
#include "commonFunctions.h"

const int FALSE = 0;
const int TRUE = 1;

int changeDirectory(char *folder, int client_server)
{
    setColorBlue();
    printf("Mudando de %s para o diretório %s\n\n", client_server == CLIENTE ? "cliente" : "servidor",folder);
    setColorDefault();

    int response = chdir(folder);
    if (response != 0) {
        setColorRed();
        printf("Falha ao tentar mudar de diretório\n");
        setColorDefault();
    }

    return response;
}

void listDirectory(char *listItems)
{
    setColorBlue();
    printf("Listando diretório atual\n\n");
    setColorDefault();
    
    printf("%s\n", listItems);
}

char *getItemsFromDirectory()
{
    DIR *d;
    d = opendir("./");
    char *listItems = (char *)calloc(1024*4, sizeof(char));
    if (d){

        struct dirent *dir;
        char dir_name[50];
        int index = 0;
        
        while ((dir = readdir(d)) != NULL) {
            strcpy(dir_name, dir->d_name);
            if (dir->d_type == DT_DIR) {
                strcat(dir_name, "/");
            }
            strcat(dir_name, "\n");
            strcat(listItems, dir_name);
        }
        strcat(listItems,"\n\0");
        closedir(d);
    }

    return listItems;
}

int seeContentFileServerInClient(char *file, char **file_return)
{
    FILE *file_content = fopen(file, "r");

    if (!file_content) {
        return -1;
    }

    int file_line = 1;
    int size = 0;
    
    char line[256];

    char line_to_append_in_buffer[270];
    char file_line_string[10];
    
    *file_return = (char *)calloc(1024*10, sizeof(char));

    while (fgets(line, 256, file_content)) {
        memset(line_to_append_in_buffer, 0, 270);
        sprintf(file_line_string, "%d", file_line);

        memcpy(line_to_append_in_buffer, file_line_string, strlen(file_line_string));
        memcpy(line_to_append_in_buffer + strlen(line_to_append_in_buffer), ": ", strlen(": "));
        memcpy(line_to_append_in_buffer + strlen(line_to_append_in_buffer), line, strlen(line));
        

        strcat(*file_return, line_to_append_in_buffer);
        file_line++;
    }

    fclose(file_content);

    return 1;
}

int checkIfIsInterval(int current_line, int initial_line, int end_line)
{
    if (current_line >= initial_line && current_line <= end_line) {
        return TRUE;
    }
    return FALSE;
}

int seeIntervalContentServerInClient(int initial_line, int end_line, char* file, char **buffer)
{
    FILE *file_content = fopen(file, "r");

    if (!file_content) {
        return -1;
    }

    int file_line = 1;
    char line[256];

    char line_to_append_in_buffer[270];
    char file_line_string[10];
    
    *buffer = (char *)calloc(1024*100, sizeof(char));

    while (fgets(line, 256, file_content)) {
        if (checkIfIsInterval(file_line, initial_line, end_line)){

            memset(line_to_append_in_buffer, 0, 270);
            sprintf(file_line_string, "%d", file_line);

            memcpy(line_to_append_in_buffer, file_line_string, strlen(file_line_string));
            memcpy(line_to_append_in_buffer + strlen(line_to_append_in_buffer), ": ", strlen(": "));
            memcpy(line_to_append_in_buffer + strlen(line_to_append_in_buffer), line, strlen(line));
            

            strcat(*buffer, line_to_append_in_buffer);
        }
        file_line++;
    }

    fclose(file_content);

    return 1;
}

int seeLineContentServerInClient(int line, char* file, char **buffer)
{
    FILE *file_content = fopen(file, "r");

    if (!file_content) {
        return -1;
    }
    int lines = 0;
    char file_line_string[10];
    char line_to_append_in_buffer[270];
    char current_line[256];
    memset(current_line, 0, 256);

    *buffer = (char *)calloc(1024 * 4, sizeof(char));

    while (fgets(current_line, 256, file_content) && lines != line) {
        lines++;
        if (lines >= line) {
            memset(line_to_append_in_buffer, 0, 270);
            sprintf(file_line_string, "%d", lines);
            
            memcpy(line_to_append_in_buffer, file_line_string, strlen(file_line_string));
            memcpy(line_to_append_in_buffer + strlen(line_to_append_in_buffer), ": ", strlen(": "));
            memcpy(line_to_append_in_buffer + strlen(line_to_append_in_buffer), current_line, strlen(current_line));
            
            strcat(*buffer, line_to_append_in_buffer);
        }
    }

    fclose(file_content);
    
    return 1;
}

int editContentFileInServer(char *file_name, int line, char *new_message)
{
    FILE *file_content = fopen(file_name, "r");

    if (!file_content) {
        return ERROR;
    }

    printf("%s\n", new_message);
    char *entire_file = getCopyOfEntireFileWithNewMessage(file_content, line, new_message);
    
    fclose(file_content);

    
    file_content = fopen(file_name, "w");

    createNewFileWithNewMessage(file_content, entire_file);
    
    fclose(file_content);
}

char *getCopyOfEntireFileWithNewMessage(FILE *file, int line, char *new_message)
{
    char *entire_file = (char *)calloc(1024 * 10, sizeof(char *));
    char *string_to_concate;
    char file_line[256];

    int i = 1;
    while(fgets(file_line, 256, file)) {
        string_to_concate = i != line ? file_line : new_message;
        strcat(entire_file, string_to_concate);
        i++;
    }

    return entire_file;
}

void createNewFileWithNewMessage(FILE *file, char *entire_file)
{
    fprintf(file, "%s", entire_file);
}

