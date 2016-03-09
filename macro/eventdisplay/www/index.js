function prtHello(){
alert("HelloWrold");
}

function setReloading(arg){
	setInterval(function() {
		var BufferImage = document.getElementById('BufferImage');
		BufferImage.src=arg+'?rand=' + Math.random();
		var MainImage = document.getElementById('mainImage');
		MainImage.width=BufferImage.clientWidth;
		MainImage.height=BufferImage.clientHeight;
		ctx     = MainImage.getContext('2d');
		ctx.drawImage(BufferImage,0,0,MainImage.width,MainImage.height);
	}, 1000);
}

function loadDoc() {
	setInterval(function() {
		var xmlhttp, text;
		xmlhttp = new XMLHttpRequest();
		xmlhttp.open('GET', 'eventIndex.js', false);
		xmlhttp.send();
		text = xmlhttp.responseText;
		var div = document.getElementById('frmFile');
		div.innerHTML =text;
	}, 1000);
}

function LoadFile(){
	setInterval(loadDoc(), 1000);
}