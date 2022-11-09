const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>
Ora : <span id="ADCValue">0</span><br>
<div id="demo">
  <button type="button" onclick="sendData(1)">LED ON</button>
  <button type="button" onclick="sendData(0)">LED OFF</button><BR>
</div>
<br>
<div id="demo">
  <button type="button" onclick="sendData_a(1)">ALARMA ON</button>
  <button type="button" onclick="sendData_a(0)">ALARMA OFF</button><BR>
</div>

<div>
  Sonerie : <span id="LEDState">NA</span>
  Alarma : <span id="alarma_state">NA</span>
</div>
<script>


function sendData(led) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("LEDState").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "setLED?LEDstate="+led, true);
  xhttp.send();
}

function sendData_a(alarma) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("alarma_state").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "alarma?alarma_state="+alarma, true);
  xhttp.send();
}


setInterval(function() {
  // Call a function repetatively with 2 Second interval
  getData();
}, 2000); //2000mSeconds update rate

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("ADCValue").innerHTML =
      this.responseText;
    }
  };
  xhttp.open("GET", "readADC", true);
  xhttp.send();
}


</script>

</body>
</html>
)=====";
