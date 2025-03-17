#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memstat.h"
#pragma warning(disable:4996)
const char* status_strings[3] = { "active", "banned", "removed" };

int is_equal(User user, Condition condition) {
    const int value_length = strlen(condition.value);
    if (strcmp(condition.field, "uid") == 0) {
        return user.uid == atoi(condition.value);
    }   

    if (strcmp(condition.field, "first_name") == 0) {
        return strncmp(user.first_name, condition.value + 1, value_length - 2) == 0;
    }

    if (strcmp(condition.field, "last_name") == 0) {
        return strncmp(user.last_name, condition.value + 1, value_length - 2) == 0;
    }

    if (strcmp(condition.field, "email") == 0) {
        return strncmp(user.email, condition.value + 1, value_length - 2) == 0;
    }

    if (strcmp(condition.field, "status") == 0) {
        for (int index = 0; index < 3; index++) {
            if (strncmp(status_strings[index], condition.value + 1, value_length - 2) == 0)
                return user.status == index;
        }
    }

    if (strcmp(condition.field, "last_visit") == 0) {
        struct tm parsed_date;
        sscanf(condition.value, "\"%d:%d:%d\"",
            &parsed_date.tm_hour, &parsed_date.tm_min, &parsed_date.tm_sec);
        return (user.last_visit.tm_hour * 3600 + user.last_visit.tm_min * 60 + user.last_visit.tm_sec)
== (parsed_date.tm_hour * 3600 + parsed_date.tm_min * 60 + parsed_date.tm_sec);
    }

    if (strcmp(condition.field, "registration_date") == 0) {
        struct tm parsed_date;
        sscanf(condition.value, "\"%d.%d.%d\"",
            &parsed_date.tm_mday, &parsed_date.tm_mon, &parsed_date.tm_year);
        return (user.registration_date.tm_year * 365 + user.registration_date.tm_mon * 31 + user.registration_date.tm_mday)
            == (parsed_date.tm_year * 365 + parsed_date.tm_mon * 31 + parsed_date.tm_mday);
    }

    return -1;
}

int is_greater(User user, Condition condition) {
    const int value_length = strlen(condition.value);
    if (strcmp(condition.field, "uid") == 0) {
        return user.uid > atoi(condition.value);
    }

    if (strcmp(condition.field, "first_name") == 0) {
        return strncmp(user.first_name, condition.value + 1, value_length - 2) > 0;
    }

    if (strcmp(condition.field, "last_name") == 0) {
        return strncmp(user.last_name, condition.value + 1, value_length - 2) > 0;
    }

    if (strcmp(condition.field, "email") == 0) {
        return strncmp(user.email, condition.value + 1, value_length - 2) > 0;
    }

    if (strcmp(condition.field, "last_visit") == 0) {
        struct tm parsed_date;
        sscanf(condition.value, "\"%d:%d:%d\"",
            &parsed_date.tm_hour, &parsed_date.tm_min, &parsed_date.tm_sec);
        return (user.last_visit.tm_hour * 3600 + user.last_visit.tm_min * 60 + user.last_visit.tm_sec)
        > (parsed_date.tm_hour * 3600 + parsed_date.tm_min * 60 + parsed_date.tm_sec);
    }

    if (strcmp(condition.field, "registration_date") == 0) {
        struct tm parsed_date;
        sscanf(condition.value, "\"%d.%d.%d\"",
            &parsed_date.tm_mday, &parsed_date.tm_mon, &parsed_date.tm_year);
        return (user.registration_date.tm_year * 365 + user.registration_date.tm_mon * 31 + user.registration_date.tm_mday)
            > (parsed_date.tm_year * 365 + parsed_date.tm_mon * 31 + parsed_date.tm_mday);
    }

    return -1;
}

int is_less(User user, Condition condition) {
    const int value_length = strlen(condition.value);
    if (strcmp(condition.field, "uid") == 0) {
        return user.uid < atoi(condition.value);
    }

    if (strcmp(condition.field, "first_name") == 0) {
        return strncmp(user.first_name, condition.value + 1, value_length - 2) < 0;
    }

    if (strcmp(condition.field, "last_name") == 0) {
        return strncmp(user.last_name, condition.value + 1, value_length - 2) < 0;
    }

    if (strcmp(condition.field, "email") == 0) {
        return strncmp(user.email, condition.value + 1, value_length - 2) < 0;
    }

    if (strcmp(condition.field, "last_visit") == 0) {
        struct tm parsed_date;
        sscanf(condition.value, "\"%d:%d:%d\"",
            &parsed_date.tm_hour, &parsed_date.tm_min, &parsed_date.tm_sec);
        return (user.last_visit.tm_hour * 3600 + user.last_visit.tm_min * 60 + user.last_visit.tm_sec)
        < (parsed_date.tm_hour * 3600 + parsed_date.tm_min * 60 + parsed_date.tm_sec);
    }

    if (strcmp(condition.field, "registration_date") == 0) {
        struct tm parsed_date;
        sscanf(condition.value, "\"%d.%d.%d\"",
            &parsed_date.tm_mday, &parsed_date.tm_mon, &parsed_date.tm_year);
        return (user.registration_date.tm_year * 365 + user.registration_date.tm_mon * 31 + user.registration_date.tm_mday)
            <(parsed_date.tm_year * 365 + parsed_date.tm_mon * 31 + parsed_date.tm_mday);
    }

    return -1;
}

