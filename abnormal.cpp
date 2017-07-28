#include"DataBase.h"
#include <stdio.h> 
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <map>
using namespace std;

char * rdm_str(int in_len)
{
	char *ch = (char *)malloc(in_len + 1);
	int i;
	if (ch == 0)
		return 0;
	srand(time(NULL) + rand());    //初始化随机数的种子
	for (i = 0; i < in_len; i++)
		ch[i] = rand() % 75 + 48;      //得到的随机数为可显示字符
	ch[i] = 0;
	return ch;
}


// 非寻常测试
int abnormal_test()
{
	DBHANDLE* db = NULL;
	clock_t start, finish;
	start = clock();


	db = db_open("demoo", READ_AND_WRITE);


	srand(time(NULL) + rand());
	int rdm_op_num = rand() % 100 + 1;
	
	
	for (int i = 0; i < 10000; i++)
	{
		string key = rdm_str(rand() % 8 + 1);
		string value = rdm_str(rand() % 8 + 1);
		db_store(db, key.c_str(), value.c_str(), INSERT);
	}
	for (int i = 0; i < 100; i++)
	{
		string chosen_key = rdm_str(rand() % 8 + 1);
		string chosen_value = rdm_str(rand() % 8 + 1);
		db_store(db, chosen_key.c_str(), chosen_value.c_str(), INSERT);
		cout <<"Key: "<<chosen_key<<"	Value:" << db_fetch(db, chosen_key.c_str()) << endl;
		db_delete(db, chosen_key.c_str());
		cout << "Key: " << chosen_key << "	Value:" << db_fetch(db, chosen_key.c_str()) << endl;
		db_delete(db, chosen_key.c_str());
		cout << "Key: " << chosen_key << "	Value:" << db_fetch(db, chosen_key.c_str()) << endl;
		db_store(db, chosen_key.c_str(),"	ugly",REPLACE);
		cout << "Key: " << chosen_key << "	Value:" << db_fetch(db, chosen_key.c_str()) << endl;
		db_store(db, chosen_key.c_str(), "wzy is handsome", INSERT);
		cout << "Key: " << chosen_key << "	Value:" << db_fetch(db, chosen_key.c_str()) << endl;
		cout << endl;
	}
	


	db_close(db);
	finish = clock();
	cout << "TIME CONSUMED:" << finish - start << "/" << CLOCKS_PER_SEC << " (s) " << endl;
	system("pause");
	return 0;
}