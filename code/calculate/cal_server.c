#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OPER_BIT 4
void error_handling(char *message);
int calculate(int op_num, int opnds[], char op);

int main(int argc, char *argv[])
{
  int serv_sock, clnt_sock;
  int i, op_num, recv_len, recv_cnt, result;
  char opinfo[BUF_SIZE];
  
  struct sockaddr_in serv_adr, clnt_adr;
  socklen_t clnt_adr_sz;

  if (argc != 2){
    printf("Usage: %s <port>\n", argv[0]);
    exit(1);
  }

  serv_sock = socket(PF_INET, SOCK_STREAM, 0);
  if (serv_sock == -1)
    error_handling("socket() error");

  memset(&serv_adr, 0, sizeof(serv_adr));
  serv_adr.sin_family = AF_INET;
  serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_adr.sin_port = htons(atoi(argv[1]));

  if (bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
    error_handling("bind() error");

  if (listen(serv_sock, 5) == -1)
    error_handling("listen() error");

  clnt_adr_sz = sizeof(clnt_adr);
  
  for (i = 0; i < 5; i++)
  {
    op_num = 0;
    clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
    if (clnt_sock == -1)
      error_handling("accept() error");
    else
      printf("Connected client %d \n", i+1);
    
    read(clnt_sock, &op_num, 1);

    recv_len = 0;
    while (recv_len < op_num*OPER_BIT + 1)
    {
      recv_cnt = read(clnt_sock, &opinfo[recv_len], BUF_SIZE-1);
      recv_len += recv_cnt;
    }
    result = calculate(op_num, (int*)opinfo, opinfo[recv_len-1]);
    write(clnt_sock, (char*)&result, sizeof(result));
    close(clnt_sock);
  }
  close(serv_sock);
  return 0;
}

int calculate(int op_num, int opnds[], char op)
{
  int result = opnds[0], i;
  switch (op)
  {
    case '+':
      for (i = 1; i < op_num; i++){
        result += opnds[i];
      }
      break;
    case '-':
      for (i = 1; i < op_num; i++)
        result -= opnds[i];
      break;
    case '*':
      for (i = 1; i < op_num; i++)
        result *= opnds[i];
      break;
  }
  return result;
}

void error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}
