#include <dm/client.h>
#include <fstream>
#include <stdexcept>
using namespace std;


int count_spaces(char* buffer, int size);

int main(int argc, char *argv[]) {
  if (argc != 3)
    return 1;

  DM::Client client;
  try {
    client.dm_init( argv[1] );
  }
  catch (runtime_error e) {
    cerr << "Something nasty happened: " << e.what() << ", exiting..." << endl;
    return 1;
  }

  // opens file and calculates number of blocks
  ifstream text( argv[2] );
  text.seekg (0, ios::end);
  int textsize = text.tellg();
  text.seekg (0, ios::beg);
  int n_blocks = (textsize / DIMBLOCK) + 1;

  // maps blocks
  char* textbuffer = new char[ n_blocks*DIMBLOCK ];
  char* size = new char[DIMBLOCK];
  char* spaces = new char[DIMBLOCK];
  char* words = new char[DIMBLOCK];
  int res = 0;
  res += client.dm_block_map( 100, size );
  res += client.dm_block_map( 101, spaces );
  res += client.dm_block_map( 102, words );
  for (int i = 1; i <= n_blocks; ++i) {
    int offset = DIMBLOCK * (i-1);
    res += client.dm_block_map( i, textbuffer+offset );
  }
  if (res < 0) {
    return 1;
  }

  // reads file and writes back to the server
  text.read(textbuffer, textsize);
  for (int i = 1; i <= n_blocks; ++i) {
    if ( client.dm_block_write(i) < 0 ) {
      return 1;
    }
  }

  // writes size
  *(int*) size = textsize;
  if ( client.dm_block_write(100) < 0 ) {
    return 1;
  }

  // writes spaces
  int nspaces = count_spaces(textbuffer, textsize);
  *(int*) spaces = nspaces;
  if ( client.dm_block_write(101) < 0 ) {
    return 1;
  }

  // waits for words
  while ( *(int*) words == 0 ) {
    if ( client.dm_block_wait(102) < 0 ) {
      return 1;
    }
    if ( client.dm_block_update(102) < 0 ) {
      return 1;
    }
  }
  int nwords;
  nwords = *(int*) words;
  *(int*) words = 0;
  client.dm_block_write(102);

  // prints result
  cout << "Read " << textsize << " bytes" << endl;
  cout << nspaces << " spaces" << endl;
  cout << nwords << " words" << endl;

  // close
  for (int i = 1; i <= n_blocks; ++i) {
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
