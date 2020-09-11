
#include "wifi.h"

void wifi_connect(void)
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi not connected");
  }else
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void initMQTT(void) 
{
    mqtt.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    mqtt.setCallback(mqttCallback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
    
    snprintf (topicSensor1, 30, "%s%s",ID_MQTT, "/sensores/sensor1");
    Serial.println(topicSensor1);

    snprintf (topicSats, 31, "%s%s",ID_MQTT, "/sensores/status/gps/sats");
    Serial.println(topicSats);
}

void connectMQTT(void) 
{
    while (!mqtt.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (mqtt.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
        } 
        else 
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) 
{
    char c;
    String msg;
    
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.write(payload, length);
    Serial.println();

    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       c = (char)payload[i];
       msg += c;
    }
    
    //avalia se a mensagem é para este NodeMCU
    if (msg.equals("1"))
    {
      Serial.println("");   
    }

    if (msg.equals("0"))
    {
      Serial.println("");   
    } 
}

void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
        
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
    
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
  
    Serial.println();
    Serial.print("Reconectado com sucesso na rede ");
    Serial.println(SSID);
    Serial.print("IP obtido: ");
    Serial.println(WiFi.localIP());
}

void checkConnections(void)
{
  if (!(WiFi.status() == WL_CONNECTED))
  {

    Serial.print("Lost WiFi!");
    
    reconectWiFi();
  }
  
  if(mqtt.connected())
  {
    Serial.print("            ");
    return;
  }
    else
        {
          Serial.println("Lost Server!");
      
          connectMQTT();
        }
}
