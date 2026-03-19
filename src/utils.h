#ifndef UTILS_H
#define UTILS_H
uint64_t *factor(uint64_t number);
bool is_divisible_by_any(uint64_t number, uint64_t * factors);
bool is_prime(uint64_t number);
bool is_valid_lcg (uint64_t coefficient, uint64_t value, uint64_t free_member,
		uint64_t modulo, uint64_t sequence_length);
void nl(FILE * output_fd);
void error(FILE * output_fd);
void output (FILE *output_fd, uint64_t element);
void no_solution(FILE *output_fd);
#endif
