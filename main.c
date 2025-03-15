#include "database.h"
#include "parser.h"

int main(void) {
	Database db;
	process_command("insert uid=25,first_name=\"hello\",last_name=\"db\",email=\"lika@rossia.ru\",last_visit=\"23:03:11\",registration_date=\"15.04.2025\",status=\'removed\'", &db);
}