int is_in(User user, Condition condition) {
    if (strcmp(condition.field, "status") != 0)
        return -1;

    char temp[100];
    strncpy(temp, condition.value, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0'; // Гарантируем null-терминатор

    char* token = strtok(temp, "[],\' "); // Разбиваем строку по [' ,]

    while (token) {
        if (strcmp(status_strings[user.status], token) == 0) {
            return 1; // Найдено совпадение
        }

        token = strtok(NULL, "[],\' ");
    }

    return 0;
}

int check_condition(User user, Condition condition) {
    if (condition.op == OP_EQUAL)
        return is_equal(user, condition);

    if (condition.op == OP_NOT_EQUAL) {
        int result = is_equal(user, condition);
        if (result == -1)
            return -1;

        return !result;
    }

    if (condition.op == OP_LESS)
        return is_less(user, condition);

    if (condition.op == OP_GREATER)
        return is_greater(user, condition);

    if (condition.op == OP_LESS_OR_EQUAL) {
        int first = is_equal(user, condition);
        int second = is_less(user, condition);
        if (first == -1 || second == -1)
            return -1;

        return first || second;
    }

    if (condition.op == OP_GREATER_OR_EQUAL) {
        int first = is_equal(user, condition);
        int second = is_greater(user, condition);
        if (first == -1 || second == -1)
            return -1;

        return first || second;
    }

    if (condition.op == OP_IN) {
        return is_in(user, condition);
    }

    return -1;
}

int insert(Database* db, User user) {
    Node* new_node = (Node*)lika_malloc(sizeof(Node));

    new_node->data = user;
    new_node->next = NULL;

    if (!db->head) {
        db->head = new_node;
    }

    else {
        Node* temp = db->head;
        while (temp->next)
            temp = temp->next;

        temp->next = new_node;
    }

    db->length++;
    printf("insert:%d\n", db->length);
    return 1;
}

int select(Database* db, ParsedSelectCommand command) {
    int found_count = 0;
    Node* current = db->head;
    while (current) {
        int match = 1;
        for (int index = 0; index < command.conditons_count; index++) {
            int result = check_condition(current->data, command.conditions[index]);
            if (result == -1)
                return 0;

            if (!result) {
                match = 0;
                break;
            }
        }

        if (!match) {
            current = current->next;
            continue;
        }

        found_count++;
        for (int index = 0; index < command.fields_count; index++) {
            if (strcmp(command.fields[index], "uid") == 0) {
                printf("uid=%d ", current->data.uid);
            }
            if (strcmp(command.fields[index], "first_name") == 0) {
                printf("first_name=\"%s\" ", current->data.first_name);
            }
            if (strcmp(command.fields[index], "last_name") == 0) {
                printf("last_name=\"%s\" ", current->data.last_name);
            }
            if (strcmp(command.fields[index], "email") == 0) {
                printf("email=\"%s\" ", current->data.email);
            }
            if (strcmp(command.fields[index], "status") == 0) {
                printf("status=\'%s\' ", status_strings[current->data.status]);
            }
            if (strcmp(command.fields[index], "last_visit") == 0) {
                printf("last_visit=\"%02d:%02d:%02d\" ",
                    current->data.last_visit.tm_hour, current->data.last_visit.tm_min, current->data.last_visit.tm_sec);
            }
            if (strcmp(command.fields[index], "registration_date") == 0) {
                printf("registration_date=\"%02d.%02d.%04d\" ",
                    current->data.registration_date.tm_mday, current->data.registration_date.tm_mon, current->data.registration_date.tm_year);
            }
        }

        printf("\n");
        current = current->next;
    }

    printf("select:%d\n", found_count);

    return 1;
}
int delete (Database* db, Condition* conditions, int conditions_count){
    int deleted_count = 0; 
    Node* current = db->head;
    Node* prev = NULL;

    while (current) {
        int match = 1; 

        for (int i = 0; i < conditions_count; i++) {
            int result = check_condition(current->data, conditions[i]);  
            if (result == -1) {
                return 0;  
            }
            if (!result) {
                match = 0;
                

            }
        }

        
        if (match) {
            Node* to_delete = current;
            if (prev) {
                prev->next = current->next; 
            }
            else {
                db->head = current->next;   
            }
            current = current->next;  

           
            lika_free(to_delete);

            deleted_count++;  
        }
        else {
            
            prev = current;
            current = current->next;
            
        }
    }
    printf("delete:%d\n", deleted_count);

    return 1; 
}


int update(Database* db, ParsedUpdateCommand command) {
    int updated_count = 0;
    Node* current = db->head;
    while (current) {
        int match = 1;
        for (int index = 0; index < command.conditions_count; index++) {
            int result = check_condition(current->data, command.conditions[index]);
            if (result == -1)
                return 0;

            if (!result) {
                match = 0;
                break;
            }
        }

        if (!match) {
            current = current->next;
            continue;
        }

        updated_count++;
        for (int index = 0; index < command.fields_count; index++) {
            if (strcmp(command.updating_fields[index], "uid") == 0) {
                current->data.uid = command.new_user.uid;
            }

            else if (strcmp(command.updating_fields[index], "first_name") == 0) {
                strcpy(current->data.first_name, command.new_user.first_name);
            }

            else if (strcmp(command.updating_fields[index], "last_name") == 0) {
                strcpy(current->data.last_name, command.new_user.last_name);
            }

            else if (strcmp(command.updating_fields[index], "email") == 0) {
                strcpy(current->data.email, command.new_user.email);
            }

            else if (strcmp(command.updating_fields[index], "last_visit") == 0) {
                current->data.last_visit = command.new_user.last_visit;
            }

            else if (strcmp(command.updating_fields[index], "registration_date") == 0) {
                current->data.registration_date = command.new_user.registration_date;
            }

            else if (strcmp(command.updating_fields[index], "status") == 0) {
                current->data.status = command.new_user.status;
            }

            else return 0;
        }

        current = current->next;
    }

    printf("update:%d\n", updated_count);
    return 1;
}

