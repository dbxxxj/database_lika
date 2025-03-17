#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "parser.h"
#include "memstat.h"

#pragma warning(disable:4996)

char* strtok_lika(char* origin, const char symbol, char** save_pointer) {
    if (origin) *save_pointer = origin;
    else if (!*save_pointer || **save_pointer == '\0') return NULL;

    char* start = *save_pointer;

    while (*start && *start == symbol) start++;

    if (*start == '\0') return NULL;

    char* token = start;
    int flag = 0;

    while (**save_pointer) {
        if (**save_pointer == '"' || **save_pointer == '\'')
            flag = !flag;

        else if (!flag && **save_pointer == symbol) {  // <-- заменили strchr() на простое сравнение
            **save_pointer = '\0';
            (*save_pointer)++;

            while (**save_pointer && **save_pointer == symbol) (*save_pointer)++;

            return token;
        }

        (*save_pointer)++;
    }

    *save_pointer = NULL;
    return token;
}

ConditionType get_operator(const char* op_str) {
    if (strcmp(op_str, "==") == 0) return OP_EQUAL;
    if (strcmp(op_str, "!=") == 0) return OP_NOT_EQUAL;
    if (strcmp(op_str, "<") == 0) return OP_LESS;
    if (strcmp(op_str, "<=") == 0) return OP_LESS_OR_EQUAL;
    if (strcmp(op_str, ">=") == 0) return OP_GREATER_OR_EQUAL;
    if (strcmp(op_str, ">") == 0) return OP_GREATER;
    if (strcmp(op_str, "in") == 0) return OP_IN;
    return -1;
}


int parse_condition(const char* condition_str, Condition* condition) {
    char op[10];

    // ќбрабатываем сложные операции (/, /in/, /include/)
    if (sscanf(condition_str, "%49[^!=<>/]/%9[^/]/%99[^\n]",
        condition->field, op, condition->value) == 3) {
        condition->op = get_operator(op);
        return 1;
    }

    // ќбрабатываем стандартные операции (==, !=, <, >, <=, >=)
    if (sscanf(condition_str, "%49[^!=<>]%2[!=<>]%99[^\n]",
        condition->field, op, condition->value) == 3) {
        condition->op = get_operator(op);
    }
    // ќбрабатываем услови€ с кавычками: status!='removed'
    else if (sscanf(condition_str, "%49[^!=<>]%2[!=<>]%*['\"]%99[^'\"]%*['\"]",
        condition->field, op, condition->value) == 3) {
        condition->op = get_operator(op);
    }
    else return 0; // ќшибка парсинга

    return 1;
}

int parse_insert(const char* args, User* user) {
    char temp[256];
    strncpy(temp, args, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    char* savepointer;
    char* token = strtok_lika(temp, ',', &savepointer);
    while (token) {
        char* equal = strchr(token, '=');
        if (!equal)
            return 0;

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
            return 0;
        }

        token = strtok_lika(NULL, ',', &savepointer);
    }

    return 1;
}

