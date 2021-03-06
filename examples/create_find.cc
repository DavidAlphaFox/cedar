
#include <cstdio>
#include <cstdlib>

#ifdef USE_PREFIX_TRIE
#include <cedarpp.h>
#else
#include <cedar.h>
#endif

#include <gflags/gflags.h>
#include <unistd.h>
#include <random>

int main (int argc, char **argv) {

  int pid = 0;
  if (argc > 1) {
  	pid = atoi(argv[1]);
  } else {
  	pid = getpid();
  }
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::mt19937 generator(pid);
  printf("seed is %d\n", pid);

  // do not generate char = 0
  constexpr size_t kMaxLineSize = 10;
  std::uniform_int_distribution<int> charGen(97, 122);
  std::uniform_int_distribution<int> lengthGen(5, kMaxLineSize);

  cedar::da <int> trie;
  char line[kMaxLineSize + 1];
  int num_strings = 100;
  for (int i = 0; i < num_strings; i++) {
	int length = lengthGen(generator);
    for (int pos = 0; pos < length; pos ++) {
		line[pos] = charGen(generator);
	}
  	trie.update (line, length - 1, i);
	line[length] = '\0';
	LOG_EVERY_N(INFO, 10000) << i;
  }

  trie.save("mmaptrie.dat");

  std::fprintf (stderr, "keys: %ld\n", trie.num_keys ());
  std::fprintf (stderr, "size: %ld\n", trie.size ());
  std::fprintf (stderr, "nonzero_size: %ld\n", trie.nonzero_size ());

  return 0;
}
