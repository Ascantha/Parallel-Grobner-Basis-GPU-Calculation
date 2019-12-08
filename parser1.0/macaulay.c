#include <stdio.h>
#include <stdlib.h>
#include "poly.h"
#include "macaulay.h"

int grevlex_mat(struct Mat_Monomial *a, struct Mat_Monomial *b, int d) {
  if (a->total > b->total)
    return 1;
  else if (b->total > a->total)
    return -1;

  for (int i=d-1; i>=0; i--) {
    if (a->exponents[i] < b->exponents[i])
      return 1;
    else if (b->exponents[i] < a->exponents[i])
      return -1;
  }

  return 0;
}

int grlex_mat(struct Mat_Monomial *a, struct Mat_Monomial *b, int d) {
  if (a->total > b->total)
    return 1;
  else if (b->total > a->total)
    return -1;

  for (int i=0; i<d; i++) {
    if (a->exponents[i] > b->exponents[i])
      return 1;
    else if (b->exponents[i] > a->exponents[i])
      return -1;
  }

  return 0;
}

int lex_mat(struct Mat_Monomial *a, struct Mat_Monomial *b, int d) {
  for (int i=0; i<d; i++) {
    if (a->exponents[i] > b->exponents[i])
      return 1;
    else if (b->exponents[i] > a->exponents[i])
      return -1;
  }

  return 0;
}


struct Macaulay *buildMacaulay(struct PolynomialSystem *system, int mono_order) {
  struct Macaulay *matrix = (struct Macaulay *) malloc (sizeof(struct Macaulay));
  matrix->dimension = system->dimension;
  matrix->mono_count = 0;

  matrix->variables = (int *) malloc (sizeof(int)*matrix->dimension);
  for(int i=0; i<matrix->dimension; i++)
    matrix->variables[i] = system->variables[i];

  struct Polynomial * poly = system->head;
  for (int o=0; o<system->size; o++) {
    struct PolyTerm *term = poly->head;
    for (int p=0; p<poly->size; p++) {
      struct Monomial *mono = term->monomial;

      //build Mat_Monomial
      struct Mat_Monomial *mat_mono = (struct Mat_Monomial *) malloc (sizeof(struct Mat_Monomial));
      mat_mono->total = totalDegree(mono);
      mat_mono->exponents = (int *) malloc (sizeof(int)*system->dimension);
      int j, matched;
      for (int i=0; i<system->dimension; i++) {
        matched = 0;
        for (j=0; j<mono->num_vars; j++) {
          if (mono->vars[j]->varNum == system->variables[i]) {
            matched = 1;
            break;
          }
        }
        if (matched == 0)
          mat_mono->exponents[i] = 0;
        else
          mat_mono->exponents[i] = mono->vars[j]->varPow;
      }

      if (matrix->mono_count == 0) {
        matrix->mono_head = mat_mono;
        matrix->mono_tail = mat_mono;
        matrix->mono_count++;
      } else {
        matched = 0;
        struct Mat_Monomial *cmp = matrix->mono_head;
        for(j=0; j<matrix->mono_count; j++) {
          int diff;
          if (mono_order == 0)
            diff = grevlex_mat(mat_mono, cmp, matrix->dimension);
          else if (mono_order == 1)
            diff = grlex_mat(mat_mono, cmp, matrix->dimension);
          else
            diff = lex_mat(mat_mono, cmp, matrix->dimension);

          if (diff == 0) {
            matched = 1;
            break;
          } else if (diff > 0) {
            if (cmp == matrix->mono_head) {
              matrix->mono_head = mat_mono;
            } else {
              cmp->prev->next = mat_mono;
              mat_mono->prev = cmp->prev;
            }
            cmp->prev = mat_mono;
            mat_mono->next = cmp;
            matrix->mono_count++;
            break;
          } else if (cmp == matrix->mono_tail && diff < 0) {
            cmp->next = mat_mono;
            mat_mono->prev = cmp;
            matrix->mono_tail = mat_mono;
            matrix->mono_count++;
            break;
          }

          cmp = cmp->next;
        }
      }

      term = term->next;
    }
    poly = poly->next;
  }

  matrix->m = (float **) malloc (sizeof(float *)*matrix->mono_count);

  return matrix;
}



void printMacaulay(struct Macaulay *matrix) {
  struct Mat_Monomial *mono = matrix->mono_head;
  for (int i=0; i<matrix->mono_count; i++) {

    printf("  ");
    for(int j=0; j<matrix->dimension; j++) {
      if(mono->exponents[j] > 0)
        printf("x%d^%d", matrix->variables[j], mono->exponents[j]);
    }

    mono = mono->next;
  }
}
