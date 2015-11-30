/*
IMPORTANT: This program must be linked with libstdc++.
Example GCC command line:
  gcc -o lbc -Wall -O2 -I./latbuilder/include -L./latbuilder/lib latbuilder_capi.c -llatbuilder -llatcommon -lstdc++
*/

#include <latbuilder/capi.h>
#include <stdio.h>

void report(const latbuilder_result* result)
{
  printf("             points: %lu\n", latbuilder_result_get_num_points(result));
  printf("          dimension: %zu\n", latbuilder_result_get_dimension(result));
  printf("  generating vector: ");
  for (size_t i = 0; i < latbuilder_result_get_dimension(result); i++)
    printf("%s%lu", i == 0 ? "" : ", ", latbuilder_result_get_gen(result)[i]);
  printf("\n");
  printf("              merit: %f\n", latbuilder_result_get_merit(result));
  printf("           CPU time: %f s\n", latbuilder_result_get_cpu_seconds(result));
}

int main()
{
  printf("Lattice Builder version: %s\n", latbuilder_get_version_string());

  const char* construction = "CBC";
  const char* size = "2^10";
  const char* dimension = "5";
  const char* norm = "2";
  const char* figure = "CU:P2";
  const char* weights[] = {"product:1.0"};
  const char* mlfilt[] = {"norm:P2-SL10:even:1,10"};

  latbuilder_status status;
  latbuilder_result* result;

  printf("\nORDINARY:\n\n");
  status = latbuilder_search_ordinary_str(construction, size, dimension, norm, figure, 1, weights, norm, 0, NULL, &result);

  if (status == LATBUILDER_ERROR)
    printf("search error: %s\n", latbuilder_get_error_string());
  else
    report(result);

  printf("\nEMBEDDED:\n\n");
  status = latbuilder_search_embedded_str(construction, size, dimension, norm, figure, 1, weights, norm, 0, NULL, 1, mlfilt, "sum", &result);

  if (status == LATBUILDER_ERROR)
    printf("search error: %s\n", latbuilder_get_error_string());
  else
    report(result);

  return 0;
}
