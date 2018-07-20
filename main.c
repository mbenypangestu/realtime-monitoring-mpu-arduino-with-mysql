#include <my_global.h>
#include <mysql.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_QUERY 255
#define CREATE_DATA "insert into data set category_id=?, acx=?, acy=?, acz=?, tmp=?, gyx=?, gyy=?, gyz=?"

MYSQL       *conn;
MYSQL_RES   *res;
MYSQL_STMT  *stmt;
MYSQL_ROW   row;

char query[MAX_QUERY];

char *server     = "localhost";
char *user       = "mygetzu";
char *password   = "P@ssw0rd|mysql";
char *database   = "arduino_project";

int AcX1,AcY1,AcZ1,Tmp1,GyX1,GyY1,GyZ1;
int AcX2,AcY2,AcZ2,Tmp2,GyX2,GyY2,GyZ2;
void setVariable();
void connectDb();
void findAll(char *table);
void save();
void disconnectDb();

int main(int argc, char **argv) {
    setVariable();
    findAll("data");
    save(1);
    save(2);

    return 0;
}

void setVariable() {
    AcX1 = 123;
    AcZ1 = 231;
    AcY1 = 243;
    Tmp1 = 111;
    GyX1 = 563;
    GyY1 = 533;
    GyZ1 = 523;

    AcX2 = 1230;
    AcZ2 = 2310;
    AcY2 = 2430;
    Tmp2 = 222;
    GyX2 = 5630;
    GyY2 = 5330;
    GyZ2 = 5230;
}

void connectDb() {
    conn = mysql_init(NULL);
	
	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(1);
	} else {
        printf("Connected !\n");
    }
}

void disconnectDb() {
    /* close connection */
    if (conn) {
        mysql_free_result(res);
        mysql_close(conn);
        printf("Disconnected !\n\n");
    }
}

void findAll(char *table) {
    connectDb();

    snprintf(query, MAX_QUERY, "SELECT * FROM %s", table);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
    } 

    res = mysql_use_result(conn);
    printf("Data : \n");
    while ((row = mysql_fetch_row(res)) != NULL)
        printf("%s | %s\n", row[0], row[1]);
    
    disconnectDb();
}

void save(int category_id) {
	MYSQL_BIND  bind[8];
    MYSQL_TIME  timestamp;

    time_t now = time(NULL);
    int acx, acy, acz, tmp, gyx, gyy, gyz;
    
    // time_t to MYSQL_TIME
	struct tm *now_struct = gmtime(&now);
	timestamp.year = now_struct->tm_year + 1900;
	timestamp.month = now_struct->tm_mon + 1;
	timestamp.day = now_struct->tm_mday;
	timestamp.hour = now_struct->tm_hour;
	timestamp.minute = now_struct->tm_min;
    timestamp.second = now_struct->tm_sec;
    
    if (category_id == 1) {
        acx = AcX1;
        acy = AcY1;
        acz = AcZ1;
        tmp = Tmp1;
        gyx = GyX1;
        gyy = GyY1;
        gyz = GyZ1;
    } else if (category_id == 2) {
        acx = AcX2;
        acy = AcY2;
        acz = AcZ2;
        tmp = Tmp2;
        gyx = GyX2;
        gyy = GyY2;
        gyz = GyZ2;
    }

    connectDb();

    stmt = mysql_stmt_init(conn);
    if (!stmt) {
		printf(" mysql_stmt_init(), out of memory\r\n");
		exit(0);
    }

    if (mysql_stmt_prepare(stmt, CREATE_DATA, strlen(CREATE_DATA))) {
		printf("mysql_stmt_prepare(), INSERT failed\r\n");
		printf("error: %s\r\n", mysql_stmt_error(stmt));
		exit(0);
    }

    memset(bind, 0, sizeof(bind));

	bind[0].buffer_type= MYSQL_TYPE_LONG;
	bind[0].buffer= (char *)&category_id;
	bind[0].is_null= 0;
	bind[0].length= 0;

	bind[1].buffer_type= MYSQL_TYPE_LONG;
	bind[1].buffer= (char *)&acx;
	bind[1].is_null= 0;
	bind[1].length= 0;

	bind[2].buffer_type= MYSQL_TYPE_LONG;
	bind[2].buffer= (char *)&acy;
	bind[2].is_null= 0;
	bind[2].length= 0;

	bind[3].buffer_type= MYSQL_TYPE_LONG;
	bind[3].buffer= (char *)&acz;
	bind[3].is_null= 0;
	bind[3].length= 0;

	bind[4].buffer_type= MYSQL_TYPE_LONG;
	bind[4].buffer= (char *)&tmp;
	bind[4].is_null= 0;
	bind[4].length= 0;

	bind[5].buffer_type= MYSQL_TYPE_LONG;
	bind[5].buffer= (char *)&gyx;
	bind[5].is_null= 0;
	bind[5].length= 0;

	bind[6].buffer_type= MYSQL_TYPE_LONG;
	bind[6].buffer= (char *)&gyy;
	bind[6].is_null= 0;
	bind[6].length= 0;

	bind[7].buffer_type= MYSQL_TYPE_LONG;
	bind[7].buffer= (char *)&gyz;
	bind[7].is_null= 0;
	bind[7].length= 0;

	// bind[8].buffer_type= MYSQL_TYPE_DATETIME;
	// bind[8].buffer= (char *)&timestamp;
	// bind[8].is_null= 0;
    // bind[8].length= 0;

    // bind parameters
	if (mysql_stmt_bind_param(stmt, bind)) {
		printf("mysql_stmt_bind_param() failed\r\n");
		printf("%s\r\n", mysql_stmt_error(stmt));
		exit(0);
    }
    
    printf("executing data...\r\n");
	if (mysql_stmt_execute(stmt)) {
		printf("mysql_stmt_execute(), 1 failed\r\n");
		printf("%s\r\n", mysql_stmt_error(stmt));
		exit(0);
    } else {
        printf("Done !.\r\n");
    }
    
    disconnectDb();
}