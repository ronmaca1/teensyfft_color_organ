#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define AUDIO       A9
#define RAMPRST     6
#define DAC1SEL     7
#define DAC2SEL     8
#define LDACALL     14
// no buffer on vref, gain of 1, shdn disabled, dac a gives us the constant below
#define CONFIG4902A 0x70
// no buffer on vref, gain of 2, shdn disabled, dac a gives us the constant below
//#define CONFIG4902A 0x50
// no buffer on vref, gain of 1, shdn disabled, dac b gives us the constant below
#define CONFIG4902B 0xF0
// no buffer on vref, gain of 1, shdn disabled, dac b gives us the constant below
//#define CONFIG4902B 0xD0


#define _DEBUG_
//#undef _DEBUG_


uint8_t band1,band2,band3,band4;
uint8_t dacoutH,dacoutL;

// this setup changes AREF to internal 1.2v bandgap reference
// GUItool: begin automatically generated code
AudioInputAnalog         adc1(AUDIO);
AudioAnalyzeFFT256       fft256_1;
AudioConnection          patchCord1(adc1, fft256_1);
// GUItool: end automatically generated code



void setup() {
#ifdef _DEBUG_
    Serial1.begin(115200);
    Serial1.print("TEST");
#endif

    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.begin();
    AudioMemory(8);
    pinMode(RAMPRST,OUTPUT);
    digitalWrite(RAMPRST, HIGH);
    pinMode(DAC1SEL,OUTPUT);
    digitalWrite(DAC1SEL,HIGH);
    pinMode(DAC2SEL,OUTPUT);
    digitalWrite(DAC2SEL,HIGH);
    pinMode(LDACALL,OUTPUT);
    digitalWrite(LDACALL,HIGH);

    //pinMode(13,OUTPUT);
    //digitalWrite(13,LOW);
    pinMode(AUDIO,INPUT);
}

void loop() {
    // put your main code here, to run repeatedly:
    //digitalWrite(13, !(digitalRead(13)));
    if (fft256_1.available()) {

        band1=(uint8_t) (fft256_1.read(0,7)*256);
        band2=(uint8_t) (fft256_1.read(8,23)*256);
        band3=(uint8_t) (fft256_1.read(24,55)*256);
        band4=(uint8_t) (fft256_1.read(56,127)*256);
#ifdef _DEBUG_
        Serial1.print("\n\r");
        Serial1.print(band1);
        Serial1.print(',');
        Serial1.print(band2);
        Serial1.print(',');
        Serial1.print(band3);
        Serial1.print(',');
        Serial1.print(band4);
#endif
        // band 1, DAC1A
        dacoutH = CONFIG4902A | ((band1>>4) & 0x0F);
        dacoutL = (band1<<4) & 0xF0;
        digitalWrite(DAC1SEL, LOW);
        delayMicroseconds(5);            // let the DAC get ready
        SPI.transfer(dacoutH);
        SPI.transfer(dacoutL);
        delayMicroseconds(5);
        digitalWrite(DAC1SEL, HIGH);

        // wait a little before the B dac
        delayMicroseconds(5);

        // band 2, DAC1B
        dacoutH = CONFIG4902B | ((band2>>4) & 0x0F);
        dacoutL = (band2<<4) & 0xF0;
        digitalWrite(DAC1SEL, LOW);
        delayMicroseconds(5);
        SPI.transfer(dacoutH);
        SPI.transfer(dacoutL);
        delayMicroseconds(5);            // let the DAC settle
        digitalWrite(DAC1SEL, HIGH);
        // done with dac1 A and B


        // band 3, DAC2A
        dacoutH = CONFIG4902A | ((band3>>4) & 0x0f);
        dacoutL = (band3<<4) & 0xF0;
        digitalWrite(DAC2SEL, LOW);
        delayMicroseconds(5);            // let the DAC get ready
        SPI.transfer(dacoutH);
        SPI.transfer(dacoutL);
        delayMicroseconds(5);
        digitalWrite(DAC2SEL, HIGH);

        // wait a little before the B dac
        delayMicroseconds(5);

        // band 4 DAC2B
        dacoutH = CONFIG4902B | ((band4>>4) & 0x0f);
        dacoutL = (band4<<4) & 0xF0;
        digitalWrite(DAC2SEL, LOW);
        delayMicroseconds(5);
        SPI.transfer(dacoutH);
        SPI.transfer(dacoutL);
        delayMicroseconds(5);            // let the DAC settle
        digitalWrite(DAC2SEL, HIGH);
        //done with dac2 A and B

        // wait a little then
        // latch all dac data to outputs
        delayMicroseconds(5);
        digitalWrite(LDACALL,LOW);
        delayMicroseconds(1);
        digitalWrite(LDACALL,HIGH);
    }
    digitalWrite(RAMPRST,LOW);
    delayMicroseconds(100);
    digitalWrite(RAMPRST,HIGH);
    delay(8); // about 120 hz for testing, replace with zero cross detection
}
