#include <iostream>
#include <cstdlib>
#include <sqlite3.h>
#include <string>
#include <stdio.h>
#include <sstream>
using namespace std;
#include "mysqlite.h"
//typedef struct user_data record;
int main(int argc, char const *argv[])
{
	record R;
	create_database("Networking_Assignmenet3.db");
	create_table("USERS");
	insert("aloke57","Hello",1,4);
	R = retrieve(4);
	cout<<" In main programme : \n User id : "<<R.id<<" Username : "<<R.username<<" Messege : "<<R.messege<<" Status : "<<R.status<<endl;
	stringstream trimmer;
	string s;
	cin>>s;
	cout<<" BEFORE TRIMM : "<<s<<endl;
	trimmer << s;
	s.clear();
	trimmer >> s;
	cout<<" AFTER TRIMM : "<<s<<endl;
	update(s,"hi");
	R = retrieve(4);
	cout<<" In main programme updated : \n Userid : "<<R.id<<" Username : "<<R.username<<" Messege : "<<R.messege<<" Status : "<<R.status<<endl;


	/*insert("trish24","LOL",1,5);
	R = retrieve(5);
	cout<<" In main programme : \n User id : "<<R.id<<" Username : "<<R.username<<" Messege : "<<R.messege<<" Status : "<<R.status<<endl;
	update(5,"ROFL");
	R = retrieve(5);
	cout<<" In main programme updated : \n Userid : "<<R.id<<" Username : "<<R.username<<" Messege : "<<R.messege<<" Status : "<<R.status<<endl;
	*/
	return 0;
}