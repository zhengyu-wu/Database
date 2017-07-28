#include <queue>
#include <vector>
#include <set>
#include "BPlusTree.H"
using namespace std;


struct BufferToken   // 记录一次操作的操作类型，key,value
{
	KEY_TYPE key;
	const char* data;
	int op;
};

class Buffer    // buffer类
{

public:

	queue<BufferToken> bufferqueue;
	set<KEY_TYPE> keys;
	int capicity;         // buffer容量

	Buffer()
	{
		capicity = 4;
	}

};
