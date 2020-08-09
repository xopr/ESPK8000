#define WIFI
#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#define prog_uint16_t uint16_t PROGMEM
#define prog_uint8_t uint8_t PROGMEM
#else
#include <WProgram.h>
#endif

#include <Wire.h>


uint16_t g_outputs = 0;
unsigned long g_nTimer;
static const uint16_t s_nFrameDuration = 500;
bool g_demoMode = true;
byte g_address = 0;

#ifdef WIFI
// Via library manager https://github.com/tzapu/WiFiManager
#include <WiFiManager.h>

//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* g_ssid     = "k8000";
const char* g_password = "k8000k8000";

ESP8266WebServer webServer( 80 );

////////////////////////////////////////////////////////////////////////////////
// web server callbacks
////////////////////////////////////////////////////////////////////////////////
void handleRoot()
{
  String message = F("<html>");
  message += F( "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><title>K8000</title>" );
  message += F( "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" );
  message += F( "<link rel=\"stylesheet\" href=\"k.css\">" );
  message += F( "</head><body>" );
  for ( int n = 0; n < 16; n++ )
  {
    String strN = String( n );
    message += F( "<input type=\"checkbox\" name=\"b\" value=\"" );
    message += strN;
    message += F( "\"  class=\"l v\" id=\"" );
    message += strN;
    message += F( "\" /><label for=\"" );
    message += strN;
    message += F( "\"></label>" );
  }

  message += F( "<br/>" );
  message += F( "Auto:<input type=\"checkbox\" class=\"l h\" id=\"auto\"/><label for=\"auto\"></label>" );
  message += F( "Demo:<input type=\"checkbox\" class=\"l h\" id=\"demo\"/><label for=\"demo\"></label>" );
  message += F( "<script src=\"k.js\"></script>" );
  message += F( "</body></html>" );

  webServer.send( 200, F( "text/html" ), message );
}

