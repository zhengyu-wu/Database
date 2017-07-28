#include "DataBase.h"


DBHANDLE::DBHANDLE()
{
	this->temppos = 0;
	this->opentype = 0;
	this->datatail = 0;
	this->freespacehead = NULL;
	this->freespacetail = NULL;
	buffer = new Buffer;
}

DBHANDLE::~DBHANDLE()
{
	if (!this)return;
	RecordSpace* freespace= this->freespacehead;
	if (freespace)
	while (freespace->Next)
	{
		freespace = freespace->Next;
		delete freespace->Prev;
	}
	if (root) 
	root->DeleteChildren();
	root = NULL;
	head = NULL;
	tail = NULL;
	if (buffer) delete buffer;
}

//重置
bool DBHANDLE::Rewind()
{
	this->tempnode = this->head;
	this->temppos = 0;
	return true;
}

//获得当前记录的Key
KEY_TYPE DBHANDLE::GetCurrentKey()
{
	if (this->tempnode!=NULL)
		return this->tempnode->GetKey(this->temppos);
	else return NULL;
}

//获得当前记录的data的字串
const char* DBHANDLE::GetCurrentDatastr()
{
	
	if (this->tempnode != NULL)
	{
		if (!dataf)
		{
			dataf.close();
			std::string datname = filename + ".dat";
			dataf.open(datname.c_str(), std::ios::in | std::ios::out | std::ios::binary);
		}
		DATA_TYPE data = this->tempnode->GetData(this->temppos);
		if (data == NULL)
			return NULL;
		dataf.seekg(data->offset);
		std::string* datastr =new std::string;
		for (int i = 0; i < data->length; i++)
			*datastr += dataf.get();
		return datastr->c_str();
	}
	else return NULL;
}

//获得当前记录的data
DATA_TYPE DBHANDLE::GetCurrentData()
{
	if (this->tempnode == NULL)
		return NULL;
		DATA_TYPE data = this->tempnode->GetData(this->temppos);
		return data;
}

//取下一条记录
bool DBHANDLE::NextPos()
{
	if (this->tempnode == NULL)
		return false;
	if (this->tempnode->GetCount() > this->temppos + 1)
	{
		this->temppos++;
		return true;
	}
	else if (this->tempnode->NextNode!=NULL)
	{
		this->tempnode = this->tempnode->NextNode;
		this->temppos=0;
		return true;
	}
	else return false;
}

//取上一条记录
bool DBHANDLE::PrevPos()
{
	if (this->temppos > 0)
	{
		this->temppos--;
		return true;
	}
	else if (this->tempnode->PrevNode != NULL)
	{
		this->tempnode = this->tempnode->PrevNode;
		this->temppos = this->tempnode->GetCount()-1;
		return true;
	}
	else return false;
}

//插入
bool DBHANDLE::DBInsert(KEY_TYPE key, const char* datastr)
{
	if (this->opentype == READ_ONLY)
		return false;
	string* keystr = new string;
	*keystr = key;
	DATA_TYPE* getdataptr = new DATA_TYPE;
	*getdataptr = NULL;
	Node* getnode = Search(root,getdataptr,key);
	if (*getdataptr != NULL)
	{
		return false;
	}
	else
	{
		if (root == NULL)
		{
			getnode = new LeafNode;
			root = getnode;
			root->SetType(NODE_TYPE_ROOT);
			this->head = (LeafNode*)root;
			this->tail = (LeafNode*)root;
		}
		DATA_TYPE* dataptr = new DATA_TYPE;
		*dataptr = NULL;
		UseRS(datastr,dataptr);
		if (getnode->InsertLeaf(keystr->c_str(), *dataptr))
		{
			this->root = (root->GetFather()) ? root->GetFather() : root;
			if (tail)
				this->tail = (tail->NextNode) ? tail->NextNode : tail;
			return true;
		}
		else return false;
	}
}

