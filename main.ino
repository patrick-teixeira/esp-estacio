#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>


ESP8266WebServer server(80);

String nomeUsuario = "";

struct WiFiCredentials {
  char network[32];
  char password[64];
  char uid[64];
};

WiFiCredentials credentials;

void readCredentialsFromEEPROM() {
  EEPROM.begin(sizeof(credentials));
  EEPROM.get(0, credentials);
  EEPROM.end();
}

void saveCredentialsToEEPROM() {
  EEPROM.begin(sizeof(credentials));
  EEPROM.put(0, credentials);
  EEPROM.end();
}


void handleRoot() {
  String page = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><style>";
  page += "body { font-family: Arial, sans-serif; margin: 0; padding: 0;}";
  page += "h1 { text-align: center;}";
  page += "form { max-width: 300px; margin: 20px auto; padding: 20px; border: 1px solid #ccc; border-radius: 5px; text-align: center; }"; 
  page += "input[type='text'], input[type='password'] { width: 100%; padding: 10px; margin: 5px 0; box-sizing: border-box; }"; 
  page += "input[type='submit'] { width: 100%; background-color: #4CAF50; color: white; padding: 10px; border: none; border-radius: 5px; cursor: pointer;}";
  page += "</style></head><body>";
  page += "<h1>Configuração da Rede WiFi</h1>";
  page += "<form action='/save' method='post'>";
  page += "Nome da Rede WiFi: <input type='text' name='network' value='" + String(credentials.network) + "'><br>";
  page += "Senha da Rede WiFi: <input type='password' name='password' value='" + String(credentials.password) + "'><br>";
  page += "Id de usuario: <input type='text' name='uid' value='" + String(credentials.uid) + "'><br>";
  page += "<input type='submit' value='Salvar'>";
  page += "</form></body></html>";
  server.send(200, "text/html", page);
}

void handleSave() {
  if (server.hasArg("network") && server.hasArg("password")) {
    String network = server.arg("network");
    String password = server.arg("password");
    String uid = server.arg("uid");
    network.toCharArray(credentials.network, sizeof(credentials.network));
    password.toCharArray(credentials.password, sizeof(credentials.password));
    uid.toCharArray(credentials.uid, sizeof(credentials.uid));
    saveCredentialsToEEPROM();
    server.send(200, "text/plain", "Configurações salvas com sucesso!");
    readCredentialsFromEEPROM();
    Serial.println(credentials.network);
    Serial.println(credentials.password);
    WiFi.disconnect();
    connect_wifi();
  } else {
    server.send(400, "text/plain", "Parâmetros ausentes");
  }
}

void connect_wifi(){
  WiFi.begin(credentials.network, credentials.password);
  for(int i = 0; i < 5; i++){
    delay(1000);
    if (WiFi.status() == WL_CONNECTED){
      Serial.println("Wifi Connected");
      return;
    }
    else{
      Serial.println("Trying to connect");
    }
  }
  WiFi.softAP("esp8266", NULL);
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.softAPIP());
  
}

void setup() {
  readCredentialsFromEEPROM();
  Serial.begin(115200);
  Serial.println(" ");
  Serial.println(credentials.network);
  Serial.println(credentials.password);
  
  Serial.println("");

  connect_wifi();

  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);

  server.begin();
  Serial.println("Servidor iniciado");
  

}

void loop() {
  server.handleClient();
 
}
