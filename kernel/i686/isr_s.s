[bits 32]

global i686_int_handler_wrapper
extern i686_int_handler

i686_int_handler_wrapper:
    pushad
    cld
    call i686_int_handler
    popad
    iret