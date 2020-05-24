; Code for a two LED candle to help explore the
; AVR and assembler, particularly random number
; generation and PWM for the Attiny13A
; A N Peck March 2020
;
.nolist
.include "tn13adef.inc" ; Define device ATtiny13A
.list

; **********************************
;        H A R D W A R E
; **********************************

; Device: ATtiny13A, Package: 8-pin-PDIP_SOIC
;
;           _________
;        1 /         |8
;      o--|RESET  VCC|--o
;      o--|PB3    PB2|--o
;      o--|PB4    PB1|--o
;      o--|GND    PB0|--o
;       4 |__________|5

; **********************************
;  C O N S T A N T S
; **********************************
; Here is where you can "play" with values
; to change the nature of the candle flicker

  .equ SLL = 10          ; lowest slow start
  .equ SLH = 50          ; highest slow start
  .equ SHL = 160         ; lowest slow end
  .equ SHH = 220         ; highest slow end
  .equ FLL = 40          ; lowest fast start
  .equ FLH = 80          ; highest fast start
  .equ FHL = 110         ; lowest fast end
  .equ FHH = 150         ; highest fast end
  .equ seed = 0x29       ; random seed
  .equ xori = 0x8B       ; random eor value
  .equ span = 23         ; count before spanner thrown
  .equ spanum = 3        ; size of spanner

; **********************************
;       R E G I S T E R S
; **********************************

  .def rSreg = R15        ; Save/Restore status port

  .def SL = r0            ; store current slow low
  .def SH = r1            ; store current slow high
  .def FL = r2            ; store current fast low
  .def FH = r3            ; store current fast high
  .def spanner = r4       ; occasional spanner
  .def gen1 = R16         ; Define multipurpose register
  .def math1 = r17        ; Define math op register one
  .def math2 = r18        ; Define math op register two
  .def sramp = r19        ; Define slow ramp current state
  .def framp = r20        ; Define fast ramp current state
  .def sstatus = r21      ; Define status of slow ramp (1=rise, 0=fall)
  .def fstatus = r22      ; Define status of fast ramp (1=rise, 0=fall)
  .def randnum = r23      ; register for working on random number

; **********************************
;         S R A M
; **********************************

  .dseg
  .org SRAM_START

; **********************************
;         C O D E
; **********************************

  .cseg
  .org 000000

; **********************************
; R E S E T  &  I N T - V E C T O R S
; **********************************

  rjmp Main              ; Reset vector
  reti ; INT0
  reti ; PCI0
  reti ; OVF0
  reti ; ERDY
  reti ; ACI
  reti ; OC0A
  reti ; OC0B
  reti ; WDT
  reti ; ADCC

; **********************************
;  M A I N   P R O G R A M   I N I T
; **********************************

Main:

  ldi gen1, Low(RAMEND)
  out SPL, gen1          ; Init LSB stack pointer

  ldi randnum, seed      ; load the random generator with seed

; start conditions for slow and fast ramps

  rcall GenSlowLow       ; generate the slow start
  rcall GenSlowHigh      ; generate the slow end
  rcall GenFastLow       ; generate the fast start
  rcall GenFastHigh      ; generate the fast end

  ldi sstatus, 1         ; set slow to rise
  ldi fstatus, 1         ; set fast to rise
  mov sramp, SL          ; initialise slow ramp
  mov framp, FL          ; initialise fast ramp

  ldi gen1, 0            ; spanner count
  mov spanner, gen1      ; initialise count

  ldi gen1, 0b00000011   ; only two pins output
  out DDRB, gen1

  ldi gen1, 0b00111100   ; the rest pullup
  out PORTB, gen1

; hardware pwm on channel A, B
; COMO0A1(1), COMO0A0(0), COMO0B1(1), COMO0B0(0),
; WGM01(1), WGM00(1)

  ldi gen1, 0b10100011
  out TCCR0A, gen1

; hardware pwm on channel B no prescaler CS00 (1)

  ldi gen1, 0b00000001
  out TCCR0B, gen1

  sei                    ; Enable interrupts

; **********************************
;    P R O G R A M   L O O P
; **********************************

Loop:

  out OCR0A, sramp       ; slow ramp to PB0
  out OCR0B, framp       ; fast ramp to PB1

SlowCheck:

CheckSlowRising:

  cpi sstatus, 1         ; if rising
  brcs CheckSlowFalling

  subi sramp, -1         ; rise

  cp sramp, SH           ; test if at slow end value
  brcs FinishedSlow      ; nope so keep rising

  rcall GenSlowLow       ; else get a new low value
  ldi sstatus, 0         ; kick over status to falling

  rjmp FinishedSlow      ; and move on

CheckSlowFalling:

  subi sramp, 1          ; not rising so fall

  cp sramp, SL           ; test if at slow start value
  brcc FinishedSlow      ; nope so keep falling

  rcall GenSlowHigh      ; else get a new high value
  ldi sstatus, 1         ; and kick over status to rising

FinishedSlow:

FastCheck:

CheckFastRising:         ; same as above but for fast ramp

  cpi fstatus, 1
  brcs CheckFastFalling

  subi framp, -2         ; faster flicker so two at a time
                         ; or more if you want!
  cp framp, FH
  brcs FinishedFast

  rcall GenFastLow
  ldi fstatus, 0

  rjmp FinishedFast

CheckFastFalling:

  subi framp, 2

  cp framp, FL
  brcc FinishedFast

  rcall GenFastHigh
  ldi fstatus, 1

FinishedFast:

  rcall waitms           ; wait a bit before next change

rjmp Loop


rollarandom:

; high in math1, low in math2, return in r16
; linear feedback shift register algorithm (LFSR)

  lsl randnum            ; left shift
  brcc maderand          ; rolled so good to go
  ldi gen1, xori         ; didn't so load second value
  eor randnum, gen1      ; XOR it

  maderand:

  mov gen1, spanner
  cpi gen1, span         ; has count been exceeded?
  brcs nospanner         ; no

  subi randnum, spanum   ; throw spanner
  ldi gen1, 0            ; spanner count
  mov spanner, gen1      ; initialise count
  rjmp checklow          ; start checking

  nospanner:
  mov gen1, spanner      ; load spanner
  subi gen1, -1          ; increment by 1
  mov spanner, gen1      ; save spanner

  checklow:              ; is the result OK?
  cp math1, randnum      ; higher than math1
  brcc rollarandom       ; nope

  checkhigh:
  cp math2, randnum      ; lower than math2
  brcs rollarandom       ; nope

  mov gen1, randnum      ; yes! output the result

  ret

; all of these are the same - load the desired limits
; roll a random (see above), then output the result
; to the appropriate register for storage

GenSlowLow:

  ldi math1, SLL
  ldi math2, SLH
  rcall rollarandom
  mov SL, gen1

  ret

GenSlowHigh:

  ldi math1, SHL
  ldi math2, SHH
  rcall rollarandom
  mov SH, gen1

  ret

GenFastLow:

  ldi math1, FLL
  ldi math2, FLH
  rcall rollarandom
  mov FL, gen1

  ret

GenFastHigh:

  ldi math1, FHL
  ldi math2, FHH
  rcall rollarandom
  mov FH, gen1

  ret

waitms:

; Generated by delay loop calculator
; at http://www.bretmulvey.com/avrdelay.html
;
; Delay 2 048 cycles
; 16ms at 0.128 MHz

  ldi  math1, 3
  ldi  math2, 150
L1:
  dec  math2
  brne L1
  dec  math1
  brne L1
  nop

  ret

; End of source code

