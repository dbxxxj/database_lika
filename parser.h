#ifndef PARSER_H
#define PARSER_H
#include"database.h"

void process_command(const char* command, Database* db);
char* strtok_lika(char* origin, const char symbol, char** save_pointer);

#endif // !PARSER_H
