/**
* Web Server, you can sent message to W5500 by IPaddress,message will display on M5Stack
* Need to install Ethernet2 arduino library 
* Please don`t install it by arduino library manange
* If install, delete it
* Ethernet2 file in M5stack lib examples -> modules -> W5500 -> Ethernet2.zip 
* https://github.com/m5stack/M5Stack/blob/master/examples/Modules/LAN_W5500/Ethernet2.zip
* Note: In this example, all characters are escaped on the WEB side and displayed in plain text. 
* The commands sent by RS485 are processed in the this program
*/

#include <M5Stack.h>
#include <SPI.h>
#include <Ethernet2.h>
#define SCK 18
#define MISO 19
#define MOSI 23
#define CS 26
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 70);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

const char index_html1[] PROGMEM = R"rawliteral(
    <!DOCTYPE html><html lang="en"><head><meta charset="UTF-8"><meta name="viewport"content="width=device-width, initial-scale=1.0"><title>M5Stack W5500</title><style>body{margin:0;padding:0;font-family:"montserrat";background-image:linear-gradient(125deg,#2c3e50,#27ae60,#2980b9,#e74c3c,#8e44ad);background-size:400%;animation:bganimation 15s infinite;overflow:hidden;height:100vh;width:100vh}.box{position:absolute;top:50%;left:50%;transform:translate(-50%,-50%);display:flex;flex-direction:column;height:32%;justify-content:space-between}h2{white-space:nowrap;color:white}@keyframes bganimation{0%{background-position:0%50%}50%{background-position:100%50%}100%{background-position:0%50%}}input{border-radius:40px;border:0;outline:0;background:#eee;height:38px;padding:0 0 0 20px}.submit-btn{height:50px;min-width:160px;padding:0 20px;background-color:#333;box-shadow:0 10px 30px 0 rgba(51,51,51,.5);-moz-box-shadow:0 10px 30px 0 rgba(51,51,51,.5);-webkit-box-shadow:0 10px 30px 0 rgba(51,51,51,.5);outline:none!important;border:none;border-radius:25px;align-items:center;color:#fff;font-size:16px;line-height:1.2;transition:all.3s ease}.submit-btn:hover{background-color:#007aff;box-shadow:4px 4px 8px 0#007aff5c;-moz-box-shadow:4px 4px 8px 0#007aff5c;-webkit-box-shadow:4px 4px 8px 0#007aff5c}.success{background-color:#1cd3c2!important;box-shadow:0 10px 30px 0 rgba(59,214,230,0.5)!important;-moz-box-shadow:0 10px 30px 0 rgba(59,214,230,0.5)!important;-webkit-box-shadow:0 10px 30px 0 rgba(59,214,230,0.5)!important}</style></head>
)rawliteral";

const char index_html2[] PROGMEM = R"rawliteral(
    <body><div class="box"><h2>W5500 Web Serial Command</h2><input id="command"type="text"placeholder="input the command"><button id="submit"class="submit-btn"onclick="send_command()"><span id="btn_text">Send</span></button></div></body><script>function after_send(){document.getElementById("btn_text").textContent='Success';document.getElementById("submit").classList.add('success');setTimeout(()=>{document.getElementById("btn_text").textContent='Send';document.getElementById("submit").classList.remove('success')},2000)}function send_command(){if(window.XMLHttpRequest){httpRequest=new XMLHttpRequest()}else if(window.ActiveXObject){httpRequest=new ActiveXObject()}let command=document.getElementById("command").value;if(command.toString().trim().length>0){httpRequest.open("POST","/cmd",true);httpRequest.onreadystatechange=after_send;httpRequest.send('cmd='+command)}else{alert('Command is empty')}}</script></html></html>
)rawliteral";


void writeResponse(EthernetClient client) {
     client.print(index_html1);
     client.println(index_html2);
}

void header(const char *string, uint16_t color)
{
    M5.Lcd.fillScreen(color);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_MAGENTA, TFT_BLUE);
    M5.Lcd.fillRect(0, 0, 320, 30, TFT_BLUE);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.drawString(string, 160, 3, 4);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setCursor(80, 40, 4);
    M5.Lcd.print(Ethernet.localIP());
}

void cmd_run(String cmd) {
  int indexNum = cmd.indexOf("cmd=");
  //Serial.println(indexNum);
  if(indexNum != -1){
      M5.Lcd.fillRect(0,80, 320, 160, TFT_BLACK);
      M5.Lcd.setCursor(0, 80, 4);
      String cmd_str = cmd.substring(indexNum + 4);
      M5.Lcd.print(cmd_str);
      if(cmd_str.indexOf("\\r\\n") != -1){
         Serial2.print(cmd_str.substring(0, (cmd_str.indexOf("\\r\\n"))));
         Serial2.print("\r\n");
      }
  }
}

void setup() {
  // Open serial communications and wait for port to open:
  M5.begin(true, false, true);
  Serial2.begin(9600, SERIAL_8N1, 5, 15);

  SPI.begin(SCK, MISO, MOSI, -1);
  Ethernet.init(CS);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  header("W5500 PoE Example", TFT_BLACK);
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");

    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    String req_str = "";
    int data_length = -1;
    boolean skip = true;

    //int empty_line_count = 0;
    while (client.connected()) 
    {
      if (client.available()) {
        char c = client.read();
        //Serial.write(c);
        req_str += c;

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply       
        if (c == '\n' && currentLineIsBlank && req_str.startsWith("GET")) {
          writeResponse(client);
          break;
        }
        if (c == '\n' && currentLineIsBlank && req_str.startsWith("POST") && !skip) {
          writeResponse(client);
          break;
        }   
        if (c == '\n' && currentLineIsBlank && req_str.startsWith("POST") && skip) {
          skip = false;
          String temp = req_str.substring(req_str.indexOf("Content-Length:") + 15);
          temp.trim();
          //Serial.print("Content-Length=");
          data_length = temp.toInt();
          /*Serial.println(data_length);
          writeResponse(client);
          break;*/
          while(data_length-- > 0)
          {
            c = client.read();
            req_str += c;
          }
          writeResponse(client);
          break;
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    
    Serial.println(req_str);
    // give the web browser time to receive the data
    // close the connection:
    client.stop();
    cmd_run(req_str);
    Serial.println("client disconnected");
  }
  
  M5.update();
}
