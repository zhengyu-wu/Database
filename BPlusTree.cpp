#include"BPlusTree.H"

Node::~Node()
{}

Node::Node()
{
	this->node_count = 0;
	this->node_type = NODE_TYPE_ROOT;
	this->node_father = NULL;
}

LeafNode::LeafNode()
{
	this->node_count = 0;
	this->node_type = NODE_TYPE_LEAF;
	this->node_father = NULL;
	this->NextNode = NULL;
	this->PrevNode = NULL;
}

LeafNode::~LeafNode()
{
}

InternalNode::InternalNode()
{
	this->node_count = 0;
	this->node_type = NODE_TYPE_INTERNAL;
	this->node_father = NULL;
}

InternalNode::~InternalNode()
{
}

// 删除该节点的所有孩子
void Node::DeleteChildren()
{
	if (this != NULL)
	{
		for (int i = 0; i < this->GetCount(); i++)
		{
			if (this->GetPointer(i))
			{
				this->GetPointer(i)->DeleteChildren(); // 递归
				delete this->GetPointer(i);
			}
		}
	}
}

Node* Node::GetBrother(const int flag)
{
	if (this->GetFather() == NULL)
		return NULL;
	else
	{
		Node* father = this->GetFather();
		for (int i = 0; i < father->GetCount(); i++)
		{
			if (strcmp(father->GetKey(i),this->GetKey(0))==0)
			{  
				if (flag == LEFT_BROTHER) 
					return father->GetPointer(i - 1);
				if (flag == RIGHT_BROTHER)            
					return father->GetPointer(i + 1);
				else return NULL;
			}
		} 
	}
	return NULL;
}

bool LeafNode::InsertLeaf(KEY_TYPE key, DATA_TYPE data)
{
	if (this->GetCount() > MAX_USED)
		return false;
	if (this->GetCount() == 0)
	{
		this->SetKey(0, key);
		this->SetData(0, data);
		this->IncCount();
		return true;
	}
	else
	{
		int i = 0;
		KEY_TYPE oldkey = this->GetKey(0);
		for (; i < this->GetCount(); i++)
		{
			if (strcmp(this->m_Keys[i], key)<0)
				continue;
			if (strcmp(this->m_Keys[i], key)>0)
				break;
			if (strcmp(this->m_Keys[i], key) == 0)
				return false;
		}
		if (strcmp(this->GetKey(this->GetCount() - 1), key)<0)
		{
			this->SetKey(GetCount(), key);
			this->SetData(GetCount(), data);
			this->IncCount();
			this->Split();
			return true;
		}
		for (int j = this->GetCount(); j > i; j--)
		{
			this->SetKey(j, this->GetKey(j-1));
			this->SetData(j, this->GetData(j-1));
		}
		
		this->SetKey(i,key);
		this->SetData(i,data);

		this->IncCount();


		Node* father = this->GetFather();//must has father
		if (i == 0)
		while (father != NULL)
		{
			bool whetherbreak = false;
			for (int j = 0; j < father->GetCount(); j++)
			if (strcmp(father->GetKey(j), oldkey) == 0)
			{
				father->SetKey(j, key);
				whetherbreak = (j == 0) ? false : true;
				break;
			}
			else if (strcmp(father->GetKey(j), key) > 0)
				whetherbreak = true;
			if (whetherbreak)
				break;
			father = father->GetFather();
		}
		this->Split();
		return true;
	}
}

bool InternalNode::InsertInternal(KEY_TYPE key, Node* pNode)
{
	if (this->GetCount() > MAX_USED)
		return false;
	if (this->GetCount() == 0)
	{
		this->SetKey(0, key);
		this->SetPointer(0, pNode);
		pNode->SetFather(this);
		this->IncCount();
		return true;
	}
	else
	{
		int i = 0;
		KEY_TYPE oldkey = this->GetKey(0);

		for (; i < this->GetCount(); i++)
		{
			if (strcmp(this->m_Keys[i], key)<0)
				continue;
			if (strcmp(this->m_Keys[i], key)>0)
				break;
			if (strcmp(this->m_Keys[i], key) == 0)
				return false;
		}
		if (strcmp(this->GetKey(this->GetCount() - 1), key)<0)
		{
			this->SetKey(GetCount(), key);
			this->SetPointer(GetCount(), pNode);
			pNode->SetFather(this);
			this->IncCount();
			this->Split();
			return true;
		}
		for (int j = this->GetCount(); j > i; j--)
		{
			this->SetKey(j, this->GetKey(j-1));
			this->SetPointer(j, this->GetPointer(j-1));
		}

		this->SetKey(i, key);
		this->SetPointer(i, pNode);
		if (pNode)
			pNode->SetFather(this);
		this->IncCount();

		Node* father = this->GetFather();//must has father
		if (i == 0)
		while (father != NULL)
		{
			bool whetherbreak = false;
			for (int j = 0; j < father->GetCount(); j++)
			if (strcmp(father->GetKey(j), oldkey) == 0)
			{
				father->SetKey(j, key);
				whetherbreak = (j == 0) ? false : true;
				break;
			}
			else if (strcmp(father->GetKey(j), key) > 0)
				whetherbreak = true;
			if (whetherbreak)
				break;
			father = father->GetFather();
		}
		this->Split();
		return true;
	}
}

