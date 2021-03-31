
#include "JSONDatabase.h"

JSONDatabase::JSONDatabase()
{
}

int JSONDatabase::open(String filename)
{
  open(filename.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
}

int JSONDatabase::open(const char *filename)
{
  open(filename, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
}

int JSONDatabase::open(String filename, int flags)
{
  open(filename.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
}

int JSONDatabase::open(const char *filename, int flags)
{
  int rc = sqlite3_open_v2(filename, &db, flags, NULL);

  if (rc != SQLITE_OK)
  {
    Serial.printf("[DB] Can't open database: %s\n", sqlite3_errmsg(db));
  }
  else
  {
    Serial.printf("[DB] Opened '%s' database successfully\n", filename);
  }

  return rc;
}

int JSONDatabase::close()
{
  int rc = sqlite3_close(db);

  if (rc != SQLITE_OK)
  {
    Serial.printf("[DB] Can't close database: %s\n", sqlite3_errmsg(db));
  }
  else
  {
    Serial.printf("[DB] Database closed\n");
  }

  return rc;
}

int JSONDatabase::create_table(String table_name, JSONVar *data)
{
  create_table(table_name.c_str(), data);
}

int JSONDatabase::create_table(const char *table_name, JSONVar *data)
{
  //char sql[256] = "";
  char sql[512] = ""; //teste
  char tmp[400] = "";
  char *error_msg;
  _headers = data->keys();

  for (int i = 0; i < _headers.length(); i++)
  {
    sqlite3_snprintf(sizeof(tmp), tmp, "%s%c \"%w\" %w", tmp, (i != 0) ? ',' : ' ', (const char *)_headers[i], (const char *)(*data)[_headers[i]]);
  }

  snprintf(sql, sizeof(sql), "CREATE TABLE IF NOT EXISTS %s (%s)", table_name, tmp);

  int rc = sqlite3_exec(db, sql, NULL, NULL, &error_msg);
  //Serial.printf("[DB][DEBUG] sql = '%s'\n", sql);

  if (error_msg)
  {
    Serial.printf("[DB] Error creating table: %s'\n", error_msg);
    Serial.printf("[DB] Extended error: %d\n", sqlite3_extended_errcode(db));
    Serial.printf("[DB] sql = '%s'\n", sql);

    sqlite3_free(error_msg);

    return rc;
  }

  Serial.printf("[DB] Table '%s' created\n", table_name);
  return rc;
}

int JSONDatabase::insert(String table_name, JSONVar *data)
{
  insert(table_name.c_str(), data);
}

int JSONDatabase::insert(const char *table_name, JSONVar *data)
{
  JSONVar keys = (*data).keys();
  char sql[256] = "";
  char headers[256] = "";
  char placeholders[64] = "";

  //Serial.println(keys);
  //Serial.println(*data);
  //return 0;

  for (int i = 0; i < keys.length(); i++)
  {

    sqlite3_snprintf(sizeof(headers), headers, "%s%c \"%w\"", headers, (i != 0) ? ',' : ' ', (const char *)keys[i]);
    sqlite3_snprintf(sizeof(placeholders), placeholders, "%s%c ?", placeholders, (i != 0) ? ',' : ' ');
  }

  snprintf(sql, sizeof(sql), "INSERT INTO %s (%s) VALUES(%s)", table_name, headers, placeholders);

  //Serial.println(sql);

  sqlite3_stmt *res;
  const char *tail;

  int rc = sqlite3_prepare_v2(db, sql, strlen(sql), &res, &tail);

  if (rc != SQLITE_OK)
  {
    Serial.printf("[DB] Failed to insert data: %s\n", sqlite3_errmsg(db));
    close();
    return rc;
  }

  for (int i = 0; i < keys.length(); i++)
  {
    JSONVar tmp = (*data)[keys[i]];
    String type = JSON.typeof(tmp);

    //Serial.printf("#%02d: %s, type: %s\n", i, (const char *) tmp, type.c_str());
    //Serial.printf("#%02d: %s, type: ", i, (const char *) tmp);
    //Serial.print(i);
    //Serial.print(" ");
    //Serial.print(tmp);
    //Serial.print(" ");
    //Serial.print(tmp.length());
    //Serial.print(" ");
    //Serial.println(type);

    if (type == "string")
    {
      const char *str = (const char *)tmp;
      sqlite3_bind_text(res, i + 1, str, strlen(str), SQLITE_STATIC);
    }
    else if (type == "number")
    {
      sqlite3_bind_double(res, i + 1, tmp);
    }
    //else if (type == "undefined") {
    //sqlite3_bind_null(res, i + 1);
    //}
    //else
    //{
    //Serial.println("[DB] Error trying to bind on database!");
    //Serial.println(JSON.stringify(data));

    //sqlite3_close(db);
    //return SQLITE_ERROR;
    //}
  }

  rc = sqlite3_step(res);

  if (rc != SQLITE_DONE)
  {
    Serial.printf("[DB] ERROR executing stmt: %s\n", sqlite3_errmsg(db));
    close();
    return rc;
  }

  rc = sqlite3_finalize(res);

  if (rc != SQLITE_OK)
  {
    Serial.printf("[DB] SQL error: %s\n", sqlite3_errmsg(db));
  }
  else
  {
    Serial.printf("[DB] Data was successfully storaged on '%s': %s\n", table_name, JSON.stringify(*data).c_str());
  }
  return rc;
}

int JSONDatabase::each(String sql, int (*callback)(void *, int, char **, char **), void *data)
{
  return each(sql.c_str(), callback, data);
}

int JSONDatabase::each(const char *sql, int (*callback)(void *, int, char **, char **), void *data)
{
  char *error_msg;

  int rc = sqlite3_exec(db, sql, callback, (void *)data, &error_msg);

  if (rc != SQLITE_OK)
  {
    Serial.printf("[DB] SQL error: %s\n", error_msg);
    sqlite3_free(error_msg);
  }
  else
  {
    Serial.printf("[DB] Successful read\n");
  }

  return rc;
}

JSONVar JSONDatabase::query(String sql)
{
  query(sql.c_str());
}

JSONVar JSONDatabase::query(const char *sql)
{
  JSONVar query;

  each(
      sql, [](void *data, int argc, char **argv, char **azColName) {
        JSONVar tmp;

        for (int i = 0; i < argc; i++)
        {
          tmp[azColName[i]] = argv[i];
        }

        JSONVar *json = (JSONVar *)data;
        int len = json->length();
        //Serial.println(len);
        //Serial.println(tmp);
        (*json)[(len >= 1) ? len : 0] = tmp;

        return 0;
      },
      &query);

  return query;
}

int JSONDatabase::all(String sql, int (*callback)(void *), void *data)
{
}

int JSONDatabase::all(const char *sql, int (*callback)(void *), void *data)
{
}

int JSONDatabase::sendBackup(int offset)
{
  int rc;
  sqlite3_stmt *res;
  int rec_count = 0;
  const char *tail;
  JSONVar data;
  Serial.println("[DB] Backup...");
  char sql[256] = "";
  snprintf(sql, sizeof(sql), "SELECT * FROM sensordata ORDER BY id LIMIT 5000 OFFSET %d", offset);
  rc = sqlite3_prepare_v2(db, sql, strlen(sql), &res, &tail);

  if (rc != SQLITE_OK)
  {
    Serial.println("[DB] ERROR ON PREPARING QUERY");
    return 0;
  }
  //int aux, aux_sum = 0;
  while (sqlite3_step(res) == SQLITE_ROW)
  {
    //aux = millis();
    data["device"] = (const char *)sqlite3_column_text(res, 0);
    data["datetime"] = (const char *)sqlite3_column_text(res, 2);
    data["temperature"] = sqlite3_column_int(res, 3);
    data["humidity"] = sqlite3_column_int(res, 4);
    data["dewpoint"] = sqlite3_column_int(res, 5);
    data["absolutehumidity"] = sqlite3_column_int(res, 6);
    data["pressure"] = sqlite3_column_int(res, 7);
    data["luminosity"] = sqlite3_column_int(res, 8);
    data["co2"] = sqlite3_column_int(res, 9);
    data["dust10"] = sqlite3_column_int(res, 10);
    data["dust25"] = sqlite3_column_int(res, 11);
    data["dust100"] = sqlite3_column_int(res, 12);
    data["dataId"] = sqlite3_column_int(res, 1); // Only to fit nodejs validation

    Serial.println("[INSERT]");
    Serial.println(data);
    //Serial.println(JSON.stringify(data).c_str());
    //Serial.println(JSON.stringify(*data).c_str());

    rec_count += 1;
    if (rec_count > 5000)
    {
      Serial.println("[DB] TO MANY RECORDS");
      return 0;
    }

    //aux_sum += millis() - aux;
  }

  // float timer = aux_sum / rec_count;
  // Serial.print("[DB] Records: ");
  // Serial.print(rec_count);
  // Serial.print(" || Time_per_record: "); // aprox 9ms per loop
  // Serial.print(timer);
  sqlite3_finalize(res);
  return 1;
}