[GLOBAL test]
test:
mov ah, 02H ;
ms-dos character output function
mov dl, 'a'
mov al, '*'
mov bh, 00h
mov bl, 1eh
mov cx, 12
int 09h
ret
