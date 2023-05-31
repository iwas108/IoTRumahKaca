var ambangBatas = 25;
var dataCount = 0;
var groupId = 1;

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

    var myChart = new Chart(ctx, {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
            label: 'Suhu',
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
    client = new Paho.MQTT.Client("2e8aeb43a4694b348a69cc8a4d4bf39a.s2.eu.hivemq.cloud", Number(8884), "undiknas-"+makeid(8));

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
        client.subscribe("undiknas/ti/sensor/suhu/"+groupId);
        client.subscribe("undiknas/ti/aktuator/kipas/"+groupId);
        client.subscribe("undiknas/ti/aktuator/kipas/"+groupId+"/ambang-batas");
        //message = new Paho.MQTT.Message("Hello");
        //message.destinationName = "/World";
        //client.send(message); 
    }

    // called when the client loses its connection
    function onConnectionLost(responseObject) {
        if (responseObject.errorCode !== 0) {
            console.log("onConnectionLost:"+responseObject.errorMessage);
        }
    }

    // called when a message arrives
    function onMessageArrived(message) {
        console.log(message.payloadString);
        if(message.destinationName == "undiknas/ti/sensor/suhu/"+groupId){
            let status = message.payloadString > ambangBatas ? "Panas" : "Normal";
            let data = message.payloadString + "°C " + "(" + status + ")";
            $("#data-suhu").html(data);

            var date = new Date;
            var seconds = date.getSeconds();
            var minutes = date.getMinutes();
            var hour = date.getHours();
            var label = hour + ":" + minutes + ":" + seconds;
            addData(myChart, label, message.payloadString);
    
            if(dataCount > 10){
                shiftData(myChart);
            }
            dataCount++;
        }
        else if(message.destinationName == "undiknas/ti/aktuator/kipas/"+groupId){
            if(message.payloadString == "ON")
            {
                $("#statusBlower").html('<button type="button" class="btn btn-success">☢️ ON</button>');
            }
            else
            {
                $("#statusBlower").html('<button type="button" class="btn btn-danger">☢️ OFF</button>');
            }
        }
        else if(message.destinationName == "undiknas/ti/aktuator/kipas/"+groupId+"/ambang-batas"){
            ambangBatas = parseFloat(message.payloadString);
            $('#rangeSuhu').val(ambangBatas);
            $("#ambangBatas").html($(ambangBatas));
        }
    }

    $('#rangeSuhu').on("change", function() {
        $("#ambangBatas").html($(this).val());
        ambangBatas = $(this).val();
        message = new Paho.MQTT.Message(ambangBatas);
        message.destinationName = "undiknas/ti/aktuator/kipas/"+groupId+"/ambang-batas";
        client.send(message);
    });

    function addData(chart, label, data) {
        chart.data.labels.push(label);
        chart.data.datasets.forEach((dataset) => {
            dataset.data.push(data);
        });
        chart.update();
    }
    
    function shiftData(chart, label, data) {
        chart.data.labels.shift();
        chart.data.datasets.forEach((dataset) => {
            dataset.data.shift();
        });
        chart.update();
    }
 
});
