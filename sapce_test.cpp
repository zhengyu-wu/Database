#include"DataBase.h"
#include <stdio.h> 
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <map>
using namespace std;


int space_test()
{
	DBHANDLE* db = NULL;
	clock_t start, finish;
	start = clock();


	
	db = db_open("space_1", READ_AND_WRITE);


	
	for (int i = 0; i < 200000; i++)
	{
		stringstream ss;
		ss << i;

		string key;
		string value;
		ss >> key;
		value = key;
		db_store(db, key.c_str(), value.c_str(), INSERT);
	}
	for (int i = 0; i < 70000; i++)
	{
		stringstream ss;
		ss << i;

		string key;
		ss >> key;
		
		db_delete(db, key.c_str());
	}

	for (int i = 200000; i < 400000; i++)
	{
		stringstream ss;
		ss << i;

		string key;
		string value;
		ss >> key;
		value = key;
		db_store(db, key.c_str(), value.c_str(), INSERT);
	}
	

	db_close(db);
	finish = clock();
	cout << "TIME CONSUMED:" << finish - start << "/" << CLOCKS_PER_SEC << " (s) " << endl;
	cout << "Operation over." << endl;
	system("pause");
	return 0;
}