int parse_select(const char* args, ParsedSelectCommand* command) {
    const int args_len = strlen(args);
    char* arg_copy = (char*)lika_malloc(args_len + 1);
    strcpy(arg_copy, args);  // strcpy, потому что строка гарантированно умещаетс€

    char* save_pointer;
    char* token = strtok_lika(arg_copy, ' ', &save_pointer);
    while (token) {
        char* field_save_pointer;
        char* field = strtok_lika(token, ',', &field_save_pointer);
        while (field) {
            // ¬ыдел€ем пам€ть дл€ пол€, если массив состоит из указателей
            command->fields[command->fields_count] = (char*)lika_malloc(strlen(field) + 1);
            strcpy(command->fields[command->fields_count], field);
            command->fields_count++;

            field = strtok_lika(NULL, ',', &field_save_pointer);
        }

        token = strtok_lika(NULL, ' ', &save_pointer);
        if (token && (strchr(token, '=') || strstr(token, "==") || strstr(token, "<") ||
            strstr(token, ">") || strstr(token, "/in/"))) {
            break;
        }
    }

    while (token) {
        char condition[40];
        strcpy(condition, token);
        if (!parse_condition(condition, &command->conditions[command->conditons_count])) {
            printf("incorrect: %.*s\n", 20, args);
            return 0;
        }

        command->conditons_count++;

        token = strtok_lika(NULL, ' ', &save_pointer);
    }

    lika_free(arg_copy);
    return 1;
}
int parse_delete(const char* args, Condition* conditions, int* conditions_count) {
    const int args_len = strlen(args);
    char* arg_copy = (char*)lika_malloc(args_len + 1);
    strcpy(arg_copy, args);
    char* save_pointer;
    char* token = strtok_lika(arg_copy, ' ', &save_pointer);

    while (token) {
        char condition[100];
        strcpy(condition, token);

        if (!parse_condition(condition, &conditions[*conditions_count])) {
            printf("incorrect: %.*s\n", 20, args);
            return 0;
        }

        (*conditions_count)++;

        token = strtok_lika(NULL, ' ', &save_pointer);
    }


    lika_free(arg_copy);
    return 1;
}
int parse_update(const char* args, ParsedUpdateCommand* command) {
    const int args_len = strlen(args);
    char* arg_copy = (char*)lika_malloc(args_len + 1);
    strcpy(arg_copy, args);

    char* save_pointer;
    char* token = strtok_lika(arg_copy, ' ', &save_pointer);

    char* field_save_pointer;
    char* field = strtok_lika(token, ',', &field_save_pointer);
    while (field) {
        char* equal = strchr(field, '=');
        if (!equal)
            return 0;

        *equal = '\0';
        char* key = field;
        char* value = equal + 1;

        const int key_length = strlen(key);
        const int value_length = strlen(value);

        for (int index = 0; index < command->fields_count; index++) // если поле уже обновл€етс€
            if (strncmp(key, command->updating_fields[index], key_length) == 0)
                return 0;

        command->updating_fields[command->fields_count] = (char*)lika_malloc(key_length + 1);
        strncpy(command->updating_fields[command->fields_count], field, key_length+1);
        command->fields_count++;

        if (strcmp(key, "uid") == 0) {
            command->new_user.uid = atoi(value);
        }

        else if (strcmp(key, "first_name") == 0) {
            strncpy(command->new_user.first_name, value + 1, value_length - 2);
        }

        else if (strcmp(key, "last_name") == 0) {
            strncpy(command->new_user.last_name, value + 1, value_length - 2);
        }

        else if (strcmp(key, "email") == 0) {
            strncpy(command->new_user.email, value + 1, value_length - 2);
        }

        else if (strcmp(key, "status") == 0) {
            for (int index = 0; index < 3; index++) {
                if (strncmp(status_strings[index], value + 1, value_length - 2) == 0)
                    command->new_user.status = index;
            }
        }

        else if (strcmp(key, "last_visit") == 0) {
            sscanf(value, "\"%d:%d:%d\"",
                &command->new_user.last_visit.tm_hour, &command->new_user.last_visit.tm_min, &command->new_user.last_visit.tm_sec);
        }

        else if (strcmp(key, "registration_date") == 0) {
            sscanf(value, "\"%d.%d.%d\"",
                &command->new_user.registration_date.tm_mday, &command->new_user.registration_date.tm_mon, &command->new_user.registration_date.tm_year);
        }

        else {
            printf("incorrect: %.*s\n", 20, args);
            return 0;
        }

        field = strtok_lika(NULL, ',', &field_save_pointer);
    }

    token = strtok_lika(NULL, ' ', &save_pointer);
    while (token) {
        char condition[40];
        strcpy(condition, token);
        if (!parse_condition(condition, &command->conditions[command->conditions_count])) {
            printf("incorrect: %.*s\n", 20, args);
            return 0;
        }

        command->conditions_count++;

        token = strtok_lika(NULL, ' ', &save_pointer);
    }

    lika_free(arg_copy);
    return 1;
}


void process_command(const char* command, Database* db) {
    if (strncmp(command, "insert", 6) == 0) {
        User user;
        memset(&user, 0, sizeof(User));
        if (parse_insert(command + 7, &user))
            insert(db, user);
    }

    if (strncmp(command, "select", 6) == 0) {
        ParsedSelectCommand parsed_command;
        memset(&parsed_command, 0, sizeof(ParsedSelectCommand));
        if (parse_select(command + 7, &parsed_command)) {
            if (!select(db, parsed_command)) {
                printf("incorrect: %.*s\n", 20, command);
            }
        }

        for (int index = 0; index < parsed_command.fields_count; index++) {
            lika_free(parsed_command.fields[index]);
        }
    }
    if (strncmp(command, "delete", 6) == 0) {
        Condition* conditions = (Condition*)lika_malloc(10 * sizeof(Condition));
        int conditions_count = 0;
        if (parse_delete(command + 7, conditions, &conditions_count)) {
            delete(db, conditions, conditions_count);
        }

        lika_free(conditions);
    }

    if (strncmp(command, "update", 6) == 0) {
        ParsedUpdateCommand parsed_command;
        memset(&parsed_command, 0, sizeof(ParsedUpdateCommand));
        if (parse_update(command + 7, &parsed_command)) {
            if (!update(db, parsed_command)) {
                printf("incorrect: %.*s\n", 20, command);
            }
        }

        else printf("incorrect: %.*s\n", 20, command);

        for (int index = 0; index < parsed_command.fields_count; index++) {
            lika_free(parsed_command.updating_fields[index]);
        }
    }



}