#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "kfl";
const char* password = "iqooneo7se";
const int relayPin = 5;  // GPIO5

ESP8266WebServer server(80);//建立新的服务器监听对象，默认端口为80
bool lightState = false;

// 优化后的HTML模板（强制UTF-8编码）
const char* htmlTemplate = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>智能灯控制</title>
  <style>
    body {
      font-family: Arial, "Microsoft YaHei", sans-serif;
      text-align: center;
      margin: 0;
      padding: 20px;
      background-color: %BG_COLOR%;
    }
    .btn {
      font-family: inherit;
      padding: 12px 24px;
      font-size: 18px;
      border: none;
      border-radius: 5px;
      color: white;
      background-color: %BTN_COLOR%;
      cursor: pointer;
    }
  </style>
</head>
<body>
  <div style="margin-top:50px">
    <a href="/toggle"><button class="btn">%BTN_TEXT%</button></a>
    <p style="font-size:18px">当前状态：%STATE%</p>
  </div>
</body>
</html>
)=====";

void setup() {
  Serial.begin(115200);//初始化串口通信，通过此方法进行收发数据
  delay(1000); // 等待串口稳定
  
  pinMode(relayPin, OUTPUT);//设置引脚d1为output状态，此时可以在代码中设置电压高低，即电源开关
  digitalWrite(relayPin, LOW); // 初始关闭
  
  WiFi.begin(ssid, password);
  Serial.println("正在连接WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n连接成功!");
  Serial.print("IP地址: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);// 注册根路径处理函数，即将设计好的网页呈现
  server.on("/toggle", handleToggle); // 注册切换路径处理函数，即当点击按钮发生变化时的处理函数
  server.begin();
}

void loop() {
  server.handleClient();//捕捉用户动作进行处理的函数
}

void handleRoot() {
  String html = htmlTemplate;
  html.replace("%BG_COLOR%", lightState ? "#f5f5f5" : "#333333");//背景颜色的逻辑函数
  html.replace("%BTN_COLOR%", lightState ? "#ff4444" : "#44ff44");
  html.replace("%BTN_TEXT%", lightState ? "关闭灯" : "打开灯");
  html.replace("%STATE%", lightState ? "开" : "关");
  
  // 三重编码保障
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.sendHeader("Content-Type", "text/html; charset=utf-8");
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/html", html);
}

void handleToggle() {
  lightState = !lightState;//初始化网页为关灯
  digitalWrite(relayPin, lightState ? HIGH : LOW); 
  
  Serial.print("状态变更: ");
  Serial.println(lightState ? "开灯" : "关灯");
  
  server.sendHeader("Location", "/", true);
  server.send(303, "text/plain", "");
}