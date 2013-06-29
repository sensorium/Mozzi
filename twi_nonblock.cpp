/*
 * twi_nonblock.cpp
 *
 * Copyright 2012 Marije Baalman.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "twi_nonblock.h"

#include <avr/interrupt.h>

uint8_t twi_writeAddress;
uint8_t * twi_writeData;
uint8_t twi_writeLength;

uint8_t twi_readAddress;
// uint8_t * twi_writeData;
uint8_t twi_readLength;

/* 
 * Function twi_init
 * Desc     readys twi pins and sets twi bitrate
 * Input    none
 * Output   none
 */
void initialize_twi_nonblock(){
  rxBufferIndex = 0;
  rxBufferLength = 0;

  txBufferIndex = 0;
  txBufferLength = 0;  

  // initialize state
  twi_state = TWI_READY;

  #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
    // activate internal pull-ups for twi
    // as per note from atmega8 manual pg167
    sbi(PORTC, 4);
    sbi(PORTC, 5);
  #else
    // activate internal pull-ups for twi
    // as per note from atmega128 manual pg204
    sbi(PORTD, 0);
    sbi(PORTD, 1);
  #endif

  // initialize twi prescaler and bit rate
  cbi(TWSR, TWPS0);
  cbi(TWSR, TWPS1);
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;

  /* twi bit rate formula from atmega128 manual pg 204
  SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
  note: TWBR should be 10 or higher for master mode
  It is 72 for a 16mhz Wiring board with 100kHz TWI */

  // enable twi module, acks, and twi interrupt
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
}


uint8_t twowire_requestFromBlocking(uint8_t address, uint8_t quantity)
{
  // clamp to buffer length
  if(quantity > BUFFER_LENGTH){
    quantity = BUFFER_LENGTH;
  }
  // perform blocking read into buffer
  uint8_t read = twi_readFromBlocking(address, rxBuffer, quantity);
  // set rx buffer iterator vars
  rxBufferIndex = 0;
  rxBufferLength = read;

  return read;
}

void twowire_beginTransmission( uint8_t address ){
  // indicate that we are transmitting
  transmitting = 1;
  // set address of targeted slave
  txAddress = address;
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;  
}

void twowire_send( uint8_t data ){
  if(transmitting){
  // in master transmitter mode
    // don't bother if buffer is full
    if(txBufferLength >= BUFFER_LENGTH){
      return;
    }
    // put byte in tx buffer
    txBuffer[txBufferIndex] = data;
    ++txBufferIndex;
    // update amount in buffer   
    txBufferLength = txBufferIndex;
  }    
}

uint8_t twowire_endTransmission(void)
{
  // transmit buffer (blocking)
  int8_t ret = twi_writeToBlocking(txAddress, txBuffer, txBufferLength, 1);
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
  // indicate that we are done transmitting
  transmitting = 0;
  return ret;
}

/* 
 * Function twi_readFrom
 * Desc     attempts to become twi bus master and read a
 *          series of bytes from a device on the bus
 * Input    address: 7bit i2c device address
 *          data: pointer to byte array
 *          length: number of bytes to read into array
 * Output   number of bytes read
 */
/// TODO: make non-blocking
uint8_t twi_readFromBlocking(uint8_t address, uint8_t* data, uint8_t length)
{
  uint8_t i;

  // ensure data will fit into buffer
  if(TWI_BUFFER_LENGTH < length){
    return 0;
  }

  // wait until twi is ready, become master receiver
  while(TWI_READY != twi_state){
    continue;
  }
  
  twi_state = TWI_MRX;
  // reset error state (0xFF.. no error occured)
  twi_error = 0xFF;

  // initialize buffer iteration vars
  twi_masterBufferIndex = 0;
  twi_masterBufferLength = length-1;  // This is not intuitive, read on...
  // On receive, the previously configured ACK/NACK setting is transmitted in
  // response to the received byte before the interrupt is signalled. 
  // Therefor we must actually set NACK when the _next_ to last byte is
  // received, causing that NACK to be sent in response to receiving the last
  // expected byte of data.

  // build sla+w, slave device address + w bit
  twi_slarw = TW_READ;
  twi_slarw |= address << 1;

  // send start condition
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA);

  // wait for read operation to complete
  while(TWI_MRX == twi_state){
    continue;
  }

  if (twi_masterBufferIndex < length)
    length = twi_masterBufferIndex;

  // copy twi buffer to data
  for(i = 0; i < length; ++i){
    data[i] = twi_masterBuffer[i];
  }
	
  return length;
}



