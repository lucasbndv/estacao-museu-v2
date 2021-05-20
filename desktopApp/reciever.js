const SerialPort = require("serialport");
const Readline = require("@serialport/parser-readline");
const fetch = require("node-fetch");
const fs = require("fs");

let readline = require("readline");
let reader = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
});

let ports_arr = [];
SerialPort.list().then(
  (ports) =>
    ports.forEach((port, index) => {
      ports_arr.push(port.path);
      console.log(port.path + " => " + index);
    }),
  (err) => console.error(err)
);

var port;
reader.question(
  "Qual é o número da porta que você deseja utilizar? \n",
  function (ans) {
    port = new SerialPort(ports_arr[ans], { baudRate: 9600 });
    let parser = port.pipe(new Readline({ delimiter: "\n" }));
    console.log("Conectado a porta " + ports_arr[ans]);
    parser.on("data", handleData);
    reader.close();
  }
);

let insert = false;
let backup_id = 0;
let current_id;
let url = "localhost:3000"; // "localhost"
//let url = "https://estacao-museu.herokuapp.com/api"; //heroku
try {
  current_id = fs.readFileSync("current_id.txt", "utf8").toString();
} catch {
  current_id = 0;
  fs.writeFileSync("current_id.txt", "0");
}
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

const handleData = async (data) => {
  try {
    if (data[0] == "[" && data[1] == "I") {
      insert = true;
      return;
    }

    if (insert) {
      let data_parsed = JSON.parse(data);
      console.log(data_parsed);

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

      // Verifying if backup is over:
      if (data_parsed.dataId == backup_id) {
        backup_id = 0;
      }

      if (data_parsed.dataId < current_id) {
        console.log("Error");
      }

      // if (data_parsed.dataId - current_id > 1) {
      //   // Not request multiple times
      //   if (backup_id == 0 || data_parsed.dataId != backup_id) {
      //     port.write(current_id.toString() + " \n");
      //     backup_id = data_parsed.dataId;
      //     console.log("Requesting backup...");
      //   }
      //   return;
      // } else {
      //   fs.writeFileSync("current_id.txt", data_parsed.dataId);
      //   console.log("No backup");
      //   current_id = data_parsed.dataId;
      // }

      fs.writeFileSync("current_id.txt", data_parsed.dataId);

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

      let resp = await fetch(url, {
        method: "POST",
        body: JSON.stringify(data_parsed),
        headers: { "Content-Type": "application/json" },
      });
      console.log("Data was Sent: ", data_parsed.datetime);
      insert = false;
    }

    return;
  } catch (e) {
    //console.log("ERROR: ", e);
  }
};
