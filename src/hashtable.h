// Concurrent hash table interface for key–salary mapping

typedef struct hash_struct
{
  uint32_t hash;
  char name[50];
  uint32_t salary;
  struct hash_struct *next;
} hashRecord;

int insert(char[] name, uint32_t salary){};

int delete(char[] name){};

int updateSalary(char[] name, uint32_t salary){};

int search(char[] name){};