#include <FS.h>
#include <SPI.h>
#include "SD.h"

#include <JSONDatabase.h>

JSONDatabase db;

void setup()
{
  Serial.begin(115200);

  if (!SD.begin()) {
    Serial.println("SD Mount Failed");
    while (1);
  }
  
  db.open("/sd/data/users2.db");

  JSONVar data;

  data["id"]      = "INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL";
  data["name"]    = "NVARCHAR(64)";
  data["email"]   = "NVARCHAR(64)";
  data["message"] = "NVARCHAR(200)";

  db.create_table("users", &data);
  
  data["id"]      = null;
  data["name"]    = "igor"; 
  data["email"]   = "igorkelvin@gmail.com";
  data["message"] = "JSONDatabase";

  db.insert("users", &data);

  JSONVar query = db.query("SELECT * FROM users ORDER BY rowid DESC LIMIT 1");

  Serial.println(query);
}

void loop()
{

}
