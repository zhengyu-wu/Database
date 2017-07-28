#include"BPlusTree.H"
#include<fstream>
#include "buffer.H"

#define READ_ONLY 0
#define READ_AND_WRITE 1

#define INSERT 0
#define REPLACE 1
#define INSERT_OR_REPLACE 2
#define DELETE 3

#define AUTO 0
#define COERCIVE 1


class DBHANDLE      // 句柄类
{
public:
	DBHANDLE();
	~DBHANDLE();
	
	const char* DBFind(KEY_TYPE key);					// 查找
	bool DBInsert(KEY_TYPE key, const char* datastr);	// 插入
	bool DBDelete(KEY_TYPE key);						// 删除
	bool DBReplace(KEY_TYPE key, const char* datastr);	// 修改
	bool DBInsertorReplace(KEY_TYPE key, const char* datastr);
	KEY_TYPE GetCurrentKey();
	const char* GetCurrentDatastr();
	DATA_TYPE GetCurrentData();
	bool NextPos();
	bool PrevPos();
	bool Open(const char* filename,int opentype);
	bool Close();

	bool Rewind();
	void FreeRS(DATA_TYPE* dataptr);                    // 空间重利用
	bool UseRS(const char* datastr, DATA_TYPE* dataptr); // 使用空闲链表中的空间
	bool ConbineFS(RecordSpace* freespace);              // 合并空闲链表中的空间

	void UseBuffer(int optype);                          // 使用buffer
	Buffer *getbuffer(){ return this->buffer; }
private:

	std::fstream dataf;	
	LeafNode* head;
	LeafNode* tail;
	int temppos;
	LeafNode* tempnode;
	Node* root;
	int opentype;
	std:: string filename;
	RecordSpace* freespacehead;
	RecordSpace* freespacetail;
	int datatail;
	Buffer *buffer;
};

LeafNode* ReadRecords(std::fstream& fio, int RScount);  // 读该叶子节点上的数据

Node* buildBPlusTree(Node**Narray,int Ncount);          // 建立一棵树

DBHANDLE* db_open(const char*pathname, int oflag);

void db_close(DBHANDLE *db);

int db_store(DBHANDLE *db, const char* key, const char* data, int flag);

char* db_fetch(DBHANDLE *db, const char* key);

void db_rewind(DBHANDLE *db);

char* db_nextrec(DBHANDLE *db, char*key);

int db_delete(DBHANDLE *db, const char* key);

int prinf(Node* root, ofstream& fio);