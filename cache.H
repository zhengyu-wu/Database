#include<vector>
#include<string>
#include<queue>
#include<map>
using namespace std;

class CacheRec   // cache类
{
public:
	map<string,string> memory;             // 存放最近操作过的<key,value>
	void insert_cache(string, string);     
	int max_num;                           // cache容量
	string search(string key);
	bool continueinsert(string key, string value);
	CacheRec()
	{
		memory.clear();
		max_num = 1000;
	}

};

void CacheRec::insert_cache(string key, string value)
{
	if (memory.size() == 1000)          // 若cache达到了容量，释放一个<key,value>
	{
		map<string, string>::iterator it;
		it = memory.begin();
		memory.erase(it);
	}
	memory[key] = value;
}

string CacheRec::search(string key)       
{
	if (memory.find(key) != memory.end())
	{
		return memory[key];
	}
	return "notfind";

}

bool CacheRec::continueinsert(string key, string value)
{
	if (memory.find(key) != memory.end())
	{
		return false;
	}
	return true;
}