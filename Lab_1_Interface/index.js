// Page Load
window.onload = function() {
    // dataset = makeData();
    csvData();
    drawGraph(dataset);
    //updateGraph(dataset);
};

// Update Data
function update() {
    generate();
    if (currentunit == "°C"){
        document.getElementById("current-temp-visual").innerHTML = Number.parseFloat(dataset[dataset.length-1].temp).toFixed(1);
    } else {
        var tempNum = (dataset[dataset.length-1].temp * 9/5) + 32
        document.getElementById("current-temp-visual").innerHTML = Number.parseFloat(tempNum).toFixed(1);
    }
    updateGraph(dataset);
}

// Generate Data for Demo
function generate() {
    // if (data.length > 10 && data.length < 20) {
    //     data.push({
    //         time: 0,
    //         temp: +null
    //     });
    // } else {

    dataset.push({
        time: 0,
        temp: +Math.random() * (30.0 - 20.0) + 20.0
    });

    // }

    for (i = 0; i < dataset.length-1; i++) {
        dataset[i].time = dataset[i].time + 1;
    }
    if(dataset.length >= shiftpoint) {
        dataset.shift();
    }
    

}


// Hardcoded Data Initialization
function makeData() {

    arr = [];
    arr.push({
        time: 5, 
        temp: +Math.random() * (30.0 - 20.0) + 20.0
    })
    arr.push({
        time: 4, 
        temp: +Math.random() * (30.0 - 20.0) + 20.0
    })
    arr.push({
        time: 3, 
        temp: +Math.random() * (30.0 - 20.0) + 20.0
    })
    arr.push({
        time: 2, 
        temp: +Math.random() * (30.0 - 20.0) + 20.0
    })
    arr.push({
        time: 1, 
        temp: +Math.random() * (30.0 - 20.0) + 20.0
    })
    arr.push({
        time: 0, 
        temp: +Math.random() * (30.0 - 20.0) + 20.0
    })
    return arr;

}

// Draw The Graph, No Data
// function drawGraph() {

//     var svgWidth = 800, svgHeight = 400;
//     var margin = { top: 20, right: 20, bottom: 30, left: 50 };
//     var width = svgWidth - margin.left - margin.right;
//     var height = svgHeight - margin.top - margin.bottom;

    // var svg = d3.select('#line-chart')
    //     .attr("width", svgWidth)
    //     .attr("height", svgHeight)
    
    // var g = svg.append("g")
    //     .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

    // var x = d3.scaleTime()
    //     .domain([300, 0])
    //     .rangeRound([0, width]);

    // var y = d3.scaleLinear()
    //     .domain([min, max])
    //     .rangeRound([height, 0]);  

    // var line = d3.line()
    //     .x(function(d) { return x(d.time)})
    //     .y(function(d) { return y(d.temp)});   

    // g.append("g")
    //     .attr("transform", "translate(0," + height + ")")
    //     .call(d3.axisBottom(x)
    //     .tickFormat(d3.format(".0f")))
    //     .append("text")
//         .attr("fill", "#000")
//         .attr("y", -10)
//         .attr("dx", "70.0em")
//         .text("Seconds Ago");

//     g.append("g")
//         .call(d3.axisLeft(y))
//         .append("text")
//         .attr("fill", "#000")
//         .attr("x", 80)
//         .attr("dy", "1.0em")
//         .text("Temperature (°C)");    

//     d3.csv("temp_data.csv")
//         .get(function(error,data){  

//         tempData = data;
//         storeData(tempData);    

//         g.append("path")
//             .datum(dataset)
//             .attr("fill", "none")
//             .attr("stroke", "rgb(0, 147, 175)")
//             .attr("stroke-width", 2.0)
//             .attr("d", line);  
        
//         document.getElementById("current-temp-visual").innerHTML = Number.parseFloat(dataset[dataset.length-1].temp).toFixed(1);     
        
//     });    

// }

function csvData() {

    d3.csv("temp_data.csv")
    .get(function(error,data){  

        var tempData = data;
        storeData(tempData);

    });

}

