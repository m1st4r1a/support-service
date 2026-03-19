#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>
#include "utils.h"
#define ARGUMENT_PARSER_MAX_ARGUMENTS_NUMBER 6
#define TEST_FILENAME_LENGTH 100
#define FREE_AND_NULL(p) do { free(p); (p) = NULL; } while(0)
#define CLOSE_AND_NULL(f) do { fclose(f); (f) = NULL; } while(0)
#define ERROR_AND_RETURN(fd) do { error(fd); is_successful = EINVAL; return; } while(0)
#define NO_SOLUTION_AND_RETURN(fd) do { no_solution(fd); return; } while(0)
bool is_successful = EXIT_SUCCESS;

void
argument_parser (FILE *output_fd, char *str, uint8_t values_count, ...)
{
  va_list values;
  uint64_t *arg_pointer[ARGUMENT_PARSER_MAX_ARGUMENTS_NUMBER], i = 0;
  (void *) memset (arg_pointer, 0,
		   ARGUMENT_PARSER_MAX_ARGUMENTS_NUMBER *
		   sizeof (*arg_pointer));
  char *identifiers[ARGUMENT_PARSER_MAX_ARGUMENTS_NUMBER];
  (void *) memset (identifiers, 0,
		   ARGUMENT_PARSER_MAX_ARGUMENTS_NUMBER *
		   sizeof (*identifiers));
  uint16_t identifiers_sizes[ARGUMENT_PARSER_MAX_ARGUMENTS_NUMBER];
  (void *) memset (identifiers_sizes, 0,
		   ARGUMENT_PARSER_MAX_ARGUMENTS_NUMBER *
		   sizeof (*identifiers_sizes));
  va_start (values, values_count);
  uint8_t sections_border = values_count / 3;
  for (i = 0; i < sections_border; i++)
    arg_pointer[i] = va_arg (values, uint64_t *);
  for (i = 0; i < sections_border; i++)
    identifiers[i] = va_arg (values, char *);
  for (i = 0; i < sections_border; i++)
    identifiers_sizes[i] = va_arg (values, int);
  va_end (values);
  char *argument_p;
  i = 0;
  while (arg_pointer[i] != 0)
    {
      if ((argument_p = strstr (str, identifiers[i])) == NULL)
	{
	  perror ("Required argument not found");
	  ERROR_AND_RETURN (output_fd);
	}
      if (sscanf (argument_p + identifiers_sizes[i],
		  "%" SCNu64, arg_pointer[i]) != 1)
	{
	  perror ("Failed to parse argument");
	  ERROR_AND_RETURN (output_fd);
	}
      i++;
    }
}

void
get_c (char *str, FILE *output_fd)
{
  uint64_t cmin, cmax, m, c, *dividers;
  argument_parser (output_fd, str, 9, &cmin, &cmax, &m, "cmin=", "cmax=",
		   "m=", 5, 5, 2);
  if (is_successful != EXIT_SUCCESS || m <= cmin)
    ERROR_AND_RETURN (output_fd);
  if (cmax > m)
    cmax = m - 1;
  dividers = factor (m);
  for (c = cmin; c <= cmax; c++)
    {
      if (!is_divisible_by_any (c, dividers))
	output (output_fd, c);
    }
  FREE_AND_NULL (dividers);
  nl (output_fd);
}

void
get_a (char *str, FILE *output_fd)
{
  uint64_t m, *dividers, i, goal = 1;
  bool found = false;
  argument_parser (output_fd, str, 3, &m, "m=", 2);
  if (is_successful != EXIT_SUCCESS)
    ERROR_AND_RETURN (output_fd);
  dividers = factor (m);
  if (m % 4 == 0)
    goal *= 2;
  for (i = 0; i < 64 && dividers[i] != 0; i++)
    {
      if (!is_prime (dividers[i]))
	continue;
      found = true;
      goal *= dividers[i];
    }
  FREE_AND_NULL (dividers);
  dividers = NULL;
  if (found)
    {
      output (output_fd, goal);
      nl (output_fd);
    }
  else
    NO_SOLUTION_AND_RETURN (output_fd);
}

