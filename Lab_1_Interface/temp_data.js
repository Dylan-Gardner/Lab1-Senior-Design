d3.csv("temp_data.csv")
    .get(function(error,data){
        tempData = data;
        storeData(tempData);
    });

function storeData(tD) {
    tD.forEach(function(d){
        dataset.push({
            time: +d.time,
            temp: +d.temp
        })    
    })
}    

function update() {
    dataset.push({
        time: 0,
        temp: +Math.random() * (30.0 - 20.0) + 20.0
    });

    // }

    for (i = 0; i < dataset.length-1; i++) {
        dataset[i].time = dataset[i].time + 1;
    }
    console.log(dataset);
}

var dataset = []  
var refesh = setInterval(update, 1000);  

