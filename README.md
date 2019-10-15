# <key,value> database based on B+tree 💾

### Features 👻

- B+ tree data structure
- Space Recycle
- Cache
- Buffer
- Variable length value


### Interfaces 🤓

- Create a database handle
  - DBHANDLE* db；
- Open a database
  - DBHANDLE* db_open(const char* pathname, int flag)
- Close a database
  - void db_close(DBHANDLE *db)
- Store data
  - int db_store(DBHANDLE *db, const char* key, const char* data, INSERT)
- Modify data
  - int db_store(DBHANDLE *db, const char* key, const char* data, REPLACE)
- Delete data
  -  int db_delete(DBHANDLE *db, const char* key)
- Search data
  - char* db_fetch(DBHANDLE *db, const char* key)

## Modules

- BPlusTree.cpp
  - Base class: Node
  - Derived class: InternalNode, LeafNode
  - Node operations like merge, split
  
- DateBase.cpp
  - Database handle class

- BPlusTree.h
  - RecordSpace Class: reuse space 
  - Linked chain to record available space
  
- cache.cpp
  - Read cache
 
- buffer.cpp
  - Write buffer

### Test 🧐

#### Correctness test

- correctness.cpp
- abnormal.cpp
- allround_test.cpp

#### Performance test

- space_test.cpp
- read_buffer_test.cpp
- write_buffer_test.cpp
- single_operation_test.cpp



  



