  .org $c000
reset:
  lda #$42
  sta $62
  lda #$33
  clc
  adc $62
  sta $3320
  
  nop

  .org $fffc
  .word reset
  .word $0000  