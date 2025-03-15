#ifndef DATABASE_H
#define DATABASE_H
# include<time.h>

typedef enum { ACTIVE, BANNED, REMOVED } Status;

typedef struct {
	int uid;
	char first_name[50];
	char last_name[50];
	char email[100];
	time_t last_visit;
	time_t registration_date;
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

void insert(Database* db, User user);
#endif // !DATABASE_H
