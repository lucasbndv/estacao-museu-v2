var socket = io();

socket.on("update_data", (income) => {
  let data = JSON.parse(income);
  let temp = document.getElementById("temp");
  let humi = document.getElementById("humi");
  let pressure = document.getElementById("pressure");
  let luminosity = document.getElementById("luminosity");
  let dust1 = document.getElementById("dust1");
  let dust2 = document.getElementById("dust2");
  let dust3 = document.getElementById("dust3");
  let last_update = document.getElementById("last_up");

  temp.innerText = data.temperature + "°";
  humi.innerText = data.humidity + "%";
  pressure.innerText = data.pressure + "atm";
  luminosity.innerText = data.luminosity;
  dust1.innerText = data.dust1;
  dust2.innerText = data.dust2;
  dust3.innerText = data.dust3;

  let date = new Date();

  last_update.innerText = date.getHours() + ":" + date.getMinutes();
});
