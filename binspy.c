
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

static int verbose_flag;

typedef unsigned char uchar;

// Pow function for integers
int ipow(int base, int exp)
{
  int result = 1;
  while (exp)
    {
      if (exp & 1)
        result *= base;
      exp >>= 1;
      base *= base;
    }
  return result;
}

// Print a byte sequence in hex
void print_pattern(unsigned char *pattern, int len)
{
  for (int i = 0; i < len; i++)
    {
      printf("%x ", pattern[i]);
    }
    puts("\n");
}

// The pattern is encoded as a long assuming that a hypothetical matrix
// exists where all combinations are laid out sequentially. For instance,
// the pattern (2, 4) would exist in position ((255 * 4), 4)
unsigned long get_pattern_signature(unsigned char *pattern, int len)
{
    unsigned long pattern_signature = 0;

    for (int j = 0; j < len - 1; j++)
      {
        pattern_signature += (255 * ((int)pattern[j] + 1));
      }
    pattern_signature += (int)pattern[len - 1];

    printf("Pattern signature is %lu\n", pattern_signature);

    return pattern_signature;
}

// Finds positions of repeated byte patterns in the given file.
// file -- handle to the file
// gram_len -- length of patterns to consider
// max_matches -- number of file positions to record for each pattern
int find_grams(FILE *file, int gram_len, int max_matches)
{
  // Allocate a matrix to record file positions of byte patterns. The matrix
  // will be of length 255^gram_len and width of max_matches, with file
  // positions recorded as longs.
  // 255^gram_len represents all possible sequences of gram_len bytes.
  int mat_len = ipow(255, gram_len) * max_matches;
  unsigned long mem_size = mat_len * sizeof(unsigned long);
  printf("Will allocate %lu bytes of memory.\n", mem_size);
  unsigned long *positions = (unsigned long *)malloc(mem_size);

  // Container for the current sequence of bytes
  unsigned char *cur_gram = malloc(sizeof(unsigned char) * gram_len);

  // Open the file and record its length
  unsigned long fsize;

  fseek(file, 0, SEEK_END);
  fsize = ftell(file);
  rewind(file);

  printf("File size is %d bytes.\n", fsize);

  for (int i = 0; i < fsize; i++)
    {
      // Read the current byte and shift cur_gram
      unsigned char b;
      fread(&b, 1, 1, file);

      for (int j = 0; j < gram_len; j++)
        {
          cur_gram[j] = cur_gram[j + 1];
        }

      cur_gram[gram_len - 1] = b;

      print_pattern(cur_gram, gram_len);

      // Record file positions of patterns
      unsigned long pattern_signature =
        get_pattern_signature(cur_gram, gram_len);

      // Add file position to the matrix
      int observation_column = 0;
      unsigned long prev_occurrence = positions[pattern_signature];
      // while (prev_occurrence != 0 && observation_column < max_matches)
      //   {
      //     prev_occurrence = positions[pattern_signature + observation_column++];
      //   }
      // Have we reached max_matches?
      // if (prev_occurrence != 0)
      //   {
      //     printf("Max matches reached for pattern: ");
      //     print_pattern(cur_gram, gram_len);
      //   }
    }

  free(positions);
}

int find_patterns(int len, char *filename)
{
  long fsize;
  FILE *file;

  // Open the file and begin reading
  file = fopen(filename, "r");
  if (file == NULL)
    {
      printf("There was a problem opening the file '%s'\n", filename);
      exit(1);
    }

  find_grams(file, 4, 4);

  fclose(file);
}

#ifdef TESTING // Unit tests begin

int main()
{
  printf("Let's run some tests\n");

  unsigned char upper_bound_pattern[2] = { 255, 255 };

  unsigned long sig = get_pattern_signature(upper_bound_pattern, 2);

  // This should be the upper bound of the matrix, or 255 x 255
  if (sig != ipow(255, 2))
    {
      printf("Upper bound signature should have been %lu, but got %lu\n",
        ipow(255, 2), sig);
      exit(1);
    }
}

#endif // Unit tests

#ifndef TESTING

int main(int argc, char **argv)
{
  int c;
  int pattern_len = 4;

  while (1)
    {
      static struct option long_options[] =
        {
          {"verbose", no_argument,       &verbose_flag, 1},

          {"length",  required_argument, 0, 'l'},
          {0, 0, 0, 0}
        };

      int option_index = 0;

      c = getopt_long(argc, argv, "vl:",
                       long_options, &option_index);

      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          if (long_options[option_index].flag != 0)
            break;

        case 'l':
          pattern_len = atoi(optarg);
          break;

        case 'v':
          verbose_flag = 1;
          break;

        case '?':
          break;

        default:
          abort();
        }
    }

  if (verbose_flag)
    puts ("verbose flag is set");

  if (optind < argc)
    {
      while (optind < argc)
        find_patterns(pattern_len, argv[optind++]);
    }
  else
    {
      printf("No input files provided.\n");
      exit(1);
    }

  exit (0);
}

#endif // TESTING = false
