const express = require("express");
const pg = require("pg");
const { Client } = require("pg");
const client = new Client({
  connectionString:
    "postgres://xdtgcxetjzbmkr:6c083aaf4ce5da274ac0d63e43a6b30a2b2227c3af1c7c16ef0a7357dfeb5d9b@ec2-18-214-208-89.compute-1.amazonaws.com:5432/d1nnj41uc9m4cn",
  ssl: {
    rejectUnauthorized: false,
  },
});

try {
  client.connect();
  console.log("Successfully connected");
} catch (e) {
  console.log(e);
}

pg.types.setTypeParser(1114, (str) => str); // forcing timestamp to be str

const router = express.Router();

router.get("/api", async (req, res) => {
  //  date_to , date_from
  //  GET /something?date_to=DATE&date_from=DATE&type=hours/days/months
  //  type: single ; hourly ; daily ; monthly

  let type = req.query.type;

  // First update
  if (type == "single") {
    try {
      const resp = await client.query(
        "SELECT * FROM estacao ORDER BY datetime DESC LIMIT 1"
      );
      res.send(JSON.stringify(resp.rows[0]));
      return;
    } catch (err) {
      console.log("Error: ", err.stacks);
      res.send("Error on getting data");
      return;
    }
  }

  // Date variables used above
  let date = req.query.date_from;
  let date_from;
  let date_to;
  let ans = {
    datetime: [],
    temperature: [],
    humidity: [],
    luminosity: [],
    co2: [],
    dust10: [],
    dust25: [],
    dust100: [],
  };

  if (type == "hourly") {
    try {
      for (let i = 0; i < 24; i++) {
        if (i < 10) {
          date_from = `${date} 0${i}:00:00`;
          date_to = `${date} 0${i}:59:59`;
        } else {
          date_from = `${date} ${i}:00:00`;
          date_to = `${date} ${i}:59:59`;
        }

        ans["datetime"].push(date_from);

        const resp = await client.query(
          `SELECT  AVG(temperature) AS temperature, AVG(humidity) AS humidity ,AVG(luminosity) AS luminosity, AVG(co2) AS co2, AVG(dust10) AS dust10, AVG(dust25) AS dust25, AVG(dust100) AS dust100 FROM estacao WHERE datetime BETWEEN '${date_from}' AND '${date_to}'`
        );

        resp.rows.forEach((data) => {
          for (prop in data) {
            // Verifying if its a number in order to round it
            let inp = data[prop];
            if (inp === null || isNaN(parseFloat(inp.toString()))) {
              ans[prop].push(inp);
            } else {
              ans[prop].push(inp.toFixed(2));
            }
          }
        });
      }

      res.send(ans);
      console.log(ans["datetime"]);

      return;
    } catch (err) {
      res.send(err.stack);
      console.log(err.stack);

      return;
    }
  }

  let days_per_month = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
  if (type == "daily") {
    let current_month = date.slice(5, 7);
    let current_month_int = parseInt(current_month);
    let current_year = date.slice(0, 4);
    try {
      for (let i = 1; i < days_per_month[current_month_int - 1] + 1; i++) {
        if (i < 10) {
          date_from = current_year + `-${current_month}-0${i}` + " 00:00:00";
          date_to = current_year + `-${current_month}-0${i}` + " 23:59:59";
        } else {
          date_from = current_year + `-${current_month}-${i}` + " 00:00:00";
          date_to = current_year + `-${current_month}-${i}` + " 23:59:59";
        }
        ans["datetime"].push(date_from.slice(0, 10));
        const resp = await client.query(
          `SELECT  AVG(temperature) AS temperature, AVG(humidity) AS humidity ,AVG(luminosity) AS luminosity, AVG(co2) AS co2, AVG(dust10) AS dust10, AVG(dust25) AS dust25, AVG(dust100) AS dust100 FROM estacao WHERE datetime BETWEEN '${date_from}' AND '${date_to}'`
        );

        resp.rows.forEach((data) => {
          for (prop in data) {
            // Verifying if its a number in order to round it
            let inp = data[prop];
            if (inp === null || isNaN(parseFloat(inp.toString()))) {
              ans[prop].push(inp);
            } else {
              ans[prop].push(inp.toFixed(2));
            }
          }
        });
      }

      console.log(ans["datetime"]);
      res.send(ans);

      return;
    } catch (err) {
      res.send(err.stack);
      console.log(err.stack);

      return;
    }
  }

  if (type == "monthly") {
    try {
      let current_year = date.slice(0, 4);
      for (let i = 1; i < 12; i++) {
        if (i < 10) {
          date_from = current_year + `-0${i}-` + "01" + " 00:00:00";
          date_to =
            current_year + `-0${i}-` + days_per_month[i - 1] + " 23:59:59";
        } else {
          date_from = current_year + `-${i}-` + "01" + " 00:00:00";
          date_to =
            current_year + `-${i}-` + days_per_month[i - 1] + " 23:59:59";
        }

        ans["datetime"].push(date_from.slice(0, 7));
        const resp = await client.query(
          `SELECT  AVG(temperature) AS temperature, AVG(humidity) AS humidity ,AVG(luminosity) AS luminosity, AVG(co2) AS co2, AVG(dust10) AS dust10, AVG(dust25) AS dust25, AVG(dust100) AS dust100 FROM estacao WHERE datetime BETWEEN '${date_from}' AND '${date_to}'`
        );

        resp.rows.forEach((data) => {
          for (prop in data) {
            // Verifying if its a number in order to round it
            let inp = data[prop];
            if (inp === null || isNaN(parseFloat(inp.toString()))) {
              ans[prop].push(inp);
            } else {
              ans[prop].push(inp.toFixed(2));
            }
          }
        });
      }

      res.send(ans);
      console.log(ans["datetime"]);

      return;
    } catch (err) {
      res.send(err.stack);
      console.log(err.stack);

      return;
    }
  }
});

router.post("/api", async (req, res) => {
  let data = req.body;
  let db_query_values = Object.values(data);

  try {
    const resp = await client.query(
      "INSERT INTO estacao (device,datetime,temperature,humidity,dewpoint,absolutehumidity,pressure,luminosity,co2,dust10,dust25,dust100,dataid) VALUES($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13)",
      db_query_values
    );
    console.log("enviado para o db");
    req.io.emit("update_data", data);
    return res.send(resp.rows[0]);
  } catch (err) {
    console.log("error");
    console.log(err.stack);
    return res.send(err.stack);
  }
});

module.exports = router;
