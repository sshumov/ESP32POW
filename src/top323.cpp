#include "main.h"
#include "lua.h"
#include <SPI.h>
#include "top323.h"

#define SPI_freq  500000                //Communicate with the ADC at 5 MHz frequency.
static float pow_tr_correct;

void enableChip(void){
  digitalWrite(CS_1, LOW);                                                                           //Enable data transfer by bringing CS line LOW
} 

void disableChip(void){
    digitalWrite(CS_1, HIGH);                                                                        //End data transfer by bringing CS line HIGH
}

uint8_t functionBitVal(uint32_t val, uint8_t byteVal){ //Returns as integer an address of a specified byte - basically a byte controlled shift register with "byteVal" controlling the byte that is read and returned
  
  uint32_t x = ((val >> (8*byteVal)) & 0xff);
  return x;
}

void write_16(uint16_t adr, uint16_t data) { 
  uint8_t  bw[2];                                                     // Це два байти для запису
  uint8_t msb = functionBitVal(adr,1);                                // Розбиваємо адресу і беремо старший байт
  uint8_t lsb = functionBitVal(adr,0);                                // Розбиваємо адресу і беремо молодший байт
  for(int8_t i = 1; i >= 0; --i){                                     // Розбиваэмо слово і записуємо окремий байт
      bw[i] = functionBitVal(data,i);                                 //MSB Byte 1  
  }
  enableChip();                                                       //Enable data transfer by bringing CS line LOW
  SPI.beginTransaction(SPISettings(SPI_freq, MSBFIRST, SPI_MODE3));   //Begin SPI transfer with most significant byte (MSB) first. Clock is high when inactive. Read at rising edge: SPIMODE3.
  SPI.transfer(WRITE);                                                //Send command to begin writing
  SPI.transfer(msb);                                                  //Pass in MSB of register to be read first.
  SPI.transfer(lsb);                                                  //Pass in LSB of register to be read next.
  for(int8_t i = 1; i >= 0; --i){                                     //Write values sequentially and bitshift for a 16 bit entry
     SPI.transfer(bw[i]);                                             //MSB Byte 1  
  }
  SPI.endTransaction();
  disableChip();                                                      //End data transfer by bringing CS line HIGH
}  

uint16_t read_16(uint16_t adr) {                                                                      //This is the algorithm that reads from a 16 bit register. The arguments are the address of the register respectively. The values of the arguments are obtained from the list of functions above.
  uint8_t  b[2];                                                                                      // Це два байти для зчитування
  enableChip();                                                                                       //Enable data transfer by bringing CS line LOW
  SPI.beginTransaction(SPISettings(SPI_freq, MSBFIRST, SPI_MODE3));                                   //Begin SPI transfer with most significant byte (MSB) first. Clock is high when inactive. Read at rising edge: SPIMODE3.
  SPI.transfer(READ);                                                                                 //Send command to begin readout
  uint8_t msb = functionBitVal(adr,1);                                                                // Розбиваємо адресу і беремо старший байт
  uint8_t lsb = functionBitVal(adr,0);                                                                // Розбиваємо адресу і беремо молодший байт
  SPI.transfer(msb);                                                                                  //Pass in MSB of register to be read first.
  SPI.transfer(lsb);                                                                                  //Pass in LSB of register to be read next.
  
  for(int8_t i = 1; i >= 0; --i){                                                                     //Read in values sequentially and bitshift for a 16 bit entry
      b[i] = SPI.transfer(0x00);                                                                      //MSB Byte 1  (Read in data on dummy write (null MOSI signal))
  }
  SPI.endTransaction();
  disableChip();                                                                                      //End data transfer by bringing CS line HIGH
  return (uint16_t)b[1]<<8 | (uint16_t)b[0];                                                           //Post-read packing and bitshifting operation
}  