void
lcg (char *str, FILE *output_fd)
{
  uint64_t coefficient, value, free_member, modulo, sequence_length;
  argument_parser (output_fd, str, 15, &coefficient, &value, &free_member,
		   &modulo, &sequence_length, "a=", "x0=", "c=", "m=", "n=",
		   2, 3, 2, 2, 2);
  if (is_successful != EXIT_SUCCESS)
    ERROR_AND_RETURN (output_fd);
  if (!is_valid_lcg
      (coefficient, value, free_member, modulo, sequence_length))
    NO_SOLUTION_AND_RETURN (output_fd);
  while (sequence_length-- > 0)
    output (output_fd, value = (coefficient * value + free_member) % modulo);
  nl (output_fd);
}

void
test (char *str, FILE *output_fd)
{
  char name_input_file[10000];
  if (sscanf (str, "test inp=%255s", name_input_file) != 1)
    ERROR_AND_RETURN (output_fd);
  FILE *test_fd = fopen (name_input_file, "r");
  if (test_fd == NULL)
    ERROR_AND_RETURN (output_fd);
  int *nums = NULL, size = 0, capacity = 10, val, max_val = -1e9;
  nums = malloc (capacity * sizeof (int));
  while (fscanf (test_fd, "%d", &val) == 1)
    {
      if (size >= capacity)
	{
	  capacity *= 2;
	  nums = realloc (nums, capacity * sizeof (int));
	}
      nums[size++] = val;
      if (val > max_val)
	max_val = val;
    }
  fclose (test_fd);
  max_val++;
  if (size == 0)
    {
      free (nums);
      ERROR_AND_RETURN (output_fd);
    }
  float sum = 0, E = ((float) size) / ((float) max_val);
  for (int i = 0; i < max_val; i++)
    {
      int temp_num = 0;
      for (int j = 0; j < size; j++)
	{
	  if (i == nums[j])
	    temp_num++;
	}
      if (E > 0)
	sum += (((float) temp_num - E) * ((float) temp_num - E)) / E;
    }
  float df = (float) max_val - 1.0f;
  float threshold = df + 2.0f * sqrtf (df > 0 ? df : 1.0f);
  if (sum < threshold)
    {
      fprintf (output_fd,
	       "Распределение случайно. Параметры:\n");
      fprintf (output_fd, "Значение хи-квадратов: %f\n",
	       sum);
      fprintf (output_fd, "Кол-во чисел: %d\n", size);
      fprintf (output_fd, "Порог случайности: %f\n",
	       threshold);
    }
  else
    {
      fprintf (output_fd,
	       "Распределение НЕ случайно. Параметры:\n");
      fprintf (output_fd, "Значение хи-квадратов: %f\n",
	       sum);
      fprintf (output_fd, "Кол-во чисел: %d\n", size);
      fprintf (output_fd, "Порог случайности: %f\n",
	       threshold);
    }
  free (nums);
}

int
main ()
{
  FILE *input_fd, *output_fd;
  if ((input_fd = fopen ("input.txt", "r")) == NULL)
    {
      perror ("Unable to open input file");
      return ENOENT;
    }
  if ((output_fd = fopen ("output.txt", "a")) == NULL)
    {
      perror ("Unable to open output file");
      return ENOENT;
    }
  char str[10000];
  fgets (str, 10000, input_fd);
  CLOSE_AND_NULL (input_fd);
  if (strstr (str, "get_c") != NULL)
    get_c (str, output_fd);
  else if (strstr (str, "get_a") != NULL)
    get_a (str, output_fd);
  else if (strstr (str, "lcg") != NULL)
    lcg (str, output_fd);
  else if (strstr (str, "test") != NULL)
    test (str, output_fd);
  else
    error (output_fd);
  CLOSE_AND_NULL (output_fd);
  return is_successful;
}
