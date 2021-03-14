const { Client } = require("pg");
const client = new Client({
  connectionString:
    "postgres://xdtgcxetjzbmkr:6c083aaf4ce5da274ac0d63e43a6b30a2b2227c3af1c7c16ef0a7357dfeb5d9b@ec2-18-214-208-89.compute-1.amazonaws.com:5432/d1nnj41uc9m4cn",
  ssl: {
    rejectUnauthorized: false,
  },
});

client.connect();
client.query(
  "CREATE TABLE estacao ( datetime TIMESTAMP ,temperature FLOAT,humidity FLOAT,pressure FLOAT,luminosity FLOAT,dust10 FLOAT,dust25 FLOAT,dust100 FLOAT);"
);

client.end();
