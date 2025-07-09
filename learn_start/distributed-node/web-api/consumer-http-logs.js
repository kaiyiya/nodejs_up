#!/usr/bin/env node
const server = require("fastify")();
const fetch = require("node-fetch");
const HOST = process.env.HOST || "127.0.0.1";
const PORT = process.env.PORT || 3000;
const TARGET = process.env.TARGET || "localhost:4000";

const log = require('./logstash.js')
  (async () => {
    await server.register(require('middie'));
    server.use((req, res, next) => {
      log('info', 'request-incoming', {
        path: req.url, method: req.method, ip: req.ip,
        ua: req.headers['user-agent'] || null
      })
      next();
    })
    server.setErrorHandler(async (error, req) => {
      log('error', 'request-failure', {
        stack: error.stack,
        path: req.url,
        method: req.method
      })
      return { error: error.message }
    });
    server.get("/", async () => {
      const req = await fetch(`http://${TARGET}/recipes/42`);
      const producer_data = await req.json();
      return {
        consumer_pid: process.pid,
        producer_data,
      };
    });
    server.get('/error', async () => { throw new Error('oh no') })
    server.listen(PORT, HOST, () => {
      console.log(`Consumer  running at http://${HOST}:${PORT}`);
      log('verbose', 'listen', { host: HOST, port: PORT })
    });

  }


  )();





