
#include"string.h"
#include<string>
#include<iostream>
#pragma once
using namespace std;

#define ORDER_M 64  // b+���Ľ� 
#define CAPACITY (ORDER_M+1)    // ����Ĵ�С 
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

//��ֵ������
typedef const char* KEY_TYPE; 
typedef RecordSpace* DATA_TYPE;

//������� 
enum NODE_TYPE
{
	NODE_TYPE_ROOT = 1,    // �����
	NODE_TYPE_INTERNAL = 2,    // �ڲ����
	NODE_TYPE_LEAF = 3,    // Ҷ�ӽ��
};

#define NULL 0

#define LEFT_BROTHER 0
#define RIGHT_BROTHER 1


class Node                             // �ڵ���
{
public:

	Node();
	virtual ~Node();

	//��ȡ�����ý������
	NODE_TYPE GetType() { return node_type; }
	void SetType(NODE_TYPE type) { node_type = type; }

	// ��ȡ��������Ч���ݸ���
	int GetCount() { return node_count; }
	void SetCount(int i) { node_count = i; }
	void IncCount(){ node_count++; }
	void DecCount(){ node_count--; }

	// ��ȡ������ĳ��Ԫ�أ����м���ָ������Ҷ�ӽ��ָ����
	virtual KEY_TYPE GetKey(int i) { return 0; }
	virtual void SetKey(int i, KEY_TYPE key) {}

	// ��ȡ������ĳ��Ԫ�أ����м���ָ������Ҷ�ӽ��ָ����
	virtual DATA_TYPE GetData(int i) { return 0; }
	virtual void SetData(int i, DATA_TYPE data) { }

	// ��ȡ������ĳ��ָ�룬���м���ָָ�룬��Ҷ�ӽ��������
	virtual Node* GetPointer(int i) { return NULL; }
	virtual void SetPointer(int i, Node* pointer) { }


	// ��ȡ�����ø����
	Node* GetFather() { return node_father; }
	void SetFather(Node* father) { node_father = father; }

	virtual bool InsertInternal(KEY_TYPE key, Node* pNode){ return false; };
	virtual bool InsertLeaf(KEY_TYPE key, DATA_TYPE data){ return false; };

	virtual void Split(){};

	bool Delete(KEY_TYPE key);

	// ��ȡһ��������ֵܽ��
	Node* GetBrother(const int flag);

	// ɾ�����
	void DeleteChildren();

protected:

	NODE_TYPE node_type;    // �������

	int node_count;			// ��Ч���ݸ���

	Node* node_father;      // ָ�򸸽���ָ��

};



//�ڲ������
class InternalNode : public Node
{
public:

	InternalNode();
	virtual ~InternalNode();

	// ��ȡ�����ü�ֵ�����û���˵�����ִ�1��ʼ
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

	// ��ȡ������ָ�룬���û���˵�����ִ�1��ʼ
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

	// ����
	bool InsertInternal(KEY_TYPE key, Node* pNode);
	
	// ���ѽ��
	 void Split();

	// ��Ͻ��
	bool Combine(Node* pNode);
	


protected:

	KEY_TYPE m_Keys[CAPACITY];           // ������
	Node* m_Pointers[CAPACITY];			 // ָ������

};


// Ҷ�ӽ���� 
class LeafNode : public Node
{
public:

	LeafNode();
	virtual ~LeafNode();

	// ��ȡ����������
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

	// ��ȡ������ָ�룬��Ҷ�ӽ�������壬ֻ��ʵ�и�����麯��
	Node* GetPointer(int i)
	{
		return NULL;
	}

	// ��������
	bool InsertLeaf(KEY_TYPE key, DATA_TYPE data);


	// ���ѽ��
	void Split();

	// ��Ͻ��
	bool Combine(Node* pNode);

	bool Move(int flag, Node* pNode);

public:
	
	LeafNode* PrevNode;                 // ǰһ����� 
	LeafNode* NextNode;                 // ��һ����� 

protected:

	KEY_TYPE m_Keys[CAPACITY];    // ������
	DATA_TYPE m_Datas[CAPACITY];	//��������
};


Node* Search(Node* tempnode, DATA_TYPE* data, KEY_TYPE key);    // �Ӹýڵ�����Ѱ��ָ��key