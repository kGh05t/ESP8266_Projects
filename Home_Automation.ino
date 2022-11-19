#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>

// Replace with your network credentials
const char* ssid = "God Plans";
const char* password = "ghost345";
const char* ap_ssid = "Home_Switch";
const char* ap_password = "ghost345";

const int output = 2;
const int light_output = 5;

//const long utcOffsetInSeconds = 19800;

String sliderValue = "140";
String time1,h,m="";
//String off_time1="";
//String off_time="";
bool check= false;
//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP, "pool.ntp.org",utcOffsetInSeconds);
//unsigned long epochTime; 
unsigned long premillis=0;
unsigned long interval=0;
String alarm="";
String light="";
int l_switch=1;
int speed=0;


const char* PARAM_INPUT_1 = "value";
const char* PARAM_INPUT_2 = "clienttime";
const char* PARAM_INPUT_3 = "alarm";
const char* PARAM_INPUT_4 = "light";
const char* PARAM_INPUT_5 = "speed";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>SwitcH</title>
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 2.3rem;}
    p {font-size: 1.9rem;}
    body {max-width: 400px; margin:0px auto; padding-bottom: 25px;}
    .slider { -webkit-appearance: none; margin: 14px; width: 360px; height: 25px; background: #FBBB1D;
      outline: none; -webkit-transition: .2s; transition: opacity .2s;}
    .slider::-webkit-slider-thumb {-webkit-appearance: none; appearance: none; width: 35px; height: 35px; background: #113ffe; cursor: pointer;}
    .slider::-moz-range-thumb { width: 15px; height: 35px; background: #993249; cursor: pointer; } 
  input:checked + .slider {
  background-color: #1AE003;}
  
  .button{
  background-color: #11BB50; /* Green */
  border: none;
  color: white;
  padding: 15px 35px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  margin: 4px 2px;
  cursor: pointer;
 
}
 .button {background-color: #ff0000;} 
 .alarmbutton {background-color: #ee0990;}

  
  </style>
</head>
<body >
  <h2></h2>
  <p><span id="textSliderValue">%SLIDERVALUE%</span></p>
  <p><input type="range" onchange="updateSliderPWM(this)" id="pwmSlider" min="0" max="1023" value="%SLIDERVALUE%" class="slider"></p>
  Time: <input type="time" id="alarmtime" value="">
    <button class="button alarmbutton" id="alarmbutton" onclick="setalarm()">Set Alarm</button>  
  <br><br>
  <label for="speed">Speed on Alarm:</label>
    <input type="number" id="speed" value ="0" min="0" max="1023" name="speed"><br><br>
    <input onclick="change()" class="button" type="button" value="Turn On Alarm" id="myButton1"></input>
    <input onclick="light()" class="button button1" type="button" value="Turn On Light" id="myButton2"></input>
    
    

<p id="button"></p>
<script>
function light() {
  var l = document.getElementById("myButton2");
   var elem = document.getElementById("myButton1");
  var sliderValue = document.getElementById("pwmSlider").value;
  var time1 = document.getElementById("alarmtime").value;
    if (l.value=="Turn On Light") {
  l.value = "Turn Off Light";
    var light=1;
    }
    else {
  l.value = "Turn On Light"; 
  var light=0;}
  if (elem.value=="Turn On Alarm") {
      var alarm="0";
    }
    else {
  var alarm="1";
  }

  console.log(sliderValue,time1,light);
  var xhr = new XMLHttpRequest();
  var s= document.getElementById("speed").value;
  xhr.open("GET", "/slider?value="+sliderValue+"&clienttime="+time1+"&alarm="+alarm+"&light="+light+"&speed="+s, true);
  xhr.send();


}
function setalarm() {
  var l = document.getElementById("myButton2");
  
  if (l.value=="Turn Off Light") {
      var light=1;
    }
    else {

  var light=0;}
  var time1 = document.getElementById("alarmtime").value;
  var sliderValue = document.getElementById("pwmSlider").value;
  document.getElementById("button").innerHTML = time1;

  alert("Alarm Time was Saved!");
  var elem = document.getElementById("myButton1");
  if (elem.value=="Turn On Alarm") {
      var alarm="0";
    }
    else {
  var alarm="1";
  }
  var s= document.getElementById("speed").value;
  console.log(sliderValue,time1,s);
  var xhr = new XMLHttpRequest();
  
  xhr.open("GET", "/slider?value="+sliderValue+"&clienttime="+time1+"&alarm="+alarm+"&light="+light+"&speed="+s, true);
  xhr.send();
}
function change(){
  var l = document.getElementById("myButton2");
  if (l.value=="Turn Off Light") {
      var light=1;
    }
    else {

  var light=0;}
  var elem = document.getElementById("myButton1");
  var sliderValue = document.getElementById("pwmSlider").value;
  var time1 = document.getElementById("alarmtime").value;
    if (elem.value=="Turn On Alarm") {
  elem.value = "Turn Off Alarm";
    var alarm="1";
    }
    else {
  elem.value = "Turn On Alarm"; var alarm="0";}
  console.log(sliderValue,time1,light,);
  var xhr = new XMLHttpRequest();
  var s= document.getElementById("speed").value;
  xhr.open("GET", "/slider?value="+sliderValue+"&clienttime="+time1+"&alarm="+alarm+"&light="+light+"&speed="+s, true);
  xhr.send();
}
function updateSliderPWM(element) {
  var l = document.getElementById("myButton2");
  if (l.value=="Turn Off Light") {
      var light=1;
    }
    else {
  var light=0;
  }
  var sliderValue = document.getElementById("pwmSlider").value;
  var time1 = document.getElementById("alarmtime").value;  
  var elem = document.getElementById("myButton1");
  if (elem.value=="Turn On Alarm") {
  //elem.value = "Turn Off Alarm";
    var alarm="0";
    }
    else {
  //elem.value = "Turn On Alarm";
  var alarm="1";}
  document.getElementById("textSliderValue").innerHTML = sliderValue;
  console.log(sliderValue,time1,light);
  var xhr = new XMLHttpRequest();
  var s= document.getElementById("speed").value;
  xhr.open("GET", "/slider?value="+sliderValue+"&clienttime="+time1+"&alarm="+alarm+"&light="+light+"&speed="+s, true);
  xhr.send();
  }
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if (var == "SLIDERVALUE"){
    return sliderValue;
  }
  
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.softAP(ap_ssid, ap_password, 11);
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 // while (WiFi.waitForConnectResult() != WL_CONNECTED) {
   // Serial.println("Connection Failed! Rebooting...");
  //  delay(5000);
   // ESP.restart();
 //}

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
   //Serial.println("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
   // printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  analogWrite(output, sliderValue.toInt());
  pinMode(light_output,OUTPUT);
  


  
  
  // Route for root / web page
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/slider?value=<inputMessage>
  server.on("/slider", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputMessage2;
    //String inputParam;
    //String inputParam2;
    String inputMessage3;
    String inputMessage4;
    String inputMessage5;


    // GET input1 value on <ESP_IP>/slider?value=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1) & request->hasParam(PARAM_INPUT_2) & request->hasParam(PARAM_INPUT_3) & request->hasParam(PARAM_INPUT_4) & request->hasParam(PARAM_INPUT_5)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      sliderValue = inputMessage;
      analogWrite(output, sliderValue.toInt());
      inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
      time1 = inputMessage2;
      inputMessage3= request->getParam(PARAM_INPUT_3)->value();
      alarm=inputMessage3;
      inputMessage4= request->getParam(PARAM_INPUT_4)->value();
      l_switch=inputMessage4.toInt();
      inputMessage5= request->getParam(PARAM_INPUT_5)->value();
      speed=inputMessage5.toInt();

      Serial.println(l_switch);
      Serial.println(speed);
      if (time1!="" && alarm=="1"){
        bool check=!check;
        premillis=millis();
        Serial.println("flag set");
        Serial.println(time1);
        h=time1.substring(0,2);
        m=time1.substring(3,5);
        int long hour=(h.toInt())*3600000;
        int long minute=(m.toInt())*60000;
        Serial.println(hour);
        Serial.println(minute);
        interval=hour+minute;
        Serial.println(interval);
      }
      
      }

    

    else {
      inputMessage = "No message sent";
    }
    Serial.println(inputMessage);
    Serial.println(inputMessage2);
  //  Serial.println(inputMessage3);
    request->send(200, "text/plain", "OK");
  });
  //timeClient.begin();
  
  // Start server
  server.begin();
}
  
void loop() {
  ArduinoOTA.handle();
  if(l_switch==1){
      digitalWrite(light_output,LOW);
  }
  else{
    digitalWrite(light_output,HIGH);
  }
  unsigned long currentMillis = millis();
  
  if (time1!="" && alarm=="1"){
    unsigned long currentMillis = millis();
          
      if ((currentMillis-premillis)>=interval){        
      analogWrite(output,speed);
      }
  }
}
