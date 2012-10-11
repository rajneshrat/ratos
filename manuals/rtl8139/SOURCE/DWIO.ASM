; ****************************************************************
; *
; * Copyright (C) 1995 Macronix International Corp.
; * MX0513 Diagnose and Test program.
; * pci.c + bmtx.c + nic.c + decdiag.c + dwio.asm
; *
; * Revision History:
; * Date     Who                   Comments
; * 07/25/95 Luke Huang
; ******************************************************************

                .model  medium
                .386
ARG1            equ     6               ;4 for small model, 6 for medium

;****************************************************************
;*                                                              *
;* Out port double word                                         *
;*                                                              *
;* From C:                                                      *
;*      outpdw (unsigned int port, long int value);             *
;*                                                              *
;****************************************************************
                .code
                public  _outpdw
_outpdw         proc    far
                push    bp              ; Save the stack frame pointer
                mov     bp, sp          ; Set up a new stack frame pointer
                mov     dx, word ptr [bp+ARG1]          ; Get the port
                mov     eax, dword ptr [bp+ARG1+2]      ; Get the vlaue
                out     dx, eax         ; Out port
                mov     sp, bp          ; Restore the stack frame
                pop     bp              ; Restore registers
                ret
_outpdw         endp

;****************************************************************
;*                                                              *
;* In port double word                                          *
;*                                                              *
;* From C:                                                      *
;*      inpdw (unsigned int port);                              *
;*                                                              *
;****************************************************************
                .code
                public  _inpdw
_inpdw          proc    far
                push    bp              ; Save the stack frame pointer
                mov     bp, sp          ; Set up a new stack frame pointer
                mov     dx, word ptr [bp+ARG1]   ; Get the port
                in      eax,dx          ; In port to eax
                ror     eax,16
                mov     dx,ax
                ror     eax,16
                mov     sp, bp          ; Restore the stack frame
                pop     bp              ; Restore registers
                ret
_inpdw          endp

;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;delay10us
;
;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                .code
                public  _delay10us
_delay10us  proc    far
        push    dx
        push    bx
        push    ax
        mov     al, 06h                 ;Counter Latch
        out     43h, al
        in      al, 40h                 ;Read Counter
	mov	ah, al
	in	al, 40h
	xchg	al, ah
        mov     dx, ax
loopagain:
        mov     al, 06h                 ;Counter Latch
        out     43h, al
        in      al, 40h                 ;Read Counter to ax
	mov	ah, al
	in	al, 40h
	xchg	al, ah
	mov	bx, dx
	sub	bx, ax
	cmp	bx, 24
        jb      SHORT loopagain         ;If not enough
        pop     ax
        pop     bx
        pop     dx
        ret
_delay10us  endp


;****************************************************************
;*                                                              *
;* Calculate the IEEE address checksum                          *
;*                                                              *
;* From C:                                                      *
;*      caleepcrc(bytecount,x[n])                               *
;*                                                              *
;****************************************************************
                .code
                public  _caleepcrc
_caleepcrc      proc    far
                push    bp              ; Save the stack frame pointer
                mov     bp, sp          ; Set up a new stack frame pointer

		mov	cx, word ptr [bp+0+ARG1]	
		shr	cx, 1
		dec	cx 	
                mov     di, word ptr [bp+2+ARG1]                   ;Calculate the IEEE address
		mov	ah, byte ptr [di]
		mov	al, byte ptr [di+1]
		add	di, 2
eeploop:
                shl     ax, 1
                jnc     SHORT NoCarry1
                sub     ax, 0FFFFh
        NoCarry1:
                mov     dh, byte ptr [di]
                mov     dl, byte ptr [di+1]
		add 	di, 2
                add     ax, dx
                jnc     SHORT NoCarry2
                sub     ax, 0FFFFh
        NoCarry2:
		loop	eeploop 

                mov     sp, bp          ; Restore the stack frame
                pop     bp              ; Restore registers
                ret
_caleepcrc      endp

                end
