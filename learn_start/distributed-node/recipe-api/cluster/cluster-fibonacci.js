#!/usr/bin/env node
const server = require("fastify")();
const HOST = process.env.HOST || "127.0.0.1";
const PORT = process.env.PORT || 4000;
console.log(`worker pid=${process.pid}`);
server.get("/:limit", async (req, reply) => {
  return String(fib(Number(req.params.limit)));
});

function fib(limit) {
  let prev = 1n,
    next = 0n,
    swap;
  while (limit) {
    swap = prev;
    prev = prev + next;
    next = swap;
    limit--;
  }
  return next;
}

server.listen(PORT, HOST, () => {
  console.log(`Producer running at http://${HOST}:${PORT}`);
});