/// ---------- non-blocking version ----------


uint8_t twi_initiateReadFrom(uint8_t address, uint8_t length)
{

  // ensure data will fit into buffer
  if(TWI_BUFFER_LENGTH < length){
    return 0;
  }
  
  twi_readLength = length;
  twi_readAddress = address;
  
  if ( TWI_READY == twi_state ){
      twi_continueReadFrom();
  } else {
    twi_state = TWI_PRE_MRX;
  }
  if (twi_error == 0xFF)
    return 0;	// success
  else if (twi_error == TW_MT_SLA_NACK)
    return 2;	// error: address send, nack received
  else if (twi_error == TW_MT_DATA_NACK)
    return 3;	// error: data send, nack received
  else
    return 4;	// other twi error
}



void twi_continueReadFrom(){
    
  twi_state = TWI_MRX;
  // reset error state (0xFF.. no error occured)
  twi_error = 0xFF;

  // initialize buffer iteration vars
  twi_masterBufferIndex = 0;
  twi_masterBufferLength = twi_readLength-1;  // This is not intuitive, read on...
  // On receive, the previously configured ACK/NACK setting is transmitted in
  // response to the received byte before the interrupt is signalled. 
  // Therefor we must actually set NACK when the _next_ to last byte is
  // received, causing that NACK to be sent in response to receiving the last
  // expected byte of data.

  // build sla+w, slave device address + w bit
  twi_slarw = TW_READ;
  twi_slarw |= twi_readAddress << 1;

  // send start condition
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA);
}



uint8_t twi_readMasterBuffer( uint8_t* data, uint8_t length ){
  uint8_t i;
  if (twi_masterBufferIndex < length)
    length = twi_masterBufferIndex;

  // copy twi buffer to data
  for(i = 0; i < length; ++i){
    data[i] = twi_masterBuffer[i];
  }
	
  return length;
}



/// ----end------ non-blocking version ----------


/* 
 * Function twi_writeTo
 * Desc     attempts to become twi bus master and write a
 *          series of bytes to a device on the bus
 * Input    address: 7bit i2c device address
 *          data: pointer to byte array
 *          length: number of bytes in array
 *          wait: boolean indicating to wait for write or not
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 */
/// TODO: make non-blocking
uint8_t twi_writeToBlocking(uint8_t address, uint8_t* data, uint8_t length, uint8_t wait)
{
  uint8_t i;

  // ensure data will fit into buffer
  if(TWI_BUFFER_LENGTH < length){
    return 1;
  }

  // wait until twi is ready, become master transmitter
  while(TWI_READY != twi_state){
    continue;
  }

  twi_state = TWI_MTX;
  // reset error state (0xFF.. no error occured)
  twi_error = 0xFF;

  // initialize buffer iteration vars
  twi_masterBufferIndex = 0;
  twi_masterBufferLength = length;
  
  // copy data to twi buffer
  for(i = 0; i < length; ++i){
    twi_masterBuffer[i] = data[i];
  }
  
  // build sla+w, slave device address + w bit
  twi_slarw = TW_WRITE;
  twi_slarw |= address << 1;
  
  // send start condition
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA);

  // wait for write operation to complete
  while(wait && (TWI_MTX == twi_state)){
    continue;
  }
  
  if (twi_error == 0xFF)
    return 0;	// success
  else if (twi_error == TW_MT_SLA_NACK)
    return 2;	// error: address send, nack received
  else if (twi_error == TW_MT_DATA_NACK)
    return 3;	// error: data send, nack received
  else
    return 4;	// other twi error
}



/// ----------------- non-blocking ---------


