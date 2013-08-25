/*  Example reading an ADXL345 accelerometer
    using I2C communication without blocking audio synthesis,
    using Mozzi sonification library.
  
    Demonstrates use of twi_nonblock functions
    to replace processor-blocking Wire methods.
  
    Circuit: Audio output on digital pin 9.
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Marije Baalman 2012.
    Small modifications by TIm Barrass to retain Mozzi compatibility.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <Ead.h> // exponential attack decay
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <twi_nonblock.h>

#define CONTROL_RATE 128 // powers of 2 please

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain;


#define ADXL345_DEVICE (0x53)    // ADXL345 device address

#define ADXL345_POWER_CTL 0x2d
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0 0x32

#define ADXL345_THRESH_INACT 0x25
#define ADXL345_TIME_INACT 0x26
#define ADXL345_ACT_INACT_CTL 0x27


static volatile uint8_t acc_status = 0;
#define ACC_IDLE 0
#define ACC_READING 1
#define ACC_WRITING 2

uint8_t accbytedata[6];

void setup_accelero(){
  initialize_twi_nonblock();

  acc_writeTo(ADXL345_POWER_CTL, 0);  // all registers off
  acc_writeTo(ADXL345_POWER_CTL, 16); // auto sleep on

  //   ADXL345_DATA_FORMAT : (self test)(spi)(int_invert)(0)(fullres)(justify)(range..)
  //   11 //= B00001011
  acc_writeTo( ADXL345_DATA_FORMAT, 11 );

  // set the threshold for inactivity
  //   acc_writeTo( ADXL345_THRESH_INACT, inactivityThreshold );
  //   acc_writeTo( ADXL345_TIME_INACT, inactivityTime );
  // 0B10001111
  //   acc_writeTo( ADXL345_ACT_INACT_CTL, B10001111 ); 
  //   acc_writeTo(ADXL345_POWER_CTL, B00111000); 

  acc_writeTo(ADXL345_POWER_CTL, 8);  // auto sleep off, measure on

  acc_status = ACC_IDLE;
}

/// ---------- non-blocking version ----------
void initiate_read_accelero(){
  // Reads num bytes starting from address register on device in to _buff array
  // indicate that we are transmitting
  //   transmitting = 1;
  // set address of targeted slave
  txAddress = ADXL345_DEVICE;
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;  

  // put byte in tx buffer
  txBuffer[txBufferIndex] = ADXL345_DATAX0;
  ++txBufferIndex;
  // update amount in buffer   
  txBufferLength = txBufferIndex;

  twi_initiateWriteTo(txAddress, txBuffer, txBufferLength);
  acc_status = ACC_WRITING;
}

void initiate_request_accelero(){
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
  // indicate that we are done transmitting
  //   transmitting = 0;

  uint8_t read = twi_initiateReadFrom(ADXL345_DEVICE, 6);
  acc_status = ACC_READING;
}

void finalise_request_accelero(){
  uint8_t read = twi_readMasterBuffer( rxBuffer, 6 );
  // set rx buffer iterator vars
  rxBufferIndex = 0;
  rxBufferLength = read;

  uint8_t i = 0;
  while( rxBufferLength - rxBufferIndex > 0)         // device may send less than requested (abnormal)
  { 
    accbytedata[i] = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
    i++;
  }

  acc_status = ACC_IDLE;
}
/// ----end------ non-blocking version ----------

// Writes val to address register on device
void acc_writeTo(byte address, byte val) {
  //   Wire.beginTransmission(ADXL345_DEVICE); // start transmission to device   
  twowire_beginTransmission(ADXL345_DEVICE); // start transmission to device   
  //   Wire.send(address);             // send register address
  twowire_send( address );
  //   Wire.send(val);                 // send value to write
  twowire_send( val );  
  //   Wire.endTransmission();         // end transmission
  twowire_endTransmission();
}


void setup(){
  setup_accelero();
  startMozzi(CONTROL_RATE);
  aSin.setFreq(800);
  int attack = 30;
  int decay = 500;
  kEnvelope.set(attack,decay);
}


int accx;
int accy;
int accz;

int envgain;

void updateControl(){  
  envgain = (int) kEnvelope.next();
  if ( envgain < 5 ){
    if ( accx > 512){
      aSin.setFreq( 400 );
      kEnvelope.start();
    } 
    else if ( accy > 512 ) {
      aSin.setFreq( 800 );
      kEnvelope.start();
    } 
    else if ( accz > 512 ) {
      aSin.setFreq( 1200 );
      kEnvelope.start();
    }
  }

  switch( acc_status ){
  case ACC_IDLE:
    accx = (int) (accbytedata[1]*256 + accbytedata[0]); // accelerometer x reading
    accy = (int) (accbytedata[3]*256 + accbytedata[2]); // accelerometer y reading
    accz = (int) (accbytedata[5]*256 + accbytedata[4]); // accelerometer z reading
    initiate_read_accelero();
    break;
  case ACC_WRITING:
    if ( TWI_MTX != twi_state ){
      initiate_request_accelero();
    }
    break;
  case ACC_READING:
    if ( TWI_MRX != twi_state ){
      finalise_request_accelero();
    }
    break;
  }
  gain = envgain;
}


int updateAudio(){
  return (aSin.next()*gain)>>8;
}


void loop(){
  audioHook(); // required here
}

