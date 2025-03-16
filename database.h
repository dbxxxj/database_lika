#ifndef DATABASE_H
#define DATABASE_H
#include <time.h>

typedef enum { ACTIVE, BANNED, REMOVED } Status;

const char* status_strings[3];

typedef struct {
	int uid;
	char first_name[50];
	char last_name[50];
	char email[100];
	struct tm last_visit;
	struct tm registration_date;
	Status status;
}User;

typedef struct Node {
	User data;
	struct Node* next;
}Node;

typedef enum {
	OP_EQUAL,            // ==
	OP_NOT_EQUAL,        // !=
	OP_LESS,             // <
	OP_LESS_OR_EQUAL,    // <=
	OP_GREATER,          // >
	OP_GREATER_OR_EQUAL, // >=
	OP_IN,               // /in/
} ConditionType;

typedef struct {
	char field[50];
	ConditionType op;
	char value[100];
} Condition;

typedef struct {
	Node* head;
	int length;
} Database;

typedef struct {
	char* fields[10];
	int fields_count;

	Condition conditions[10];
	int conditons_count;
} ParsedSelectCommand;

int insert(Database* db, User user);
int select(Database* db, ParsedSelectCommand command);
int delete(Database* db, Condition* conditions, int conditions_count);
#endif // !DATABASE_H
