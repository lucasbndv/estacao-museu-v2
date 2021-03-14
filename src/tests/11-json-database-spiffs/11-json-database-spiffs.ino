#include <FS.h>
#include "SPIFFS.h"

#include <JSONDatabase.h>

JSONDatabase db;

int read_data(void *data, int argc, char **argv, char **azColName)
{
  JSONVar tmp;
  
  for (int i = 0; i < argc; i++)
  {
    tmp[azColName[i]] = argv[i];
  }

  JSONVar *json = (JSONVar *) data;
  int len = json->length();

  (*json)[(len >= 1) ? len : 0] = tmp;
  
  return 0;
}

void setup()
{
  Serial.begin(115200);

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount Failed");
    while (1);
  }
  
  Serial.println(sqlite3_initialize());
  db.open("/spiffs/users2.db", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);

  JSONVar data;

  data["id"]      = "INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL";
  data["name"]    = "NVARCHAR(64)";
  data["email"]   = "NVARCHAR(64)";
  data["message"] = "NVARCHAR(200)";


  int rc;
  if (rc = db.create_table("users", &data))
  {
    Serial.println(rc);
    while (1);
  }

  data["id"]      = null;
  data["name"]    = "igor";
  data["email"]   = "igorkelvin@gmail.com";
  data["message"] = "JSONDatabase test 123";

  if (db.insert("users", &data)) while (1);

  JSONVar query;
  
  if (db.each("SELECT * FROM users ORDER BY rowid DESC LIMIT 10", read_data, &query)) while (1);

  Serial.println(query);
}

void loop()
{

}
