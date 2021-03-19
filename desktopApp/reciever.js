const SerialPort = require("serialport");
const Readline = require("@serialport/parser-readline");
const fetch = require("node-fetch");
const fs = require("fs");

const s = new SerialPort("COM4", { baudRate: 115200 });
let parser = s.pipe(new Readline({ delimiter: "\n" }));

let aux = false;
let current_id = fs.readFileSync("current_id.txt", "utf8").toString();
let data_types = [
  "string", //device
  "string", //datetime
  "number", //temperature
  "number", //humidity
  "number", //dewPoint
  "number", //absoluteHumidity
  "number", //pressure
  "number", //luminosity
  "number", //CO2
  "number", //dust10
  "number", //dust25
  "number", //dust100
  "number", //dataId
];
parser.on("data", async (data) => {
  try {
    if (data[0] == "[" && data[1] == "I") {
      aux = true;
      return;
    }

    if (aux) {
      let data_parsed = JSON.parse(data);
      console.log(data);

      // check data
      let new_data = Object.values(data_parsed);

      if (new_data.length != data_types.length) {
        console.log("Error on data length");
        return;
      }

      for (let i = 0; i < data_types.length; i++) {
        if (typeof new_data[i] != data_types[i]) {
          console.log("Error on datatype");
          return;
        }
      }

      // Checking if there is need for backup

      // if (data_parsed.id - current_id > 1) {
      //   parser.write(current_id);
      //   console.log("Requesting backup...");
      //   return;
      // } else {
      //   fs.writeFileSync("current_id.txt", data_parsed.id);
      // }

      // Rounding
      for (key in data_parsed) {
        if (key != "datetime" && key != "device") {
          if (key == "humidity") {
            data_parsed[key] = parseFloat(data_parsed[key].toFixed(0));
          } else {
            data_parsed[key] = parseFloat(data_parsed[key].toFixed(1));
          }
        }
      }

      let resp = await fetch("http://localhost:3000/api", {
        method: "POST",
        body: JSON.stringify(data_parsed),
        headers: { "Content-Type": "application/json" },
      });
      console.log("Enviado para o servidor");
      aux = false;
    }

    return;
  } catch (e) {
    //console.log("ERROR: ", e);
  }
});
