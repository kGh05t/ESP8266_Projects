#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library
ESP8266WiFiMulti wifiMulti;
// Replace with your network credentials
const char* ssid = "Tank_Switch";
const char* password = "ghost345";
int trigPin = 4;    // Trigger
int echoPin = 5;    // Echo
long duration, cm;
int buzzpin=0;
int relaypin=15;
bool monitoring= false;
unsigned long premillis, alarmmillis,sonarmillis=0;
long interval=10000;

bool levelcheck, alarmflag,onetimecheck,motorflag=false;



// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<head>
  <title>UltraSonic Water Monitor</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.1rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 480px;
    margin: auto;
    margin-bottom: auto;
    margin-left: auto;
  }
  .card {
    background-color: #4dfff7;
      /*! vertical-align:; */
  border-radius:60px;
    box-shadow: 0px 0px 50px 40px rgba(285,0,111,.5);
    padding-top:inherit;
  width:auto;
    padding-bottom:inherit;
    margin: auto;
  }
  .button {
  position:relative;
  
    padding: 40px 50px;
    font-size: 48px;
    text-align: right;
    outline: none;
    color: #fff;
    background-color: #0023ff;
    border: none;
    border-radius: 60px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.7rem;
     color:#8c8c8c;
     font-weight: bold;
   }
   
   .button {
    position: relative;
  right:0;
    display: inline-block;
    box-sizing: border-box;
    border: none;
    border-radius: 30px;
    padding: 0 16px;
    min-width: 64px;
    height: 36px;
    vertical-align: middle;
    text-align: center;
    text-overflow: ellipsis;
    text-transform: uppercase;
    color: rgb(var(--pure-material-onprimary-rgb, 255, 255, 255));
    background-color: rgb(var(--pure-material-primary-rgb, 33, 150, 243));
    box-shadow: 0 3px 1px -2px rgba(0, 0, 0, 0.2), 0 2px 2px 0 rgba(69, 22, 238, 0.14), 0 1px 5px 0 rgba(0, 0, 0, 0.12);
    font-family: var(--pure-material-font, "Roboto", "Segoe UI", BlinkMacSystemFont, system-ui,);
    font-size: 14px;
    font-weight: bold;
    line-height: 36px;
    overflow: hidden;
    outline: inset;
    cursor: pointer;
    transition: box-shadow 0.2s;
}

.button::-moz-focus-inner {
    border: none;
}

/* Overlay */
.button::before {
    content: "";
    position: absolute;
    top: 0;
    bottom: 0;
    left: 0;
    right: 0;
    background-color: rgb(var(--pure-material-onprimary-rgb, 255, 255, 255));
    opacity: 0;
    transition: opacity 0.2s;
}

/* Ripple */
.button::after {
    content: "";
    position: absolute;
    left: 50%;
    top: 50%;
    border-radius: 50%;
    padding: 50%;
    width: 32px; /* Safari */
    height: 32px; /* Safari */
    background-color: rgb(var(--pure-material-onprimary-rgb, 255, 255, 255));
    opacity: 0;
    transform: translate(-50%, -50%) scale(1);
    transition: opacity 1s, transform 0.5s;
}

/* Hover, Focus */
.button:hover,
.button:focus {
    box-shadow: 0 2px 4px -1px rgba(0, 0, 0, 0.2), 0 4px 5px 0 rgba(0, 0, 0, 0.14), 0 1px 10px 0 rgba(0, 0, 0, 0.12);
}

.button:hover::before {
    opacity: 0.08;
}

.button:focus::before {
    opacity: 0.24;
}

.button:hover:focus::before {
    opacity: 0.3;
}

/* Active */
.button:active {
    box-shadow: 0 5px 5px -3px rgba(0, 0, 0, 0.2), 0 8px 10px 1px rgba(0, 0, 0, 0.14), 0 3px 14px 2px rgba(0, 0, 0, 0.12);
}

.button:active::after {
    opacity: 0.32;
    transform: translate(-50%, -50%) scale(0);
    transition: transform 0s;
}

/* Disabled */
.button:disabled {
    color: rgba(var(--pure-material-onsurface-rgb, 0, 0, 0), 0.38);
    background-color: rgba(var(--pure-material-onsurface-rgb, 0, 0, 0), 0.12);
    box-shadow: none;
    cursor: initial;
}

.button:disabled::before {
    opacity: 0;
}

