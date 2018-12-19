#include "pipe_networking.h"

void process_data(char *data){
  while(*data){
    *data = 26 - (*data) + 2 * 'a';
    data++;
  }
}


int main() {

  int to_client;
  int from_client;

  while(1){
    from_client = server_handshake( &to_client );
    
  }

}

/*
get data from client
process the data
reply to client
when client exits, reset

 */
