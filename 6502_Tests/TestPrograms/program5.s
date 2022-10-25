  .org $c000
reset:
  lda #$90
  clc
  adc #$60
  brk
  nop
  sta $3320
  
  nop
  nop

  .org $f000
interrupt:
  pha
  lda #$42
  sta $1562
  pla
  rti

  .org $fffa
  .word interrupt
  .word reset
  .word interrupt