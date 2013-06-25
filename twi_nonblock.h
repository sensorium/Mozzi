/*
 * twi_nonblock.h
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
 
#ifndef TWI_NONBLOCK_H_
#define TWI_NONBLOCK_H_

#include "Arduino.h"

#include <compat/twi.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

// --- twi reading variables
  #ifndef TWI_FREQ
  #define TWI_FREQ 100000L
  #endif

  #ifndef TWI_BUFFER_LENGTH
  #define TWI_BUFFER_LENGTH 32
  #endif

  #define TWI_READY 0
  #define TWI_PRE_MRX   1
  #define TWI_MRX   2
  #define TWI_PRE_MTX   3
  #define TWI_MTX   4
  #define TWI_SRX   5
  #define TWI_STX   6

static volatile uint8_t twi_state;
static volatile uint8_t twi_oldstate;
// static uint8_t twiint_masrw;
static uint8_t twi_slarw;

static uint8_t twi_masterBuffer[TWI_BUFFER_LENGTH];
static volatile uint8_t twi_masterBufferIndex;
static uint8_t twi_masterBufferLength;

static volatile uint8_t twi_error;

#define BUFFER_LENGTH 32
static uint8_t rxBuffer[BUFFER_LENGTH];
static uint8_t rxBufferIndex = 0;
static uint8_t rxBufferLength = 0;

static uint8_t txAddress = 0;
static uint8_t txBuffer[BUFFER_LENGTH];
static uint8_t txBufferIndex = 0;
static uint8_t txBufferLength = 0;

static uint8_t transmitting;


void initialize_twi_nonblock();

uint8_t twowire_requestFrom(uint8_t address, uint8_t quantity);
void twowire_beginTransmission( uint8_t address );
void twowire_send( uint8_t data );
uint8_t twowire_endTransmission(void);

/// non-blocking functions:
uint8_t twi_initiateReadFrom(uint8_t address, uint8_t length);
void twi_continueReadFrom();

uint8_t twi_readMasterBuffer( uint8_t* data, uint8_t length );

uint8_t twi_initiateWriteTo(uint8_t address, uint8_t* data, uint8_t length );
void twi_continueWriteTo();


void twi_reply(uint8_t ack);
void twi_stop(void);
void twi_releaseBus(void);

/// blocking versions:
uint8_t twi_readFromBlocking(uint8_t address, uint8_t* data, uint8_t length);
uint8_t twi_writeToBlocking(uint8_t address, uint8_t* data, uint8_t length, uint8_t wait);


#endif