//改写
bool DBHANDLE::DBReplace(KEY_TYPE key, const char* datastr)
{
	if (this->opentype == READ_ONLY)
		return false;
	DATA_TYPE* getdataptr = new DATA_TYPE;
	*getdataptr = NULL;
	Node* getnode = Search(root, getdataptr, key);
	if (*getdataptr == NULL)
		return false;
	else
	{
		FreeRS(getdataptr);
		if (*getdataptr != NULL)
			delete (*getdataptr);
		*getdataptr = NULL;
		UseRS(datastr, getdataptr);
		for (int i = 0; i < getnode->GetCount();i++)
		if (strcmp(getnode->GetKey(i), key) == 0)
			getnode->SetData(i,*getdataptr);
		return true;
	}
}

//用于插入或改写
bool DBHANDLE::DBInsertorReplace(KEY_TYPE key, const char* datastr)
{
	if (this->opentype == READ_ONLY)
		return false;
	DATA_TYPE* getdataptr = new DATA_TYPE;
	*getdataptr = NULL;
	Node* getnode = Search(root, getdataptr, key);
	if (*getdataptr == NULL)
	{
		string* keystr = new string;
		*keystr = key;
		if (root == NULL)
		{
			getnode = new LeafNode;
			root = getnode;
			root->SetType(NODE_TYPE_ROOT);
			this->head = (LeafNode*)root;
			this->tail = (LeafNode*)root;
		}
		DATA_TYPE* dataptr = new DATA_TYPE;
		*dataptr = NULL;
		UseRS(datastr,dataptr);
		if (getnode->InsertLeaf(keystr->c_str(), *dataptr))
		{
			this->root = (root->GetFather()) ? root->GetFather() : root;
			if (tail)
				this->tail = (tail->NextNode) ? tail->NextNode : tail;
			return true;
		}
		else return false;
	}
	else
	{
		FreeRS(getdataptr);
		if (*getdataptr != NULL)
		delete (*getdataptr);
		*getdataptr = NULL;
		UseRS(datastr,getdataptr);
		return true;
	}
}

//删除key所对应的记录
bool DBHANDLE::DBDelete(KEY_TYPE key)
{
	if (this->opentype == READ_ONLY)
		return false;
	DATA_TYPE* getdataptr = new DATA_TYPE;
	*getdataptr = NULL;
	Node* getnode = Search(root, getdataptr, key);
	if (*getdataptr != NULL)
	{
		FreeRS(getdataptr);
		*getdataptr = NULL;
		if (getnode->Delete(key))
		{
			if (root->GetCount() == 0)
			{
				delete root;
				root = NULL;
				this->head = NULL;
				this->tail = NULL;
				return true;
			}
			if (root->GetCount() == 1 && root->GetPointer(0) != NULL)
			{
				root = root->GetPointer(0);
				delete root->GetFather();
				root->SetFather(NULL);
				root->SetType(NODE_TYPE_ROOT);
			}
			if (this->head == NULL)
			{
				Node* temp = root;
				if (temp)
				while (temp->GetPointer(0)!=NULL)
					temp = temp->GetPointer(0);
				head = (LeafNode*)temp;
			}
			if (this->tail == NULL)
			{
				Node* temp = root;
				if (temp)
				while (temp->GetPointer(temp->GetCount()-1)!=NULL)
					temp = temp->GetPointer(temp->GetCount() - 1);
				tail = (LeafNode*)temp;
			}
			return true;
		}
	}
	else return false;
}

//如果前后记录相连，将freespace和后面一个freespace记录结合成一个
bool DBHANDLE::ConbineFS(RecordSpace* freespace)
{
	if (freespace == NULL)
		return false;
	RecordSpace* nextfs = freespace->Next;
	if (nextfs != NULL)
	{
		if (nextfs->offset == freespace->offset + freespace->length)
		{
			if (nextfs == this->freespacetail)
				this->freespacetail = freespace;
			freespace->length += nextfs->length;
			freespace->Next = nextfs->Next;
			if(freespace->Next)
				freespace->Next->Prev= freespace;
			delete nextfs;
			nextfs = NULL;
			return true;
		}
	}
	return false;
}

