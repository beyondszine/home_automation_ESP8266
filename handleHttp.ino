/** Handle root or redirect to captive portal 

Functions in the file
      void handleRoot() 
      void handleWifiSave() 
      void handleactivateAPmode()
      void handleNotFound()
      void handleResetCreds()
      void handleESPStats()

*/


void handleRoot() 
{
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head></head><body>"
    "<h1>HELLO WORLD!!</h1>"
  );
  if (server.client().localIP() == apIP) {
    server.sendContent(String("<p>You are connected through the soft AP: ") + softAP_ssid + "</p>");
  } else {
    server.sendContent(String("<p>You are connected through the wifi network: ") + ssid + "</p>");
  }
  server.sendContent(
    "<p>You may want to <a href='/wifi'>config the wifi connection</a>.</p>"
    "</body></html>"
  );
  server.client().stop(); // Stop is needed because we sent no content length
}


void handleWifi() 
{
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head></head><body>"
    "<h1>Wifi config</h1>"
  );
  if (server.client().localIP() == apIP) 
  {
    server.sendContent(String("<p>You are connected through the soft AP: ") + softAP_ssid + "</p>");
  }
  else 
  {
    server.sendContent(String("<p>You are connected through the wifi network: ") + ssid + "</p>");
  }
  server.sendContent(
    "\r\n<br />"
    "<table><tr><th align='left'>SoftAP config</th></tr>"
  );
  server.sendContent(String() + "<tr><td>SSID " + String(softAP_ssid) + "</td></tr>");
  server.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.softAPIP()) + "</td></tr>");
  server.sendContent(
    "</table>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN config</th></tr>"
  );
  server.sendContent(String() + "<tr><td>SSID " + String(ssid) + "</td></tr>");
  server.sendContent(String() + "<tr><td>IP " + toStringIp(WiFi.localIP()) + "</td></tr>");
  server.sendContent(
    "</table>"
    "\r\n<br />"
    "<table><tr><th align='left'>WLAN list (refresh if any missing)</th></tr>"
  );
  if(DEBUG)
    Serial.println("scan start");
  int n = WiFi.scanNetworks();
  if(DEBUG)  
    Serial.println("scan done");
  if (n > 0)
  {
    for (int i = 0; i < n; i++)
    {
      server.sendContent(String() + "\r\n<tr><td>SSID " + WiFi.SSID(i) + String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":" *") + " (" + WiFi.RSSI(i) + ")</td></tr>");
    }
  } 
  else 
  {
    server.sendContent(String() + "<tr><td>No WLAN found</td></tr>");
  }
  server.sendContent(
    "</table>"
    "\r\n<br /><form method='POST' action='wifisave'><h4>Connect to network:</h4>"
    "<input type='text' placeholder='network' name='n'/>"
    "<br /><input type='password' placeholder='password' name='p'/>"
    "<br /><input type='submit' value='Connect/Disconnect'/></form>"
    "<p>You may want to <a href='/'>return to the home page</a>.</p>"
    "</body></html>"
  );
  server.client().stop(); // Stop is needed because we sent no content length
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handleWifiSave() 
{
  String t_ssid = server.arg("n");
  String t_pass = server.arg("p");
  if(DEBUG)   
  {
    Serial.println("wifi save");
    Serial.print("obtained parameters via psot call\t\t");
    Serial.print(t_ssid);
    Serial.print("\t");
    Serial.println(t_pass);
  }
  t_ssid.toCharArray(ssid,t_ssid.length()+1);
  t_pass.toCharArray(password,t_pass.length()+1);
  if(DEBUG)  
  {
    Serial.println(ssid);
    Serial.println(password);
  }
  String message="OK! Saved\n<br />";
  
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", message); // Empty content inhibits Content-length header so we have to close the socket ourselves.

  server.client().stop(); // Stop is needed because we sent no content length
  delay(100);

  saveCredentials();
  credentialsPresent = true;
}


void handleactivateAPmode()
{
  String message="";
  if(DEBUG)  
    Serial.println("Changing AP mode NOW");
  int statusAP = server.arg("status").toInt();
  if(statusAP)
  {
    if(DEBUG)
      Serial.println("activating Access Point on Device");
    activateAPmode(); 
    message="{\"Status\":\"Success\", \"AP\": \"Activated\"}";  
  }
  else
  {
    WiFi.softAPdisconnect(true);   
    delay(500);
    if(DEBUG)    
      Serial.println("de-activated AP mode");
    message="{\"Status\":\"Success\", \"AP\": \"De-Activated\"}";  
  }

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", message); // Empty content inhibits Content-length header so we have to close the socket ourselves.

  server.client().stop(); // Stop is needed because we sent no content length
  delay(100);
}

void handleNotFound() 
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) 
  {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send ( 404, "text/plain", message );
}

