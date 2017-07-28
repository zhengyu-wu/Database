#include"DataBase.h"
#include <stdio.h> 
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <map>
using namespace std;


char * gnrt_rdm_str(int in_len)
{
	char *ch = (char *)malloc(in_len + 1);
	int i;
	if ( ch == 0)
	return 0;
	srand(time(NULL) + rand());    //初始化随机数的种子
	for (i = 0; i < in_len; i++)
	ch[i] = rand() % 75 + 48;      //得到的随机数为可显示字符
	ch[i] = 0;
	return ch;
}

// 综合测试
int allround_test()
{
	DBHANDLE* db = NULL;
	clock_t start, finish;
	start = clock();
	
	
	fstream fio;
	fio.open("0.txt", ios::out);
	fio.close();
	fio.open("0.txt", ios::in | ios::out | ios::binary);
	db = db_open("004", READ_AND_WRITE);

	
	map<string, string> rec_map;
	vector<string> keys_vec;
	for (int i = 0; i < 100000; i++)
	{
		string key = gnrt_rdm_str(rand() % 8 + 1);
		string value = gnrt_rdm_str(rand() % 8 + 1);
		db_store(db, key.c_str(), value.c_str(), INSERT);
		rec_map[key] = value;
		keys_vec.push_back(key);
	}
	map<string, string>::iterator it;
	for (it = rec_map.begin(); it != rec_map.end(); it++)
	{
		string key = it->first;
		string value = it->second;
		if (db_fetch(db, key.c_str()) == value) 
		{
			cout << "Store and fetch correctly.	Key：" << key << "		value: " << value << endl;
		}
	}
	cout << endl;
	for (int i = 0; i < 500000; i++)
	{
		if (i % 37 == 0)
		{
			srand(time(NULL) + rand());
			int num = rand() / keys_vec.size();
			string key = keys_vec[num];
			db_delete(db, key.c_str());	
		}
		if ( i % 11 == 0)
		{
			string key = gnrt_rdm_str(rand() % 8 + 1);
			string value = gnrt_rdm_str(rand() % 8 + 1);
			db_store(db, key.c_str(), value.c_str(), INSERT);
			if (value == db_fetch(db, key.c_str()))
			{
				cout << "Add and fetch correctly.	Key：" << key << "		value: " << value << endl;
			}
		}
		if (i % 17 == 0)
		{
			srand(time(NULL) + rand());
			int num = rand() / keys_vec.size();
			string key = keys_vec[num];
			string newvalue = gnrt_rdm_str(rand() % 8 + 1);
			db_store(db, key.c_str(), newvalue.c_str(), REPLACE);	
			if (newvalue == db_fetch(db, key.c_str()))
			{
				cout << "Replace and fetch correctly.	Key：" << key << "		value: " << newvalue << endl;
			}
		}
	}

	db_close(db);
	finish = clock();
	cout << "TIME CONSUMED:" << finish - start << "/" << CLOCKS_PER_SEC << " (s) " << endl;
	system("pause");
	return 0;
}