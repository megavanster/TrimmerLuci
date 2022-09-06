/*
 2018/11/28 canovam creazione
 
 Sketch che si interfaccia al LOGO per gestire la tensione sulle lampade led del capannone.
 Prende in input un voltaggio da 0 a 5 volt inviato dal plc della Siemens e come uscita comanda
 un MCP4231 (ovvero una resistenza variabile).
 
 MCP4231-502E/P: 
 Pin 1  su Arduino 2
 Pin 2  su Arduino 4
 Pin 3  su Arduino 5 
 Pin 4  va a massa
 Pin 5  uscita 1
 Pin 6  uscita 1
 Pin 7  uscita 1
 Pin 8  uscita 2 - non collegata
 Pin 9  uscita 2 - non collegata 
 Pin 10 uscita 2 - non collegata
 Pin 11 va a massa
 Pin 12 non collegato 
 Pin 13 non collegato
 Pin 14 va a VCC 
*/

int InState;                        	// Valore in input letto dal logo (0-1024)
int valoreResistenza;               	// Valore equivalente della resistenza
int valoreVoltaggio;                	// Valore volgaggio letto

int CS_signal = 2;                      // Chip Select signal onsul pin 2 of Arduino
int CLK_signal = 4;                     // Clock signal on pin 4 of Arduino
int MOSI_signal = 5;                    // MOSI signal on pin 5 of Arduino

byte cmd_byte2 = B00010001 ;            // Command byte
int initial_value = 100;                // Setting up the initial value



void initialize() 
{                       
  InState = 0;
  valoreResistenza = 0;
  valoreVoltaggio = 0;
  
  // invia il valore inziale per avviare la comunicazione
  spi_out(CS_signal, cmd_byte2, initial_value);
}

void spi_out(int CS, byte cmd_byte, byte data_byte)
{                        
    // we need this function to send command byte and data byte to the chip    
    // to start the transmission, the chip select must be low
    digitalWrite (CS, LOW);                                                 
    // invio il COMMAND BYTE
    spi_transfer(cmd_byte);     
    delay(2);
    // invio il DATA BYTE
    spi_transfer(data_byte); 
    delay(2);
    // to stop the transmission, the chip select must be high
    digitalWrite(CS, HIGH);                                                 
}

void spi_transfer(byte working) 
{
    for(int i = 1; i <= 8; i++) 
    {                                           
      // Set up a loop of 8 iterations (8 bits in a byte)
      if (working > 127) 
      { 
        // If the MSB is a 1 then set MOSI high
        digitalWrite (MOSI_signal,HIGH) ;                                    
      } 
      else 
      { 
        // If the MSB is a 0 then set MOSI low                                           
        digitalWrite (MOSI_signal, LOW) ;                 
      }                                                                       

      // Pulse the CLK_signal high
      digitalWrite (CLK_signal,HIGH) ;
      // Bit-shift the working byte
      working = working << 1 ;                                                
      // Pulse the CLK_signal low
      digitalWrite(CLK_signal,LOW) ;                                          
    }
}

void setup() 
{
    // Attivo la comunicazione con il componente
    pinMode (CS_signal, OUTPUT);
    pinMode (CLK_signal, OUTPUT);
    pinMode (MOSI_signal, OUTPUT);

    initialize();

    Serial.begin(9600);                                                     
    Serial.println("Pronta!");
}

void loop() 
{

    String appoggio = "";
    
    // Leggo il valore dal logo
    InState = analogRead( A1 );

    // in millivolt
    valoreVoltaggio = map(InState, 0, 1023, 0, 5000);
    appoggio = word(valoreVoltaggio);
    Serial.println( "Voltaggio letto: " + appoggio + " mV");
    // float voltage = analogRead( A0 ) * (5.0 / 1023.0);

    // ricavo la resistenza equivalente 
    valoreResistenza = map(valoreVoltaggio, 0, 5000, 0, 128 );
    appoggio = word( map( valoreResistenza, 0, 128, 0, 5000 ) );
    Serial.println( "Livello resistenza equivalente: " + appoggio + " ohm");
    
    //valoreVoltaggio = 2500;
    valoreResistenza = map(valoreVoltaggio, 0, 5000, 0, 128 );
    
    // setto il valore della resistenza
    spi_out(CS_signal, cmd_byte2, valoreResistenza);
    delay(1000);         
}
