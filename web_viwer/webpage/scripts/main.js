var socket = io();

let init = async () => {
  let resp = await fetch("http://localhost:3000/api", {
    method: "GET",
  });
  let data = await resp.json();
  updateData(data);
};

init();
socket.on("update_data", (income) => {
  //let data = JSON.parse(income);
  updateData(income);
});

let updateData = async (data) => {
  let temp = document.getElementById("temp");
  let humi = document.getElementById("humi");
  let pressure = document.getElementById("pressure");
  let luminosity = document.getElementById("luminosity");
  let dust1 = document.getElementById("dust1");
  let dust2 = document.getElementById("dust2");
  let dust3 = document.getElementById("dust3");
  let last_update_date = document.getElementById("last_update_date");
  let last_update_hour = document.getElementById("last_update_hour");

  temp.innerText = data.temperature;
  humi.innerText = data.humidity;
  pressure.innerText = data.pressure;
  luminosity.innerText = data.luminosity;
  dust1.innerText = data.dust10;
  dust2.innerText = data.dust25;
  dust3.innerText = data.dust100;

  last_update_date.style.fontSize = "25px";
  last_update_hour.style.fontSize = "25px";
  last_update_date.innerText = data.datetime.slice(0, 10);
  last_update_hour.innerText = data.datetime.slice(11);
};
