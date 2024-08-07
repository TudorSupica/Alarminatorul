const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<div>
  <button class="button" onclick="send(1)">LED ON</button>
  <button class="button" onclick="send(0)">LED OFF</button><BR>
</div>
 <br>
<div>
  LED State: <span id="state">NA</span>
</div>
<script>
function send(led_sts) 
{
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("state").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "led_set?state="+led_sts, true);
  xhttp.send();
}

setInterval(function() 
{
  getData();
}, 2000); 

</script>
</center>
</body>
</html>
)=====";
