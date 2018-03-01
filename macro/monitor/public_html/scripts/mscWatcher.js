
const http = require('http');
const url = require('url');
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
//var d3 = require('d3');
//var jsdom = require('jsdom');
var date = new Date();
var htext = null;
function makeHist(obj) {
    var time = new Date().getTime();
    var binCount = 8192;
    var width = 1280;
    var height = 720;
    var max = binCount;
    var min = 1;
    var wScale = d3.scaleLinear().domain([min, max]).range([0, width]);
    var document = jsdom.jsdom();
    time = new Date().getTime() - time;
    console.log('time spent = ', time);
    var docText = jsdom.serializeDocument(document);
    return docText;
}

var client;
var timeoutData;
var timeoutParse;
var strQue = [];

function conn() {
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
    var time = new Date().getTime();
    while (strQue.length > 0) {
        var chunk = strQue.shift();
        strBuf += chunk;
        var str = chunk.toString();
        console.log('   last ' + str[str.length - 1])
        //var regex = '([^\\n]+)\\n(.*)';
        if (str[str.length - 1] == '\n') {
            //console.log(strBuf);
            var obj = JSON.parse(strBuf);
            strBuf = '';
            str = null;
            t = 0;
            //htext = makeHist(obj);
            if (hists.length >= 4)
            for (var i = 0; i < 4; i++) {
                hists[i].fArray = obj.DAQ.histPlot['channel ' + (i + 1).toString()];
                //console.log(hist.fArray);
            }
            console.log(obj);
            //console.log('parsed ' + obj.DAQ.histPlot.histogramStep)
            //console.log('parsed ' + obj.DAQ.histPlot["channel 1"])
            break;
        }
    }
    time = new Date().getTime() - time;
    //console.log('time spent = ', time);
    timeoutData = setTimeout(doParse, 100);
}

var hists = [];
for (var i = 0; i < 4; i++){
    var hist = jsroot.CreateHistogram('TH1I', nbinsX);
    hist.fName = 'Channel '+ (i+1);
    hist.fTitle = hist.fName;
    hist.fXaxis.fTitle = 'Time';
    hist.fXaxis.fTitleColor = 810;
    hist.fXaxis.fLabelSize = 0.04;
    hist.fXaxis.fTitleSize = 0.06;
    hist.fYaxis.fTitleOffset = 0.1;
    hist.fXaxis.fLineColor = 602;
    hist.fXaxis.fFillStyle = 1001;

    hist.fYaxis.fTitle = 'Amplitude';
    hist.fYaxis.fLabelOffset = 0.005;
    hist.fYaxis.fLabelSize = 0.04;
    hist.fYaxis.fTickLength = 0.03;
    hist.fYaxis.fTitleOffset = 0.3;
    hist.fYaxis.fTitleSize = 0.06;
    hist.fYaxis.fTitleColor = 810;
    //hist.fYaxis.fTitleFont" : 42,
    hists.push(hist);
}
    var canv = jsroot.Create('TCanvas');
    //var hist = JSON.parse(strhis);
    conn();
    timeoutData = setInterval(getData, 1000);
    timeoutParse = setTimeout(doParse, 100);

    const server = http.createServer((req, res) => {
        //console.log('Request: ', req);
        var query = url.parse(req.url, true).query;
        console.log("req");
        console.log(query);
        var hid = query['histid'];
        if (hid === undefined)
            hid = 0;
        res.statusCode = 200;
        res.writeHead(200, {"Content-Type": "application/json"});
        if ((hists.length >= hid) && (hists[hid] != null)) {
            var str = JSON.stringify(hists[hid], null, '\t');
            //res.setHeader('Content-Type', 'application/json');
            // res.setHeader('Content-Length', str.length);
            // res.writeHead(200, {'Content-Type': 'text/plain'});
            res.write(str);
            res.end();
        }
        else
            res.end();
    });
    server.listen(port, hostname, () => {
        console.log(`Server running at http://${hostname}:${port}/`);
    });