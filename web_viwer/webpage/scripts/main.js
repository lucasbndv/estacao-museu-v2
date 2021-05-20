var socket = io();
var chart;
let state = { type: "hourly", date: "", data: {}, loading: false };

let cards = [
  {
    title: "Temperatura(C°)",
    icon: "fas fa-temperature-high fa-2x",
    id: "temperature",
  },
  { title: "Umidade Relativa(%)", icon: "fas fa-tint fa-2x", id: "humidity" },
  {
    title: "Ponto de Orvalho(C°)",
    icon: "./assets/dewpoint.png",
    id: "dewpoint",
  },
  {
    title: "Umidade Absoluta(%)",
    icon: "./assets/absolut_humidity.png",
    id: "absolutehumidity",
  },
  {
    title: "Pressão(hPa)",
    icon: "fas fa-tachometer-alt fa-2x",
    id: "pressure",
  },
  {
    title: "Luminosidade(LDR)",
    icon: "fas fa-bolt fa-2x",
    id: "luminosity",
  },
  { title: "CO2 (ppm)", icon: "./assets/co2.png", id: "co2" },
  { title: "Poeira 1.0 (ppm)", icon: "fas fa-smog fa-2x", id: "dust10" },
  { title: "Poeira 2.5 (ppm)", icon: "fas fa-smog fa-2x", id: "dust25" },
  { title: "Poeira 10 (ppm)", icon: "fas fa-smog fa-2x", id: "dust100" },
  {
    title: "Ultima atualização",
    icon: "fas fa-clock fa-2x",
    id: "last_update_date:last_update_time",
  },
];

let init = async () => {
  let date = new Date().toLocaleDateString();
  state.date = date.slice(6) + `-${date.slice(3, 5)}-` + date.slice(0, 2);

  createCards();
  setupEvents();

  try {
    let resp = await fetch("http://localhost:3000/api?type=single", {
      method: "GET",
    });
    let data = await resp.json();
    updateData(data);

    // Chart
    setLoading(true);
    resp = await fetch(
      `http://localhost:3000/api?type=hourly&date_from=${state.date}`,
      {
        method: "GET",
      }
    );
    data = await resp.json();
    state.data = data;
    loadChart(data);
    setLoading(false);
  } catch (e) {
    console.log(e);
  }
  // Fresh update
};

socket.on("update_data", (income) => {
  let data = JSON.parse(income);
  console.log(income);
  updateData(income);
});

let updateData = (data) => {
  let all_data = document.getElementsByClassName("all_data");
  let date = data.datetime.slice(0, 10);
  let time = data.datetime.slice(11);

  for (let i = 0; i < all_data.length; i++) {
    if (all_data[i].id == "last_update_date") {
      all_data[i].innerText = date;
    } else if (all_data[i].id == "last_update_time") {
      all_data[i].innerText = time;
    } else {
      all_data[i].innerText = data[all_data[i].id];
    }
  }
};

let createCards = () => {
  let container = document.getElementById("card_container");
  let id_html, title_html, icon_html;
  let html = "";
  for (let i = 0; i < cards.length; i++) {
    icon_html = `<div class="card"><div class="card_icon">`;
    if (cards[i].icon[0] == "f") {
      icon_html += `<i class = "${cards[i].icon}"></i></div>`;
    } else {
      icon_html += `<img src="${cards[i].icon}"></img></div>`;
    }

    title_html = `<div class="card_texts"><h3>${cards[i].title}</h3>`;
    if (cards[i].title == "Ultima atualização") {
      let aux = cards[i].id.split(":");
      id_html = `<p id = "${aux[0]}" class="all_data"></p><p id = "${aux[1]}" class="all_data"></p></div></div>`;
    } else {
      id_html = `<p id="${cards[i].id}" class="all_data">0</p></div></div>`;
    }

    html += icon_html + title_html + id_html;
  }
  container.innerHTML = html;
};

function loadChart(input) {
  let datasets = createDatasets(input);
  // Hourly
  let times = [];
  input.datetime.forEach((item) => times.push(item.slice(11)));

  const data = {
    labels: times,
    datasets,
  };

  const config = {
    type: "line",
    data,
    options: {},
  };
  var ctx = document.getElementById("myChart");
  chart = new Chart(ctx, config);
}

