#include "parser.h"
#include <string.h>
#include <stdio.h>
#include <time.h>
#include<stdlib>

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

        else if (!flag && strchr(symbol, **save_pointer)) {
            **save_pointer = '\0';  
            (*save_pointer)++;     

            
            while (**save_pointer && strchr(symbol, **save_pointer)) (*save_pointer)++;

            return token;
        }

        (*save_pointer)++;
    }

    *save_pointer = NULL; 
    return token;
}
struct tm parse_date(const char* date_str) {
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));  // Обнуляем структуру
    strptime(date_str, "%Y-%m-%d", &tm);  // Парсим строку в структуру tm
    return tm;
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
void parse_select(const char* args, Database* db) {
    ConditionType get_operator(const char* op_str) {
        if (strcmp(op_str, "==") == 0) return OP_EQUAL;
        if (strcmp(op_str, "!=") == 0) return OP_NOT_EQUAL;
        if (strcmp(op_str, "<") == 0) return OP_LESS;
        if (strcmp(op_str, ">") == 0) return OP_GREATER;
        if (strcmp(op_str, "/in/") == 0) return OP_IN;
        if (strcmp(op_str, "/include/") == 0) return OP_INCLUDE;
        return -1;
    }
    int parse_condition(const char* condition_str, Condition * condition) {
        char op[10];
        if (sscanf(condition_str, "%49[^!=<>/]/%9[^/]/%99[^\n]",
            condition->field, op, condition->value) == 3) {
            condition->op = get_operator(op);
            return 1;
        }
        if (sscanf(condition_str, "%49[^!=<>]%1[!=<>]%99[^\n]",
            condition->field, op, condition->value) == 3) {
            condition->op = get_operator(op);
            return 1;
        }
        return 0;
    }
    int check_condition(User user, Condition * condition) {
        if (condition->op == OP_EQUAL) {
            if (strcmp(condition->field, "first_name") == 0) {
                return strcmp(user.first_name, condition->value) == 0;
            }
            else if (strcmp(condition->field, "last_name") == 0) {
                return strcmp(user.last_name, condition->value) == 0;
            }
            else if (strcmp(condition->field, "status") == 0) {
                return strcmp(user.status, condition->value) == 0;
            }
            else if (strcmp(condition->field, "age") == 0) {
                return user.age == atoi(condition->value);
            }
            else if (strcmp(condition->field, "birthday") == 0) {
                // Сравниваем структуры tm с использованием memcmp
                return memcmp(&user.birthday, &parse_date(condition->value), sizeof(struct tm)) == 0;
            }
        }

        if (condition->op == OP_LESS) {
            if (strcmp(condition->field, "age") == 0) {
                return user.age < atoi(condition->value);
            }
            else if (strcmp(condition->field, "birthday") == 0) {
                // Сравниваем структуры tm с использованием memcmp
                return memcmp(&user.birthday, &parse_date(condition->value), sizeof(struct tm)) < 0;
            }
        }

        if (condition->op == OP_GREATER) {
            if (strcmp(condition->field, "age") == 0) {
                return user.age > atoi(condition->value);
            }
            else if (strcmp(condition->field, "birthday") == 0) {
                // Сравниваем структуры tm с использованием memcmp
                return memcmp(&user.birthday, &parse_date(condition->value), sizeof(struct tm)) > 0;
            }
        }

            // Возвращаем 0, если ни одно из условий не выполнено
       return 0;
    }
}

void process_command(char* command, Database* db) {

	if (strncmp(command, "insert", 6) == 0) {
        User user;
        memset(&user, 0, sizeof(User));
		parse_insert(command + 7, &user);
        insert(db, user);
	}

}

