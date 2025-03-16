#ifndef DATABASE_H
#define DATABASE_H
# include<time.h>

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

typedef struct {
	Node* head;
	int length;
} Database;
typedef enum {
	OP_EQUAL,       // ==
	OP_NOT_EQUAL,   // !=
	OP_LESS,        // <
	OP_GREATER,     // >
	OP_IN,          // /in/
	OP_INCLUDE      // /include/
} ConditionType;
typedef struct {
	char field[50];
	ConditionType op;
	char value[100];
} Condition;

void insert(Database* db, User user);
void select(Database* db, const char* conditions);
#endif // !DATABASE_H
