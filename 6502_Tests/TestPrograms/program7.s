multA = 12
multB = 23

MPD = $3000
MPR = $3001
TMP = $3002
RESULT = $3003

  .org $c000
reset:
  lda #multA
  sta MPD
  lda #multB
  sta MPR
  lda #$00
  sta TMP
  sta RESULT
  sta RESULT + 1
  ldx #$08

mult:
  lsr MPR
  bcc noadd
  lda RESULT
  clc
  adc MPD
  sta RESULT
  lda RESULT + 1
  adc TMP
  sta RESULT + 1

noadd:
  asl MPD
  rol TMP
  dex
  bne mult

  nop
  nop

  .org $fffa
  .word reset
  .word reset
  .word reset