.button:disabled::after {
    opacity: 0;
}
   
   .pure-material-button-contained {
    position: relative;
  right:0;
    display: inline-block;
    box-sizing: border-box;
    border: lightgreen;
    border-radius: 30px;
    padding: 0 16px;
    min-width: 64px;
    height: 36px;
    vertical-align: middle;
    text-align: center;
    text-overflow: ellipsis;
    text-transform: uppercase;
    color: rgb(var(--pure-material-onprimary-rgb, 255, 255, 255));
    background-color: rgb(var(--pure-material-primary-rgb, 230, 12, 12));
    box-shadow: 0 3px 1px -2px rgba(0, 0, 0, 0.2), 0 2px 2px 0 rgba(0, 0, 0, 0.14), 0 1px 5px 0 rgba(0, 0, 0, 0.12);
    font-family: var(--pure-material-font, "Roboto", "Segoe UI", BlinkMacSystemFont, system-ui, -apple-system);
    font-weight: bold;
    line-height: 36px;
    overflow: hidden;
    outline: inset;
    cursor: pointer;
    transition: box-shadow 0.2s;
}

.pure-material-button-contained::-moz-focus-inner {
    border: none;
}

/* Overlay */
.pure-material-button-contained::before {
    content: "";
    position: absolute;
    top: 0;
    bottom: 0;
    left: 0;
    right: 0;
    background-color: rgb(var(--pure-material-onprimary-rgb, 255, 255, 255));
    opacity: 0;
    transition: opacity 0.2s;
}

/* Ripple */
.pure-material-button-contained::after {
    content: "";
    position: absolute;
    left: 50%;
    top: 50%;
    border-radius: 50%;
    padding: 50%;
    width: 32px; /* Safari */
    height: 32px; /* Safari */
    background-color: rgb(var(--pure-material-onprimary-rgb, 255, 255, 255));
    opacity: 0;
    transform: translate(-50%, -50%) scale(1);
    transition: opacity 1s, transform 0.5s;
}

/* Hover, Focus */
.pure-material-button-contained:hover,
.pure-material-button-contained:focus {
    box-shadow: 0 2px 4px -1px rgba(0, 0, 0, 0.2), 0 4px 5px 0 rgba(0, 0, 0, 0.14), 0 1px 10px 0 rgba(0, 0, 0, 0.12);
}

.pure-material-button-contained:hover::before {
    opacity: 0.08;
}

.pure-material-button-contained:focus::before {
    opacity: 0.24;
}

.pure-material-button-contained:hover:focus::before {
    opacity: 0.3;
}

/* Active */
.pure-material-button-contained:active {
    box-shadow: 0 5px 5px -3px rgba(0, 0, 0, 0.2), 0 8px 10px 1px rgba(0, 0, 0, 0.14), 0 3px 14px 2px rgba(0, 0, 0, 0.12);
}

.pure-material-button-contained:active::after {
    opacity: 0.32;
    transform: translate(-50%, -50%) scale(0);
    transition: transform 0s;
}

/* Disabled */
.pure-material-button-contained:disabled {
    color: rgba(var(--pure-material-onsurface-rgb, 0, 0, 0), 0.38);
    background-color: rgba(var(--pure-material-onsurface-rgb, 0, 0, 0), 0.12);
    box-shadow: none;
    cursor: initial;
}

.pure-material-button-contained:disabled::before {
    opacity: 0;
}

.pure-material-button-contained:disabled::after {
    opacity: 0;
}



.motorbutton {
    position: relative;
  right:0;
    display: inline-block;
    box-sizing: border-box;
    border: none;
    border-radius: 60px;
    padding: 0 16px;
    min-width: 64px;
    height: 36px;
    vertical-align: middle;
  
    text-align: center;
    text-overflow: ellipsis;
    text-transform: uppercase;
    color: rgb(var(--pure-material-onprimary-rgb, 255, 255, 255));
    background-color: rgb(var(--pure-material-primary-rgb, 230, 12, 12));
    box-shadow: 0 3px 1px -2px rgba(0, 0, 0, 0.2), 0 2px 2px 0 rgba(0, 0, 0, 0.14), 0 1px 5px 0 rgba(0, 0, 0, 0.12);
    font-family: var(--pure-material-font, "Roboto", "Segoe UI", BlinkMacSystemFont, system-ui, -apple-system);
    font-size: 13px;
    font-weight: bold;
    line-height: 36px;
    overflow: hidden;
    outline: inset;
    cursor: pointer;
    transition: box-shadow 0.2s;
}

.motorbutton::-moz-focus-inner {
    border: none;
}

