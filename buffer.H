#include <queue>
#include <vector>
#include <set>
#include "BPlusTree.H"
using namespace std;


struct BufferToken   // ��¼һ�β����Ĳ������ͣ�key,value
{
	KEY_TYPE key;
	const char* data;
	int op;
};

class Buffer    // buffer��
{

public:

	queue<BufferToken> bufferqueue;
	set<KEY_TYPE> keys;
	int capicity;         // buffer����

	Buffer()
	{
		capicity = 4;
	}

};