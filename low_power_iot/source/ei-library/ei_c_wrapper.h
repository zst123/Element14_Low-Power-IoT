#if defined(__cplusplus)
extern "C" {

#else

// prevent including "model-parameters/model_metadata.h"
// which will lead to compile error for multiple variable definitions
#define _EI_CLASSIFIER_MODEL_METADATA_H_
#define EI_CLASSIFIER_LABEL_COUNT (3) // From model_metadata.h

#include <stdbool.h>
#include "edge-impulse-sdk/classifier/ei_classifier_types.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"

#endif // defined(__cplusplus)

void ei_c_wrapper_printf(const char *format, ...);
void ei_c_wrapper_printf_float(float f);

EI_IMPULSE_ERROR ei_c_wrapper_run_classifier(
		size_t total_length,
		int (*get_data)(size_t, size_t, float *),
	    ei_impulse_result_t *result,
	    bool debug);

#if defined(__cplusplus)
}
#endif // defined(__cplusplus)
