
/*
** Exemplo simples de conexão com servidor SOCKS versão 5.
** Detalhes: http://tools.ietf.org/html/rfc1928
** 
** Coded by Constantine - 24/03/2015. 
** Site: constantine.sourceforge.net
**  
** windows... 
**     gcc.exe socks5.c -o socks -lws2_32 -std=c99
** 
** Linux...
**     gcc socks5.c -o socks5 -std=c99
** 
** Output...
**     Address = 121.40.103.137:1080
**     Sended (3) = 0x5 0x1 0x0
**     Receved data(2): 0x5 0x0
**     Conexao estabelecida com sucesso!
** 
**     Send data step two...
**     Receved data(10): 0x5 0x0 0x0 0x1 0x79 0x28 0x67 0xffffff89 0x6f 0xffffffd3
** 
**     Header...
**     GET /text HTTP/1.1
**     Host: wtfismyip.com
** 
**     out...
**     HTTP/1.1 200 OK
**     Content-Type: text/plain; charset=utf-8
**     Cache-Control: no-cache, no-store, max-age=0, must-revalidate
**     Pragma: no-cache
**     Expires: 0
**     Content-Length: 15
**     Date: Tue, 24 Mar 2015 21:13:37 GMT
**     Connection: keep-alive
** 
**     121.40.103.137
** 
** Greatz for P0cl4bs and my friends. (https://github.com/P0cL4bs)
**    L1sb3th, foreach, 0x29a, xstpl, sup3rman, j0shua3w, Mmxm, Anonymous_, lpax
**    c00ler, m0nad, sigsegv, enygmata, eremitah, raphaelsc, Otacon, KvN, Al3xG0, Erick
**    H3LLS1ng, Eletronico, l4rg4d0, mvrech, DarkCrypter, nbdu1nder, Orc, Snow, Nosomy
**    Arion, LiFux, Zeus, Chainksain, shadow, _mlk_, sexpistol, Haxixe, shadow
**    0fx66, Tr3v0r, ph4ck3r, bl4de, DarkCrypter, Depois, K4r4t3k1d... and all my friends.
*/

// Usuários Linux deixe a linha abaixo comentada.
#define WINDOWS_USER

#include <stdio.h>
#include <string.h>

#ifdef WINDOWS_USER
  #include <Winsock2.h>
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
#endif

#ifndef MAX_PATH
  #define MAX_PATH 256
#endif

#ifndef INVALID_SOCKET
  #define INVALID_SOCKET (-1)
#endif

void print_hex (char *buff, int size)
{
  int a;
  
  for (a = 0; a < size; a++) 
    printf("0x%x ", buff[a]);
  printf("\n");
}

int main (int argc, char ** argv) 
{
  int sock;
  int irecv;
  char brecv [MAX_PATH];
  char header [MAX_PATH*2];
  
#ifdef WINDOWS_USER
  /* Inicializa winsock. */
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2,2),&wsa) == 0) 
  {
#endif
    
    /* Cria socket. */
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != INVALID_SOCKET) 
    {
      /* Configurações da conexão. */
      struct sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_port = htons( 1080 );
      addr.sin_addr.s_addr =  inet_addr("121.40.103.137");
        
      printf("\nAddress = %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

      /* Conecta em servidor. */
      if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0) 
      {  
        char buff [] = 
        { 
          0x05,    /* Versão do protocolo. */
          0x01,    /* Comando (0x01 = connect). */
          0x00     /* Reversed. */
        };

        /* Envia dados para servidor. */
        send(sock, buff, sizeof(buff), 0);
        printf("Sended (%d) = ", sizeof(buff));
        print_hex(buff, sizeof(buff));

        /* Recebe dados para processar as mensagens da negociação com servidor SOCKS 5. */
        irecv = recv(sock, brecv, MAX_PATH, 0);
        if ( irecv > 0) 
        {
          printf("Receved data(%d): ", irecv);
          print_hex(brecv, irecv);
          
          /* Tratamento das mensagens. */
          
          if (brecv[0] != 0x05) { /* Verifica versão do protocolo SOCKS utilizado pelo servidor. */
            printf("Invalid server SOCKS version!\n");
            goto connection_end;
          }

          if (brecv[1] == 0x02) { /* É necessário autenticação. */
            printf("Necessário autenticacao!\n");
            goto connection_end;
          }
          
          /* Conexão estabelecida com sucesso, não é necessário autenticação. */
          if (brecv[1] == 0x00) 
          {                      
            printf("Conexao estabelecida com sucesso!\n");
            
            /* Envia endereço do destinatário. */
            char buff_step_two [MAX_PATH] = 
            { 
              0x05,    /* Versão do protocolo. */
              0x01,    /* Comando (0x01 = connect). */
              0x00,    /* Reversed. */
              0x01    /* IP V4. */
            };
            
            printf("\nSend data step two...\n");
            
            struct hostent *st_hostent;
            struct sockaddr_in addr;
            
            addr.sin_port = htons( 80 );
            addr.sin_addr.s_addr = inet_addr("54.200.182.206");
            
            memcpy(buff_step_two + 4, &addr.sin_addr.s_addr, 4);
            memcpy(buff_step_two + 8, &addr.sin_port, 2);
            
            send(sock, buff_step_two, 10, 0);
            
            memset(brecv, '\0', MAX_PATH);
            irecv = recv(sock, brecv, 10, 0);
            if ( irecv > 0) 
            {
              /*
              ** (Útil apenas para conexões 'BIND')
              ** Retorno...
              ** 
              ** Byte     | Descrição 
              ** 0        | Versão do SOCKS.
              ** 1        | 0x0 = Conexão estabelecida com sucesso.
              ** 2        | Reserved.
              ** 3        | Tipo da conexão, 0x01 = IPv4.
              ** 4 -> 8   | Endereço IPv4.
              ** 8 -> 10  | Porta.
              */
              printf("Receved data(%d): ", irecv);
              print_hex(brecv, irecv);
              
              /* HTTP header. */
              sprintf(header, "GET /text HTTP/1.1\r\n"
                              "Host: wtfismyip.com\r\n\r\n");

              printf("\nHeader...\n%s\n", header);
              send(sock, header, strlen(header), 0);

              memset(brecv, '\0', MAX_PATH);
              if ( recv(sock, brecv, MAX_PATH, 0) > 0)
                printf("out...\n%s\n", brecv);             
            }
            
            goto connection_end;
          }
        }
      }

      connection_end:
#ifdef WINDOWS_USER
      closesocket(sock);
#else
      close(sock);
#endif
    }

#ifdef WINDOWS_USER
    /* Descarrega winsock. */
    WSACleanup();
  }
#endif

  return 0;
}

/* EOF. */
