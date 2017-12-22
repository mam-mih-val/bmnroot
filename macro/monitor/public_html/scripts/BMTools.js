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