/* Overlay */
.motorbutton::before {
    content: "";
    position: absolute;
    top: 0;
    bottom: 0;
    left: 0;
    right: 0;
    background-color: rgb(var(--pure-material-onprimary-rgb, 255, 255, 255));
    opacity: 0;
    transition: opacity 0.2s;
}

/* Ripple */
.motorbutton::after {
    content: "";
    position: absolute;
    left: 50%;
    top: 50%;
    border-radius: 50%;
    padding: 50%;
    width: 32px; /* Safari */
    height: 32px; /* Safari */
    background-color: rgb(var(--pure-material-onprimary-rgb, 255, 255, 255));
    opacity: 0;
    transform: translate(-50%, -50%) scale(1);
    transition: opacity 1s, transform 0.5s;
}

/* Hover, Focus */
.motorbutton:hover,
.motorbutton:focus {
    box-shadow: 0 2px 4px -1px rgba(0, 0, 0, 0.2), 0 4px 5px 0 rgba(0, 0, 0, 0.14), 0 1px 10px 0 rgba(0, 0, 0, 0.12);
}

.motorbutton:hover::before {
    opacity: 0.08;
}

.motorbutton:focus::before {
    opacity: 0.24;
}

.motorbutton:hover:focus::before {
    opacity: 0.3;
}

/* Active */
.motorbutton:active {
    box-shadow: 0 5px 5px -3px rgba(0, 0, 0, 0.2), 0 8px 10px 1px rgba(0, 0, 0, 0.14), 0 3px 14px 2px rgba(0, 0, 0, 0.12);
}

.motorbutton:active::after {
    opacity: 0.32;
    transform: translate(-50%, -50%) scale(0);
    transition: transform 0s;
}

/* Disabled */
.motorbutton:disabled {
    color: rgba(var(--pure-material-onsurface-rgb, 0, 0, 0), 0.38);
    background-color: rgba(var(--pure-material-onsurface-rgb, 0, 0, 0), 0.12);
    box-shadow: none;
    cursor: initial;
}

.motorbutton:disabled::before {
    opacity: 0;
}

.motorbutton:disabled::after {
    opacity: 0;
}
  </style>
<title>UltraSonic Water Tank Monitor</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>

  </div>
  <div class="content">
    <div class="card">
  <div style="font-size:200%;color:#ee2f23;font-weight:bold; position:center;"  class="title">UltraSonic Water Tank Monitor</div>
  <div class="card-body">
  <p></p>
    <span style="background-color:cyan;font-size:40px;">  Percent Filled: </span><span style="font-size:50px;color:#a24451"id="state">Press Monitor</span></p>
    <p><span style="background-color:cyan;font-size:10px;">  Current Water distance(empty at= 102cm): </span><span style="font-size:20px;color:#a24451" id="distance">Press Monitor</span></p>
    
  <span style="background-color:cyan;font-size:40px;">   Motor is : </span> <span style="font-size:50px;color:#ee2f23"  id="motor">%MOTORSTATUS%</span>
    <p><button id="button" class="button">Monitor</button></p>
 <p><button id="motorbutton" class="motorbutton">Turn On Motor</button></p>
 
  <p><button id="alarmbutton" class="pure-material-button-contained">Reset Alarm+Turn Off Motor</button></p>
</div>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  var array=[];
  var j;
  var count;
  var length;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  websocket.send('status');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
     var cm;
     var parsed_data=parseInt(event.data);
    if(typeof(parsed_data)=="number" && (event.data)!="0" && (event.data)!="alarm" && (event.data)!="monitor" && (event.data)!="motor" && (event.data)!="state"){      
          
          count=array.push(parsed_data); 
                            //length of aray after pushing  
      if(event.data=="9999"){
                    array.sort(function(a, b) {
                    return a - b;
                  });
                 console.log(array);
                for(var i=1;i<array.length;i++){
                    if(array[i]>array[0]+7){
                    j=i;
                    array=array.slice(0,j);
                    console.log("sliced array:="+array);
                    length=array.length;
                    }
                    }
              averaged_cm=(array.reduce((a, b) => a + b, 0))/length;
              finalcm=averaged_cm-20;
         
                state= (1-(finalcm/82))*100;    
                document.getElementById('state').innerHTML = (Math.round(state * 100) / 100).toFixed(2);
                document.getElementById('distance').innerHTML = (Math.round(averaged_cm * 100) / 100).toFixed(2);
                console.log("percentage calculated"+finalcm);
                array=[];
              }

              
}
   else if((event.data)=="0"){
      document.getElementById('state').innerHTML = "Sensor is OFF.!(getting 0 cm)";
  }
   else if(event.data=="monitor"){
   alert("Monitor Started! Please Wait for 10 Seconds!");
  }
   else if(event.data=="alarm"){
  alert("Alarm and Motor are turned off!");
  }
  else if(event.data=="motor"){
  alert("Motor Started!");
  }
  else if(event.data=="state"){
    
    if (document.getElementById('motor').innerHTML == "OFF"){
      document.getElementById('motor').innerHTML = "ON";
    }
    else if (document.getElementById('motor').innerHTML == "ON"){
      document.getElementById('motor').innerHTML ="OFF";
    }
         
  }
  }
  


  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
  document.getElementById('alarmbutton').addEventListener('click', alarmtoggle);
  document.getElementById('motorbutton').addEventListener('click', motortoggle);
  }
  function toggle(){
    websocket.send('monitor');
  }
  function alarmtoggle(){
    websocket.send('alarm');
  }
  function motortoggle(){
    websocket.send('motor');
  }

