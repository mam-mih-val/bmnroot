
    const http = require('http');

    const hostname = '127.0.0.1';
    const port = 3000;

    var net = require('net');
    var dataHost = '10.18.11.32';
    var dataPort = 33306;
    var strBuf = '';
    var t = 0;
    var runs = 0;
    var d3 = require('d3');
    var jsdom = require('jsdom');
    var sap = require('save-svg-as-png');
    var date = new Date();
    var htext = null;
    function makeHist(obj) {
        var binCount = 4096;
        var width = 1200;
        var height = 900;
        var max = binCount;//d3.max(nums);
        var min = 1;//d3.min(nums);
        var wScale = d3.scaleLinear().domain([min, max]).range([0, width]);
        var document = jsdom.jsdom();

        for (var iChannel = 1; iChannel <= 4; iChannel++) {
            var chName = 'channel ' + iChannel;
            var nums = obj.DAQ.histPlot[chName];
            var h = document.createElement("H1");
            var t = document.createTextNode(chName);
            h.appendChild(t);
            document.body.appendChild(h);
            var svg = d3.select(document.body).append("svg")
                .attr("height", "100%")
                .attr("width", "100%");
            //nums[2000] = 20;
            //nums[4095] = 40;
            var maxy = d3.max(nums)+ 10;
            var miny = 1;
            var hScale = d3.scaleLog().domain([miny, maxy]).range([0, height]);
            /*var xAxis = d3.svg.axis().scale(wScale);
            var yAxis = d3.svg.axis().scale(hScale).orient("left");*/
            var xAxis = d3.axisBottom()
                .scale(wScale);
            var yAxis = d3.axisRight()
                .scale(hScale);
            svg.append("g")
                .attr("transform", "translate(0," + width -10  + ")")
                .call(xAxis);
            svg.append("g")
                //.attr("transform", "translate(" + height -100 + ", 0)")
                .call(yAxis);
                //Draw a grid
            var yAxisGrid = yAxis
                .tickSize(width, 0)
                .tickFormat("")
                //.orient("right")

            var xAxisGrid = xAxis
                .tickSize(height, 0)
                .tickFormat("")
                //.orient("top")

            svg.append("g")
                .classed('y', true)
                .classed('axis', true)
                .call(yAxisGrid);

            svg.append("g")
                .classed('x', true)
                .classed('axis', true)
                .call(xAxisGrid);

            var time = new Date().getTime();

            svg.selectAll("rect")
                .data(nums)
                .enter().append("rect")
                .attr("height", function (d, i) {
                    return hScale(d)
                })
                .attr("width", "3")
                .attr("x", function (d, i) {
                    return wScale(i)
                })
                .attr("y", function (d, i) {
                    return height - hScale(d)
                })
                .attr("stroke", "black")
                .attr("stroke-width", "1")
                .style('width', function (d, i) {
                    return wScale(i) + 'px';
                });

            /*
            var html = document.querySelector("svg").parentNode.innerHTML;
            var XMLSerializer = require('xmldom').XMLSerializer;
            svgData = new XMLSerializer().serializeToString( svg );
            //sap.saveSvgAsPng(svgData, "diagram.png", {scale: 0.5});
            //var svg = document.querySelector( "svg" );
            //var svgData = new XMLSerializer().serializeToString( svg );
           html = document.querySelector("svg").parentNode.innerHTML;
            //console.log(html)

            //var canvas = d3.select(document.body).append("canvas");
            var canvas = document.createElement("canvas");
            canvas.width = 800;//document.querySelector("svg").parentNode.attr("width");
            canvas.height = 600;//svg.height;//d3.select("svg").attr("height");
            console.log('svg width ' + svg.width);
            console.log('can className ' + canvas.className);

            var ctx = canvas.getContext("2d");
            console.log('ctx ' + ctx)

            var img = document.createElement( "img" );
            img.setAttribute( "src", "data:image/svg+xml;base64," + svgData.toString('base64') );

            //img.onload = function() {
                ctx.drawImage( img, 0, 0 );

                var canvasdata = canvas.toDataURL("image/png");

                console.log(canvasdata)
                var pngimg = '<img src="'+canvasdata+'">';
                d3.select("#pngdataurl").html(pngimg);

                var a = document.createElement("a");

                a.download = "name"+".png";
                a.href = canvasdata;
                console.log(a.click())
            //};
*/
            time = new Date().getTime() - time;
            console.log('time spent = ', time);
        }
        var docText = jsdom.serializeDocument(document);
        return docText;
    }
    var client;
    var timeoutData;
    var timeoutParse;
    var strQue = Array();
    function conn(){
        client = new net.Socket();
        client.connect(dataPort, dataHost, function () {
            strBuf = '';
            t = 0;
            console.log('CONNECTED TO: ' + dataHost + ':' + dataPort);
        });
        client.on('data', function (data) {
            strQue.push(data);
            //console.log('DATA: ' + t++ + ' fragment: \n' + data);
        });
        client.on('end', function () {
            //client.destroy();
            console.log('end message');
        });
        client.on('close', function () {
            client.destroy();
            console.log('Connection closed');
            setTimeout(conn, 3000);
        });
        client.on('error', function () {
            client.destroy();
            console.log('Connection error');
            //setTimeout(conn(), 3000);
        });
        /*client.on('\n', function () {
            console.log('str ' + strBuf)
            alert('str ' + strBuf)
            if (strBuf.length == 0)
                return;
            alert('end')
            console.log('end');
            //client.destroy();
        });*/
    }
    function getData() {
        client.write('a');
        //timeoutData = setInterval(getData, 10000);
    }

    function doParse() {
        while (strQue.length > 0){
            var chunk = strQue.shift();
            strBuf += chunk;
            var str = chunk.toString();
            //console.log('   last ' + str[str.length - 1])
            //var regex = '([^\\n]+)\\n(.*)';
            if (str[str.length - 1] == '\n') {
                //console.log(strBuf);
                var obj = JSON.parse(strBuf);
                strBuf = '';
                str = null;
                t = 0;
                htext = makeHist(obj);
                break;
                //console.log(htext);
                //console.log('parsed ' + obj.DAQ.histPlot.histogramStep)
                // console.log('parsed ' + obj.DAQ.histPlot["channel 1"])
            }
        }
        timeoutData = setTimeout(doParse, 3000);
    }

    conn();
    timeoutData = setInterval(getData, 3000);
    timeoutParse = setTimeout(doParse, 5000);

    const server = http.createServer((req, res) => {
        console.log('Run ' + runs++);
        res.statusCode = 200;
        res.setHeader('Content-Type', 'text/html');
        if (htext != null)
            res.end(htext);
        else
            res.end();
    });
    server.listen(port, hostname, () => {
        console.log(`Server running at http://${hostname}:${port}/`);
    });