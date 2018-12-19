#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client
  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.
  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  printf("Server: begin making Well Known Pipe\n");
  mkfifo("WKP",0644);
  printf("Server: finished making Well Known Pipe\n\n");

  printf("Server: begin opening Well Known Pipe\n");
  int upstream = open("WKP", O_RDONLY);
  printf("Server: finished opening Well Known Pipe\n\n");

  printf("Server: begin reading from Well Known Pipe to get name of private pipe\n");
  char name[HANDSHAKE_BUFFER_SIZE];
  while(!read(upstream, name, HANDSHAKE_BUFFER_SIZE));
  printf("Server: finished reading from Well Known Pipe to get name of private pipe, value read: [%s]\n\n", name);

  printf("Server: begin opening private pipe with name [%s]\n", name);
  *to_client = open(name, O_WRONLY);
  printf("Server: finished opening private pipe with name [%s]\n\n", name);

  printf("Server: begin removing Well Known Pipe\n");
  remove("WKP");
  printf("Server: finished removing Well Known Pipe\n\n");

  printf("Server: begin writing to private pipe, value to write: [%s]\n", ACK);
  write(*to_client, ACK, HANDSHAKE_BUFFER_SIZE);
  printf("Server: finished writing to private pipe, value written: [%s]\n\n", ACK);

  printf("Server: begin reading from Well Known Pipe to verify acknowledgement\n");
  while(!read(upstream, name, HANDSHAKE_BUFFER_SIZE));
  printf("Server: finished reading from Well Known Pipe to verify acknowledgement, value read: [%s]\n\n", name);

  printf("Server: begin verifying values\n");
  if(strcmp(name, ACK)) printf("Error verifying acknowledgement\n");
  else printf("Server: acknowledgement was successfully verified!\n");
  printf("Server: finished verifying values\n\n");

  return upstream;
}


/*=========================
  client_handshake
  args: int * to_server
  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.
  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int pid = getpid();
  char buf[HANDSHAKE_BUFFER_SIZE], *count = buf;
  while(pid){
    *count = (pid % 10) + 48;
    pid /= 10;
    count++;
  }
  *count = '\0';
  printf("Client: begin making private pipe\n");
  mkfifo(buf, 0644);
  printf("Client: finished making private pipe\n\n");

  printf("Client: begin opening Well Known Pipe to send name of private pipe\n");
  *to_server = open("WKP",O_WRONLY);
  printf("Client: finished opening Well Known Pipe to send name of private pipe\n\n");

  printf("Client: begin writing name of private pipe to Well Known Pipe, value to write: [%s]\n", buf);
  write(*to_server, buf, HANDSHAKE_BUFFER_SIZE);
  printf("Client: finished writing name of private pipe to Well Known Pipe, value written: [%s]\n\n", buf);

  char acknowledge[HANDSHAKE_BUFFER_SIZE];
  printf("Client: begin opening private pipe\n");
  int downstream = open(buf, O_RDONLY);
  printf("Client: finished opening private pipe\n\n");

  printf("Client: begin reading acknowledgement from private pipe\n");
  while(!read(downstream, acknowledge, HANDSHAKE_BUFFER_SIZE));
  printf("Client: finished reading acknowledgement from private pipe, value read: [%s]\n\n", acknowledge);

  printf("Client: begin removing private pipe\n");
  remove(buf);
  printf("Client: finished removing private pipe\n\n");

  printf("Client: begin writing received message to Well Known Pipe, value to write: [%s]\n", acknowledge);
  write(*to_server, acknowledge, HANDSHAKE_BUFFER_SIZE);
  printf("Client: finished writing received message to Well Known Pipe, value written: [%s]\n\n", acknowledge);

  return downstream;
}
