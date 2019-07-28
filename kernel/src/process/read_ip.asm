global read_ip

bits 64

read_ip:
    mov rax, [rsp]
    ret