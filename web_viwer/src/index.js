const express = require("express");
const app = express();
const server = require("http").Server(app);
const io = require("socket.io")(server);
const bodyParser = require("body-parser");
const apiRoutes = require("./routes/api");

app.use(bodyParser.json());
app.use(apiRoutes);
app.use(express.static("webpage"));

server.listen(3000, () => {
  console.log(`Listening on port: ${server.address().port} ... `);
});

io.on("connection", (socket) => {
  console.log("Connected");
  socket.emit(
    "update_data",
    JSON.stringify({
      temperature: 30,
      humidity: 60,
      pressure: 1000,
      luminosity: 300,
      dust1: 5,
      dust2: 6,
      dust3: 3,
    })
  );
});
