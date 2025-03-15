#include "parser.h"
#include<string.h>
#include<stdio.h>

#pragma warning(disable:4996)

char* strtok_lika(char* origin, const char symbol, char** save_pointer) {
    if (origin) *save_pointer = origin;
    else if (!*save_pointer) return NULL;  // ����������: ������ ��������� ��������� �� ������

    char* start = *save_pointer;
    while (*start && *start == symbol)
        start++;  // ���������� ��������� �����������

    if (*start == '\0') return NULL;  // ���� ������ ������ ����� ������������ � �����

    char* token = start;
    int flag = 0;

    while (**save_pointer) {
        if (**save_pointer == '"' || **save_pointer == '\'')
            flag = !flag;  // ����������� ���� �������

        else if (!flag && **save_pointer == symbol) {
            **save_pointer = '\0';  // �������� ����������� �� ����� ������
            (*save_pointer)++;  // �������� ���������, ������� ���������� ������������� �������
            return token;
        }

        (*save_pointer)++;
    }

    *save_pointer = NULL;  // ���������, ��� ������ �����������
    return token;
}


void parse_insert(const char* args, Database* db) {
	User user;
	memset(&user, 0, sizeof(User));

	char temp[256];
	strncpy(temp, args, sizeof(temp));
	temp[sizeof(temp) - 1] = '\0';

    char* savepointer;
    char* token = strtok_lika(temp, ',', &savepointer);
    while (token) {
        printf("%s\n", token);
        token = strtok_lika(NULL, ',', &savepointer);
    }
}

void process_command(char* command, Database* db) {
	//if (strlen(command) == 0 || command[0] = '\n') {
	//	return;
	//}

	if (strncmp(command, "insert", 6) == 0) {
		parse_insert(command + 7, db);
	}
}

