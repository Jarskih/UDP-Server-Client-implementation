var PORT = 5555;
var HOST = 'localhost';
var dgram = require('dgram');
var server = dgram.createSocket('udp4');
var servers = [];
var timeOutTime = 900 * 1000; // 15 min to ms

const messagetypes = {
    HEARTBEAT: "0",
    SERVER_REQUEST: "1",
    DATA_PACKAGE: "6"
}

server.on('listening', function () {
    var address = server.address();
    console.log('UDP Server listening on ' + address.address + ":" + address.port);
});

server.on('message', function (message, remote) {

    servers = servers.filter((server) => {
        return Date.now() - server.time < timeOutTime
    });

    console.log("Message from server:" + remote.address + ':' + remote.port + ' - ' + message.toString());
    handleMessage(message, remote);
});

function handleMessage(message, remote) {
    switch (message.toString()) {
        case (messagetypes.HEARTBEAT):
            registerServer(remote);
            break;
        case (messagetypes.SERVER_REQUEST):
            sendServer(remote);
            break;
        default:
            console.error("Unknown message");
            break;
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
            console.log("Time updated");
            updated = true;
            return;
        }
    });

    if (!updated) {
        servers.push(connection);
        console.log("Added server");
        console.table(servers);
    }
};

function sendServer(remote) {
    if (servers.length > 0) {

        const offset = 0;
        const package_type = messagetypes.DATA_PACKAGE;
        const addressArr = servers[0].address.split('.');

        var byteArray = new Uint8Array(addressArr.length + 1);
        for (var i = 0; i < addressArr.length + 1; i++) {
            if (i === 0) {
                byteArray[i] = package_type;
                continue;
            }
            byteArray[i] = addressArr[i - 1]
        }

        server.send(byteArray, offset, byteArray.length, remote.port, remote.address);
        console.log("Sent server to client");
        console.log(byteArray);
    } else {
        console.log("Could not send server to client. No server registered")
    }
}

server.bind(PORT, HOST);