function storeData(tD) {
    tD.forEach(function(d){
        dataset.push({
            time: +d.time,
            temp: +d.temp
        })    
    })
}

function drawGraph(dataset) {
    var svgWidth = 800, svgHeight = 400;
    var margin = { top: 20, right: 20, bottom: 30, left: 50 };
    var width = svgWidth - margin.left - margin.right;
    var height = svgHeight - margin.top - margin.bottom;
    
    var svg = d3.select('#line-chart')
        .attr("width", svgWidth)
        .attr("height", svgHeight)      
        
    var g = svg.append("g")
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")");
    
    var x = d3.scaleTime()
        .domain([300, 0])
        .rangeRound([0, width]);
    
    var y = d3.scaleLinear()
        .domain([min, max])
        .rangeRound([height, 0]);  
    
    var line = d3.line()
        .defined(function (d){ return d.temp != 0; })
        .x(function(d) { return x(d.time)})
        .y(function(d) { return y(d.temp)});
    
    g.append("g")
        .attr("transform", "translate(0," + height + ")")
        .call(d3.axisBottom(x)
        .tickFormat(d3.format(".0f")))
        .append("text")
        .attr("fill", "#000")
        .attr("y", -10)
        .attr("dx", "70.0em")
        .text("Seconds Ago");
    
    g.append("g")
        .call(d3.axisLeft(y))
        .append("text")
        .attr("fill", "#000")
        .attr("x", 80)
        .attr("dy", "1.0em")
        .text("Temperature (°C)");  

    // var path = svg.selectAll("path").data(dataset);

    // path.exit().remove();

    // g.append("path")
    //     .datum(dataset)
    //     .attr("fill", "none")
    //     .attr("stroke", "rgb(0, 147, 175)")
    //     .attr("stroke-width", 2.0)
    //     .attr("d", line);    

    // path.enter().append("path")   
    
    // console.log("initial draw")
    
}

// Update The Graph
function updateGraph(dataset) {

    var svgWidth = 800, svgHeight = 400;
    var margin = { top: 20, right: 20, bottom: 30, left: 50 };
    var width = svgWidth - margin.left - margin.right;
    var height = svgHeight - margin.top - margin.bottom;

    d3.selectAll("svg > *").remove();
    
    var svg = d3.select('#line-chart')
        .attr("width", svgWidth)
        .attr("height", svgHeight)       
        
    var g = svg.append("g")
        .attr("transform", "translate(" + margin.left + "," + margin.top + ")"); 
        
    var path = svg.selectAll("path").data(dataset);
    
    var x = d3.scaleTime()
        .domain([300, 0])
        .rangeRound([0, width]);
    
    var y = d3.scaleLinear()
        .domain([min, max])
        .rangeRound([height, 0]);  
    
    var line = d3.line()
        .defined(function (d){ return d.temp != 0; })
        .x(function(d) { return x(d.time)})
        .y(function(d) { return y(d.temp)});
    
    g.append("g")
        .attr("transform", "translate(0," + height + ")")
        .call(d3.axisBottom(x)
        .tickFormat(d3.format(".0f")))
        .append("text")
        .attr("fill", "#000")
        .attr("y", -10)
        .attr("dx", "70.0em")
        .text("Seconds Ago");
    
    g.append("g")
        .call(d3.axisLeft(y))
        .append("text")
        .attr("fill", "#000")
        .attr("x", 80)
        .attr("dy", "1.0em")
        .text("Temperature (°C)");  

    g.append("path")
        .datum(dataset)
        .attr("fill", "none")
        .attr("stroke", "rgb(0, 147, 175)")
        .attr("stroke-width", 2.0)
        .attr("d", line);

    //path.exit().remove();    

    //path.enter().append("path")     

    // else {

    //     var path = svg.selectAll("path").data(dataset);

    //     path.enter().append("path")

    // }

    // d3.select('#line-chart')
    //     .selectAll('path')
    //     .data(dataset)
    //     .enter();    
        
    //document.getElementById("current-temp-visual").innerHTML = Number.parseFloat(dataset[dataset.length-1].temp).toFixed(1);  

        // g.append("path")
        //     .datum(dataset)
        //     .attr("fill", "none")
        //     .attr("stroke", "rgb(0, 147, 175)")
        //     .attr("stroke-width", 2.0)
        //     .attr("d", line)    
 

}

