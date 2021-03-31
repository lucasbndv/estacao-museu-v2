#ifndef JSONDatabase_h
#define JSONDatabase_h

#include <sqlite3.h>
#include <SPI.h>
#include "SD.h"
#include <Arduino_JSON.h>

#if defined(ARRAY_SIZE)
#else
#define ARRAY_SIZE(x) ((sizeof(x)) / (sizeof(*(x))))
#endif

class JSONDatabase
{
public:
  sqlite3 *db;

  JSONDatabase();

  int open(String filename);
  int open(const char *filename);
  int open(String filename, int flags);
  int open(const char *, int flags);

  int close();

  int create_table(String table_name, JSONVar *headers);
  int create_table(const char *table_name, JSONVar *headers);

  int insert(String table_name, JSONVar *data);
  int insert(const char *table_name, JSONVar *data);

  int each(String sql, int (*callback)(void *, int, char **, char **), void *data = NULL);
  int each(const char *sql, int (*callback)(void *, int, char **, char **), void *data = NULL);

  JSONVar query(String sql);
  JSONVar query(const char *sql);

  int all(String sql, int (*callback)(void *), void *data = NULL);
  int all(const char *sql, int (*callback)(void *), void *data = NULL);

  virtual int is_open() { return db != null; }
  virtual JSONVar get_headers() { return _headers; };

  int sendBackup(int offset);

private:
  JSONVar _headers;
};

#endif /* JSONDatabase_h */
