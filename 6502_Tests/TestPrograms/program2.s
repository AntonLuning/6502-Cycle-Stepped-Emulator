  .org $c000
reset:
  ldx #$05
  lda #$42
  sta $20,X
  ldy #$ff
  ldx $26,Y
  stx $1166
  
  nop

  .org $fffc
  .word reset
  .word $0000  