document.getElementById('unit-change-button').onclick = function changeUnits() {
    if (document.getElementById('unit-change-button').innerHTML == "Change to Fahrenheit"){
        unitChange("°F", "Celsius");
    } else if (document.getElementById('unit-change-button').innerHTML == "Change to Celsius") {
        unitChange("°C", "Fahrenheit");
    }
}

document.getElementById('text-update-entry').onkeypress = function(e){
    if (!e) e = window.event;
    var keyCode = e.keyCode || e.which;
    if (keyCode == '13'){
        alert("Setting up Text Updates with " + document.getElementById('text-update-entry').value);
    }
}

// User Messages Demo

// document.getElementById('message-test-button').onclick = function changeContent() { 

//     switch (document.getElementById('current-data-message').innerHTML){
//         case "✓ Data is Available":
//             document.getElementById('current-data-message').innerHTML = "Data is Unavailable";
//             document.getElementById('current-data-message').style.color = "red";
//             break;
//         case "Data is Unavailable":
//             document.getElementById('current-data-message').innerHTML = "✓ Data is Available";
//             document.getElementById('current-data-message').style.color = "green";
//             break;    
//     }

//     switch (document.getElementById('current-sensor-message').innerHTML){
//         case "✓ Sensor is Plugged In":
//             document.getElementById('current-sensor-message').innerHTML = "Sensor is Unplugged";
//             document.getElementById('current-sensor-message').style.color = "red";
//             break;
//         case "Sensor is Unplugged":
//             document.getElementById('current-sensor-message').innerHTML = "✓ Sensor is Plugged In";
//             document.getElementById('current-sensor-message').style.color = "green";
//             break;    
//     }

// }

function unitChange(unit, string) {
    document.getElementById('unit-change-button').innerHTML = "Change to " + string;
    document.getElementById('current-temp-heading').innerHTML = "Current Temperature (" + unit + ")";
    currentunit = unit;
    update();
    updateGraph();
}

var init = true;
var dataset = [];
var refesh = setInterval(update, 1000);
var shiftpoint = 301;
var max = 50;
var min = 10;
var currentunit = "°C";


/* Continued Implementation

- Unplugged Sensor Message
- No Data Available Message
- User Action to Turn Off Temperature Display
- Data from the Hardware if it was one once the Web UI boots up
- Make Graph Scrollable Horizontally
- Scalable
- Missing Data (Hardware is Off, Data is too Small, Data is too Large)
- If the Hardware is Off the Live Data should continue to come and be missing

Finished but just need to implement it with data from hardware
- Missing Data can be shown with null in the data array

*/






// /*
// Line Graph Bitcoin Example
// */

// //  API to fetch historical data of Bitcoin Price Index
// const api = 'https://api.coindesk.com/v1/bpi/historical/close.json?start=2017-12-31&end=2018-04-01';

/**
 * Loading data from API when DOM Content has been loaded'.
 */
// document.addEventListener("DOMContentLoaded", function(event) {
// fetch(api)
//     .then(function(response) { return response.json(); })
//     .then(function(data) {
//         var parsedData = parseData(data);
//         drawChart(parsedData);
//     })
//     .catch(function(err) { console.log(err); })
// });

// document.getElementById('test').onclick() = function() {
//     alert("something happened");
// };

/**
 * Parse data into key-value pairs
 * @param {object} data Object containing historical data of BPI
 */
// function parseData(data) {
//     var arr = [];
//     for (var i in data.bpi) {
//         arr.push({
//             date: new Date(i), //date
//             value: +data.bpi[i] //convert string to number
//         });
//     }
//     return arr;

// }





