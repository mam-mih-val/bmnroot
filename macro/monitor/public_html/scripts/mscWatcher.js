
    const http = require('http');
    var jsroot = require("jsroot");

    const hostname = '127.0.0.1';
    const port = 3000;

    var net = require('net');
    var dataHost = '10.18.11.32';
    var dataPort = 33306;
    var strBuf = '';
    var nbinsX = 8192;
    var t = 0;
    var runs = 0;
    var d3 = require('d3');
    var jsdom = require('jsdom');
    var sap = require('save-svg-as-png');
    var date = new Date();
    var htext = null;
    function makeHist(obj) {
        var time = new Date().getTime();
        var binCount = 8192;
        var width = 1280;
        var height = 720;
        var max = binCount;//d3.max(nums);
        var min = 1;//d3.min(nums);
        var wScale = d3.scaleLinear().domain([min, max]).range([0, width]);
        var document = jsdom.jsdom();
           /* "fNbins" : 6,
            "fXmin" : 0,
            "fXmax" : 6,*/
        /*for (var iChannel = 1; iChannel <= 4; iChannel++) {
            var chName = 'channel ' + iChannel;
            var nums = obj.DAQ.histPlot[chName];
            var h = document.createElement("H1");
            var t = document.createTextNode(chName);
            h.appendChild(t);
            console.log('nums len ' + nums.length)
            document.body.appendChild(h);
            var svg = d3.select(document.body).append("svg")
                .attr("height", "100%")
                .attr("width", "100%");
            var maxy = d3.max(nums)+ 10;
            var miny = 1;
            var hScale = d3.scaleLog().domain([miny, maxy]).range([0, height]);
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
                .attr("width", "1")
                .attr("x", function (d, i) {
                    return wScale(i)
                })
                .attr("y", function (d, i) {
                    return height - hScale(d)
                });*/

        /*    time = new Date().getTime() - time;
            console.log('time spent = ', time);
        }*/

        time = new Date().getTime() - time;
        console.log('time spent = ', time);
        var docText = jsdom.serializeDocument(document);
        return docText;
    }
    var client;
    var timeoutData;
    var timeoutParse;
    var strQue = [];
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

                //hist.fArray = obj.DAQ.histPlot["channel 1"];
                hist.fName = "qq";
                hist.fTitle = "qq";
                //hist.fXaxis.fTimeDisplay = true;
                hist.fXaxis.fTitleColor = 810;
                hist.fXaxis.fLineColor = 602;
                hist.fXaxis.fFillStyle = 1001;
                break;
                //console.log(htext);
                //console.log('parsed ' + obj.DAQ.histPlot.histogramStep)
                // console.log('parsed ' + obj.DAQ.histPlot["channel 1"])
            }
        }
        timeoutData = setTimeout(doParse, 100);
    }


    var hist = jsroot.CreateHistogram("TH1I", nbinsX);
