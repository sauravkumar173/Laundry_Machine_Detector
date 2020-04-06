#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>


// MMA8452Q I2C address is 0x1C(28)
#define Addr 0x1C
// Length of array to hold latest acceleration data
#define ARRAYLENGTH 30
// X Y Z callibration integers to offset any sensor errors
#define XCALIBRATE 12
#define YCALIBRATE -55
#define ZCALIBRATE -1035
// Maximum and minimum acceleration values to trigger device as shaking
#define XMAX_ACCL 300
#define XMIN_ACCL -400
#define YMAX_ACCL 300
#define YMIN_ACCL -400
#define ZMAX_ACCL 300
#define ZMIN_ACCL -400
// Server IP to send POST requests and JSON
#define POSTADDR "http://192.168.20.22:3000/logs"
// Variables used to determine laundry machine state
#define MACHINE_NOT_RUNNING 0
#define MACHINE_START_RUNNING 1
#define MACHINE_SUCCESS_STOP 1
#define MACHINE_INPROGRESS 0
#define START_MESSAGE_SENT 1
#define START_MESSAGE_NOT_SENT 0

// Struct storing X Y Z acceleration data
typedef struct {
  int xReading;
  int yReading;
  int zReading;
}sensorData;

sensorData acclData;

// Flags to indicate if the machine has started and if 
// a message has been sent
int machineStartState;
int machineStopState;
int machineStartSent;

// Arrays to hold latest X Y Z data. Used to determine if 
// device has been still long enough
int latestXVal[ARRAYLENGTH];
int latestYVal[ARRAYLENGTH];
int latestZVal[ARRAYLENGTH];


// WIFI information for connection
const char* ssid     = "SSID";
const char* password = "PASSWORD";

// Port 80 is used since its default port for HTTP
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds
const long timeoutTime = 2000;

/*
 * @brief: Initialises the I2C protocol to read the accelerometer data 
 * input: void
 * return: void
 */
void init_MMA8452() {
  // Initialise I2C communication as MASTER
  Wire.begin();
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select control register
  Wire.write(0x2A);
  // StandBy mode
  Wire.write(0x00);
  // Stop I2C Transmission
  Wire.endTransmission();
 
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select control register
  Wire.write(0x2A);
  // Active mode
  Wire.write(0x01);
  // Stop I2C Transmission
  Wire.endTransmission();
 
  // Start I2C Transmission
  Wire.beginTransmission(Addr);
  // Select control register
  Wire.write(0x0E);
  // Set range to +/- 2g
  Wire.write(0x00);
  // Stop I2C Transmission
  Wire.endTransmission();
  delay(300);
}

/*
 * @brief: retreives accelerometer data from I2C protocol and converts the data
 *         to useable format.
 * input: void
 * return: void
 */
void get_MMA8452_data(){
  unsigned int data[7];
  // Request 7 bytes of data
  Wire.requestFrom(Addr, 7);
 
  // Read 7 bytes of data
  if(Wire.available() == 7)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
    data[3] = Wire.read();
    data[4] = Wire.read();
    data[5] = Wire.read();
    data[6] = Wire.read();
  }
 
  // Convert the data to 12-bits
  int xAccl = ((data[1] * 256) + data[2]) / 16;
  if (xAccl > 2047)
  {
    xAccl -= 4096;
  }
 
  int yAccl = ((data[3] * 256) + data[4]) / 16;
  if (yAccl > 2047)
  {
    yAccl -= 4096;
  }
 
  int zAccl = ((data[5] * 256) + data[6]) / 16;
  if (zAccl > 2047)
  {
    zAccl -= 4096;
  }

  // Stores X Y Z reading into struct
  acclData.xReading = xAccl + XCALIBRATE;
  acclData.yReading = yAccl + YCALIBRATE;
  acclData.zReading = zAccl + ZCALIBRATE;
  
}

/*
 * @brief: Stores that last ARRAYLENGTH amount of X Y Z accelerometer data
 * input: void
 * return: void
 */
void store_MMA8452_data() {
  for(int i = ARRAYLENGTH - 1; i > 0; i--) {
        latestXVal[i] = latestXVal[i-1];
        latestYVal[i] = latestYVal[i-1];
        latestZVal[i] = latestZVal[i-1];
  }
    
  latestXVal[0] = acclData.xReading; 
  latestYVal[0] = acclData.yReading; 
  latestZVal[0] = acclData.zReading; 
  
}

/*
 * @brief: checks if any data stored in the X Y Z arrays are larger or smaller than
 *         the minimum and maximum set
 * input: void
 * return: 0 - machine has not stopped as datapoint is outside the set limits
 *         1 - machine has stopped
 */
int detect_machine_stop() {
  for(int i = 0; i < ARRAYLENGTH; i++) {
    // Loops through each element is stored history of data to see if all are considered as
    // a small enough shake
    if((latestXVal[i] > XMAX_ACCL || latestXVal[i] < XMIN_ACCL)
        || (latestYVal[i] > YMAX_ACCL || latestYVal[i] < YMIN_ACCL) 
        || (latestZVal[i] > ZMAX_ACCL || latestZVal[i] < ZMIN_ACCL)) {

      return 0;    
    } 
  }
  
  return 1;
}

