#include "pipe_networking.h"

void process_data(char *data){
  while(*data){
    *data = 25 - (*data) + 2 * 'a';
    data++;
  }
}

static void sighandler(int signo){
  if(signo == SIGINT){
    remove("WKP");
    exit(0);
  }
}

int main() {
  signal(SIGINT,sighandler);

  int to_client;
  int from_client;

  while(1){
    from_client = server_handshake( &to_client );
    char output[BUFFER_SIZE];
    while(read(from_client, output, BUFFER_SIZE)){
        output[strlen(output) - 1] = '\0';
        printf("Server: processing data of value [%s]\n", output);
        process_data(output);
        write(to_client, output, BUFFER_SIZE);
        printf("Server: data processed with value [%s] and sent to client\n\n", output);
    }

  }

}

/*
get data from client
process the data
reply to client
when client exits, reset

 */
