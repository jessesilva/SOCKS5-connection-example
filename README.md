# SOCKS5 connection example

Exemplo de conexão com servidor proxy SOCKS5.

Em casos de dúvidas: http://tools.ietf.org/html/rfc1928




Compilando...

- windows...
 
      gcc.exe socks5.c -o socks -lws2_32 -std=c99
 


- Linux...


      gcc socks5.c -o socks5 -std=c99
