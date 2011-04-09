#include <dm/client.h>
#include <fstream>
using namespace std;


int count_spaces(char* buffer, int size);

int main(int argc, char *argv[]) {
  if (argc != 3)
    return 1;

  DM::Client client;
  client.dm_init( argv[1] );

  // open file and calculate n of blocks
  ifstream text( argv[2] );
  text.seekg (0, ios::end);
  int textsize = text.tellg();
  text.seekg (0, ios::beg);
  int n_blocks = (textsize / DIMBLOCK) + 1;

  char* textbuffer = new char[ n_blocks*DIMBLOCK ];
  char* size = new char[DIMBLOCK];
  char* spaces = new char[DIMBLOCK];
  char* words = new char[DIMBLOCK];
  client.dm_block_map( 100, size );
  client.dm_block_map( 101, spaces );
  client.dm_block_map( 102, words );
  for (int i = 1; i <= n_blocks; ++i) {
    int offset = DIMBLOCK * (i-1);
    client.dm_block_map( i, textbuffer+offset );
  }

  // reads file and writes back to the server
  text.read(textbuffer, textsize);
  for (int i = 1; i <= n_blocks; ++i) {
    client.dm_block_write( i );
  }

  *(int*) size = textsize;
  client.dm_block_write( 100 );

  *(int*) spaces = count_spaces(textbuffer, textsize);
  client.dm_block_write( 101 );

  while ( *(int*) words == 0 ) {
    client.dm_block_wait( 102 );
    client.dm_block_update( 102 );
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

int count_spaces(char* buffer, int size) {
  int result;
  for (int i = 0; i < size; ++i) {
    if (buffer[i] == ' ')
      result++;
  }
  return result;
}
