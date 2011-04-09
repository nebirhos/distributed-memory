#include <dm/client.h>
using namespace std;


int count_words(char* buffer, int size);

int main(int argc, char *argv[]) {
  if (argc != 2)
    return 1;

  DM::Client client;
  client.dm_init( argv[1] );

  char* size = new char[DIMBLOCK];
  char* spaces = new char[DIMBLOCK];
  char* words = new char[DIMBLOCK];
  client.dm_block_map( 100, size );
  client.dm_block_map( 101, spaces );
  client.dm_block_map( 102, words );

  while ( *(int*) size == 0 ) {
    client.dm_block_wait( 100 );
    client.dm_block_update( 100 );
  }

  int textsize = *(int*) size;
  int n_blocks = (textsize / DIMBLOCK) + 1;
  char* textbuffer = new char[ n_blocks*DIMBLOCK ];
  for (int i = 1; i <= n_blocks; ++i) {
    int offset = DIMBLOCK * (i-1);
    client.dm_block_map( i, textbuffer+offset );
  }

  *(int*) words = count_words(textbuffer, textsize);
  client.dm_block_write( 102 );

  while ( *(int*) spaces == 0 ) {
    client.dm_block_wait( 101 );
    client.dm_block_update( 101 );
  }

  cout << "Read " << textsize << " bytes" << endl;
  cout << *(int*) spaces << " spaces" << endl;
  cout << *(int*) words << " words" << endl;

  // Reset
  *(int*) size = 0;
  *(int*) spaces = 0;
  *(int*) words = 0;
  client.dm_block_write( 100 );
  client.dm_block_write( 101 );
  client.dm_block_write( 102 );

  for (int i = 1; i < n_blocks; ++i) {
    client.dm_block_unmap( i );
  }
  client.dm_block_unmap( 100 );
  client.dm_block_unmap( 101 );
  client.dm_block_unmap( 102 );
  delete[] textbuffer;
  delete[] size;
  delete[] spaces;
  delete[] words;

  return 0;
}

int count_words(char* buffer, int size) {
  int result = 0;
  bool word = true;

  for (int i = 0; i < size; ++i) {
    if ( buffer[i] == ' ' || buffer[i] == '\n' || buffer[i] == '\r' ) {
        word = true;
    }
    else {
      if (word) {
        result++;
        word = false;
      }
    }
  }
  return result;
}
