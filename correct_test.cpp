#include"DataBase.h"
#include <stdio.h> 
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <map>
using namespace std;


char * gnrt_rdmstr(int in_len)
{
	char *ch = (char *)malloc(in_len + 1);
	int i;
	if (ch == 0)
		return 0;
	srand(time(NULL) + rand());    //初始化随机数的种子
	for (i = 0; i < in_len; i++)
		ch[i] = rand() % 75 + 48;      //控制得到的随机数为可显示字符
	ch[i] = 0;
	return ch;
}

int main()
{
	DBHANDLE* db = NULL;
	clock_t start, finish;
	start = clock();


	db = db_open("demo", READ_AND_WRITE); 

	map<string, string> rdm_rpl_map;
	map<string, string> rdm_add_map;
	vector<string> rdm_dlt_keys;
	vector<string> rdm_rpl_keys;
	vector<string> rdm_add_keys;
	srand(time(NULL) + rand());
	int rdm_dlt_num = rand() % 100+1;
	srand(time(NULL) + rand());
	int rdm_rpl_num = rand() % 100+1;
	srand(time(NULL) + rand());
	int rdm_add_num = rand() % 100 + 1;
	/*string key = gnrt_rdmstr(rand() % 8 + 1);
	string value = gnrt_rdmstr(rand() % 8 + 1);
	db_store(db, key.c_str(), value.c_str(), INSERT);
	cout << db_fetch(db, key.c_str()) << endl;
	db_store(db, key.c_str(), "HHH", REPLACE);
	cout << db_fetch(db, key.c_str()) << endl;
	db_delete(db, key.c_str());
	cout << db_fetch(db, key.c_str()) << endl;
	cout << endl;*/

	for (int i = 0; i < 10000; i++)
	{
		string key = gnrt_rdmstr(rand() % 8 + 1);
		string value = gnrt_rdmstr(rand() % 8 + 1);
		db_store(db, key.c_str(), value.c_str(),INSERT);
		if (i % rdm_dlt_num == 0) rdm_dlt_keys.push_back(key);
		if (i % rdm_rpl_num == 0) rdm_rpl_keys.push_back(key);
	}
	for (int i = 0; i < rdm_rpl_keys.size(); i++)
	{
		string rpl_str = gnrt_rdmstr(rand() % 8 + 1);
		db_store(db, rdm_rpl_keys[i].c_str(), rpl_str.c_str(), REPLACE);
		rdm_rpl_map[rdm_rpl_keys[i]] = rpl_str;
	}
	for (int i = 0; i < rdm_rpl_keys.size(); i++)
	{
		if (rdm_rpl_map[rdm_rpl_keys[i]] == db_fetch(db, rdm_rpl_keys[i].c_str()))
		{
			cout << "Repalce succsessfully. Key: " << rdm_rpl_keys[i] << "		New Value: " << rdm_rpl_map[rdm_rpl_keys[i]] << endl;
		}
	}
	for (int i = 0; i < rdm_dlt_keys.size(); i++)
	{
		db_delete(db, rdm_dlt_keys[i].c_str());
	}
	for (int i = 0; i < rdm_dlt_keys.size(); i++)
	{
		if (db_fetch(db, rdm_dlt_keys[i].c_str()) == "This key doesn't exist")
		{
			cout << "Delete successfully. Delete key: " << rdm_dlt_keys[i] << endl;
		}
	}
	for (int i = 0; i < rdm_add_num; i++)
	{
		string key = gnrt_rdmstr(rand() % 8 + 1);
		string value = gnrt_rdmstr(rand() % 8 + 1);
		db_store(db, key.c_str(), value.c_str(), INSERT);
		rdm_add_map[key] = value;
		rdm_add_keys.push_back(key);
	}
	for (int i = 0; i < rdm_add_num; i++)
	{
		string key = rdm_add_keys[i];
		if (db_fetch(db, key.c_str()) == rdm_add_map[key])
		{
			cout << "Store successfullt. Key: " << key << "		Value: " << rdm_add_map[key] << endl;
		}
	}
	

	db_close(db);
	finish = clock();
	cout << "TIME CONSUMED:" << finish - start << "/" << CLOCKS_PER_SEC << " (s) " << endl;
	system("pause");
	return 0;
}