uint8_t twi_initiateWriteTo(uint8_t address, uint8_t* data, uint8_t length )
{
  // ensure data will fit into buffer
  if(TWI_BUFFER_LENGTH < length){
    return 1;
  }
  twi_writeAddress = address;
  twi_writeData = data;
  twi_writeLength = length;

  if ( TWI_READY == twi_state ){
      twi_continueWriteTo();
  } else {
    twi_state = TWI_PRE_MTX;
  }
  if (twi_error == 0xFF)
    return 0;	// success
  else if (twi_error == TW_MT_SLA_NACK)
    return 2;	// error: address send, nack received
  else if (twi_error == TW_MT_DATA_NACK)
    return 3;	// error: data send, nack received
  else
    return 4;	// other twi error
}



void twi_continueWriteTo(){
  uint8_t i;
  // wait until twi is ready, become master transmitter
//   while(TWI_READY != twi_state){
//     continue;
//   }

  twi_state = TWI_MTX;
  // reset error state (0xFF.. no error occured)
  twi_error = 0xFF;

  // initialize buffer iteration vars
  twi_masterBufferIndex = 0;
  twi_masterBufferLength = twi_writeLength;
  
  // copy data to twi buffer
  for(i = 0; i < twi_writeLength; ++i){
    twi_masterBuffer[i] = twi_writeData[i];
  }
  
  // build sla+w, slave device address + w bit
  twi_slarw = TW_WRITE;
  twi_slarw |= twi_writeAddress << 1;
  
  // send start condition
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTA);
}


// -----------end non-blocking --------------------


/* 
 * Function twi_reply
 * Desc     sends byte or readys receive line
 * Input    ack: byte indicating to ack or to nack
 * Output   none
 */
void twi_reply(uint8_t ack)
{
  // transmit master read ready signal, with or without ack
  if(ack){
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
  }else{
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
  }
}



/* 
 * Function twi_stop
 * Desc     relinquishes bus master status
 * Input    none
 * Output   none
 */
void twi_stop(void)
{
  // send stop condition
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);

  // wait for stop condition to be exectued on bus
  // TWINT is not set after a stop condition!
  while(TWCR & _BV(TWSTO)){ //FIXME: does this cause a delay?
    continue;
  }

  twi_oldstate = twi_state;
  // update twi state
  twi_state = TWI_READY;
  if ( twi_oldstate == TWI_PRE_MTX ){
      twi_continueWriteTo();
  } else if ( twi_oldstate == TWI_PRE_MRX ){
      twi_continueReadFrom();
  }
}



/* 
 * Function twi_releaseBus
 * Desc     releases bus control
 * Input    none
 * Output   none
 */
void twi_releaseBus(void)
{
  // release bus
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);

  twi_oldstate = twi_state;
  // update twi state
  twi_state = TWI_READY;
  if ( twi_oldstate == TWI_PRE_MTX ){
      twi_continueWriteTo();
  } else if ( twi_oldstate == TWI_PRE_MRX ){
      twi_continueReadFrom();
  }
}

