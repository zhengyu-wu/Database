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


class DBHANDLE      // �����
{
public:
	DBHANDLE();
	~DBHANDLE();
	
	const char* DBFind(KEY_TYPE key);					// ����
	bool DBInsert(KEY_TYPE key, const char* datastr);	// ����
	bool DBDelete(KEY_TYPE key);						// ɾ��
	bool DBReplace(KEY_TYPE key, const char* datastr);	// �޸�
	bool DBInsertorReplace(KEY_TYPE key, const char* datastr);
	KEY_TYPE GetCurrentKey();
	const char* GetCurrentDatastr();
	DATA_TYPE GetCurrentData();
	bool NextPos();
	bool PrevPos();
	bool Open(const char* filename,int opentype);
	bool Close();

	bool Rewind();
	void FreeRS(DATA_TYPE* dataptr);                    // �ռ�������
	bool UseRS(const char* datastr, DATA_TYPE* dataptr); // ʹ�ÿ��������еĿռ�
	bool ConbineFS(RecordSpace* freespace);              // �ϲ����������еĿռ�

	void UseBuffer(int optype);                          // ʹ��buffer
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

LeafNode* ReadRecords(std::fstream& fio, int RScount);  // ����Ҷ�ӽڵ��ϵ�����

Node* buildBPlusTree(Node**Narray,int Ncount);          // ����һ����

DBHANDLE* db_open(const char*pathname, int oflag);

void db_close(DBHANDLE *db);

int db_store(DBHANDLE *db, const char* key, const char* data, int flag);

char* db_fetch(DBHANDLE *db, const char* key);

void db_rewind(DBHANDLE *db);

char* db_nextrec(DBHANDLE *db, char*key);

int db_delete(DBHANDLE *db, const char* key);

int prinf(Node* root, ofstream& fio);