void handleResetCreds() 
{
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  server.sendContent(
    "<html><head></head><body>"
    "<h1>Old credentials being deleted!!</h1>"
  );
  server.client().stop(); // Stop is needed because we sent no content length
  delay(100);
  resetCredentials();
}

void handleESPStats()
{
  String message="Heap Size:\t";
  message += ESP.getFreeHeap();
  message += "<br />\nESP chid id\t";
  message += ESP.getChipId();
  message += "\n<br />";  

  message += "Connected to ";
  message += ssid;
  message += "\n<br />IP address: ";
  message += toStringIp(WiFi.localIP());
  message += "\n<br /> Status :\t";
  message += WiFi.status();
  message += "\n<br />";

  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", message); // Empty content inhibits Content-length header so we have to close the socket ourselves.  
}


void handleactivateFan()
{
  int resp=0;
  if(DEBUG)  
    Serial.print("Activate Fan Handler");
  String value = server.arg("value");

  int val = value.toInt();
    
  String message = "Activate Fan Command\t";
  if(val==1 || val ==0)
    {
      message += "Valid";
      resp = 1;
    }
  else
  {
    message += "Invalid";
    resp = 0;
  }
  message += "\nStatus\t"; 
  message += value;
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", message); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  if(resp)
    digitalWrite(Fan, val);  
}

void handleactivateGateLock()
{
  int resp=0;
  if(DEBUG)  
    Serial.print("Activate GateLock Handler");
  String value = server.arg("value");

  int val = value.toInt();
    
  String message = "Activate GateLock Command\t";
  if(val==1 || val ==0)
    {
      message += "Valid";
      resp = 1;
    }
  else
  {
    message += "Invalid";
    resp = 0;
  }
  message += "\nStatus\t"; 
  message += value;
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", message); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  if(resp)
    digitalWrite(GateLock, val);  
}

void handleactivateLed()
{
  int resp=0;
  if(DEBUG)  
    Serial.print("Activate Led Handler");
  String value = server.arg("value");

  int val = value.toInt();
    
  String message = "Activate Led Command\t";
  if(val==1 || val ==0)
    {
      message += "Valid";
      resp = 1;
    }
  else
  {
    message += "Invalid";
    resp = 0;
  }
  message += "\nStatus\t"; 
  message += value;
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", message); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  if(resp)
    digitalWrite(Led, val);  
}


void handleactivateLight()
{
  int resp=0;
  if(DEBUG)  
    Serial.print("Activate Light Handler");
  String value = server.arg("value");

  int val = value.toInt();
    
  String message = "Activate Light Command\t";
  if(val==1 || val ==0)
    {
      message += "Valid";
      resp = 1;
    }
  else
  {
    message += "Invalid";
    resp = 0;
  }
  message += "\nStatus\t"; 
  message += value;
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", message); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  if(resp)
    digitalWrite(Light, val);  
}


void handleactivateCFL()
{
  int resp=0;
  if(DEBUG)  
    Serial.print("Activate CFL Handler");
  String value = server.arg("value");

  int val = value.toInt();
    
  String message = "Activate CFL Command\t";
  if(val==1 || val ==0)
    {
      message += "Valid";
      resp = 1;
    }
  else
  {
    message += "Invalid";
    resp = 0;
  }
  message += "\nStatus\t"; 
  message += value;
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", message); // Empty content inhibits Content-length header so we have to close the socket ourselves.
//  if(resp)
//    digitalWrite(CFL, val);  
}

void handleall()
{
  int resp=0;
  if(DEBUG)  
    Serial.print("Activate All Off Handler");
  String value = server.arg("value");

  int val = value.toInt();
    
  String message = "Activate All Off Command\t";
  if(val==1 || val ==0)
    {
      message += "Valid";
      resp = 1;
    }
  else
  {
    message += "Invalid";
    resp = 0;
  }
  message += "\nStatus\t"; 
  message += value;
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", message); // Empty content inhibits Content-length header so we have to close the socket ourselves.
  if(resp)
  {
    digitalWrite(Fan, val);  
    digitalWrite(Light, val);  
    digitalWrite(GateLock, val);  
    digitalWrite(Led, val);  
//    digitalWrite(CFL, val);  
  
  }
}

void handleconnectWiFi()
{
  connectWifi();
  String message ="Connecting to Wifi";
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(200, "text/html", message); // Empty content inhibits Content-length header so we have to close the socket ourselves.
}

