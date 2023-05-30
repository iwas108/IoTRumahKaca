function makeid(length) {
    let result = '';
    const characters = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    const charactersLength = characters.length;
    let counter = 0;
    while (counter < length) {
      result += characters.charAt(Math.floor(Math.random() * charactersLength));
      counter += 1;
    }
    return result;
}
// A $( document ).ready() block.
$( document ).ready(function() {
    const ctx = document.getElementById('myChart');

    new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
            label: 'Suhu Rumah Kaca',
            data: [],
            borderWidth: 1
            }]
        },
        options: {
            scales: {
                y: {
                    beginAtZero: true
                }
            },
            responsive: true,
            aspectRatio: 1|2
        }
    });

    // Create a client instance
    client = new Paho.MQTT.Client("2e8aeb43a4694b348a69cc8a4d4bf39a.s2.eu.hivemq.cloud", Number(8884), "undiknas-"+makeid());

    // set callback handlers
    client.onConnectionLost = onConnectionLost;
    client.onMessageArrived = onMessageArrived;

    // connect the client
    client.connect({
        onSuccess:onConnect, 
        userName: "undiknas", 
        password: "Und1kn45",
        useSSL: true
    });

    // called when the client connects
    function onConnect() {
        // Once a connection has been made, make a subscription and send a message.
        console.log("onConnect");
        client.subscribe("");
        message = new Paho.MQTT.Message("Hello");
        message.destinationName = "/World";
        client.send(message); 
    }

    // called when the client loses its connection
    function onConnectionLost(responseObject) {
        if (responseObject.errorCode !== 0) {
            console.log("onConnectionLost:"+responseObject.errorMessage);
        }
    }

    // called when a message arrives
    function onMessageArrived(message) {
        console.log("onMessageArrived:"+message.payloadString);
    }
 
});
