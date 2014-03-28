/*

Copyright (c) 2014, Mobile Minds LTD.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "MM_PinInterrupts.h"

_PinInterrupts PinInterrupts;

pinChangeCB _PinInterrupts::_pinChangeCBs[MM_PININTERRUPTS_MAX_CALLBACKS];
uint8_t _PinInterrupts::_pinStatus;

_PinInterrupts::_PinInterrupts()
{
  uint8_t i;
  
  _PinInterrupts::_pinStatus = PINB;
  
  // Clear all interrupt callbacks
  for (i=0; i<MM_PININTERRUPTS_MAX_CALLBACKS; i++)
    _PinInterrupts::_pinChangeCBs[i] = NULL;
}

void _PinInterrupts::enable()
{
  PCICR |= 0x01;
}

void _PinInterrupts::disable()
{
  PCICR &= ~0x01;
}

int _PinInterrupts::attach(int pin, pinChangeCB callback)
{
  PCICR |= 0x01;
  
  _PinInterrupts::_pinStatus = PINB;
  
  switch (pin)
  {
    case 6:  _PinInterrupts::_pinChangeCBs[0] = callback; PCMSK0 |= 0x40; return 0;
    case 7:  _PinInterrupts::_pinChangeCBs[1] = callback; PCMSK0 |= 0x20; return 0;
    case 10: _PinInterrupts::_pinChangeCBs[2] = callback; PCMSK0 |= 0x02; return 0;
    case 11: _PinInterrupts::_pinChangeCBs[3] = callback; PCMSK0 |= 0x04; return 0;
    case 12: _PinInterrupts::_pinChangeCBs[4] = callback; PCMSK0 |= 0x08; return 0;
    default: return -1;
  }
}

void _PinInterrupts::detach(int pin)
{
  int i;
  
  switch (pin)
  {
    case 6:  _PinInterrupts::_pinChangeCBs[0] = NULL; PCMSK0 &= ~0x40; break;
    case 7:  _PinInterrupts::_pinChangeCBs[1] = NULL; PCMSK0 &= ~0x20; break;
    case 10: _PinInterrupts::_pinChangeCBs[2] = NULL; PCMSK0 &= ~0x02; break;
    case 11: _PinInterrupts::_pinChangeCBs[3] = NULL; PCMSK0 &= ~0x04; break;
    case 12: _PinInterrupts::_pinChangeCBs[4] = NULL; PCMSK0 &= ~0x08; break;
    default: return;
  }
  
  // If no interrupts enabled, then disable the control register
  for (i=0; i<MM_PININTERRUPTS_MAX_CALLBACKS; i++)
  {
    if (_PinInterrupts::_pinChangeCBs[i] != NULL)
      return;
  }
  
  PCICR &= ~0x01;
}

// The Pin Change Interupt vector handler
ISR(PCINT0_vect)
{
  uint8_t oldPinB;
  
  oldPinB = _PinInterrupts::_pinStatus;
  
  if (_PinInterrupts::_pinChangeCBs[0] != NULL)
  {
    if ((oldPinB & 0x40) != (PINB & 0x40))
    {
      _PinInterrupts::_pinChangeCBs[0]();
    }
  }
  if (_PinInterrupts::_pinChangeCBs[1] != NULL)
  {
    if ((oldPinB & 0x20) != (PINB & 0x20))
    {
      _PinInterrupts::_pinChangeCBs[1]();
    }
  }
  if (_PinInterrupts::_pinChangeCBs[2] != NULL)
  {
    if ((oldPinB & 0x02) != (PINB & 0x02))
    {
      _PinInterrupts::_pinChangeCBs[2]();
    }
  }
  if (_PinInterrupts::_pinChangeCBs[3] != NULL)
  {
    if ((oldPinB & 0x04) != (PINB & 0x04))
    {
      _PinInterrupts::_pinChangeCBs[3]();
    }
  }
  if (_PinInterrupts::_pinChangeCBs[4] != NULL)
  {
    if ((oldPinB & 0x08) != (PINB & 0x08))
    {
      _PinInterrupts::_pinChangeCBs[4]();
    }
  }
  
  _PinInterrupts::_pinStatus = PINB;
}
