  .org $c000
reset:
  lda #$ff
  sta $3002
  
  nop
  nop

  .org $fffc
  .word reset
  .word $0000  