//释放该条记录，加入空闲链表
void DBHANDLE::FreeRS(DATA_TYPE* dataptr)
{
	if (dataptr == NULL)
		return;
	if (*dataptr == NULL)
		return;
	//in the end
	if ((*dataptr)->offset + (*dataptr)->length == this->datatail)
	{
		datatail = (*dataptr)->offset;
		RecordSpace* temp = this->freespacetail;
		if (temp != NULL)
		{
			if (temp->offset + temp->length == datatail)
			{
				datatail = temp->offset;
				this->freespacetail = temp->Prev;
				if (this->freespacetail)
					this->freespacetail->Next = NULL;
				if (this->freespacehead == temp)
					this->freespacehead = NULL;
				delete temp;
				temp = NULL;
			}
		}
	}
	//in the middle position
	else
	{
		DATA_TYPE freedata = new RecordSpace;
		freedata->length = (*dataptr)->length;
		freedata->offset = (*dataptr)->offset;
		RecordSpace* temp = this->freespacehead;
		if (temp == NULL)
		{
			freedata->Prev = NULL;
			freedata->Next = NULL;
			this->freespacehead = freedata;
			this->freespacetail = freedata;
			return;
		}
		else if (freedata->offset < temp->offset)
		{
			temp->Prev = freedata;
			freedata->Prev = NULL;
			freedata->Next = temp;
			this->freespacehead = freedata;
			ConbineFS(freedata);
			return;
		}
		else
		{
			temp = temp->Next;
			while (temp != NULL)
			{
				if (temp->offset > freedata->offset)
					break;
				else
					temp = temp->Next;
			}
			if (temp != NULL)
			{
				freedata->Prev = temp->Prev;
				if (temp->Prev!=NULL)
				temp->Prev->Next = freedata;
				freedata->Next = temp;
				temp->Prev = freedata;
				if (ConbineFS(freedata->Prev))
					*dataptr = NULL;
				if(ConbineFS(temp->Prev));
					*dataptr = NULL;
			}
			else
			{
				freespacetail->Next = freedata;
				freedata->Prev = freespacetail;
				freedata->Next = NULL;
				freespacetail = freedata;
				ConbineFS(freedata->Prev);
				*dataptr = NULL;
			}
		}
	}
}

bool DBHANDLE::UseRS(const char* datastr,DATA_TYPE* dataptr)
{
	RecordSpace* temp = freespacehead;
	if (dataptr == NULL)
		return false;
	if (*dataptr==NULL)
		*dataptr = new RecordSpace;
	(*dataptr)->length = strlen(datastr);
	while (temp != NULL)
	if (temp->length < (*dataptr)->length)
	{
		temp = temp->Next;
	}
	else break;
	if (temp == NULL)
	{
		(*dataptr)->offset = datatail;
		if (!dataf)
		{
			dataf.close();
			std::string datname = filename + ".dat";
			dataf.open(datname.c_str(), std::ios::in | std::ios::out | std::ios::binary);
		}
		if (!dataf)
			cout << "uselessusenull";
		this->dataf.seekp(datatail);
		(this->dataf) << datastr;
		datatail = (*dataptr)->offset + (*dataptr)->length;
		return (*dataptr);
	}
	else
	{
		if (temp->length == strlen(datastr))
		{
			if (temp == freespacehead)			
			{
				freespacehead = temp->Next;
				if (freespacehead)
				freespacehead->Prev = NULL;
			}
			if (temp == freespacetail)
			{
				freespacetail = temp->Prev;
				if (freespacetail)
				freespacetail->Next = NULL;
			}
			if (temp->Prev!=NULL)
			temp->Prev->Next = temp->Next;
			if (temp->Next!=NULL)
			temp->Next->Prev = temp->Prev;


			if (!dataf)
			{
				dataf.close();
				std::string datname = filename + ".dat";
				dataf.open(datname.c_str(), std::ios::in | std::ios::out | std::ios::binary);
			}
			if (!dataf)
				cout << "uselesselse";
			this->dataf.seekp(temp->offset);
			(this->dataf) << datastr;
			(*dataptr)->offset = temp->offset;
			delete temp;
			temp = NULL;
			return (*dataptr);
		}
		else
		{
			temp->length = temp->length - strlen(datastr);
			if (!dataf)
			{
				dataf.close();
				std::string datname = filename + ".dat";
				dataf.open(datname.c_str(), std::ios::in | std::ios::out | std::ios::binary);
			}
			if (!dataf)
				cout << "uselesselse";
			this->dataf.seekp(temp->offset);
			(this->dataf) << datastr;
			(*dataptr)->offset = temp->offset;
			temp->offset = temp->offset + strlen(datastr);
			return (*dataptr); 
		}
	}

}

