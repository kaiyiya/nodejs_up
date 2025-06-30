#!/usr/bin/env node
const server = require("fastify")();
const fetch = require("node-fetch");
const HOST = process.env.HOST || "192.168.87.51";
const PORT = process.env.PORT || 3002;
const TARGET = process.env.TARGET || "localhost:4000";

server.get("/health", async () => {
  console.log("health check");
  return "ok";
});
server.get("/", async () => {
  const req = await fetch(`http://${TARGET}/recipes/42`);
  const producer_data = await req.json();
  return {
    consumer_pid: process.pid,
    producer_data,
  };
});
server.listen(PORT, HOST, () => {
  console.log(`Consumer  running at http://${HOST}:${PORT}`);
});