function updateChartByFilter(input, type) {
  let datasets = createDatasets(input);
  let times = [];
  if (type == "hourly") {
    times = [
      "01:00",
      "02:00",
      "03:00",
      "04:00",
      "05:00",
      "06:00",
      "07:00",
      "08:00",
      "09:00",
      "10:00",
      "11:00",
      "12:00",
      "13:00",
      "14:00",
      "15:00",
      "16:00",
      "17:00",
      "18:00",
      "19:00",
      "20:00",
      "21:00",
      "22:00",
      "23:00",
    ];
  } else if (type == "daily") {
    let days_per_month = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
    for (
      let i = 1;
      i < days_per_month[parseInt(input.datetime[0].slice(5, 7)) - 1];
      i++
    ) {
      if (i < 10) {
        times.push(`0${i}`);
      } else {
        times.push(i);
      }
    }
  } else if (type == "monthly") {
    times = [
      "Janeiro",
      "Fevereiro",
      "Março",
      "Abril",
      "Maio",
      "Junho",
      "Julho",
      "Agosto",
      "Setembro",
      "Outubro",
      "Novembro",
      "Dezembro",
    ];
  }

  chart.data.datasets = datasets;
  chart.data.labels = times;
  chart.update();
}

function random_rgba() {
  var o = Math.round,
    r = Math.random,
    s = 255;
  return (
    "rgba(" +
    o(r() * s) +
    "," +
    o(r() * s) +
    "," +
    o(r() * s) +
    "," +
    r().toFixed(1) +
    ")"
  );
}

function setLoading(load) {
  state.loading = load;
  let loading = document.getElementById("loader");
  if (load) {
    loading.style.display = "flex";
  } else {
    loading.style.display = "none";
  }
}

function createDatasets(data) {
  let datasets = [];
  let i = 0;
  let labels = Object.keys(data);
  let randomColor;
  for (prop in data) {
    randomColor = random_rgba();

    if (prop != "datetime")
      datasets.push({
        label: labels[i],
        backgroundColor: randomColor,
        borderColor: randomColor,
        data: data[prop],
      });
    i++;
  }
  return datasets;
}

async function fetchWithFilter(type) {
  setLoading(true);
  let date = state.date;
  console.log(date);
  resp = await fetch(
    `http://localhost:3000/api?type=${type}&date_from=${date}`,
    {
      method: "GET",
    }
  );
  data = await resp.json();
  state.data = data;
  updateChartByFilter(data, type);
  setLoading(false);
}

function setupEvents() {
  // Default values
  let date_input = document.getElementById("date_input");
  date_input.value = state.date;

  // Filter Buttons
  let hourly = document.getElementById("hourly");
  let daily = document.getElementById("daily");
  let monthly = document.getElementById("monthly");

  hourly.addEventListener("click", async () => {
    fetchWithFilter("hourly");
    state.type = "hourly";
    daily.style.backgroundColor = "rgba(255, 255, 255, 0.781)";
    hourly.style.backgroundColor = "green";
    monthly.style.backgroundColor = "rgba(255, 255, 255, 0.781)";
  });
  daily.addEventListener("click", async () => {
    fetchWithFilter("daily");
    state.type = "daily";
    daily.style.backgroundColor = "green";
    hourly.style.backgroundColor = "rgba(255, 255, 255, 0.781)";
    monthly.style.backgroundColor = "rgba(255, 255, 255, 0.781)";
  });
  monthly.addEventListener("click", async () => {
    fetchWithFilter("monthly");
    state.type = "monthly";
    daily.style.backgroundColor = "rgba(255, 255, 255, 0.781)";
    hourly.style.backgroundColor = "rgba(255, 255, 255, 0.781)";
    monthly.style.backgroundColor = "green";
  });

  date_input.addEventListener("change", async () => {
    state.date = date_input.value;
    fetchWithFilter(state.type);
  });

  // Function buttons
  let download_button = document.getElementById("download");

  download_button.addEventListener("click", () => {
    if (state.loading) {
      window.alert(
        "Os dados ainda estão sendo carregados, por favor aguardar!"
      );
      return;
    }
    downloadData(state.data);
  });
}

function downloadData(input) {
  // transform to csv
  let keys = Object.keys(input);
  let ref = keys[0];
  keys[0] = `"${keys[0]}`;
  keys[keys.length - 1] = `${keys[keys.length - 1]}"`;
  let csv = [keys.join('","') + "\n"];
  let row = [];
  for (let i = 0; i < input[ref].length; i++) {
    for (prop in input) {
      row.push(`"${input[prop][i]}"`); // create a array of the row
    }

    csv += row.join(",") + " \n";
    row = [];
  }

  // Download part
  let blob = new Blob([csv], { type: "text/csv" });
  const url = window.URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.setAttribute("hidden", "");
  a.setAttribute("href", url);
  a.setAttribute("download", `dados-${state.date}.csv`);
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
}
window.onload = init;