const char* DBHANDLE::DBFind(KEY_TYPE key)
{
	DATA_TYPE* getdataptr = new DATA_TYPE;
	*getdataptr = NULL;
	Node* getnode = Search(root, getdataptr, key);
	if (*getdataptr != NULL)
	{
		if (!dataf)
		{
			dataf.close();
			std::string datname = filename + ".dat";
			dataf.open(datname.c_str(), std::ios::in | std::ios::out | std::ios::binary);
		}
		if (!dataf)
			cout << "uselessfind";
		DATA_TYPE data = *getdataptr;
		this->dataf.seekg(data->offset);
		std::string* str = new string;
		char ch;
		for (int i = 0; i < data->length; i++)
		{
			ch = dataf.get();
			str->push_back(ch);
		}

		return str->c_str();
	}
	else
		return "This key doesn't exist";
}

LeafNode* ReadRecords(std::fstream& fio, int RScount)	// 把一定条数的data(不超过max_used)存入一个叶子节点
{
	if (RScount > MAX_USED)
		return NULL;
	LeafNode*tempLN = new LeafNode;
	int KeyLength = 0;
	int offset = 0, length = 0;
	char blank = ' ';
	for (int i = 0; i < RScount; i++)
	{
		DATA_TYPE* thisRS = new DATA_TYPE;
		
		fio >> KeyLength;
		fio.get();
		std::string* keystr = new std::string;
		for (int j = 0; j < KeyLength; j++)
			*keystr += fio.get();
		fio.get();
		fio >> offset >> blank >> length >> blank;
		(*thisRS) = new RecordSpace;
		(*thisRS)->length = length;
		(*thisRS)->offset = offset;
		tempLN->SetKey(i, keystr->c_str());
		tempLN->SetData(i, (*thisRS));
		tempLN->IncCount();
	}
	return tempLN;
}

Node* buildBPlusTree(Node**Narray, int Ncount)
{
	if (Ncount == 0)
		return NULL;
	int FNcount = Ncount / MIN_USED;
	if (FNcount <= 1)
	{
		InternalNode* root = new InternalNode();
		root->SetFather(NULL);
		for (int i = 0; i < Ncount; i++)
		{
			root->SetKey(i, Narray[i]->GetKey(0));
			root->SetPointer(i, Narray[i]);
			Narray[i]->SetFather(root);
			root->IncCount();
		}
		return root;
	}
	else
	{
		InternalNode**FNarray = new InternalNode*[FNcount];
		int restofNcount = Ncount, FNcounti = 0,Ncounti = 0;
		while (restofNcount >= MAX_USED)
		{
			FNarray[FNcounti] = new InternalNode;
			for (int i = 0; i < MIN_USED; i++,Ncounti++)
			{
				FNarray[FNcounti]->SetKey(i, Narray[Ncounti]->GetKey(0));
				FNarray[FNcounti]->SetPointer(i, Narray[Ncounti]);
				Narray[Ncounti]->SetFather(FNarray[FNcounti]);
				FNarray[FNcounti]->IncCount();
			}
			restofNcount -= MIN_USED;
			FNcounti++;
		}
		FNarray[FNcounti] = new InternalNode;
		for (int i = 0; i < restofNcount; i++, Ncounti++)
		{
			FNarray[FNcounti]->SetKey(i, Narray[Ncounti]->GetKey(0));
			FNarray[FNcounti]->SetPointer(i, Narray[Ncounti]);
			Narray[Ncounti]->SetFather(FNarray[FNcounti]);
			FNarray[FNcounti]->IncCount();
		}
		return buildBPlusTree((Node**)FNarray,FNcount);
	}
}

