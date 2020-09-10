
function FillRefTableByList(list, selID) {
    var sel = document.getElementById(selID);
    sel.innerHTML = "";
    var op0 = document.createElement("option");
    op0.innerHTML = " - ";
    op0.value = 0;
    sel.appendChild(op0);
    list.arr.forEach(function (item) {
        var op = document.createElement("option");
        var s = 'Run ' + item.i_run_number + ', beam ' + item.str_beam_particle + ', energy ' + item.d_energy;
        s += ',  target ' + item.str_target_particle;
        if (item.d_field_voltage > -1)
            s += ', Voltage ' + Math.round(item.d_field_voltage);
        op.innerHTML = s;
        op.value = item.i_run_number;
        sel.appendChild(op);
        sel.value = 0;
    });
}

function FillNameTableByList(list, selID) {
    var sel = document.getElementById(selID);
    sel.innerHTML = "";
    var op0 = document.createElement("option");
    op0.innerHTML = " - ";
    op0.value = 0;
    sel.appendChild(op0);
    var iv = 0;
    list.arr.forEach(function (item) {
        var op = document.createElement("option");
        var s = item.name;
        op.innerHTML = s;
        op.value = iv++;
        sel.appendChild(op);
        sel.value = 0;
    });
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
        if (info.arr.length > 0) {
            var runInfo = info.arr[0];
            //alert('i_run_number '+runInfo.i_run_number);
            //sessionStorage.setItem("runID", runInfo.i_run_number);
        }
    }).send(null);
}