uint32_t read_32(uint16_t adr) {                                                                       ///This is the algorithm that reads from a 32 bit register. The arguments are the MSB and LSB of the address of the register respectively. The values of the arguments are obtained from the list of functions above.  Caution, some register elements contain information that is only 24 bit with padding on the MSB
  uint8_t  b[4];                                                                                      // Це чотири байти для зчитування
  enableChip();                                                                                       //Enable data transfer by bringing CS line LOW
  SPI.beginTransaction(SPISettings(SPI_freq, MSBFIRST, SPI_MODE3));                                   //Begin SPI transfer with most significant byte (MSB) first. Clock is high when inactive. Read at rising edge: SPIMODE3.
  SPI.transfer(READ);                                                                                 //Send command to begin readout
  uint8_t msb = functionBitVal(adr,1);                                                                // Розбиваємо адресу і беремо старший байт
  uint8_t lsb = functionBitVal(adr,0);                                                                // Розбиваємо адресу і беремо молодший байт
  SPI.transfer(msb);                                                                                  //Pass in MSB of register to be read first.
  SPI.transfer(lsb);                                                                                  //Pass in LSB of register to be read next.
  
  for(int8_t i = 3; i >= 0; --i){                                                                     //Read in values sequentially and bitshift for a 32 bit entry
      b[i] = SPI.transfer(0x00);                                                                      //MSB Byte 1  (Read in data on dummy write (null MOSI signal))
  }
  SPI.endTransaction();
  disableChip();                                                                                      //End data transfer by bringing CS line HIGH
  return (uint32_t)b[3]<<24 |(uint32_t)b[2]<<16 |(uint32_t)b[1]<<8 | (uint32_t)b[0];                  //Post-read packing and bitshifting operation
}

static int top323_get(lua_State *L) {
    uint32_t vrmsa,vrmsb,vrmsc;
    uint32_t irmsa,irmsb,irmsc;
    uint32_t apowera,apowerb,apowerc;
    uint16_t th1,th2,th3;
    vrmsa = read_32(AVRMS);vrmsb = read_32(BVRMS);vrmsc = read_32(CVRMS);
    irmsa = read_32(AIRMS);irmsb = read_32(BIRMS);irmsc = read_32(CIRMS);
    apowera = read_32(AWATT);apowerb = read_32(BWATT);apowerc = read_32(CWATT);
    th1=analogRead(TH1);th2=analogRead(TH2);th3=analogRead(TH3);
    lua_pushinteger(L,vrmsa);lua_pushinteger(L,vrmsb);lua_pushinteger(L,vrmsc);
    lua_pushinteger(L,irmsa);lua_pushinteger(L,irmsb);lua_pushinteger(L,irmsc);
    lua_pushinteger(L,apowera);lua_pushinteger(L,apowerb);lua_pushinteger(L,apowerc);
    lua_pushinteger(L,th1);lua_pushinteger(L,th2);lua_pushinteger(L,th3);
    return 12;
}

inline float getVrmsA(){  
  
    uint32_t value = read_32(AVRMS);
    float decimal = 0.5 * (float)value * ((8880+1)/1) / 1.0 / sqrt (2) / 4191910.0;
    return decimal;
}

inline float getVrmsB(){  
  
    uint32_t value = read_32(BVRMS);
    float decimal = 0.5 * (float)value * ((8880+1)/1) / 1.0 / sqrt (2) / 4191910.0;
    return decimal;
} 

inline float getVrmsC(){  
  
    uint32_t value = read_32(CVRMS);
    float decimal = 0.5 * (float)value * ((8880+1)/1) / 1.0 / sqrt (2) / 4191910.0;
    return decimal;
} 

inline float getIrmsA(){  
  
    uint32_t value = read_32(AIRMS);
    float decimal = 0.5 * (float)value * 1000/pow_tr_correct  / 1.0 / sqrt (2) / 4191910.0;
    return decimal;
}

