window.onload = function() {
    data = makeData();
    drawGraph(data);
    document.getElementById("current-temp-visual").innerHTML = Number.parseFloat(data[data.length-1].temp).toFixed(1);
};

var currentTime = [];
var data = [];

/*
Test Button
*/
document.onclick = clickListener;

function clickListener(e) {
    if(e.target.tagName == 'BUTTON') {
        generate();
        d3.selectAll("svg > *").remove()
        document.getElementById("current-temp-visual").innerHTML = Number.parseFloat(data[data.length-1].temp).toFixed(1);
        drawGraph(data);
    }
};

function generate() {
    currentTime[5] = currentTime[5] + 1;
    data.push({
        time: currentTime,
        temp: +Math.random() * (30.0 - 20.0) + 20.0
    });
    // alert(data.length);
    //alert(data[data.length-1].temp);
    // alert(currentTime);

}

/*
Hardcoded Data
*/

function makeData() {

    arr = [];
    arr.push({
        time: 4, 
        temp: +25.5
    });
    arr.push({
        time: 3, 
        temp: +26.0
    });
    arr.push({
        time: 2, 
        temp: +25.5
    });
    arr.push({
        time: 1, 
        temp: +27.0
    });
    arr.push({
        time: 0, 
        temp: +29.0
    })
    currentTime = [2019, 9, 22, 9, 30, 24, 00];
    return arr;

}

/*
Line Graph
*/

function drawGraph(data) {

var svgWidth = 800, svgHeight = 400;
var margin = { top: 20, right: 20, bottom: 30, left: 50 };
var width = svgWidth - margin.left - margin.right;
var height = svgHeight - margin.top - margin.bottom;


var svg = d3.select('#line-chart')
    .attr("width", svgWidth)
    .attr("height", svgHeight);
    
var g = svg.append("g")
    .attr("transform", "translate(" + margin.left + "," + margin.top + ")");

var x = d3.scaleTime()
    .domain([300, 0])
    .rangeRound([0, width]);

var y = d3.scaleLinear()
    .domain([10, 50])
    .rangeRound([height, 0]);  

var line = d3.line()
    .x(function(d) { return x(d.time)})
    .y(function(d) { return y(d.temp)});

g.append("g")
    .attr("transform", "translate(0," + height + ")")
    .call(d3.axisBottom(x))
    .append("text")
    .attr("fill", "#000")
    .attr("y", -10)
    .attr("dx", "70.0em")
    .text("Seconds Ago");

g.append("g")
    .call(d3.axisLeft(y))
    .append("text")
    .attr("fill", "#000")
    .attr("x", 60)
    .attr("dy", "1.0em")
    .text("Temperature");

g.append("path")
    .datum(data)
    .attr("fill", "none")
    .attr("stroke", "rgb(0, 147, 175)")
    .attr("stroke-width", 2.0)
    .attr("d", line);
    
}

/* Continued Implementation

- Connection to Hardware
- New data appends correctly
- Set axises, 0-300 seconds, 10-50 degrees C
- Text Updates
- Scalable
- Message when Hardware is off
- Missing Data

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