/*
 * @brief: checks incoming data is larger or smaller than the minimum and maximum set
 * input: void
 * return: 0 - machine has not started
 *         1 - machine has started as datapoint is outside set limit
 */
int detect_machine_start() {
  // Checks if recieved data is outside the acceleration limits and considered a shakes by the machine
  if((acclData.xReading > XMAX_ACCL || acclData.xReading < XMIN_ACCL) 
      && (acclData.yReading > YMAX_ACCL || acclData.yReading < YMIN_ACCL) 
      && (acclData.zReading > ZMAX_ACCL || acclData.zReading < ZMIN_ACCL)) {
        
    return MACHINE_START_RUNNING;
    
  } else {

    return MACHINE_NOT_RUNNING;   
  }
}

/*
 * @brief: sends log information as a POST request to server to be added to SQL database
 * input: void
 * return: void
 */
void send_to_logger(String logMessage, String machineState) {
    
    HTTPClient http;
    
    // Begins HTTP connection to the ruby server
    http.begin(POSTADDR); 
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Creates POST request string with the custom content attached
    String postData = "log[machine_state]=" + machineState + "&log[log_message]=" + logMessage +
      "&log[log_x_accl]=" + acclData.xReading + "&log[log_y_accl]=" + acclData.yReading + 
      "&log[log_z_accl]=" + acclData.zReading;

    // Sends POST request to RUBY server
    int httpCode = http.POST(postData);
    Serial.println(httpCode);
        
    http.end();
}

/*
 * @brief: Runs the arduino server and checks for any connection. Afterwards checks if client tried to
 * access API and sends JSON with data
 * input: void
 * return: void
 */
void run_arduino_server(void) {
  // Creates a WIFI client on ESP8266
  WiFiClient client = server.available();
  
  // Checks client connection from ruby server
  if (client) {
    // Reveals new client connection
    Serial.println("New Client.");

    String currentLine = "";               
    currentTime = millis();
    previousTime = currentTime;

    // Loop checks if client is connected and the timout time hasnt expired
    while (client.connected() && currentTime - previousTime <= timeoutTime) { 
      
      currentTime = millis();        
      
      if (client.available()) {      
        // Reads data sent from client and stores in header variable
        char c = client.read();          
        Serial.write(c);                    
        header += c;
        if (c == '\n') {                

          // Sends client connection info
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/json");
            client.println("Connection: close");
            client.println();

            // Detects Get request for machine data
            if(header.indexOf("GET /machine/data") >= 0) {
              // Creates JSON string based on machine state and accelerometer data
              Serial.println("Sending data");
              char jsonResponse[100];
              char* machineStatus = "Error";
              if(machineStartState == MACHINE_START_RUNNING) {
                Serial.println("machine running");
                machineStatus = "Running";
              } else if (machineStartState == MACHINE_NOT_RUNNING) {
                Serial.println("machine stopping");
                machineStatus = "Stopped";
              } else {
                Serial.println("machine error");
                machineStatus = "Error";
              }

              // Sends JSON data to ruby server
              sprintf(jsonResponse, "{\"status\": \"%s\",\"xVal\": \"%d\",\"yVal\": \"%d\",\"zVal\": \"%d\"}", 
                  machineStatus ,acclData.xReading, acclData.yReading, acclData.zReading);
              Serial.println(jsonResponse);
              client.print(jsonResponse);
            }
            
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') { 
          currentLine += c;    
        }
        
      }  
    }
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  
  
}

void setup()
{
  // Initialise Serial Communication, set baud rate = 9600
  Serial.begin(115200);
  init_MMA8452();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.print(ssid);
  
  // Start Connection
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("Connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  machineStartState = MACHINE_NOT_RUNNING;
  machineStopState = MACHINE_NOT_RUNNING;
  machineStartSent = START_MESSAGE_NOT_SENT;

  // Starts local server on ESP8266
  server.begin();
  
}
 
void loop() {
  // Receives and stores accelerometer data
  get_MMA8452_data();
  store_MMA8452_data();

  // If machine is stop currently running, checks to see if the device
  // has shaken enough to be considered on
  if(machineStartState == MACHINE_NOT_RUNNING) {
    machineStartState = detect_machine_start();
  }

  if(machineStartState == MACHINE_START_RUNNING ) {
    // Sends Log message to ruby server detailing machine is running
    if(machineStartSent == START_MESSAGE_NOT_SENT) {
      send_to_logger("Washing Machine has been started", "Machine Start");
      // Turns on LED
      digitalWrite(LED_BUILTIN, LOW);
      machineStartSent = START_MESSAGE_SENT;
    }

    // Checks to see if machine has stopped shaking for long enough
    machineStopState = detect_machine_stop();
    
    if(machineStopState == MACHINE_SUCCESS_STOP) {
      // Sends message to ruby server that machine has been stopped
      send_to_logger("Washing Machine has been stopped", "Machine Stop");
      // Turns off LED
      digitalWrite(LED_BUILTIN, HIGH);
      // Resets flag variable
      machineStartState = MACHINE_NOT_RUNNING;
      machineStopState = MACHINE_NOT_RUNNING;
      machineStartSent = START_MESSAGE_NOT_SENT;
    }
      
  }

  run_arduino_server();
  
  delay(100);
}
