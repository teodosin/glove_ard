/*********************************************************************
 This is an example for our nRF52 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>


// Create the custom flex sensor service
BLEService rukaService = BLEService(0x2222); // 0x1812

// Create the custom flex sensor characteristic
BLECharacteristic flexChar = BLECharacteristic(UUID16_CHR_GENERIC_LEVEL); // 0x2AF9

// Create the imu characteristic
BLECharacteristic imuChar = BLECharacteristic(UUID16_UNIT_ACCELERATION_METRES_PER_SECOND_SQUARED); //0x2713


// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery

void ble_setup()
{
#if CFG_DEBUG
  // Blocking wait for connection when debug mode is enabled via IDE
  while ( !Serial ) yield();
#endif


  // Setup the BLE LED to be enabled on CONNECT
  // Note: This is actually the default behavior, but provided
  // here in case you want to control this LED manually via PIN 19
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth 
  // more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setName("Ruka");
  Bluefruit.setTxPower(8);    // Check bluefruit.h for supported values
  //Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();


  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);

  // Configure and Start the custom flex sensor service
  rukaService.begin();

  flexChar.setProperties(CHR_PROPS_READ);
  flexChar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  flexChar.setMaxLen(10);
  flexChar.setUserDescriptor("flex");
  flexChar.begin();
  // flexChar.write8(0);

  imuChar.setProperties(CHR_PROPS_READ);
  imuChar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  imuChar.setMaxLen(12);
  imuChar.setUserDescriptor("imu");
  imuChar.begin();


  // Set up and start advertising
  
  startAdv();

}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  Bluefruit.Advertising.addService(rukaService);
  
  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

void send_flex(int flexVals[], int num){
  Serial.print("Raw flex values: ");
  for (int i = 0; i < sizeof(flexVals); i++){
    Serial.print(flexVals[i]);
    Serial.print(" | ");
  }
  Serial.println("");

  // Convert the 14-bit value to a 16-bit unsigned integer
  uint8_t flexData[num * 2];

  // Create a byte array to store the 16-bit value
  for (int i = 0; i < num; i++) {
    uint16_t flexValue = (uint16_t)flexVals[i];
    flexData[i * 2] = (uint8_t)(flexValue >> 8);     // High byte
    flexData[i * 2 + 1] = (uint8_t)(flexValue & 0xFF);  // Low byte
  }

  Serial.print("Converted flex values: ");
  for (int i = 0; i < sizeof(flexData); i++){
    Serial.print(flexData[i]);
    Serial.print(" | ");

  }
  Serial.println("");

  // Send the flex sensor value as a byte array
  flexChar.write(flexData, sizeof(flexData));
  // Notify the connected client about the flex value
  //flexChar.notify(flexData, sizeof(flexData));

  uint16_t backConverted[num];
  for (int i = 0; i < num; i++){
    uint16_t value = (uint16_t)((flexData[i * 2] << 8) | flexData[i * 2 + 1]);
    backConverted[i] = value;
  }
  Serial.print("Backconverted: ");
  for (int i = 0; i < sizeof(backConverted); i++){
    Serial.print(backConverted[i]);
    Serial.print(" | ");
  }
  Serial.println("");

  bool validConversion = true;
  for (int i = 0; i < num; i++){
    if (flexVals[i] != backConverted[i]){
      validConversion = false;
    }
  }
  Serial.print("same conversion: ");
  Serial.println(validConversion);
}

void send_imu(float imuVals[]){
  uint8_t imuData[12];

  for (int i = 0; i < sizeof(imuVals); i++){
    uint16_t intValue = static_cast<uint16_t>(imuVals[i] * 100); // Multiply by 100 to preserve 2 decimal places
    imuData[i * 2] = static_cast<uint8_t>(intValue >> 8);       // High byte
    imuData[i * 2 + 1] = static_cast<uint8_t>(intValue & 0xFF); // Low byte
  }

  imuChar.write(imuData, sizeof(imuData));
}

void ble_loop()
{
  // Forward data from HW Serial to BLEUART
  while (Serial.available())
  {
    // Delay to wait for enough input, since we have a limited transmission buffer
    delay(2);

    uint8_t buf[64];
    int count = Serial.readBytes(buf, sizeof(buf));
    bleuart.write( buf, count );
  }

  // Forward from BLEUART to HW Serial
  while ( bleuart.available() )
  {
    uint8_t ch;
    ch = (uint8_t) bleuart.read();
    Serial.write(ch);
  }
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}
