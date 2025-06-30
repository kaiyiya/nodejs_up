const http = require("http");
const server = http.createServer((req, res) => {
  res.end("ok");
});
// server.maxConnections = 2;
server.listen(4000, "192.168.87.51");
