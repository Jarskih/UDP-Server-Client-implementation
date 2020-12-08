var PORT = 5555;
var HOST = '192.168.0.101'; // Change to server IP address
var dgram = require('dgram');
var server = dgram.createSocket('udp4');
var servers = [];
var timeOutTime = 30 * 1000; // 30 seconds to ms

const messagetypes = {
    HEARTBEAT: 1,
    SERVER_REQUEST: 2,
    DATA_PACKAGE: 6
}

server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

server.on('message', function (byteArray, remote) {

    servers = servers.filter((server) => {
        return Date.now() - server.time < timeOutTime
    });

    handleMessage(byteArray, remote);
});

function handleMessage(byteArray, remote) {
    var message;
    for (var i = 0; i < byteArray.length; i++) {
        message = (message << 8) | byteArray[i];
    }

    if (message == messagetypes.HEARTBEAT) {
        registerServer(remote);
        return;
    }

    if (message == messagetypes.SERVER_REQUEST) {
        sendServer(remote);
        return;
    }
};

function registerServer(remote) {
    const connection = {
        address: remote.address,
        time: Date.now()
    }
    let updated = false;
    servers.forEach(server => {
        if (server.address === remote.address) {
            server.time = Date.now();
            console.log("registerServer: Heartbeat received from server");
            updated = true;
            return;
        }
    });

    if (!updated) {
        servers.push(connection);
        console.log("registerServer: Added a new server");
        console.table(servers);
    }
};

function sendServer(remote) {
    console.log("sendServer: Client requested server address")
    if (servers.length > 0) {

        const offset = 0;
        const package_type = messagetypes.DATA_PACKAGE;
        const addressArr = servers[0].address.split('.'); // Choose first server in the server array

        var byteArray = new Uint8Array(addressArr.length + 1);
        for (var i = 0; i < addressArr.length + 1; i++) {
            if (i === 0) {
                byteArray[i] = package_type;
                continue;
            }
            byteArray[i] = addressArr[i - 1]
        }

        server.send(byteArray, offset, byteArray.length, remote.port, remote.address);
        console.log("sendServer: Sent server to client");
    } else {
        console.log("sendServer: Could not send server to client. No server registered")
    }
}

server.bind(PORT, HOST);