bool DBHANDLE::Open(const char* filename, int opentype)
{
	
	this->filename = filename;
	std:: string idxname = filename;
	idxname += ".idx";
	std::fstream idxfio;
	std::string datname = filename;
	datname += ".dat";
	this->opentype = opentype;

	idxfio.open(idxname.c_str(), std::ios::in | std::ios::out | std::ios::binary);
	this->dataf.open(datname.c_str(), std::ios::in | std::ios::out | std::ios::binary);   // 可读可写，二进制

	int FScount = 0;
	idxfio >> FScount;

	if (!idxfio)                                      // 若是空文件，则初始化空闲链表
	{
		cout << "create a new file."<<endl;
		this->dataf.open(datname.c_str(), std::ios::out);
		this->dataf.close();
		this->dataf.open(datname.c_str(), std::ios::in | std::ios::out | std::ios::binary);

		this->datatail = 0;
		this->freespacehead = NULL;
		this->freespacetail = NULL;
		this->head = NULL; this->tail = NULL; this->root = NULL;
		return true;
	}


	while (idxfio.get() == '/')
		continue;

	int offset = 0,length = 0; 
	char blank;

	RecordSpace* thisFS = NULL,*lastFS = NULL;
	this->freespacehead = NULL;
	while (FScount != 0)                                         // 从idx中读出空闲链表
	{
		idxfio >> offset >>blank >> length>>blank;
		thisFS = new RecordSpace();
		thisFS->offset = offset;
		thisFS->length = length;
		thisFS->Prev = lastFS;
		if (lastFS == NULL)
			this->freespacehead= thisFS;
		else lastFS->Next = thisFS;
		lastFS = thisFS;
		FScount--;
	}
	this->freespacetail = thisFS;
	if (this->freespacetail)
	this->freespacetail->Next = NULL;
												
	int RScount=0, KeyLength = 0;
	idxfio >> RScount;
	while (idxfio.get() == '/')
		continue;

	if (RScount == 0)
	{
		idxfio.close();
		return true;
	}

	LeafNode* tempLN = NULL;                                  //  从idx中读出叶子节点链表
	LeafNode* lastLN = NULL;
	if (RScount<MAX_USED)
	{
		this->root = ReadRecords(idxfio, RScount);
		this->root->SetType(NODE_TYPE_ROOT);
		this->head =  (LeafNode*) this ->root;
		this->tail = this->head;
	}
	else
	{
		int LNcount = RScount / MIN_USED;
		Node** LNarray = new Node*[LNcount];
		lastLN = ReadRecords(idxfio, MIN_USED);
		this->head = lastLN;
		LNarray[0] = lastLN;
		int LNarrayi = 1;
		RScount -= MIN_USED;
		while (RScount >= MAX_USED)
		{
			tempLN = ReadRecords(idxfio, MIN_USED);
			tempLN->PrevNode = lastLN;
			if (lastLN != NULL)
				lastLN->NextNode = tempLN;
			LNarray[LNarrayi] = tempLN;
			RScount -= MIN_USED;
			LNarrayi++;
			lastLN = tempLN;
		}

		tempLN = ReadRecords(idxfio, RScount);
		tempLN->PrevNode = lastLN;
		if (lastLN != NULL)
			lastLN->NextNode = tempLN;
		LNarray[LNarrayi] = tempLN;

		this->tail = tempLN;
		this->root = buildBPlusTree(LNarray,LNcount);          // 建立树的拓扑结构
		this->root->SetType(NODE_TYPE_ROOT);
	}
	idxfio >> datatail;
	return true;
}

