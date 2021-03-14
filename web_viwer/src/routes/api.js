const express = require("express");
const { Client } = require("pg");
const client = new Client({
  connectionString:
    "postgres://xdtgcxetjzbmkr:6c083aaf4ce5da274ac0d63e43a6b30a2b2227c3af1c7c16ef0a7357dfeb5d9b@ec2-18-214-208-89.compute-1.amazonaws.com:5432/d1nnj41uc9m4cn",
  ssl: {
    rejectUnauthorized: false,
  },
});
client.connect();

const router = express.Router();

router.get("/api", async (req, res) => {
  //  date_to , date_from
  //  GET /something?date_to=DATE&date_from=DATE&scale=compare/days/months/years
  let date_to = req.query.date_to;
  let date_from = req.query.date_from;
  let scale = req.query.scale;

  if (!date_to || !date_from || !scale){
    try{
      const resp = await client.query("SELECT * FROM estacao ORDER BY datetime DESC LIMIT 1");
      res.send(resp.rows[0]);
      return;
    }catch(err){
      console.log("Error: ", err.stacks);
      res.send("Error on getting data");
      return;
    }
  }

  try {
    const resp = await client.query("SELECT * FROM estacao");
    console.log(resp);
    res.send(resp.rows);
    client.end();
    return;
  } catch (err) {
    res.send(err.stack);
    client.end();
    return;
  }
});

router.post("/api", async (req, res) => {
  let data = req.body;
  if (
    !data.datetime ||
    !data.temperature ||
    !data.humidity ||
    !data.pressure ||
    !data.luminosity ||
    !data.dust10 ||
    !data.dust25 ||
    !data.dust100
  ) {
    res.send("ERROR ON BODY");
    console.log("Error on body");
    return;
  }

  let db_querry_values = Object.values(data);
  //db_querry.push(date.toISOString().slice(0, 19).replace("T", " "));

  try {
    const resp = await client.query(
      "INSERT INTO estacao (datetime,temperature,pressure,humidity,luminosity,dust10,dust25,dust100) VALUES($1,$2,$3,$4,$5,$6,$7,$8)",
      db_querry_values
    );

    res.send(resp.rows[0]);
    console.log("enviado para o db");
    return;
  } catch (err) {
    console.log("error");

    return res.send(err.stack);
  }
});

module.exports = router;
