#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

//#define MAX_DATA 512
//#define MAX_ROWS 500

struct Address {
    int id;
    int set;
    int age;
    char *name;
    char *email;
};

struct Database {
    int MAX_DATA;
    int MAX_ROWS;
    struct Address *rows;
};

struct Connection {
    FILE *file;
    struct Database *db;
};

struct Connection *conn;

void Database_close();

void die(const char *message)
{
    if (errno) {
        perror(message);
    } else {
	printf("ERROR: %s\n", message);
    }

    Database_close();
    exit(1);
}

void Address_print(struct Address *addr)
{
    printf("%d %s %d %s\n", addr->id, addr->name, addr->age, addr->email);
}

int Address_size(int MAX_DATA)
{
    return (sizeof(int)*2 + MAX_DATA*2); 
}

int Database_rows_size(int MAX_DATA, int MAX_ROWS)
{
    return (Address_size(MAX_DATA) * MAX_ROWS);
}

void Database_load()
{
    int mData, mRows;
    int rc = fread(&mData, sizeof(int), 1, conn->file);
    rc &= fread(&mRows, sizeof(int), 1, conn->file);
    
    conn->db = malloc(sizeof(struct Database));
    conn->db->MAX_DATA = mData;
    conn->db->MAX_ROWS = mRows;
    conn->db->rows = malloc(Database_rows_size(mData, mRows));

    if ((!conn->db) || (!conn->db->rows))
	die("Memory error.");

    int i = 0;
    for (i = 0; i < mRows; i++) {
	rc &= fread(&conn->db->rows[i].id, sizeof(int), 1, conn->file);
        rc &= fread(&conn->db->rows[i].set, sizeof(int), 1, conn->file);
        conn->db->rows[i].name = malloc(mData);
	rc &= fread(conn->db->rows[i].name, mData, 1, conn->file);
	conn->db->rows[i].email = malloc(mData);
	rc &= fread(conn->db->rows[i].email, mData, 1, conn->file);
	rc &= fread(&conn->db->rows[i].age, sizeof(int), 1, conn->file);
    }

    //rc &= fread(conn->db->rows, Database_rows_size(mData, mRows), 1, conn->file);
    if (rc != 1)
	die("Failed to load database.");
}

void Database_open(const char *filename, char mode)
{
    conn = malloc(sizeof(struct Connection));
    if (!conn)
        die("Memory error");

    if (mode == 'c') {
	conn->file = fopen(filename, "w");
    } else {
	conn->file = fopen(filename, "r+");

	if (conn->file) {
	    Database_load();
	}
    }

    if (!conn->file) {
        die("Failed to open the file");
    }
}

void Database_close()
{
    if (conn) {
        if (conn->file)
	    fclose(conn->file);
	if (conn->db) {
	    if (conn->db->rows) {
	        int i = 0;
	        for (i = 0; i < conn->db->MAX_ROWS; i++) {
	            if (conn->db->rows[i].name)
		        free(conn->db->rows[i].name);
		    if (conn->db->rows[i].email)
		        free(conn->db->rows[i].email);
	        }
		free(conn->db->rows);
	    }
	    free(conn->db);
	}
	free(conn);
    }
}

void Database_write()
{
    rewind(conn->file);

    int mData = conn->db->MAX_DATA;
    int mRows = conn->db->MAX_ROWS;
    int rc = fwrite(&mData, sizeof(int), 1, conn->file);
    rc &= fwrite(&mRows, sizeof(int), 1, conn->file);
    
    int i = 0;
    for (i = 0; i < mRows; i++) {
        rc &= fwrite(&conn->db->rows[i].id, sizeof(int), 1, conn->file);
        rc &= fwrite(&conn->db->rows[i].set, sizeof(int), 1, conn->file);
	rc &= fwrite(conn->db->rows[i].name, mData, 1, conn->file);
	rc &= fwrite(conn->db->rows[i].email, mData, 1, conn->file);
	rc &= fwrite(&conn->db->rows[i].age, sizeof(int), 1, conn->file);
    }
    
    if (rc != 1)
	die("Failed to write database.");

    rc = fflush(conn->file);

    if (rc == -1)
	die("Cannot flush database.");
}

