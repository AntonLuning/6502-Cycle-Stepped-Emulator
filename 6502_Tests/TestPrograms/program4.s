  .org $c000
reset:
  lda #$90
  clc
  sbc #$60
  sta $3320
  
  nop

  .org $fffc
  .word reset
  .word $0000  