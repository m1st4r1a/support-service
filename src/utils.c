#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define FACTORS_SIZE 64
uint64_t *
factor (uint64_t number)
{
  uint64_t *factors = calloc (FACTORS_SIZE, sizeof (uint64_t)), factors_p =
    0, divider = 3;
  if (number % 2 == 0)
    {
      factors[factors_p++] = 2;
      while (number % 2 == 0)
	number /= 2;
    }
  while (number > 1)
    {
      if (number % divider != 0)
	{
	  divider += 2;
	  continue;
	}
      factors[factors_p] = divider;
      while (number % divider == 0)
	number /= divider;
      factors_p++;
      divider += 2;
    }
  return factors;
}

bool
is_prime (uint64_t number)
{
  if (number < 2)
    return false;
  if (number == 2)
    return true;
  uint64_t border = (number >> 1) + 1, divider;
  for (divider = 2; divider <= border; divider++)
    {
      if (number % divider == 0)
	return false;
    }
  return true;
}

bool
is_divisible_by_any (uint64_t number, uint64_t *factors)
{
  uint64_t factors_p = 0;
  for (; factors_p < FACTORS_SIZE && factors[factors_p] != 0; factors_p++)
    {
      if (number % factors[factors_p] == 0)
	return true;
    }
  return false;
}

void
error (FILE *output_fd)
{
  if (fputs ("incorrect command\n", output_fd) == EOF)
    perror ("Failed to report incorrected command message to output");
}

void
no_solution (FILE *output_fd)
{
  if (fputs ("no solution", output_fd) == EOF)
    perror ("Failed to report no solution message to output");
}

bool
is_valid_lcg (uint64_t coefficient, uint64_t value, uint64_t free_member,
	      uint64_t modulo, uint64_t sequence_length)
{
  if (coefficient >= modulo || free_member >= modulo || value >= modulo)
    return false;
  if (sequence_length == 0 || modulo <= 1)
    return false;
  return true;
}

void
output (FILE *output_fd, uint64_t element)
{
  if (fprintf (output_fd, "%" PRIu64 " ", element) == EOF)
    perror ("Failed to output a number");
}

void
nl (FILE *output_fd)
{
  if (fputs ("", output_fd) == EOF)
    perror ("Failed to print a newline");
}