void InternalNode::Split()
{
	if (this->GetCount() == CAPACITY)
	{
		InternalNode* bro = new InternalNode;
		bro->SetType(this->GetType());
		for (int i = MIN_USED; i < CAPACITY; i++)
		{
			bro->SetKey(i - MIN_USED,this->GetKey(i));
			this->SetKey(i, NULL);
			bro->SetPointer(i - MIN_USED, this->GetPointer(i));
			this->SetPointer(i, NULL);
			if (bro->GetPointer(i - MIN_USED))
				bro->GetPointer(i - MIN_USED)->SetFather(bro);
			bro->IncCount();
			this->DecCount();
		}
		bro->SetFather(this->GetFather());
		if (this->GetFather() !=NULL)
			this->GetFather()->InsertInternal(bro->GetKey(0), bro);
		else
		{
			InternalNode* father = new InternalNode;
			father->SetType(NODE_TYPE_ROOT);
			this->SetType(NODE_TYPE_INTERNAL);
			bro->SetType(NODE_TYPE_INTERNAL);
			father->SetKey(0,this->GetKey(0));
			father->SetPointer(0, this);
			father->SetKey(1, bro->GetKey(0));
			father->SetPointer(1, bro);
			this->SetFather(father);
			bro->SetFather(father);
			father->SetCount(2);  
		}
	}
}

void LeafNode::Split()
{
	if (this->GetCount() == CAPACITY)
	{
		LeafNode* bro = new LeafNode;

		for (int i = MIN_USED; i < CAPACITY; i++)
		{
			bro->SetKey(i - MIN_USED,this->GetKey(i));
			this->SetKey(i, NULL);
			bro->SetData(i - MIN_USED, this->GetData(i));
			this->SetData(i, NULL);
			bro->IncCount();
			this->DecCount();
		}
		bro->SetFather(this->GetFather());
		bro->PrevNode = this;
		bro->NextNode = this->NextNode;
		if (bro->NextNode!=NULL)
			bro->NextNode->PrevNode = bro; 
		this->NextNode = bro;

		if (this->GetFather() != NULL) 
		{
			this->GetFather()->InsertInternal(bro->GetKey(0), bro);
		}
		else
		{
			InternalNode* father = new InternalNode;
			father->SetType(NODE_TYPE_ROOT);
			this->SetType(NODE_TYPE_LEAF);
			bro->SetType(NODE_TYPE_LEAF);
			this->SetFather(father);
			bro->SetFather(father);
			father->SetKey(0, this->GetKey(0));
			father->SetPointer(0, this);
			father->SetKey(1, bro->GetKey(0));
			father->SetPointer(1, bro);
			father->SetCount(2);
		}
	}
}

