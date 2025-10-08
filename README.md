# Concurrent Hash Table – COP4600

Group project for UCF Operating Systems course.  
Implements a concurrent hash table supporting insert, delete, search, and print operations with reader-writer locks.

## Features
- Multithreaded insert, delete, search, and print
- Reader-writer lock synchronization
- Jenkins one-at-a-time hash function
- Command-driven execution from `commands.txt`

## Build & Run
```bash
make
./chash