void handleHelp()
{
  String message = F( "<html>" );
  message += F( "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><title>K8000 :: help</title>" );
  message += F( "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">" );
  message += F( "<link rel=\"stylesheet\" href=\"k.css\">" );
  //    <script src="k.js"></script>
  message += F( "</head><body>" );

  message += F( "Help:" );
  message += F( "</body></html>" );
  webServer.send( 200, F( "text/html" ), message );
}

void handleStyleSheet()
{
  String message = "";
  message += F( "input[type=\"checkbox\"].l{display:none}" );
  message += F( "input[type=\"checkbox\"].l+label{cursor:pointer;font-size:1em}" );

  message += F( ".l+label{background:#fafbfa;border-radius:1em;display:inline-block;position:relative;-webkit-transition:all 0.1s ease-in;transition:all 0.1s ease-in}" );
  message += F( ".h+label{width:3.64em;height:2.05em}" );
  message += F( ".v+label{width:2.05em;height:3.64em}" );

  message += F( ".l+label:before,.l+label:after{content:' ';position:absolute;border-radius:1em;-webkit-transition:all 0.1s ease-in;transition:all 0.1s ease-in;top:0;left:0}" );
  message += F( ".h+label:before,.h+label:after{height:100%}" );
  message += F( ".v+label:before,.v+label:after{width:100%}" );

  message += F( ".l+label:after{box-shadow:inset 0 0 0 0 #eee, 0 0 0.03em rgba(0,0,0,0.4)}" );
  message += F( ".h+label:after{width:100%}" );
  message += F( ".v+label:after{height:100%}" );

  message += F( ".l+label:before{background:#fff;z-index:1;box-shadow:0 0.09em 0.03em rgba(0,0,0,0.05), 0 0 0.03em rgba(0,0,0,0.3)}" );
  message += F( ".h+label:before{width:2em}" );
  message += F( ".v+label:before{height:2em}" );

  message += F( ".l:active+label:after{box-shadow:inset 0 0 0 1.1em #eee, 0 0 0.03em #eee}" );

  message += F( ".h:active+label:before{width:2.5em}" );
  message += F( ".v:active+label:before{height:2.5em}" );

  message += F( ".h:checked:active+label:before{left:1.14em}" );
  message += F( ".v:checked:active+label:before{top:1.14em}" );

  message += F( ".l+label:active{box-shadow:0 0.03em 0.06em rgba(0,0,0,0.05), inset 0 0.03em 0.09em rgba(0,0,0,0.1)}" );

  message += F( ".h:checked+label:before{left:1.64em}" );
  message += F( ".v:checked+label:before{top:1.64em}" );

  message += F( ".l:checked+label:after{background:#da4c60;box-shadow:0 0 0.03em #da4c60}" );
  webServer.send( 200, F( "text/css" ), message );
}

void handleScript()
{
  String message = "var data=null;";
  message += F( "window.addEventListener(\"load\",\
function(){\
var a=document.querySelector(\"#auto\"),d=document.querySelector(\"#demo\"),b=document.querySelectorAll(\".l[name=b]\");\
[].forEach.call(b,function(e){\
 e.addEventListener(\"change\",function(){\
  data=\"p=\"+readButtons().toString(16)+\"&d=\"+(d.checked?1:0);\
  if (!a.checked)deviceSync();\
  });\
 });\
a.addEventListener(\"change\",function(){\
 deviceSync();\
 });\
d.addEventListener(\"change\",function(){\
 data=\"p=\"+readButtons().toString(16)+\"&d=\"+(d.checked?1:0);deviceSync();\
 });\
deviceSync();});" );

  message += F( "function deviceSync(){\
var x=new XMLHttpRequest();\
x.addEventListener(\"load\",onDeviceData);\
x.addEventListener(\"error\",onDeviceData);\
x.open(\"POST\",\"k.json\");\
x.responseType=\"json\";\
x.send(data);data=null;}" );

  message += F( "function onDeviceData(e){\
if(e.target.response){\
 writeButtons(parseInt(e.target.response.d,16));}\
if(document.querySelector(\"#auto\").checked)\
 window.requestAnimationFrame(deviceSync);}" );
 
  message += F( "function readButtons(){var a=document.querySelectorAll(\".l[name=b]\"),v=0;[].forEach.call(a,function(e){if(e.checked)v|=(1<<e.value);});return v;}" );
  message += F( "function writeButtons(v){var a=document.querySelectorAll(\".l[name=b]\");[].forEach.call(a,function(e){e.checked=v&(1<<e.value);});}" );

  webServer.send( 200, F( "text/javascript" ), message );
}

void handleData()
{
  char strOutputs[5];
  String message;
  char* pRead;

  for ( uint8_t i = 0; i < webServer.args( ); i++ )
  {
    switch( webServer.argName( i )[ 0 ] )
    {
      case 'a':
        // Check if we want auto update
        break;

      case 'd':
        // Check if we want demo mode
        g_demoMode = webServer.arg( i )[ 0 ] == '1' ? true : false;
        Serial.print( F( "Demo mode: " ) );
        Serial.println( g_demoMode ? F( "on") : F( "off" ) );
        break;

      case 'p':
        // Parse number and update port
        webServer.arg( i ).toCharArray( strOutputs, 5 );
        // Terminate string just to be sure
        strOutputs[ 4 ] = 0;

        Serial.print( F( "Port data: " ) );
        Serial.println( strOutputs );
        g_outputs = strtol( strOutputs, &pRead, 16 );

        // Update the ports
        writeDevice( );
        break;
    }
  }

  g_outputs = readDevice( );
  // Output value to string
  sprintf( strOutputs, "%x", g_outputs );
  
  message = F( "{\"d\":\"" );
  message += String( strOutputs );
  message += F( "\"}" );

  webServer.send( 200, F( "application/json" ), message );
}

void handleNotFound()
{
  String message = F( "File Not Found\n\n" );
  message += F( "URI: " );
  message += webServer.uri( );
  message += F( "\nMethod: " );
  message += ( webServer.method() == HTTP_GET ) ? "GET" : "POST";
  message += F( "\nArguments: " );
  message += webServer.args( );
  message += F( "\n" );
  for ( uint8_t i = 0; i < webServer.args( ); i++ )
  {
    message += " " + webServer.argName( i ) + ": " + webServer.arg( i ) + "\n";
  }

  webServer.send( 404, F( "text/plain" ), message );
}

void configModeCallback (WiFiManager *myWiFiManager)
{
  Serial.print( F( "Entering config mode (" ) );
  Serial.print( WiFi.softAPIP( ) );
  Serial.print( F( ", " ) );
  Serial.print( myWiFiManager->getConfigPortalSSID( ) );
  Serial.print( F( ") " ) );
}

//////////////////////////////////////////////////////////////////////////////////////
bool setupWifi()
{
  WiFiManager wifiManager;

  // set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback( configModeCallback );
  //192.168.4.1

  // 30 sec connect timeout
  wifiManager.setTimeout( 120 );

  //WITHOUT THIS THE AP DOES NOT SEEM TO WORK PROPERLY WITH SDK 1.5 , update to at least 1.5.1
  //WiFi.mode(WIFI_STA);

  wifiManager.autoConnect( g_ssid, g_password );

  IPAddress ip;
  ip = WiFi.softAPIP();

  // We start by connecting to a WiFi network
  // Uncomment if you want Station mode only
  //WiFi.mode( WIFI_STA );
  // Default mode is AccessPoint and Station combined
  //WiFi.mode( WIFI_AP_STA );

  Serial.print( "Connected: ");
  ip = WiFi.localIP( );
  //sprintf( strRawMessage, "^G%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3] );

  Serial.println( ip );
  return true;
}

bool setupWebserver()
{
  webServer.on( "/", handleRoot );
  webServer.on( "/h", handleHelp );
  webServer.on( "/k.css", handleStyleSheet );
  webServer.on( "/k.js", handleScript );
  webServer.on( "/k.json", handleData );
  webServer.onNotFound( handleNotFound );
  webServer.begin();

  return true;
}
#endif

void setup()
{

  Serial.begin( 115200 );
  delay( 10 );
  Serial.println( F("K8000 test app v0.2") );

  // Init
  Serial.println( "Initializing.." );

  // For ESP.;
  //pin 4 = SDA, pin 5 = SCL (tx,rx,5,4,0 order)
  Wire.begin( );
  // Check for k8000 card by scanning PCF8574A id's
  Serial.print( F( "I2C: " ) );
  g_address = scanI2C( 0x37 );
  switch ( g_address  )
  {
    case -1:
      Serial.println( F( "device failure!" ) );
      g_address = 0;
      break;

    case 0:
      Serial.println( F( "no devices found" ) );
      break;
      
    case 0x20:
      // PCF8574 coinvox keypad (0x20)
      Serial.println( F( "PCF8574 found (coinvox)" ) );
      break;

    case 0x38:
      // PCF8576 coinvox LCD (0x38)
      // PCF8574A k8000-1a (0x38) //38h-3Fh
      // PCF8574A k8000-1b (0x39)
      if ( scanI2C( 0x39 ) == 0x39 )
      {
        Serial.println( F( "K8000(master) found" ) );
      }
      else
      {
        Serial.println( F( "PCF8574A or PCF8576 found, ignoring" ) );
        g_address = 0;
      }

      break;


      // TDA8444 K8000-1 DAC (0x40)
      
      // PCF8591 K8000-1 dac+adc (0x48)


    //0x20
    default:
      Serial.print( F( "unknown device: " ) );
      Serial.println( g_address, HEX );
      g_address = 0;
      break;
  }

#ifdef WIFI
  // Set up wifi
  Serial.print( F( "Wifi: " ) );
  setupWifi( );

  // Set up webserver
  Serial.print( F( "HTTP server:" ) );
  if ( setupWebserver( ) )
    Serial.println( F( "started" ) );
  else
    Serial.println( F( "failed!" ) );
#endif
}

void loop()
{
  unsigned long nTimer = millis();
  if ( g_nTimer + s_nFrameDuration < nTimer )
  {
    // We might run into a state where we skiped a frame when serving wifi;
    // adding the value instead of synching to the timer might correct that
    //g_nTimer = nTimer;
    g_nTimer += s_nFrameDuration;

    // If we're in demo mode, do the actual 'frame'
    if ( g_demoMode )
    {
      g_outputs = fillBits( g_outputs, true );
      //g_outputs = rollBits( g_outputs, true );
      //g_outputs++;
      writeDevice( );
    }

  }

  //yield();// still causes reset
  delayMicroseconds( 100 );
  //delay( 1 );

#ifdef WIFI
  webServer.handleClient();
#endif
}

uint16_t readDevice( )
{
  byte error;
  uint16_t value = 0;

  if ( !g_address )
    return 0;

  // Talk to PCF8574A
  Wire.beginTransmission( g_address );
  Wire.requestFrom( g_address, (uint8_t)1);
  value = Wire.read();
  error = Wire.endTransmission( );

  Wire.beginTransmission( (uint8_t)(g_address + 1) );
  Wire.requestFrom( (uint8_t)(g_address + 1), (uint8_t)1);
  value |= ( Wire.read() << 8 );
  error = Wire.endTransmission( );
  
  return value;
}

void writeDevice( )
{
  byte error;

  if ( !g_address )
    return;

  // Talk to PCF8574A
  Wire.beginTransmission( g_address );
  Wire.write( uint8_t( g_outputs & 0xff ) );
  error = Wire.endTransmission( );

  Wire.beginTransmission( g_address + 1 );
  Wire.write( uint8_t( g_outputs >> 8 ) );
  error = Wire.endTransmission( );
}

char scanI2C( )
{
  return scanI2C( 1 );
}

char scanI2C( char _nStartAddress )
{
  byte error, address;
  int nDevices;

  nDevices = 0;
  for(address = _nStartAddress; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission( address );
    error = Wire.endTransmission( );

    if ( error == 0 )
    {
      /*
      Serial.print( F("I2C device found at address 0x") );
      if ( address < 16 )
        Serial.print( F("0") );
      Serial.print( address, HEX );
      Serial.println( F("") );
      */
      return address;
      
      //nDevices++;
    }
    else if ( error == 4 )
    {
      // Failed, return -address ?
      return -1;
      /*
      Serial.print( F("Unknow error at address 0x") );
      if ( address < 16 )
        Serial.print( F("0") );
      Serial.println( address, HEX );
      */
    }
  }

  // No devices found
  return 0;
  /*
  if ( nDevices == 0 )
    Serial.println( F("No I2C devices found\n") );
  else
    Serial.println( F("scan complete\n") );
  */
}


template <class T> T rollBits( T _value, bool _bRight )
{
  if ( _bRight )
    return (_value >> 1) | (_value << (sizeof(_value) * 8 - 1));
  else
    return (_value << 1) | (_value >> (sizeof(_value) * 8 - 1));
}

/*
uint16_t fillBits( uint16_t _value, bool _bRight )
{
  if ( _bRight )
    return (_value >> 1) | (~_value << (sizeof(_value) * 8 - 1));
  else
    return (_value << 1) | (~_value >> (sizeof(_value) * 8 - 1));
}
*/

template <class T> T fillBits( T _value, bool _bRight )
{
  if ( _bRight )
    return (_value >> 1) | (~_value << (sizeof(_value) * 8 - 1));
  else
    return (_value << 1) | (~_value >> (sizeof(_value) * 8 - 1));
}