bool Node::Delete(KEY_TYPE key)
{	
	for (int i = 0; i < this->GetCount(); i++)
	{
		if (strcmp(this->GetKey(i), key) == 0)
		{
			//0.根节点特殊情况
			if (this->GetType() == NODE_TYPE_ROOT)
			{
				if (this->GetCount() == 1 && i == 0)
				{
					this->SetKey(0, NULL);
					this->SetData(0, NULL);
					this->SetPointer(0, NULL);
					this->SetCount(0);
					return true;
				}
				int j = i;
				for (; j < this->GetCount(); j++)
				{
					this->SetKey(j, this->GetKey(j + 1));
					this->SetData(j, this->GetData(j + 1));
					this->SetPointer(j, this->GetPointer(j + 1));
				}
				this->SetKey(this->GetCount() - 1, NULL);
				this->SetData(this->GetCount() - 1, NULL);
				this->SetPointer(this->GetCount() - 1, NULL);
				this->DecCount();
				return true;
			}

			//1.直接删除
			if (this->GetCount()>MIN_USED)
			{
				int j = i;
				for (; j < this->GetCount(); j++)
				{
					this->SetKey(j, this->GetKey(j + 1));
					this->SetData(j, this->GetData(j + 1));
					this->SetPointer(j, this->GetPointer(j + 1));
				}
				this->SetKey(this->GetCount() - 1, NULL);
				this->SetData(this->GetCount() - 1, NULL);
				this->SetPointer(this->GetCount() - 1, NULL);
				this->DecCount();
				if (i == 0)
				{
					Node* father = this->GetFather();
					while (father != NULL)
					{
						bool whetherbreak = false;
						for (int j = 0; j < father->GetCount(); j++)
						{
							if (strcmp(father->GetKey(j), key) == 0)
							{
								father->SetKey(j, this->GetKey(0));
								whetherbreak = (j == 0) ? false : true;
								break;
							}
						}
						if (whetherbreak)
							break;
						father = father->GetFather();
					}
				}
				return true;
			}
			else
			{
				Node* leftbro = this->GetBrother(LEFT_BROTHER);
				Node* rightbro = this->GetBrother(RIGHT_BROTHER);

				//2.从右兄弟移/结合右兄弟
				if (rightbro != NULL)
				{
					//2.1从右兄弟移
					if (rightbro->GetCount() > MIN_USED)
					{
						int j = i;
						for (; j < this->GetCount() - 1; j++)
						{
							this->SetKey(j, this->GetKey(j + 1));
							this->SetData(j, this->GetData(j + 1));
							this->SetPointer(j, this->GetPointer(j + 1));
						}
						KEY_TYPE thiskey = rightbro->GetKey(0);

						this->SetKey(this->GetCount() - 1, rightbro->GetKey(0));
						this->SetData(this->GetCount() - 1, rightbro->GetData(0));
						this->SetPointer(this->GetCount() - 1, rightbro->GetPointer(0));
						if (this->GetPointer(this->GetCount() - 1))
							this->GetPointer(this->GetCount() - 1)->SetFather(this);

						for (int i = 0; i < rightbro->GetCount() - 1; i++)
						{
							rightbro->SetKey(i, rightbro->GetKey(i + 1));
							rightbro->SetData(i, rightbro->GetData(i + 1));
							rightbro->SetPointer(i, rightbro->GetPointer(i + 1));
						}
						rightbro->SetKey(rightbro->GetCount() - 1, NULL);
						rightbro->SetData(rightbro->GetCount() - 1, NULL);
						rightbro->SetPointer(rightbro->GetCount() - 1, NULL);
						rightbro->DecCount();

						Node* father = rightbro->GetFather();
						for (int k = 0; k < father->GetCount(); k++)
							if (strcmp(father->GetKey(k), thiskey) == 0)
							{
								father->SetKey(k, rightbro->GetKey(0));
								break;
							}
						if (i == 0)
							while (father != NULL)
							{
								bool whetherbreak = false;
								for (int j = 0; j < father->GetCount(); j++)
									if (strcmp(father->GetKey(j), key) == 0)
									{
										father->SetKey(j, this->GetKey(0));
										whetherbreak = (j == 0) ? false : true;
									}
									else if (strcmp(father->GetKey(j), key) > 0)
										whetherbreak = true;
								if (whetherbreak)
									break;
								father = father->GetFather();
							}
						return true;
					}
					//2.2结合右兄弟
					else
					{
						KEY_TYPE brokey = rightbro->GetKey(0);

						//兄弟一定正好MINUSED 正好删第一个
						int j = i;
						for (; j < this->GetCount() - 1; j++)
						{
							this->SetKey(j, this->GetKey(j + 1));
							this->SetData(j, this->GetData(j + 1));
							this->SetPointer(j, this->GetPointer(j + 1));
						}
						for (int k = 0; k < rightbro->GetCount(); k++)
						{
							this->SetKey(j, rightbro->GetKey(k));
							this->SetData(j, rightbro->GetData(k));
							this->SetPointer(j, rightbro->GetPointer(k));
							if (this->GetPointer(j))
								this->GetPointer(j)->SetFather(this);
							j++;
						}
						this->SetCount(this->GetCount() + rightbro->GetCount() - 1);
						if (this->GetType() == NODE_TYPE_LEAF)
						{
							((LeafNode*)this)->NextNode = ((LeafNode*)rightbro)->NextNode;
							if (((LeafNode*)this)->NextNode)
								((LeafNode*)this)->NextNode->PrevNode = ((LeafNode*)this);
						}
						Node* father = this->GetFather();//must has father
						if (i == 0)
							while (father != NULL)
							{
								bool whetherbreak = false;
								for (int j = 0; j < father->GetCount(); j++)
									if (strcmp(father->GetKey(j), key) == 0)
									{
										father->SetKey(j, this->GetKey(0));
										whetherbreak = (j == 0) ? false : true;
									}
									else if (strcmp(father->GetKey(j), key) > 0)
										whetherbreak = true;
								if (whetherbreak)
									break;
								father = father->GetFather();
							}
						//处理父节点

						this->GetFather()->Delete(brokey);
						return right;
					}
					//end 2.2结合右兄弟
				}
				//end 2.从右兄弟移一个/结合右兄弟

				//3.从左兄弟移一个/结合左兄弟
				else if (leftbro != NULL)
				{
					//3.1从左移
					if (leftbro->GetCount() > MIN_USED)
					{
						for (int j = i; j > 0; j--)
						{
							this->SetKey(j, this->GetKey(j - 1));
							this->SetData(j, this->GetData(j - 1));
							this->SetPointer(j, this->GetPointer(j - 1));
						}

						KEY_TYPE oldkey = this->GetKey(0);//原该节点key
						KEY_TYPE newkey = leftbro->GetKey(leftbro->GetCount() - 1);//现该节点key

						this->SetKey(0, leftbro->GetKey(leftbro->GetCount() - 1));
						this->SetData(0, leftbro->GetData(leftbro->GetCount() - 1));
						this->SetPointer(0, leftbro->GetPointer(leftbro->GetCount() - 1));
						if (this->GetPointer(0))
							this->GetPointer(0)->SetFather(this);

						leftbro->SetKey(leftbro->GetCount() - 1, NULL);
						leftbro->SetData(leftbro->GetCount() - 1, NULL);
						leftbro->SetPointer(leftbro->GetCount() - 1, NULL);
						leftbro->DecCount();

						Node* father = this->GetFather();
						for (int i = 0; i < father->GetCount() - 1; i++)
							if (strcmp(father->GetKey(i), oldkey) == 0)
							{
								father->SetKey(i, newkey);
								break;
							}
						return true;
					}
					//3.2结合左兄弟
					else
					{
						KEY_TYPE brokey = leftbro->GetKey(0);
						KEY_TYPE thiskey = this->GetKey(0);
						//左兄弟一定正好MINUSED
						int j = this->GetCount() - 1;
						int movedistance = leftbro->GetCount() - 1;
						while (j >= 0)
						{
							if (j == i)
							{
								movedistance++;
								j--;
								continue;
							}

							this->SetKey(j + movedistance, this->GetKey(j));
							this->SetData(j + movedistance, this->GetData(j));
							this->SetPointer(j + movedistance, this->GetPointer(j));
							j--;
						}
						j = 0;
						for (; j < leftbro->GetCount(); j++)
						{
							this->SetKey(j, leftbro->GetKey(j));
							this->SetData(j, leftbro->GetData(j));
							this->SetPointer(j, leftbro->GetPointer(j));
							if (this->GetPointer(0))
								this->GetPointer(0)->SetFather(this);
						}
						this->SetCount(this->GetCount() + leftbro->GetCount() - 1);
						if (this->GetType() == NODE_TYPE_LEAF)
						{
							((LeafNode*)this)->PrevNode = ((LeafNode*)leftbro)->PrevNode;
							if (((LeafNode*)this)->PrevNode != NULL)
								((LeafNode*)this)->PrevNode->NextNode = (LeafNode*)this;
							((LeafNode*)leftbro)->NextNode = NULL;
							((LeafNode*)leftbro)->PrevNode = NULL;
						}
						Node* father = this->GetFather();
						for (int k = 0; k < father->GetCount(); k++)
							if (strcmp(father->GetKey(k), brokey) == 0)
								father->SetPointer(k, this);
						return father->Delete(thiskey);
					}
				}
			}
			break;
		}
	}
	
	return false;
}


