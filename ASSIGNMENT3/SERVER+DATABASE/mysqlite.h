#ifndef mysqlite_h_
#define mysqlite_h_
extern struct user_data
{  
   string username,message,status,pin;
   int id;
}dummy;
typedef struct user_data record;
void create_table(string table_name);
void create_database(string database_name);
void insert(string user_name,string data,int state,int id);
void update(string user_name,string data,string stat);
int count_active_users(void);
void clean_result(void);
int retrieve_status(string user_name);
record* retrieve_active_user(void);
record retrieve(int id);
bool socket_update(string user_name,string status,int id);
string TABLE_NAME = "USERS";
string DATABASE_NAME = "Networking_Assignmenet3.db";
record result;
string A;
record *ALL_USER = NULL;
int GLOBAL = 0;
static int all_user(void *data,int argc,char *argv[],char **azColName){
   ALL_USER[GLOBAL++].username = argv[0];
   return 0;
}
static int call_back(void *data,int argc,char *argv[],char **azColName){
   int i;
   result.id = atoi(argv[0]);
   result.username = argv[1];
   result.pin = argv[2];
   result.message = argv[3];
   result.status = argv[4];
   return 0;
}
static int call_back_status(void *data,int argc,char *argv[],char **azColName){
   record *res = (record *)data;
   res ->status = argv[0];
   return 0;
}
static int count_callback(void *count,int argc,char *argv[],char **azColname){
	int *res = (int *)count;
	res[0] = atoi(argv[0]);
	return 0;
}
void create_database(string database_name){
   int db;
   sqlite3 *database_object=NULL;
   db= sqlite3_open((const char *)database_name.c_str(),&database_object);
   if(database_object){
      DATABASE_NAME = database_name;
   }
   else{
      sqlite3_errmsg(database_object);
   }  
   sqlite3_close(database_object); 
}
void create_table(string table_name){
   int db;
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   if(db == SQLITE_OK){
      TABLE_NAME = table_name;
      string sql_query = "CREATE TABLE "+TABLE_NAME+"("  \
         "ID           INT    PRIMARY KEY   NOT NULL," \
         "USERNAME           TEXT    NOT NULL," \
         "PASSWORD           TEXT    NOT NULL," \
         "DATA           CHAR(256)    NOT NULL," \
         "STATUS        INT      NOT NULL);";
      db = sqlite3_exec(database_object,(const char *)(sql_query.c_str()),call_back,0,&error);
      if(db == SQLITE_OK){
         /*
            ** cout<<" Table "<<table_name<<" has been succesfully created"<<endl;
         */
      }
      else{
         cout<<error<<endl;
      }
   }
   else{
      sqlite3_errmsg(database_object);
   }
   sqlite3_close(database_object);
}
void insert(string user_name,string data,int state,int id,string pas){
   int db;
   A = user_name;
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   if(db == SQLITE_OK){
      stringstream temp,temp2;
      string s,s2;
      temp << state;
      temp2 << id;
      s2 = temp2.str();
      s = temp.str();
      string sql_query = "INSERT INTO "+TABLE_NAME +"(ID,USERNAME,PASSWORD,DATA,STATUS)" \
                           " VALUES ("+s2+","+"'"+A+"'"+","+"'"+pas+"',"+"'"+data+"'"+","+"'"+s+"'"+");";
      db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),call_back, 0,&error);
      if(db == SQLITE_OK){
         /*
            ** SUCCESSFULL QUERY
         */
      }
      else{
         cout<<error<<endl;
         exit(0);
      }
   }
   else{
      sqlite3_errmsg(database_object);
   }
   sqlite3_close(database_object);
}
void update(string user_name,string data,string stat = "1"){
   int db;
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   if(db == SQLITE_OK){
      string sql_query = "UPDATE "+TABLE_NAME+" SET DATA"+" = "+"'"+data+"', STATUS = "+stat+" WHERE USERNAME = "+"'"+user_name+"'"+";";
      db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),call_back,0,&error);
      if(db == SQLITE_OK){
      }
      else{
         cout<<" Error in Update : "<<error<<endl;
      }
   }
   else{
      sqlite3_errmsg(database_object);
   }
   sqlite3_close(database_object);
}
bool socket_update(string user_name,string status,int id = 0){
   int db;
   sqlite3 *database_object=NULL;
   char *error = 0;
   stringstream temp;
   temp << id;
   string data = temp.str();
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   if(db == SQLITE_OK){
      string sql_query = "UPDATE "+TABLE_NAME+" SET ID = "+data+", STATUS = "+status+" WHERE USERNAME = "+"'"+user_name+"'"+";";
      db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),call_back,0,&error);
      if(db == SQLITE_OK){
         return true;
      }
      else{
         cout<<" Error in Socket Update : "<<error<<endl;
         return false;
      }
   }
   else{
      sqlite3_errmsg(database_object);
   }
   sqlite3_close(database_object);
}
record retrieve(int id){
   int db;
   stringstream temp2;
   string s2;
   temp2 << id;
   s2 = temp2.str();
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   clean_result();
   if(db == SQLITE_OK){
      string sql_query = "SELECT * FROM "+TABLE_NAME+" WHERE id = "+s2+";";
      db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),call_back,0,&error);
      if(db == SQLITE_OK){
      }
      else{
         cout<<"Error in retrieve "<<error<<endl;
      }
   }
   else{
      sqlite3_errmsg(database_object);
   }
   sqlite3_close(database_object);
   return result;
}
int count_active_users(void){
	int db;
	int count = 0;
   	sqlite3 *database_object=NULL;
   	char *error = 0;
   	db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   	if(db == SQLITE_OK){
    	string sql_query = "SELECT COUNT(*) FROM "+TABLE_NAME+" WHERE STATUS = 1;";
      	db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),count_callback,(void *)&count,&error);
      	if(db == SQLITE_OK){
      	}
      	else{
        	   cout<<error<<endl;
      	}
   	}
   	else{
         sqlite3_errmsg(database_object);
   	}
   	sqlite3_close(database_object);
   	return count;
}
void clean_result(void){
   result.username = "";
   result.message = "";
}
int retrieve_status(string user_name){
   int db;
   sqlite3 *database_object=NULL;
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   //clean_result();
   record user_status;
   if(db == SQLITE_OK){
      string sql_query = "SELECT STATUS FROM "+TABLE_NAME+" WHERE USERNAME = "+"'"+user_name+"';";
      db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),call_back_status,(void *)&user_status,&error);
      if(db == SQLITE_OK){
      }
      else{
         cout<<"Error in retrieve "<<error<<endl;
      }
   }
   else{
      sqlite3_errmsg(database_object);
   }
   sqlite3_close(database_object);
   if(user_status.status == "1"){
      return 1;
   }
   else if(user_status.status == "0"){
      return 0;
   }
   else 
      return -1;
}
record* retrieve_active_user(void){
   int db;
   int K = 0;
   sqlite3 *database_object=NULL;
   ALL_USER = NULL;
   ALL_USER = new record[count_active_users()];
   char *error = 0;
   db = sqlite3_open(DATABASE_NAME.c_str(),&database_object);
   clean_result();
   if(db == SQLITE_OK){
      string sql_query = "SELECT USERNAME FROM "+TABLE_NAME+" WHERE STATUS = 1;";
      db = sqlite3_exec(database_object,(const char *)sql_query.c_str(),all_user,(void *)&K,&error);
      if(db == SQLITE_OK){
      }
      else{
         cout<<"Error in retrieve "<<error<<endl;
      }
   }
   else{
      sqlite3_errmsg(database_object);
   }
   sqlite3_close(database_object);
   GLOBAL = 0;
   return ALL_USER;
}
#endif