
// List of functions in the file for quick view
//void loadCredentials();
//void saveCredentials();
//void resetCredentials();
//void factory_reset();


/** Load WLAN credentials from EEPROM */
bool WiFiCredentialsPresent()
{
  char tempssid[32] = "";
  char temppassword[32] = "";  
  EEPROM.get(0, tempssid);
  EEPROM.get(0+sizeof(tempssid), temppassword);
  char tempOk[2+1];
  EEPROM.get(0+sizeof(tempssid)+sizeof(temppassword), tempOk);
  if (String(tempOk) != String("OK")) 
  {
    return false;
  }
  else 
  {
    return true;
  }
}

void loadCredentials() 
{
  EEPROM.get(0, ssid);
  EEPROM.get(0+sizeof(ssid), password);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssid)+sizeof(password), ok);

  if (String(ok) != String("OK")) 
  {
    ssid[0] = 0;
    password[0] = 0;
  }
  if(DEBUG)
  {
    Serial.println("Recovered credentials:");
    Serial.println(ssid);
    Serial.println(strlen(password)>0?"********":"<no password>");
  }
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() 
{
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.commit();
  
  Serial.println("--------");
  for(int i=0;i<100;i++)
    Serial.print(EEPROM.read(i),HEX);
  Serial.println("\n--------");
}

void resetCredentials() 
{
  // Wifi disconnect : clears out the chunks in heap and EEPROM both.
  WiFi.disconnect();
  delay(1000);
  EEPROM.begin(512);
  if(DEBUG)
    Serial.println("Clearing EEPROM of previously saved credentials");
  for(int i=0;i<100;i++)
    EEPROM.put(i,0);
  EEPROM.commit();
  EEPROM.end();
  if(DEBUG)  
    Serial.println("cleared EEPROM!");
//  ESP.restart();
  delay(2000); 
}

