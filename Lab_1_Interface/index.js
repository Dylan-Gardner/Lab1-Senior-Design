// Page Load
window.onload = function() {
    data = makeData();
    drawGraph(data);
    document.getElementById("current-temp-visual").innerHTML = Number.parseFloat(data[data.length-1].temp).toFixed(1);
};

// Update Data
function update() {
    generate();
    if (currentunit == "°C"){
        document.getElementById("current-temp-visual").innerHTML = Number.parseFloat(data[data.length-1].temp).toFixed(1);
    } else {
        var tempNum = (data[data.length-1].temp * 9/5) + 32
        document.getElementById("current-temp-visual").innerHTML = Number.parseFloat(tempNum).toFixed(1);
    }
    updateGraph(data);
}

// Generate Data for Demo
function generate() {
    // if (data.length > 10 && data.length < 20) {
    //     data.push({
    //         time: 0,
    //         temp: +null
    //     });
    // } else {

    data.push({
        time: 0,
        temp: +Math.random() * (30.0 - 20.0) + 20.0
    });

    // }

    for (i = 0; i < data.length-1; i++) {
        data[i].time = data[i].time + 1;
    }
    if(data.length >= shiftpoint) {
        data.shift();
    }
    

}


// Hardcoded Data Initialization
function makeData() {

    arr = [];
    arr.push({
        time: 0, 
        temp: +Math.random() * (30.0 - 20.0) + 20.0
    });
    return arr;

}

// Draw The Graph, No Data
function drawGraph(data) {

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

}

// Update The Graph
function updateGraph(data) {

    var svgWidth = 800, svgHeight = 400;
    var margin = { top: 20, right: 20, bottom: 30, left: 50 };
    var width = svgWidth - margin.left - margin.right;
    var height = svgHeight - margin.top - margin.bottom;
    
    var svg = d3.select('#line-chart')
        .attr("width", svgWidth)
        .attr("height", svgHeight)

    var u = d3.select('#line-chart')
        .selectAll('path')
        .data(data)
        
    u.exit().remove();    

    // var yAx = d3.selectAll('.ticks')

    // yAx.exit().remove();
        
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

    g.append("path")
        .datum(data)
        .attr("fill", "none")
        .attr("stroke", "rgb(0, 147, 175)")
        .attr("stroke-width", 2.0)
        .attr("d", line);   

}

document.getElementById('unit-change-button').onclick = function changeContent() {
    if (document.getElementById('unit-change-button').innerHTML == "Change to Fahrenheit"){
        unitChange("°F", "Celsius");
    } else if (document.getElementById('unit-change-button').innerHTML == "Change to Celsius") {
        unitChange("°C", "Fahrenheit");
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
    updateGraph(data);
}

var data = [];
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





