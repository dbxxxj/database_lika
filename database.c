#include "database.h"
#include "memstat.h"

const char* status_strings[3] = { "active", "banned", "removed" };

void insert(Database* db, User user) {
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
}
