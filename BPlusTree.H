
#include"string.h"
#include<string>
#include<iostream>
#pragma once
using namespace std;

#define ORDER_M 64  // b+树的阶 
#define CAPACITY (ORDER_M+1)    // 数组的大小 
#define MIN_USED (ORDER_M/2)  
#define MAX_USED (ORDER_M)

class RecordSpace
{
public:
	int offset;
	int length;
	RecordSpace* Next;
	RecordSpace* Prev;
	RecordSpace()
	{
		offset = 0;
		length = 0;
		Next = NULL;
		Prev = NULL;
	}
};

//键值的类型
typedef const char* KEY_TYPE; 
typedef RecordSpace* DATA_TYPE;

//结点类型 
enum NODE_TYPE
{
	NODE_TYPE_ROOT = 1,    // 根结点
	NODE_TYPE_INTERNAL = 2,    // 内部结点
	NODE_TYPE_LEAF = 3,    // 叶子结点
};

#define NULL 0

#define LEFT_BROTHER 0
#define RIGHT_BROTHER 1


class Node                             // 节点类
{
public:

	Node();
	virtual ~Node();

	//获取和设置结点类型
	NODE_TYPE GetType() { return node_type; }
	void SetType(NODE_TYPE type) { node_type = type; }

	// 获取和设置有效数据个数
	int GetCount() { return node_count; }
	void SetCount(int i) { node_count = i; }
	void IncCount(){ node_count++; }
	void DecCount(){ node_count--; }

	// 获取和设置某个元素，对中间结点指键，对叶子结点指数据
	virtual KEY_TYPE GetKey(int i) { return 0; }
	virtual void SetKey(int i, KEY_TYPE key) {}

	// 获取和设置某个元素，对中间结点指键，对叶子结点指数据
	virtual DATA_TYPE GetData(int i) { return 0; }
	virtual void SetData(int i, DATA_TYPE data) { }

	// 获取和设置某个指针，对中间结点指指针，对叶子结点无意义
	virtual Node* GetPointer(int i) { return NULL; }
	virtual void SetPointer(int i, Node* pointer) { }


	// 获取和设置父结点
	Node* GetFather() { return node_father; }
	void SetFather(Node* father) { node_father = father; }

	virtual bool InsertInternal(KEY_TYPE key, Node* pNode){ return false; };
	virtual bool InsertLeaf(KEY_TYPE key, DATA_TYPE data){ return false; };

	virtual void Split(){};

	bool Delete(KEY_TYPE key);

	// 获取一个最近的兄弟结点
	Node* GetBrother(const int flag);

	// 删除结点
	void DeleteChildren();

protected:

	NODE_TYPE node_type;    // 结点类型

	int node_count;			// 有效数据个数

	Node* node_father;      // 指向父结点的指针

};



//内部结点类
class InternalNode : public Node
{
public:

	InternalNode();
	virtual ~InternalNode();

	// 获取和设置键值，对用户来说，数字从1开始
	KEY_TYPE GetKey(int i)
	{
		if ((i >= 0) && (i <= MAX_USED))
		{
			return m_Keys[i];
		}
		else
		{
			return NULL;
		}
	}

	void SetKey(int i, KEY_TYPE key)
	{
		if ((i >= 0) && (i <= MAX_USED))
		{
			m_Keys[i] = key;
		}
	}

	// 获取和设置指针，对用户来说，数字从1开始
	Node* GetPointer(int i)
	{
		if ((i >= 0) && (i <= MAX_USED))
		{
			return m_Pointers[i];
		}
		else
		{
			return NULL;
		}
	}

	void SetPointer(int i, Node* pointer)
	{
		if ((i >= 0) && (i <= MAX_USED))
		{
			m_Pointers[i] = pointer;
		}
	}

	// 插入
	bool InsertInternal(KEY_TYPE key, Node* pNode);
	
	// 分裂结点
	 void Split();

	// 结合结点
	bool Combine(Node* pNode);
	


protected:

	KEY_TYPE m_Keys[CAPACITY];           // 键数组
	Node* m_Pointers[CAPACITY];			 // 指针数组

};


// 叶子结点类 
class LeafNode : public Node
{
public:

	LeafNode();
	virtual ~LeafNode();

	// 获取和设置数据
	KEY_TYPE GetKey(int i)
	{
		if ((i >= 0) && (i <= MAX_USED))
		{
			return m_Keys[i];
		}
		else
		{
			return NULL;
		}
	}

	void SetKey(int i, KEY_TYPE key)
	{
		if ((i >= 0) && (i <= MAX_USED))
		{
			m_Keys[i] = key;
		}
	}

	DATA_TYPE GetData(int i)
	{
		if ((i >= 0) && (i <= MAX_USED))
		{
			return m_Datas[i];
		}
		else
		{
			return NULL;
		}
	}

	void SetData(int i, DATA_TYPE data)
	{
		if ((i >= 0) && (i <= MAX_USED))
		{
			m_Datas[i] = data;
		}
	}

	// 获取和设置指针，对叶子结点无意义，只是实行父类的虚函数
	Node* GetPointer(int i)
	{
		return NULL;
	}

	// 插入数据
	bool InsertLeaf(KEY_TYPE key, DATA_TYPE data);


	// 分裂结点
	void Split();

	// 结合结点
	bool Combine(Node* pNode);

	bool Move(int flag, Node* pNode);

public:
	
	LeafNode* PrevNode;                 // 前一个结点 
	LeafNode* NextNode;                 // 后一个结点 

protected:

	KEY_TYPE m_Keys[CAPACITY];    // 键数组
	DATA_TYPE m_Datas[CAPACITY];	//数据数组
};


Node* Search(Node* tempnode, DATA_TYPE* data, KEY_TYPE key);    // 从该节点向下寻找指定key
