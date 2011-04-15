#include <dm/client.h>
#include <stdexcept>
using namespace std;


int count_words(char* buffer, int size);

int main(int argc, char *argv[]) {
  if (argc != 2)
    return 1;

  DM::Client client;
  try {
    client.dm_init( argv[1] );
  }
  catch (runtime_error e) {
    cerr << "Something nasty happened: " << e.what() << ", exiting..." << endl;
    return 1;
  }

  // maps blocks
  char* size = new char[DIMBLOCK];
  char* spaces = new char[DIMBLOCK];
  char* words = new char[DIMBLOCK];
  int res = 0;
  res += client.dm_block_map( 100, size );
  res += client.dm_block_map( 101, spaces );
  res += client.dm_block_map( 102, words );
  if (res < 0) {
    return 1;
  }

  // waits for size
  while ( *(int*) size == 0 ) {
    if ( client.dm_block_wait(100) < 0 ) {
      return 1;
    }
    if ( client.dm_block_update(100) < 0 ) {
      return 1;
    }
  }
  int textsize;
  textsize = *(int*) size;
  *(int*) size = 0;
  client.dm_block_write(100);

  // maps text blocks
  int n_blocks = (textsize / DIMBLOCK) + 1;
  char* textbuffer = new char[ n_blocks*DIMBLOCK ];
  for (int i = 1; i <= n_blocks; ++i) {
    int offset = DIMBLOCK * (i-1);
    if ( client.dm_block_map(i, textbuffer+offset) < 0 ) {
      return 1;
    }
  }

  // writes words
  int nwords = count_words(textbuffer, textsize);
  *(int*) words = nwords;
  if ( client.dm_block_write(102) < 0 ) {
    return 1;
  }

  // waits for spaces
  while ( *(int*) spaces == 0 ) {
    if ( client.dm_block_wait(101) < 0 ) {
      return 1;
    }
    if ( client.dm_block_update(101) < 0 ) {
      return 1;
    }
  }
  int nspaces;
  nspaces = *(int*) spaces;
  *(int*) spaces = 0;
  client.dm_block_write(101);

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
