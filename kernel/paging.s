[bits 32]

; extern set_esp
; global get_esp

; get_esp:
;     push esp
;     call set_esp
;     pop esp
;     ret