.section .multiboot
header_start:
    .magic:      .long 0xe85250d6
    .flag:       .long 0
    .len:        .long header_end - header_start
    .checksum:   .long 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))
    .word 0
    .word 0
    .long 8
header_end:
