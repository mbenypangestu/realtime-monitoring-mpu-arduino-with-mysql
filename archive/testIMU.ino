
#include<Wire.h>

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
char *user       = "root";
char *password   = "";
char *database   = "arduino_project";

void connectDb();
void findAll(char *table);
void save();
void disconnectDb();

  const int MPU2=0x69,MPU1=0x68;
  int16_t AcX1,AcY1,AcZ1,Tmp1,GyX1,GyY1,GyZ1;
  int16_t AcX2,AcY2,AcZ2,Tmp2,GyX2,GyY2,GyZ2;
  
   
  //-------------------------------------------------\setup loop\------------------------------------------------------------ 
   void setup(){ 
        Wire.begin(); 
        Wire.beginTransmission(MPU1);
        Wire.write(0x6B);// PWR_MGMT_1 register 
        Wire.write(0); // set to zero (wakes up the MPU-6050)
        Wire.endTransmission(true);Wire.begin(); 
        Wire.beginTransmission(MPU2);
        Wire.write(0x6B);// PWR_MGMT_1 register 
        Wire.write(0); // set to zero (wakes up the MPU-6050)
        Wire.endTransmission(true);
        Serial.begin(115200); 
       } 
       
  //---------------------------------------------------\void loop\------------------------------------------------------------
   void loop(){
     
        //get values for first mpu having address of 0x68   
        GetMpuValue1(MPU1);
        
        //get values for second mpu having address of 0x69
        GetMpuValue2(MPU2);

        save(1);
        save(2);

        //kirim data lewat serial
        Serial.print(" ");//clear memori sebelumnya
        Serial.print("AX1=");
        Serial.print(AcX1);
        Serial.print(",AY1="); 
        Serial.print(AcY1);
        Serial.print(",AZ1=");
        Serial.print(AcZ1); 
        Serial.print(",GX1=");
        Serial.print(GyX1);
        Serial.print(",GY1="); 
        Serial.print(GyY1); 
        Serial.print(",GZ1=");
        Serial.print(GyZ1); 
        Serial.print(",AX2=");
        Serial.print(AcX2);
        Serial.print(",AY2="); 
        Serial.print(AcY2);
        Serial.print(",AZ2=");
        Serial.print(AcZ2);  
        Serial.print(",GX2=");
        Serial.print(GyX2); 
        Serial.print(",GY2="); 
        Serial.print(GyY2);
        Serial.print(",GZ2=");
        Serial.println(GyZ2); 
        delay(1000);
      }
   
  //----------------------------------------------\user defined functions\-------------------------------------------------- 
        
   
   void GetMpuValue1(const int MPU){ 
     
        Wire.beginTransmission(MPU); 
        Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) 
        Wire.endTransmission(false);
        Wire.requestFrom(MPU, 14, true); // request a total of 14 registers 
        AcX1=Wire.read()<<8| Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L) 
        AcY1=Wire.read()<<8|  Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
        AcZ1=Wire.read()<<8| Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L) 
        Tmp1=Wire.read()<<8| Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L) 
        GyX1=Wire.read()<<8| Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L) 
        GyY1=Wire.read()<<8| Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L) 
        GyZ1=Wire.read()<<8| Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)   
       }
       
       
   void GetMpuValue2(const int MPU){ 
     
        Wire.beginTransmission(MPU); 
        Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) 
        Wire.endTransmission(false);
        Wire.requestFrom(MPU, 14, true); // request a total of 14 registers 
        AcX2=Wire.read()<<8| Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L) 
        AcY2=Wire.read()<<8|  Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
        AcZ2=Wire.read()<<8| Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L) 
        Tmp2=Wire.read()<<8| Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L) 
        GyX2=Wire.read()<<8| Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L) 
        GyY2=Wire.read()<<8| Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L) 
        GyZ2=Wire.read()<<8| Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L) 
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
  //   bind[8].length= 0;

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