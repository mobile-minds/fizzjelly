#include <MM_PinInterrupts.h>

void Interrupt_IO6()
{
  digitalWrite(STATUSLED, 0);
  Serial.println("IO6 Triggered");
}

void Interrupt_IO7()
{
  digitalWrite(STATUSLED, 1);
  Serial.println("IO7 Triggered");
}

void Interrupt_Expansion0()
{
  Serial.println("Expansion 0 Triggered");
}

void Interrupt_Expansion1()
{
  Serial.println("Expansion 1 Triggered");
}

void Interrupt_Expansion2()
{
  Serial.println("Expansion 2 Triggered");
}


void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  while (!Serial);
  
  Serial.println("FizzJelly Interrupt Example");
  
  pinMode(STATUSLED, OUTPUT);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  digitalWrite(STATUSLED, 0);
  
  PinInterrupts.attach(6, Interrupt_IO6);
  PinInterrupts.attach(7, Interrupt_IO7);
  PinInterrupts.attach(10, Interrupt_Expansion0);
  PinInterrupts.attach(11, Interrupt_Expansion1);
  PinInterrupts.attach(12, Interrupt_Expansion2);
  
}

void loop() 
{
  // put your main code here, to run repeatedly: 
  
}
