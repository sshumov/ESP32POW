#ifndef _TOP323_
#define _TOP323_

#define     TH1                   32
#define     TH2                   39
#define     TH3                   36
#define     R1                    04
#define     R2                    02
#define     R3                    17
#define     R4                    16
#define     R5                    12
#define     R6                    13
#define     CP                    27             // Номер GPIO ESP32 використовуємий для зміни стану реле (тригер)
#define     CS_1                  05

#define     READ                  0b00000001     //This value tells  that data is to be read from the requested register.
#define     WRITE                 0b00000000     //This value tells  that data is to be written to the requested register.

#define Gain      0xE60F // (R/W) Default: 0x0000, Unsigned, PGA gains at ADC inputs. (16 bit)
#define Run       0xE228 // (R/W) Default: 0x0000, Unsigned, The run register starts and stops the DSP (see the Digital Signal Processor section). (16 bit)

#define AIRMS     0x43C0 // (R), Default: N/A, Signed, Phase A current rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define AVRMS     0x43C1 // (R), Default: N/A, Signed, Phase A voltage rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define BIRMS     0x43C2 // (R), Default: N/A, Signed, Phase B current rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define BVRMS     0x43C3 // (R), Default: N/A, Signed, Phase B voltage rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define CIRMS     0x43C4 // (R), Default: N/A, Signed, Phase C current rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define CVRMS     0x43C5 // (R), Default: N/A, Signed, Phase C voltage rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define AWATT     0xE513 // (R), Default: N/A, Signed, (24 bit, 32 SE - sign extended to 32 bit), Instantaneous value of Phase A total active power.
#define BWATT     0xE514 // (R), Default: N/A, Signed, (24 bit, 32 SE - sign extended to 32 bit), Instantaneous value of Phase B total active power.
#define CWATT     0xE515 // (R), Default: N/A, Signed, (24 bit, 32 SE - sign extended to 32 bit), Instantaneous value of Phase C total active power.

#endif
void init_top323(lua_State *L);


