#include <esp_now.h>
#include <WiFi.h>
#include "DRV8833.h"


esp_now_peer_info_t peerInfo;
uint8_t broadcastAddress[] = {0xA0, 0xB7, 0x65, 0x4A, 0x4D, 0x60};  // mac do esp32 receptor, para descobrir o mac de qualquer esp32, acesse o site random nerd

typedef struct pacote {
    int bot;
    int len;
    int ID;
    int ch[20];
} pacote;
pacote pack_rx;

DRV8833 motor(4, 27, 13, 14);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // Init ESP-NOW
  WiFi.mode(WIFI_STA);
  motor.begin();
  motor.stop();
  if (esp_now_init() != ESP_OK) {
      Serial.println("Error initializing ESP-NOW");
      motor.stop();
      return;
  }

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
      Serial.println("Failed to add peer");
      return;
  }

  // Register callback for received data
  esp_now_register_recv_cb(OnDataReceived);

}

void loop() {
  
}

//  ---------------------------------------------------------------------------------------------------------
//  --> OnDataReceived é onde vai o código das ações que devem ser recebidas e realizadas pelo ESP32 receptor
//  ---------------------------------------------------------------------------------------------------------

void OnDataReceived(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // Check if the received data matches the size of the pacote struct
  if (data_len == sizeof(pacote)) {
    // Copy received data to the pack_rx structure
    memcpy(&pack_rx, data, sizeof(pacote));

    // Process the received data as needed


    // *********** Descomente o trecho abaixo para o serial print dos canais do controle transmissor*************


    // Serial.print("Received Data: ID = ");  
    // Serial.print(pack_rx.ID);
    // Serial.print(", Channels: ");
    // for (int i = 0; i < pack_rx.len; i++) {
    //   Serial.print(" ");
    //   Serial.print(pack_rx.ch[i]);
    // }
    // Serial.println();
    // **********************************************************************************************************


    // Trecho de comando para movimentação do robô Mini Sumô Ultra T
    // ------------------------------------------------------
    int x = map(pack_rx.ch[0], 1000, 2000, -1024, 1024);
    int y = map(pack_rx.ch[1], 1000, 2000, -1024, 1024);

    int vel_esq = constrain( (-x + y ), -1024, 1024); // modificar as expressões dependendo das conexões dos pinos e dos motores
    int vel_dir = constrain( ( x + y ), -1024, 1024); 

    motor.move(vel_esq, vel_dir);
    // ------------------------------------------------------
  


  } else {
    Serial.println("Received data with incorrect size");
  }
}