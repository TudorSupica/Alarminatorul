const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>

    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Sonerie_MSP</title>

  <style>
      
      .buton {
        background-color: #4CAF50;
        border-radius: 12px;
        color: white;
        width: 300px;
        padding: 30px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 30px;
        margin: 4px 2px;
        cursor: pointer;
    }


      divv {
        width: 300px;
        padding: 20px;
        border: 4px solid gray;
        margin: 0;
      }
    </style>
  
  </head>


  <body style="background-color:powderblue;">

  <center>
  <font face="Comic sans MS" size="5">
  <h2>Panou control MSP-Sonerie</h2>

    <div>
      <button class="button buton" onclick="send(1)"> LED ON</button>
      <button class="button buton" onclick="send(0)">LED OFF</button>
      <br>
    </div>

    <br>

    <divv>
      LED State:
      <span id="state">NA</span>
    </divv>

    



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
