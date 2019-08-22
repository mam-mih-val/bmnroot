
function FillRefTable(path, selID) {
    var sel = document.getElementById(selID);
    //if ((sel.size == 0) || (sel.innerHTML == "")) 
        JSROOT.NewHttpRequest(path, 'text', function (obj) {
            var refList = JSON.parse(obj);
            sel.innerHTML = "";
            var op0 = document.createElement("option");
            op0.innerHTML = " - ";
            op0.value = 0;
            sel.appendChild(op0);
            refList.arr.forEach(function (item) {
                var op = document.createElement("option");
                var s = 'Run ' + item.i_run_number + ', beam ' + item.str_beam_particle + ', energy ' + item.d_energy;
                s += ',  target ' + item.str_target_particle;
                if (item.d_field_voltage > -1)
                    s += ', Voltage ' + Math.round(item.d_field_voltage);
                op.innerHTML = s;
                op.value = item.i_run_number;
                sel.appendChild(op);
            });
            var id = sel.value;
            var refInput = document.getElementById("refInput");
            refInput.value = id;
            sessionStorage.setItem('runID', item.i_run_number);
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
        if (info.arr.length > 0) {
            var runInfo = info.arr[0];
            //alert('i_run_number '+runInfo.i_run_number);
            sessionStorage.setItem("runID", runInfo.i_run_number);
        }
    }).send(null);
}


