#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#ifndef DEBUG
#define DEBUG 1
#endif

//    Function in this file
//    void activateAPmode();
//    void setup();
//    void connectWifi() ;
//    void activateAPmode();
//    void loop(); 
//    bool isSSIDPresent();

/* Set these to your desired softAP credentials. They are not configurable at runtime */
const char *softAP_ssid = "SmartHome";
const char *softAP_password = "*******";

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "smarthome";

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[32] = "";
char password[32] = "";

// Web server
ESP8266WebServer server(80);

/* Soft AP network parameters */
IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

/** Last time I tried to connect to WLAN */
long lastConnectTry = 0;
bool credentialsPresent = false;


// HOME AUTOMATION VARS
int Fan=D0;
int GateLock=D5;
int Light=D6;
int Led=D7;

//--------------------------------------------------------------------------------------------------
void setup() 
{
  delay(200);
  #if defined(ESP8266)
    Serial.println("Working for ESP8266");
    // do some other specific stuff if you want to
  #else
    Serial.println("some other arch");
  #endif

  // initiallise peripherals
  Serial.begin(115200);
  EEPROM.begin(512);
  Serial.setDebugOutput(true);
  activateAPmode();
  init_gpio();
  setupEndPoints();
  if (WiFiCredentialsPresent())
  {
    Serial.println("Wifi Credentials are present");
    loadCredentials(); // Load WLAN credentials from network
    credentialsPresent = true;
  }
  else
  {
    Serial.println("NO Wifi Credentials are present");
    credentialsPresent = false;
    WiFi.disconnect(true);
  }
}


void loop() 
{
  // putting it first so that if the code may return from anywhere below this is executed always.
   server.handleClient();
 
  // Wifi retries always interferes with softAP operation.!!!!
  int state = WiFi.status();
  if(credentialsPresent && strlen(ssid) > 0 && state != WL_CONNECTED)
  {
    if( isSSIDPresent() )
    {
      if(DEBUG)
        Serial.println("SSID present");
      int res = connectWifi();
      if(res == WL_CONNECT_FAILED)
      {
        Serial.println("possibly Wrong Password! Trying Again") ; 
        delay(10000);
        return;
      }
      if (res == WL_CONNECTED) 
      {
        if(DEBUG)
        {
          Serial.println ( "" );
          Serial.print ( "Connected to " );
          Serial.println ( ssid );
          Serial.print ( "IP address: " );
          Serial.println ( WiFi.localIP() );
        }        
        setupMDNS();
      }// if not connected to wifi already
    }// check if ssid is present
    else
    {
      if(DEBUG)
        Serial.println("No Such SSID present YET!");
    }
  } // if credentialsPresent
} // LOOP ends

//---------------------------------------------------------------------------------------------------
int connectWifi() 
{
  WiFi.begin ( ssid, password );
  int s = WiFi.waitForConnectResult();
  return s;
}

void activateAPmode()
{
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid, softAP_password);

  delay(1000); // Without delay I've seen the IP address blank
  if(DEBUG)
  {
    Serial.println();
    Serial.print("Configuring access point...");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  }
}

void init_gpio()
{
  pinMode(Fan,OUTPUT);
  pinMode(GateLock,OUTPUT);
  pinMode(Light,OUTPUT);
  pinMode(Led,OUTPUT);
//  pinMode(CFL,OUTPUT);
  
  digitalWrite(Fan,HIGH);
  digitalWrite(GateLock,HIGH);
  digitalWrite(Light,HIGH);
  digitalWrite(Led,HIGH);
//  digitalWrite(CFL,HIGH);
}


bool isSSIDPresent()
{
  bool found = false;
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
  {
    Serial.println("no networks found; Can not connect to anything!!");
    return found;
  }
  else
  {
    if(DEBUG)
    {
      Serial.print(n);
      Serial.println(" networks found");
    }
    for (int i = 0; i < n; ++i)
    {
      if(DEBUG)
      {
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
        delay(10);
      }
      if(String(WiFi.SSID(i)) == String(ssid))
      {
        Serial.println("Desired SSID found!!  Connecting to it with given password");
        found = true;
        break;
      } // if Wifi ssid matches 
    } // for loop ends
  }// else ends here 
  if(DEBUG)
    Serial.println("exiting SSID present");
  return found;
}

void setupEndPoints()
{
  if (DEBUG)
    Serial.println("Setting up END Points");
  server.on("/", handleRoot);
  server.on("/wifisave", handleWifiSave);
  server.on("/activateAPmode", handleactivateAPmode);
  server.on("/resetCredentials",handleResetCreds);
  server.on("/getESPstats",handleESPStats);
  server.on("/connectWiFi",handleconnectWiFi);
  
  server.on("/activateFan",handleactivateFan);
  server.on("/activateGateLock",handleactivateGateLock);
  server.on("/activateLight",handleactivateLight);
  server.on("/activateLed",handleactivateLed);
//  server.on("/activateCFL",handleactivateCFL);
  server.on("/all",handleall);
  
  server.onNotFound ( handleNotFound );
  server.begin(); // Web server start
  if(DEBUG)
    Serial.println("HTTP server started");
  
}

void setupMDNS()
{
  // Setup MDNS responder
  if (!MDNS.begin(myHostname))
  {
    if(DEBUG)
      Serial.println("Error setting up MDNS responder!");
  } 
  else 
  {
    if(DEBUG)
      Serial.println("mDNS responder started");
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
  }  
  
}