//在一个节点tempnode查找某一个key的位置
//成功时返回相应的data和所在的节点getnode
//失败时返回NULL和所应插入的节点getnode
Node* Search(Node* tempnode, DATA_TYPE* data, KEY_TYPE key)
{
	if (tempnode == NULL)
	{
		*data = NULL;
		return NULL;
	}
	//叶子节点操作
	if (tempnode->GetType() == NODE_TYPE_LEAF || tempnode->GetPointer(0) == NULL)
	{
		
		if (strcmp(tempnode->GetKey(0), key) > 0)
		{
			*data = NULL;
			return tempnode;
		}
		for (int i = 0; i < tempnode->GetCount(); i++)
		{
			if (strcmp(tempnode->GetKey(i), key) == 0)
			{
				*data = tempnode->GetData(i);
				return tempnode;
			}
			else if (strcmp(tempnode->GetKey(i), key) > 0)
				break;
		}
		*data = NULL;
		return tempnode;
	}
	//中间结点操作
	else
	{
		if (strcmp(tempnode->GetKey(0),key)>=0)
			return Search(tempnode->GetPointer(0), data, key);
		for (int i = 0; i < tempnode->GetCount()-1; i++)
		{
			if (strcmp(tempnode->GetKey(i), key) <= 0 && strcmp(tempnode->GetKey(i + 1), key) >0 )
				return Search(tempnode->GetPointer(i), data, key);
		}
		return Search(tempnode->GetPointer(tempnode->GetCount() - 1), data, key);
	}
}
