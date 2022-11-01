  .org $c000
reset:
  lda #$50
  ldx #$00
  
loop:
  inx
  asl
  bne loop

  nop
  nop

  .org $fffa
  .word reset
  .word reset
  .word reset