const SerialPort = require("serialport");
const Readline = require("@serialport/parser-readline");
const fetch = require("node-fetch");
const fs = require("fs");

const s = new SerialPort("COM4", { baudRate: 115200 });
let parser = s.pipe(new Readline({ delimiter: "\n" }));

let aux = false;
let current_id = fs.readFileSync("current_id.txt", "utf8").toString();
let data_types = [
  "string",
  "number",
  "number",
  "number",
  "number",
  "number",
  "number",
  "number",
  "number",
];
parser.on("data", async (data) => {
  try {
    if (data[0] == "[" && data[1] == "I") {
      aux = true;
      return;
    }

    if (aux) {
      let data_parsed = JSON.parse(data);

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

      if (data_parsed.id - current_id > 1) {
        parser.write(current_id);
        console.log("Requesting backup...");
        return;
      } else {
        fs.writeFileSync("current_id.txt", data_parsed.id);
      }
      // 2 times parseFloat in order to get float back

      data_parsed.temperature = parseFloat(
        parseFloat(data_parsed.temperature).toFixed(2)
      );
      data_parsed.pressure = parseFloat(
        parseFloat(data_parsed.pressure).toFixed(2)
      );
      data_parsed.humidity = parseFloat(
        parseFloat(data_parsed.humidity).toFixed(2)
      );
      data_parsed.luminosity = parseFloat(
        parseFloat(data_parsed.luminosity).toFixed(2)
      );
      data_parsed.dust10 = parseFloat(
        parseFloat(data_parsed.dust10).toFixed(2)
      );
      data_parsed.dust25 = parseFloat(
        parseFloat(data_parsed.dust25).toFixed(2)
      );
      data_parsed.dust100 = parseFloat(
        parseFloat(data_parsed.dust100).toFixed(2)
      );

      let sensor_data = {
        datetime: date,
        ...data_parsed,
      };

      let resp = await fetch("http://localhost:3000/api", {
        method: "POST",
        body: JSON.stringify(sensor_data),
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