void Database_create(int MAX_DATA, int MAX_ROWS)
{
    int i = 0;
    conn->db = malloc(sizeof(struct Database));
    conn->db->MAX_DATA = MAX_DATA;
    conn->db->MAX_ROWS = MAX_ROWS;
    conn->db->rows = malloc(Database_rows_size(MAX_DATA, MAX_ROWS));

    for (i = 0; i < MAX_ROWS; i++) {
	conn->db->rows[i].id = i;
        conn->db->rows[i].set = 0;	
	conn->db->rows[i].name = malloc(MAX_DATA);
	memset(conn->db->rows[i].name, 0, MAX_DATA);
	conn->db->rows[i].email = malloc(MAX_DATA);
	memset(conn->db->rows[i].email, 0, MAX_DATA);
	conn->db->rows[i].age = 0;
    }
}

void Database_set(int id, const char *name, const char *email, int age)
{
    struct Address *addr = &conn->db->rows[id];
    if (addr->set)
	die("Already set, delete it first");

    addr->set = 1;
    // WARNING: bug, read the "How to Break It" and fix this
    char *res = strncpy(addr->name, name, conn->db->MAX_DATA);
    addr->name[conn->db->MAX_DATA - 1] = '\0';
    // demonstrate the strncpy bug
    if (!res)
	die("Name copy failed");

    res = strncpy(addr->email, email, conn->db->MAX_DATA);
    addr->email[conn->db->MAX_DATA - 1] = '\0';
    if (!res)
	die("Email copy failed");

    addr->age = age;
}

void Database_get(int id)
{
    struct Address *addr = &conn->db->rows[id];

    if (addr->set) {
	Address_print(addr);
    } else {
	die("ID is not set");
    }
}

void Database_delete(int id)
{
    //struct Address addr = {.id = id, .set = 0};
    conn->db->rows[id].set = 0;
    conn->db->rows[id].age = 0;
    if (conn->db->rows[id].name) {
        free(conn->db->rows[id].name);
	conn->db->rows[id].name = malloc(conn->db->MAX_DATA);
	memset(conn->db->rows[id].name, 0, conn->db->MAX_DATA);
    }
    if (conn->db->rows[id].email) {
        free(conn->db->rows[id].email);
	conn->db->rows[id].email = malloc(conn->db->MAX_DATA);
	memset(conn->db->rows[id].email, 0, conn->db->MAX_DATA);
    }
}

void Database_list()
{
    int i = 0;
    struct Database *db = conn->db;

    for (i = 0; i < db->MAX_ROWS; i++) {
        struct Address *cur = &db->rows[i];

	if (cur->set) {
	    Address_print(cur);
	}
    }
}

// returns id of the first record in conn->db where attr value is 
// the same as value, or -1 otherwise
int Database_find(char *attr, char *value)
{
    int res = -1;
    int i = 0;
    int test = -1;

    for (i = 0; i < conn->db->MAX_ROWS; i++) {
	if (strcmp(attr, "name") == 0) {
	    test = strcmp(value, conn->db->rows[i].name);
	} else if (strcmp(attr, "email") == 0) {
	    test = strcmp(value, conn->db->rows[i].email);
	} else if (strcmp(attr, "age") == 0) {
	    test = !(conn->db->rows[i].age == atoi(value));
	} else {
	    return -2;
	}
        if (test == 0) {
	    res = i;
	    break;
	}
    }
    return res;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
	die("USAGE: ex17 <dbfile> <action> [action params]");

    char *filename = argv[1];
    char action = argv[2][0];
    Database_open(filename, action);
    int found = -1;
    //int id = 0;

    //if (argc > 3) id = atoi(argv[3]);
    //if (id >= MAX_ROWS) die("There's not that many records.", conn);

    switch (action) {
        case 'c':
	    if (argc != 5)
		die("Need MAX_DATA and MAX_ROWS to create");
	    Database_create(atoi(argv[3]), atoi(argv[4]));
	    Database_write();
	    break;

	case 'g':
	    if (argc != 4)
		die("Need an id to get");

	    Database_get(atoi(argv[3]));
	    break;

	case 's':
	    if (argc != 7)
	        die("Need id, name, email and age to set");

	    Database_set(atoi(argv[3]), argv[4], argv[5], atoi(argv[6]));
	    Database_write();
	    break;

	case 'd':
            if (argc != 4)
		die("Need an id to delete");

	    Database_delete(atoi(argv[3]));
	    Database_write();
	    break;

	case 'l':
	    Database_list();
	    break;

	case 'f':
	    if (argc != 5)
		die("Need term name and value to search for");
	    found = Database_find(argv[3], argv[4]);
	    if (found == -2) {
	        die("Attribute not recognized");
	    } else if (found == -1) {
		die("Value not found");
	    } else {
		Database_get(found);
	    }
	    break;

	default:
	    die("Invalid action: c=create, g=get, s=set, d=del, l=list, f=find");
    }

    Database_close();

    return 0;
}
