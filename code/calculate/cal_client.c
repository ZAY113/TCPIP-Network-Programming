#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OP_BIT 4
#define RLT_SIZE 4
void error_handling(char *message);

int main(int argc, char *argv[])
{
  int sock;
  int op_num, i, result;
  char opinfo[BUF_SIZE];
  struct sockaddr_in serv_adr;

  if (argc != 3)
  {
    printf("Usage: %s <IP> <port>\n", argv[0]);
    exit(1);
  }

  sock = socket(PF_INET, SOCK_STREAM, 0);
  if (sock == -1)
    error_handling("socket() error");

  memset(&serv_adr, 0, sizeof(serv_adr));
  serv_adr.sin_family = AF_INET;
  serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
  serv_adr.sin_port = htons(atoi(argv[2]));

  if (connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
    error_handling("connect() error");
  else
    puts("Connected..........");

  fputs("Operand count: ", stdout);
  scanf("%d", &op_num);
  opinfo[0] = (char)op_num;

  for (i = 0; i < op_num; i++)
  {
    printf("Operand %d: ", i+1);
    scanf("%d", (int*)&opinfo[i*OP_BIT+1]);
  }

  fgetc(stdin);
  fputs("Operator: ", stdout);
  scanf("%c", &opinfo[op_num*OP_BIT+1]);
  write(sock, opinfo,op_num*OP_BIT+2);
  read(sock, &result, RLT_SIZE);

  printf("Operation result: %d \n", result);
  close(sock);
  return 0;
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}