bool DBHANDLE::Close()
{
	cout << "Closing file..." << endl;
	if (opentype == READ_AND_WRITE)
	{
		std::string idxname = filename + ".idx";
		std::ofstream idxfio;
		idxfio.open(idxname.c_str());
		idxfio.seekp(0);
		idxfio << "//////////-";
		int FScount = 0;
		RecordSpace* FStemp = this->freespacehead;
		while (FStemp != NULL)
		{
			FScount++;
			idxfio << FStemp->offset << ':' << FStemp->length << '/';
			FStemp = FStemp->Next;
		}
		int location = idxfio.tellp();
		
		idxfio << "//////////-";
		this->Rewind();
		int DScount = prinf(root, idxfio);
		idxfio << datatail;
		idxfio.seekp(location);
		idxfio << DScount;
		idxfio.seekp(0);
		idxfio << FScount;
		idxfio.close();
	}
	dataf.close();
	root->DeleteChildren();
	

	RecordSpace* temp = this->freespacehead;
	RecordSpace* tempnext = NULL;
	while (temp != NULL)
	{
		tempnext = temp->Next;
		delete temp;
		temp = tempnext;
	}
	cout << "Close file successfully!" << endl;
	return true;

}

void DBHANDLE::UseBuffer(int optype)
{
	if (optype == AUTO)
	{
		int num = buffer->bufferqueue.size();
		if (num <= buffer->capicity) return;
		while (buffer->bufferqueue.empty())
		{
			BufferToken bt = buffer->bufferqueue.front();
			buffer->bufferqueue.pop();
			if (bt.op == DELETE)
			{
				DBDelete(bt.key);
			}
			if (bt.op == INSERT)
			{
				if (buffer->keys.find(bt.key) == buffer->keys.end())
				DBInsert(bt.key ,bt.data);
			}
			if (bt.op == REPLACE)
			{
				DBReplace(bt.key, bt.data);
			}
		}
	}
	if (optype == COERCIVE)
	{
		while (buffer->bufferqueue.empty())
		{
			BufferToken bt = buffer->bufferqueue.front();
			buffer->bufferqueue.pop();
			if (bt.op == DELETE)
			{
				DBDelete(bt.key);
			}
			if (bt.op == INSERT)
			{
				DBInsert(bt.key, bt.data);
			}
			if (bt.op == REPLACE)
			{
				DBReplace(bt.key, bt.data);
			}
		}
	}
}

DBHANDLE* db_open(const char*pathname, int oflag)
{
	DBHANDLE* db = new DBHANDLE;
	db->Open(pathname, oflag);

	return db;
}

void db_close(DBHANDLE *db)
{
	db->Close();
}

int db_store(DBHANDLE *db, const char* key, const char* data, int flag)
{
	if (flag == INSERT)
		return db->DBInsert(key, data);
	else if (flag == REPLACE)
		return db->DBReplace(key, data);
	else if (flag == INSERT_OR_REPLACE)
		return db->DBInsertorReplace(key, data);
	else return -1;
}

char* db_fetch(DBHANDLE *db, const char* key)
{
	return (char*)db->DBFind(key);
}

void db_rewind(DBHANDLE *db)
{
	db->Rewind();
}

int db_delete(DBHANDLE *db, const char* key)
{
	return db->DBDelete(key);
}

int prinf(Node* root, ofstream& fio)
{
	if (root == NULL)return 0;
	int totalnum=0;
	if (root->GetPointer(0) != NULL)
	{
		for (int i = 0; i < root->GetCount(); i++)
			totalnum += prinf(root->GetPointer(i), fio);
		return totalnum;
	}
	else
	{
		for (int i = 0; i < root->GetCount(); i++)
		{
			fio << strlen(root->GetKey(i)) << '/' << root->GetKey(i) << '/' << root->GetData(i)->offset << ':' << root->GetData(i)->length << '/';
		}
		return root->GetCount();
	}
}