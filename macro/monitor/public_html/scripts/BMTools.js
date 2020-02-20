/* 
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
function FillRefSelector(path, selID) {
    JSROOT.NewHttpRequest(path, 'text', function (obj) {
        var refList = JSON.parse(obj);
        var sel = document.getElementById(selID);
        sel.innerHTML = "";
        refList.arr.forEach(function (item) {
            var op = document.createElement("option");
            op.value = item.fString;
            op.innerHTML = item.fString;
            sel.appendChild(op);
        });
    }).send(null);

}

function FillRefTable(path, selID) {
    var sel = document.getElementById(selID);
    if ((sel.size == 0) || (sel.innerHTML == ""))
        JSROOT.NewHttpRequest(path, 'text', function (obj) {
            var refList = JSON.parse(obj);
            var refLen = refList.arr.length;
            var storedLen = sessionStorage.getItem('refLen');
            //alert('stored ' + storedLen + '  reflen '+ refLen);
            if (refLen == storedLen)
                return;
            sel.innerHTML = "";
            refList.arr.forEach(function (item) {
                var op = document.createElement("option");
                var s = 'Run ' + item.i_run_number + ', beam ' + item.str_beam_particle + ', energy ' + item.d_energy;
                s += ',  target ' + item.str_target_particle;
                if (item.d_field_voltage > -1)
                    s += ', Voltage ' + item.d_field_voltage;
                op.innerHTML = s;
                op.value = item.i_run_number;
                sel.appendChild(op);
            });
            var id = sel.value;
            var refInput = document.getElementById("refInput");
            //refInput.value = id;
            sessionStorage.setItem('runID', item.i_run_number);
            sessionStorage.setItem('refLen', refLen);
        }).send(null);
}

function makeSelection() {
    var rt = document.getElementById("refTable");
    var id = rt.value;
    var refInput = document.getElementById("refInput");
    if (rt.innerHTML == "")
        return;
    refInput.value = id;
}

function setRunInfo(path) {
    JSROOT.NewHttpRequest(path, 'text', function (obj) {
        var info = JSON.parse(obj);
        if (info.arr.length > 0){
            var runInfo = info.arr[0];
            //alert('i_run_number '+runInfo.i_run_number);
            sessionStorage.setItem("runID", runInfo.i_run_number);
        }
    }).send(null);
}

function createSelectorsTable(selID) {
    var div = document.getElementById(selID);
    var table = div.create("table");
    table.borderWidth = 1;

}

