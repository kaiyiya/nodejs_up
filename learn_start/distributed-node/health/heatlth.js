const server = require('fastify')();
const HOST = '127.0.0.1';
const PORT = 3300;
const redis = new (require("ioredis"))({
    host: '192.168.126.6',
    port: 6379,
    enableOfflineQueue: false
});
const pg = new (require('pg').Client)({
    host: '192.168.126.6',
    port: 5432,
    user: 'tmp',
    password: 'hunter2',
    database: 'tmp'
});
pg.connect();
server.get('/health', async (req, reply) => {
    try {
        const res = await pg.query('SELECT $1::text as status', ['ACK']);
        console.log(res);

        if (res.rows[0].status !== 'ACK') reply.code(500).send('DOWN');

    } catch (e) {
        reply.code(500).send('DOWN');
    }
    let status = 'OK';
    try {
        if (await redis.ping() !== 'PONG') status = 'DEGRADED';

    } catch (e) {
        status = 'DEGRADED';

    }

    reply.code(200).send(status);
})

server.listen(PORT, HOST, () => console.log(`http://${HOST}:${PORT}/`));