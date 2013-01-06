[GLOBAL GetEIPofInterruptedTask]
GetEIPofInterruptedTask:
  pop ebx
  pop eax
  push eax
  jmp ebx

[GLOBAL GetEIP]
GetEIP:
  pop eax
  push eax
  ret
