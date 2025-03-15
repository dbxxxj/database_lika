#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

#pragma warning(disable:4996)

char* strtok_lika(char* origin, const char symbol, char** save_pointer) {
    if (origin) *save_pointer = origin;
    else if (!*save_pointer || **save_pointer == '\0') return NULL;

    char* start = *save_pointer;

    // Пропускаем начальные разделители
    while (*start && *start == symbol) start++;

    if (*start == '\0') return NULL;  // Если строка пустая после разделителей — конец

    char* token = start;
    int flag = 0;

    while (**save_pointer) {
        if (**save_pointer == '"' || **save_pointer == '\'')
            flag = !flag;  // Переключаем флаг кавычек

        else if (!flag && **save_pointer == symbol) {
            **save_pointer = '\0';  // Завершаем текущий токен
            (*save_pointer)++;      // Сдвигаем указатель на следующий символ

            // Пропускаем последующие разделители
            while (**save_pointer == symbol) (*save_pointer)++;

            return token;
        }

        (*save_pointer)++;
    }

    *save_pointer = NULL;  // Указываем, что строка закончилась
    return token;
}

void parse_insert(const char* args, User* user) {
	char temp[256];
	strncpy(temp, args, sizeof(temp));
	temp[sizeof(temp) - 1] = '\0';

    char* savepointer;
    char* token = strtok_lika(temp, ',', &savepointer);
    while (token) {
        char* equal = strchr(token, '=');
        if (!equal)
            return;

        *equal = '\0';
        char* key = token;
        char* value = equal + 1;
        const int value_length = strlen(value);

        if (strcmp(key, "uid") == 0) {
            user->uid = atoi(value);
        }

        else if (strcmp(key, "first_name") == 0) {
            strncpy(user->first_name, value + 1, value_length - 2);
        }

        else if (strcmp(key, "last_name") == 0) {
            strncpy(user->last_name, value + 1, value_length - 2);
        }

        else if (strcmp(key, "email") == 0) {
            strncpy(user->email, value + 1, value_length - 2);
        }

        else if (strcmp(key, "status") == 0) {
            for (int index = 0; index < 3; index++) {
                if (strncmp(status_strings[index], value + 1, value_length - 2) == 0)
                    user->status = index;
            }
        }

        else if (strcmp(key, "last_visit") == 0) {
            sscanf(value, "\"%d:%d:%d\"",
                &user->last_visit.tm_hour, &user->last_visit.tm_min, &user->last_visit.tm_sec);
        }

        else if (strcmp(key, "registration_date") == 0) {
            sscanf(value, "\"%d.%d.%d\"",
                &user->registration_date.tm_mday, &user->registration_date.tm_mon, &user->registration_date.tm_year);
        }

        else {
            printf("incorrect: %.*s\n", 20, args);
            return;
        }

        token = strtok_lika(NULL, ',', &savepointer);
    }
}

void process_command(char* command, Database* db) {
	//if (strlen(command) == 0 || command[0] = '\n') {
	//	return;
	//}

	if (strncmp(command, "insert", 6) == 0) {
        User user;
        memset(&user, 0, sizeof(User));
		parse_insert(command + 7, &user);
        insert(db, user);
	}
}

