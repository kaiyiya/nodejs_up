#!/usr/bin/env node
const cluster = require("cluster");
const os = require("os");

if (cluster.isMaster) {
  // 主进程：创建工作进程
  const cpuCount = os.cpus().length;
  console.log(`Master ${process.pid} started with ${cpuCount} workers`);

  for (let i = 0; i < cpuCount; i++) {
    cluster.fork();
  }

  cluster.on("exit", (worker) => {
    console.log(`Worker ${worker.process.pid} died. Restarting...`);
    cluster.fork();
  });
} else {
  // 工作进程：启动服务
  const server = require("fastify")();
  const HOST = process.env.HOST || "127.0.0.1";
  const PORT = process.env.PORT || 4000;

  console.log(`Worker pid=${process.pid} started`);

  server.get("/:limit", async (req, reply) => {
    return String(fib(Number(req.params.limit)));
  });

  function fib(limit) {
    /* 原有计算逻辑 */
  }

  server.listen(PORT, HOST);
}
