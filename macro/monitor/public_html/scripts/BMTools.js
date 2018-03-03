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
    JSROOT.NewHttpRequest(path, 'text', function (obj) {
        var refList = JSON.parse(obj);
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
        refInput.value = id;
    }).send(null);
}

function makeSelection() {
    var rt = document.getElementById("refTable");
    var id = rt.value;
    var refInput = document.getElementById("refInput");
    refInput.value = id;
}

function () {
    
}