</script>
</body>
</html>
)rawliteral";

void notifyClients() {
  ws.textAll(String(cm));
}
String processor(const String& var){
 // Serial.println(var);
  if(var == "MOTORSTATUS"){
    if (!relaypin){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String signal=String((char*)data);
    if(signal=="monitor") {      
      Serial.println("monitor received");
      monitoring=true; 
      sonarmillis=millis();        
      ws.textAll("monitor");      
      Serial.println(monitoring);      
    }
    else if(signal=="alarm"){
      alarmflag=false;
      motorflag=false;
      ws.textAll("alarm");
      Serial.println("alarm received and alarmflag and motorflag false");
    }
    else if(signal=="motor"){
      motorflag=true;
      ws.textAll("motor");
      Serial.println("motor receieved and motorflag set to true");
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        Serial.println("data received from client");
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


void setup(){
  Serial.begin(115200);// Serial port for debugging purposes
  wifiMulti.addAP("Home Network", "ghost345");
  wifiMulti.addAP("God Plans", "ghost345");   // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("TOM", "123456789");
  

  Serial.println("Connecting ...");
  if(wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    //delay(500);
    Serial.println("Given Hotspot(s) is/are not available");
  }
  else{
    delay(500);
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address_Station:\t");
  Serial.println(WiFi.localIP());
  }
  WiFi.softAP(ssid, password);             // Start the access point
  Serial.print("Access Point \"");
  Serial.print(ssid);
  Serial.println("\" started");
  Serial.print("IP address_SoftAp_host:\t");
  Serial.println(WiFi.softAPIP()); // Send the IP address of 
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  initWebSocket();
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
   request->send_P(200, "text/html", index_html, processor);
  });
  // Start server
  server.begin();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relaypin,OUTPUT);
  pinMode(buzzpin,OUTPUT);
  digitalWrite(trigPin, LOW);
  digitalWrite(relaypin, HIGH);
  digitalWrite(buzzpin,LOW);
}

int sonar() {
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  delay(80);
  cm = (duration/2) / 29.1;
  delay(200);
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  delay(150);
  return cm;
}
void loop() {
 ws.cleanupClients();  
 ArduinoOTA.handle(); 
 unsigned long currmillis=millis();
  if(monitoring){           
          Serial.println("sonar activated for 10 second monitoring!");
          if(currmillis-sonarmillis<interval){
             sonar();
             ws.textAll(String(cm));                          
            }
            else{
              ws.textAll("9999");
              monitoring=false;
             Serial.println("monitor data sending finished");
              Serial.println("monitoring set to false");
            }
            }
  if(motorflag){
    digitalWrite(relaypin,LOW);
    ws.textAll("status");
    Serial.println("motor started");
    sonar();
    if(cm<=23 && !onetimecheck){      
      alarmmillis=millis();
      onetimecheck=true;
      levelcheck=true;
      Serial.println("levelcheck reached");
      }
    if(levelcheck){
      currmillis=millis();
      if(currmillis-alarmmillis<(interval+1000)){
      digitalWrite(buzzpin,HIGH);
      Serial.println("alarm reached");      
      }
      else{
        digitalWrite(relaypin,HIGH);
        levelcheck=false;
        motorflag=false;
        ws.textAll("state");
        onetimecheck=false;
        digitalWrite(buzzpin,HIGH);
        Serial.println("motor turned off");
       
      }
   }
    }
  }
  