inline float getIrmsB(){  
  
    uint32_t value = read_32(BIRMS);
    float decimal = 0.5 * (float)value * 1000/pow_tr_correct / 1.0 / sqrt (2) / 4191910.0;
    return decimal;
}

inline float getIrmsC(){  
  
    uint32_t value = read_32(CIRMS);
    float decimal = 0.5 * (float)value * 1000/pow_tr_correct / 1.0 / sqrt (2) / 4191910.0;
    return decimal;
}

inline float getActivePowerA(){ 

    int32_t value = read_32(AWATT);
    float decimal = (float)value * 1000/pow_tr_correct * ((8880+1)/1) / 1.0 / 1.0 / 8.0 / 2094759.0;  
    return decimal;
}

inline float getActivePowerB(){ 

    int32_t value = read_32(BWATT);
    float decimal = (float)value * 1000/pow_tr_correct * ((8880+1)/1) / 1.0 / 1.0 / 8.0 / 2094759.0;  
    return decimal;
}

inline float getActivePowerC(){ 

    int32_t value = read_32(CWATT);
    float decimal = (float)value * 1000/pow_tr_correct * ((8880+1)/1) / 1.0 / 1.0 / 8.0 / 2094759.0;  
    return decimal;
}

void ADCInit(){
  pinMode(CS_1, OUTPUT); 
  for (uint8_t i = 0; i < 3; i++){         // To use the SPI port, toggle the SS/HSA pin three times from high to low.
      digitalWrite(CS_1, HIGH);
      delay(1);
      digitalWrite(CS_1, LOW);  
      delay(1);
  }
  delay(50);
  SPI.setBitOrder(MSBFIRST);  //Define MSB as first (explicitly)
  for (uint8_t i = 0; i < 3; i++){
      write_16(Gain, 0b0000000000000000);    //Default: 0b0000000000000000
      write_16(Run, 0x0001);
  }
}

static int top323_init(lua_State *L) {
    print_DEBUG("POW module");
    ADCInit();
    return 0;
}

static int top323_RelayOn(lua_State *L){
  uint number = luaL_checkinteger(L,1);
  Serial.print("Relay: ");Serial.print(number);Serial.println(" ON");
  digitalWrite(CP, LOW);delayMicroseconds(100);
  digitalWrite(number, HIGH);delayMicroseconds(100);
  digitalWrite(CP, HIGH);
  return 0;
}

static int top323_RelayOff(lua_State *L){
  uint number = luaL_checkinteger(L,1);
  Serial.print("Relay: ");Serial.print(number);Serial.println(" OFF");
  digitalWrite(CP, LOW);  delayMicroseconds(100);
  digitalWrite(number, LOW);delayMicroseconds(100);
  digitalWrite(CP, HIGH);delayMicroseconds(100);
  digitalWrite(CP, LOW);
  return 0;
}


static const luaL_Reg top323lib[] = {
    {"init", top323_init},
    {"get", top323_get},
    {"relayOn", top323_RelayOn},
    {"relayOff", top323_RelayOff},
    {NULL, NULL}
};


LUAMOD_API int luaopen_top323 (lua_State *L) {
    luaL_newlib(L, top323lib);
    return 1;
}

void init_top323(lua_State *L) {
    pinMode(TH1, INPUT);pinMode(TH2, INPUT);pinMode(TH3, INPUT);

    pinMode(R1, OUTPUT);pinMode(R2, OUTPUT);
    pinMode(R3, OUTPUT);pinMode(R4, OUTPUT);
    pinMode(R5, OUTPUT);pinMode(R6, OUTPUT);pinMode(CP, OUTPUT);
    digitalWrite(R1, LOW);digitalWrite(R2, LOW);
    digitalWrite(R3, LOW);digitalWrite(R4, LOW);
    digitalWrite(R5, LOW);digitalWrite(R6, LOW);digitalWrite(CP, LOW);

    luaL_requiref(L, "top323", luaopen_top323, 1);
    lua_pop(L, 1);
}

