#include"DataBase.h"

#include <stdio.h> 
#include <stdlib.h>
#include <ctime>
#include <sstream>
#include <map>
using namespace std;


char * generate_str(int in_len)
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

int write_buffer_test()
{
	DBHANDLE* db = NULL;
	clock_t start, finish;
	

	db = db_open("catchtest_0", READ_AND_WRITE);

	start = clock();
	for (int i = 0; i < 100000; i++)
	{
		string key = generate_str(rand() % 8 + 1);
		string value = generate_str(rand() % 8 + 1);
		BufferToken bt;
		bt.data = value.c_str();
		bt.key = key.c_str();
		bt.op = INSERT;
		//db->getbuffer()->keys.insert(bt.key);
		//db->getbuffer()->bufferqueue.push(bt);
		//db->UseBuffer(AUTO);
		db_store(db, key.c_str(), value.c_str(), INSERT);

	}
	
	db_close(db);
	finish = clock();
	cout << "TIME CONSUMED:" << finish - start << "/" << CLOCKS_PER_SEC << " (s) " << endl;
	system("pause");
	return 0;
}