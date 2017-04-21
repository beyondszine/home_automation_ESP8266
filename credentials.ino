
// List of functions in the file
//bool keyPresentEEPROM();
//void loadCredentials();
//void saveCredentials();
//void resetCredentials();
//void factory_reset();
//void resetNonce();
//void resetKeys();
//bool saveKeysEEPROM();
//String getKeyEEPROM(char ch);
//bool keyPresentEEPROM();





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
    wifiCredsPresent = false;
    return 0;
  }
  else 
  {
    wifiCredsPresent = true;
    return 1;
  }
}

void loadCredentials() 
{
  EEPROM.begin(512);
  EEPROM.get(0, ssid);
  EEPROM.get(0+sizeof(ssid), password);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.end();
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
  EEPROM.begin(512);
  EEPROM.put(0, ssid);
  EEPROM.put(0+sizeof(ssid), password);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(ssid)+sizeof(password), ok);
  EEPROM.commit();
  EEPROM.end();
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

void factory_reset()
{
  resetCredentials();
  resetNonce();
  resetKeys();
}

void resetNonce()
{
  if(DEBUG)  
    Serial.println("Resetting Nonce to initial state");
  int nonceAddress = 1000;
  EEPROM.begin(4096); 
  for(int i=0;i<6;i++)
    EEPROM.write(nonceAddress + i,'0'); 
  EEPROM.end();  
}

void resetKeys()
{
  if(DEBUG)  
    Serial.println("Resetting Keys to blank");
  int keyAddress = 1100;
  EEPROM.begin(4096); 
  for(int i=0;i<64;i++)
    EEPROM.write(keyAddress + i,0); 
  EEPROM.end();  
}


bool saveKeysEEPROM()
{
  if(DEBUG)
    Serial.println("saving keys in EEPROM");
  int keyBaseAddr = 1100;
  int skeyOffset = 20;
  EEPROM.begin(4096); 
  for(int i=0;i<16;i++)
  {
    EEPROM.write(keyBaseAddr + i,dkey[i]);
    EEPROM.write(keyBaseAddr + i + skeyOffset,skey[i]);
  }

  EEPROM.write(keyBaseAddr + 40, 'O');
  EEPROM.write(keyBaseAddr + 41, 'K');
  EEPROM.end();  
}

String getKeyEEPROM(char ch)
{
  if(DEBUG)  
    Serial.println("Getting key from EEPROM");
  String tempokey="";
  int keyBaseAddr = 1100;
  EEPROM.begin(4096); 

  if(ch == 'd')
  {
    for(int i=0;i<16;i++)
    tempokey  +=  (char)EEPROM.read(keyBaseAddr+i);
  }
  else if(ch == 's')
  {
    keyBaseAddr = 1120;
    for(int i=0;i<16;i++)
    tempokey  +=  (char)EEPROM.read(keyBaseAddr+i);
  }
  EEPROM.end();
  if(DEBUG)  
    Serial.println(tempokey);  
  return tempokey;
}

bool keyPresentEEPROM()
{
  int keyBaseAddr = 1100;
  bool keysPresent = false;
  EEPROM.begin(4096); 
  if(DEBUG)
  {
    Serial.println("checking if keys present");
    Serial.println((char)EEPROM.read(keyBaseAddr + 40));
    Serial.println((char)EEPROM.read(keyBaseAddr + 41));
  }
  if( (EEPROM.read(keyBaseAddr + 40) == 79) && (EEPROM.read(keyBaseAddr + 41) == 75) )
    keysPresent = true;  
  EEPROM.end();  
  return keysPresent;
}


void saveNonceEEPROM(String var)
{
  
  byte tempNonce[6]="";
  var.getBytes(tempNonce,var.length()+1);
  if(DEBUG)
  {
    Serial.println("Testing temp nonce buffer");
    for(int i=0;i<6;i++)
      Serial.print(tempNonce[i]);
    Serial.println("printed");
  }
  int nonceAddress = 1000;
  EEPROM.begin(4096);
  for(int i=0;i<6;i++)
    EEPROM.write(nonceAddress + i, tempNonce[i]); 
  EEPROM.end();  
}

String loadNonceEEPROM() 
{
  int nonceAddress = 1000;
  String tempnonce = "";

  EEPROM.begin(4096);
  if(DEBUG)
  {
    Serial.print("nonce loaded from EEPROM:\t");
    Serial.println("");
  }
  for(int i=0;i<6;i++)
    tempnonce[i] = (char)EEPROM.read(nonceAddress+i);

  if(DEBUG)
  {
    Serial.println("updated nonce is\t");
    Serial.println(tempnonce);
  }
  EEPROM.end();
  return tempnonce;
}
