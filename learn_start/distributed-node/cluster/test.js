const cluster = require('cluster')
console.log(`master pid is ${process.pid}`);
cluster.setupMaster({
    exec: '../recipe-api/producer-http-basic.js'
})
cluster.fork();
cluster.fork()
cluster.on('disconnect', (worker) => {
    console.log('disconnect', worker.id);
}).on('exit', (worker, code, signal) => {
    console.log('exit', worker.id, code, signal);
}).on('listening', (worker, { address, port }) => {
    console.log('listening', worker.id, `${address}:${port}`);

})


