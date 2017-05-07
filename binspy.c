
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

static int verbose_flag;

typedef unsigned char uchar;

// Pow function for integers
// int ipow(int base, int exp)
// {
//     int result = 1;
//     while (exp)
//     {
//         if (exp & 1)
//             result *= base;
//         exp >>= 1;
//         base *= base;
//     }
//     return result;
// }

// Here's another option:

// int *mat = (int *)malloc(rows * cols * sizeof(int));
// Then, you simulate the matrix using

// int offset = i * cols + j;
// // now mat[offset] corresponds to m(i, j)
// for row-major ordering and

// int offset = i + rows * j;
// // not mat[offset] corresponds to m(i, j)
// for column-major ordering.


int find_patterns(int len, char *filename)
{
  long fsize;
  FILE *file;

  // Allocate a matrix of all possible 2-byte sequences. We will use these for
  // comparisons, and then allocate longer patterns as they are recognized.
  int rows = 255 * 255;
  int cols = 2;
  uchar *mat = (uchar *)malloc(rows * cols * sizeof(uchar));

  // Also, an array to mark file position where they occurred.
  int *positions = malloc(rows * sizeof(int));

  for (int i = 0; i < rows; i++)
    {
      mat[i*cols] = i / 255;
      // This will repeat automatically as we surpass the capacity of a byte
      mat[i*cols+1] = i;
    }

  // Open the file and begin reading
  file = fopen(filename, "r");
  if (file == NULL)
    {
      printf("There was a problem opening the file '%s'\n", filename);
      exit(1);
    }

  fseek(file, 0, SEEK_END);
  fsize = ftell(file);
  rewind(file);

  printf("File size is %d bytes.\n", fsize);

  // The current bigram
  uchar *cur_gram = malloc(sizeof(uchar)*2);

  for (int i = 0; i < fsize; i++)
    {
      cur_gram[1] = cur_gram[0];
      uchar b;
      fread(&b, 1, 1, file);
      cur_gram[0] = b;

      printf("%x %x\n", cur_gram[0], cur_gram[1]);
    }

  fclose(file);
  free(mat);
}

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