// SIGNAL(TWI_vect)
// ISR(TWI_vect, ISR_NOBLOCK )
ISR(TWI_vect)
{
  switch(TW_STATUS){
    // All Master
    case TW_START:     // sent start condition
    case TW_REP_START: // sent repeated start condition
      // copy device address and r/w bit to output register and ack
      TWDR = twi_slarw;
      twi_reply(1);
      break;

    // Master Transmitter
    case TW_MT_SLA_ACK:  // slave receiver acked address
    case TW_MT_DATA_ACK: // slave receiver acked data
      // if there is data to send, send it, otherwise stop 
      if(twi_masterBufferIndex < twi_masterBufferLength){
        // copy data to output register and ack
        TWDR = twi_masterBuffer[twi_masterBufferIndex++];
        twi_reply(1);
      }else{
        twi_stop();
      }
      break;
    case TW_MT_SLA_NACK:  // address sent, nack received
      twi_error = TW_MT_SLA_NACK;
      twi_stop();
      break;
    case TW_MT_DATA_NACK: // data sent, nack received
      twi_error = TW_MT_DATA_NACK;
      twi_stop();
      break;
    case TW_MT_ARB_LOST: // lost bus arbitration
      twi_error = TW_MT_ARB_LOST;
      twi_releaseBus();
      break;

    // Master Receiver
    case TW_MR_DATA_ACK: // data received, ack sent
      // put byte into buffer
      twi_masterBuffer[twi_masterBufferIndex++] = TWDR;
    case TW_MR_SLA_ACK:  // address sent, ack received
      // ack if more bytes are expected, otherwise nack
      if(twi_masterBufferIndex < twi_masterBufferLength){
        twi_reply(1);
      }else{
        twi_reply(0);
      }
      break;
    case TW_MR_DATA_NACK: // data received, nack sent
      // put final byte into buffer
      twi_masterBuffer[twi_masterBufferIndex++] = TWDR;
    case TW_MR_SLA_NACK: // address sent, nack received
      twi_stop();
      break;
    // TW_MR_ARB_LOST handled by TW_MT_ARB_LOST case

//     // Slave Receiver
//     case TW_SR_SLA_ACK:   // addressed, returned ack
//     case TW_SR_GCALL_ACK: // addressed generally, returned ack
//     case TW_SR_ARB_LOST_SLA_ACK:   // lost arbitration, returned ack
//     case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration, returned ack
//       // enter slave receiver mode
//       twi_state = TWI_SRX;
//       // indicate that rx buffer can be overwritten and ack
//       twi_rxBufferIndex = 0;
//       twi_reply(1);
//       break;
//     case TW_SR_DATA_ACK:       // data received, returned ack
//     case TW_SR_GCALL_DATA_ACK: // data received generally, returned ack
//       // if there is still room in the rx buffer
//       if(twi_rxBufferIndex < TWI_BUFFER_LENGTH){
//         // put byte in buffer and ack
//         twi_rxBuffer[twi_rxBufferIndex++] = TWDR;
//         twi_reply(1);
//       }else{
//         // otherwise nack
//         twi_reply(0);
//       }
//       break;
//     case TW_SR_STOP: // stop or repeated start condition received
//       // put a null char after data if there's room
//       if(twi_rxBufferIndex < TWI_BUFFER_LENGTH){
//         twi_rxBuffer[twi_rxBufferIndex] = '\0';
//       }
//       // sends ack and stops interface for clock stretching
//       twi_stop();
//       // callback to user defined callback
//       twi_onSlaveReceive(twi_rxBuffer, twi_rxBufferIndex);
//       // since we submit rx buffer to "wire" library, we can reset it
//       twi_rxBufferIndex = 0;
//       // ack future responses and leave slave receiver state
//       twi_releaseBus();
//       break;
//     case TW_SR_DATA_NACK:       // data received, returned nack
//     case TW_SR_GCALL_DATA_NACK: // data received generally, returned nack
//       // nack back at master
//       twi_reply(0);
//       break;
//     
//     // Slave Transmitter
//     case TW_ST_SLA_ACK:          // addressed, returned ack
//     case TW_ST_ARB_LOST_SLA_ACK: // arbitration lost, returned ack
//       // enter slave transmitter mode
//       twi_state = TWI_STX;
//       // ready the tx buffer index for iteration
//       twi_txBufferIndex = 0;
//       // set tx buffer length to be zero, to verify if user changes it
//       twi_txBufferLength = 0;
//       // request for txBuffer to be filled and length to be set
//       // note: user must call twi_transmit(bytes, length) to do this
//       twi_onSlaveTransmit();
//       // if they didn't change buffer & length, initialize it
//       if(0 == twi_txBufferLength){
//         twi_txBufferLength = 1;
//         twi_txBuffer[0] = 0x00;
//       }
//       // transmit first byte from buffer, fall
//     case TW_ST_DATA_ACK: // byte sent, ack returned
//       // copy data to output register
//       TWDR = twi_txBuffer[twi_txBufferIndex++];
//       // if there is more to send, ack, otherwise nack
//       if(twi_txBufferIndex < twi_txBufferLength){
//         twi_reply(1);
//       }else{
//         twi_reply(0);
//       }
//       break;
//     case TW_ST_DATA_NACK: // received nack, we are done 
//     case TW_ST_LAST_DATA: // received ack, but we are done already!
//       // ack future responses
//       twi_reply(1);
//       // leave slave receiver state
//       twi_state = TWI_READY;
//       break;

    // All
    case TW_NO_INFO:   // no state information
      break;
    case TW_BUS_ERROR: // bus error, illegal stop/start
      twi_error = TW_BUS_ERROR;
      twi_stop();
      break;
  }
}
