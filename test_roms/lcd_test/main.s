.setcpu "6502"

.segment "CODE"

reset:
  sei
  cld

  lda #$42
  sta $0200

loop:
  jmp loop

