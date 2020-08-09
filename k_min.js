var data=null;
window.addEventListener("load",function(){var a=document.querySelector("#auto")var b=document.querySelectorAll(".l[name=b]");[].forEach.call(b,function(e){e.addEventListener("change",function(){data={d:readButtons().toString(16),a:a.checked?1:0};if (!a.checked)deviceSync();});});a.addEventListener("change",function(){data={d:readButtons().toString(16),a:a.checked?1:0};deviceSync();});deviceSync();});

function deviceSync(){var x=new XMLHttpRequest();x.addEventListener("load",onDeviceData);x.addEventListener("error",onDeviceData);x.open("POST","k.json");x.responseType="json";x.send(data);data=null;}

function onDeviceData(e){if(e.target.response){writeButtons(parseInt(e.target.response.d,16));}if(document.querySelector("#auto").checked)window.requestAnimationFrame(deviceSync);}

function readButtons(){var a=document.querySelectorAll(".l[name=b]"),v=0;[].forEach.call(a,function(e){if(e.checked)v|=(1<<e.value);});return v;}
function writeButtons(v){var a=document.querySelectorAll(".l[name=b]");[].forEach.call(a,function(e){e.checked=v&(1<<e.value);});}