var strhis = '{\n' +
    '  "_typename" : "TH1F",\n' +
    '  "fUniqueID" : 0,\n' +
    '  "fBits" : 50331656,\n' +
    '  "fName" : "GEM_Station_0_module_0_layer_0",\n' +
    '  "fTitle" : "GEM_Station_0_module_0_layer_0",\n' +
    '  "fLineColor" : 602,\n' +
    '  "fLineStyle" : 1,\n' +
    '  "fLineWidth" : 1,\n' +
    '  "fFillColor" : 0,\n' +
    '  "fFillStyle" : 1001,\n' +
    '  "fMarkerColor" : 1,\n' +
    '  "fMarkerStyle" : 1,\n' +
    '  "fMarkerSize" : 1,\n' +
    '  "fNcells" : 827,\n' +
    '  "fXaxis" : {\n' +
    '    "_typename" : "TAxis",\n' +
    '    "fUniqueID" : 0,\n' +
    '    "fBits" : 50331648,\n' +
    '    "fName" : "xaxis",\n' +
    '    "fTitle" : "Strip Number",\n' +
    '    "fNdivisions" : 510,\n' +
    '    "fAxisColor" : 1,\n' +
    '    "fLabelColor" : 1,\n' +
    '    "fLabelFont" : 42,\n' +
    '    "fLabelOffset" : 0.005,\n' +
    '    "fLabelSize" : 0.08,\n' +
    '    "fTickLength" : 0.03,\n' +
    '    "fTitleOffset" : 1,\n' +
    '    "fTitleSize" : 0.06,\n' +
    '    "fTitleColor" : 810,\n' +
    '    "fTitleFont" : 42,\n' +
    '    "fNbins" : 825,\n' +
    '    "fXmin" : 0,\n' +
    '    "fXmax" : 825,\n' +
    '    "fXbins" : [],\n' +
    '    "fFirst" : 0,\n' +
    '    "fLast" : 0,\n' +
    '    "fBits2" : 0,\n' +
    '    "fTimeDisplay" : false,\n' +
    '    "fTimeFormat" : "",\n' +
    '    "fLabels" : null,\n' +
    '    "fModLabs" : null\n' +
    '  },\n' +
    '  "fYaxis" : {\n' +
    '    "_typename" : "TAxis",\n' +
    '    "fUniqueID" : 0,\n' +
    '    "fBits" : 50331648,\n' +
    '    "fName" : "yaxis",\n' +
    '    "fTitle" : "Activation Count",\n' +
    '    "fNdivisions" : 510,\n' +
    '    "fAxisColor" : 1,\n' +
    '    "fLabelColor" : 1,\n' +
    '    "fLabelFont" : 42,\n' +
    '    "fLabelOffset" : 0.005,\n' +
    '    "fLabelSize" : 0.08,\n' +
    '    "fTickLength" : 0.03,\n' +
    '    "fTitleOffset" : 0,\n' +
    '    "fTitleSize" : 0.06,\n' +
    '    "fTitleColor" : 810,\n' +
    '    "fTitleFont" : 42,\n' +
    '    "fNbins" : 1,\n' +
    '    "fXmin" : 0,\n' +
    '    "fXmax" : 1,\n' +
    '    "fXbins" : [],\n' +
    '    "fFirst" : 0,\n' +
    '    "fLast" : 0,\n' +
    '    "fBits2" : 0,\n' +
    '    "fTimeDisplay" : false,\n' +
    '    "fTimeFormat" : "",\n' +
    '    "fLabels" : null,\n' +
    '    "fModLabs" : null\n' +
    '  },\n' +
    '  "fZaxis" : {\n' +
    '    "_typename" : "TAxis",\n' +
    '    "fUniqueID" : 0,\n' +
    '    "fBits" : 50331648,\n' +
    '    "fName" : "zaxis",\n' +
    '    "fTitle" : "",\n' +
    '    "fNdivisions" : 510,\n' +
    '    "fAxisColor" : 1,\n' +
    '    "fLabelColor" : 1,\n' +
    '    "fLabelFont" : 42,\n' +
    '    "fLabelOffset" : 0.005,\n' +
    '    "fLabelSize" : 0.035,\n' +
    '    "fTickLength" : 0.03,\n' +
    '    "fTitleOffset" : 1,\n' +
    '    "fTitleSize" : 0.035,\n' +
    '    "fTitleColor" : 1,\n' +
    '    "fTitleFont" : 42,\n' +
    '    "fNbins" : 1,\n' +
    '    "fXmin" : 0,\n' +
    '    "fXmax" : 1,\n' +
    '    "fXbins" : [],\n' +
    '    "fFirst" : 0,\n' +
    '    "fLast" : 0,\n' +
    '    "fBits2" : 0,\n' +
    '    "fTimeDisplay" : false,\n' +
    '    "fTimeFormat" : "",\n' +
    '    "fLabels" : null,\n' +
    '    "fModLabs" : null\n' +
    '  },\n' +
    '  "fBarOffset" : 0,\n' +
    '  "fBarWidth" : 1000,\n' +
    '  "fEntries" : 0,\n' +
    '  "fTsumw" : 0,\n' +
    '  "fTsumw2" : 0,\n' +
    '  "fTsumwx" : 0,\n' +
    '  "fTsumwx2" : 0,\n' +
    '  "fMaximum" : -1111,\n' +
    '  "fMinimum" : -1111,\n' +
    '  "fNormFactor" : 0,\n' +
    '  "fContour" : [],\n' +
    '  "fSumw2" : [],\n' +
    '  "fOption" : "",\n' +
    '  "fFunctions" : {\n' +
    '    "_typename" : "TList",\n' +
    '    "name" : "TList",\n' +
    '    "arr" : [],\n' +
    '    "opt" : []\n' +
    '  },\n' +
    '  "fBufferSize" : 0,\n' +
    '  "fBuffer" : [],\n' +
    '  "fBinStatErrOpt" : 0,\n' +
    '  "fArray" : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]\n' +
    '}';
    //var hist = JSON.parse(strhis);
    conn();
    timeoutData = setInterval(getData, 1000);
    timeoutParse = setTimeout(doParse, 100);

    const server = http.createServer((req, res) => {
        //console.log('Request: ', req);
        console.log("req");
    res.statusCode = 200;
    //res.setHeader('Cache-Control', 'private, no-cache, no-store, must-revalidate, max-age=0, proxy-revalidate, s-maxage=0');
    res.writeHead(200, {"Content-Type": "application/json"});
       if (hist != null) {
           var str = JSON.stringify(hist, null, '\t');
           //res.setHeader('Content-Type', 'application/json');
           //res.setHeader('Content-Length', str.length);
           //res.writeHead(200, {'Content-Type': 'text/plain'});
           res.write(strhis);
           res.end();
       }
        else
            res.end();
    });
    server.listen(port, hostname, () => {
        console.log(`Server running at http://${hostname}:${port}/`);
    });