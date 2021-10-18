// This file implements a C Layer Wrapper around the Edge Impulse functions
// Done by Manzel Seet (Oct 2021)

#include "FreeRTOS.h"
#include "task.h"
#include <stdarg.h>
#include "edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "edge-impulse-sdk/tensorflow/lite/micro/kernels/fully_connected.h"

using namespace ei;

extern "C" {
	#include "ei_c_wrapper.h"
}

// Porting

__attribute__((weak)) EI_IMPULSE_ERROR ei_run_impulse_check_canceled() {
    return EI_IMPULSE_OK;
}

__attribute__((weak)) EI_IMPULSE_ERROR ei_sleep(int32_t time_ms) {
    vTaskDelay(time_ms);
    return EI_IMPULSE_OK;
}

uint64_t ei_read_timer_ms() {
    return (uint64_t) xTaskGetTickCount();
}

uint64_t ei_read_timer_us() {
    return ei_read_timer_ms() * 1000;
}

__attribute__((weak)) void ei_printf(const char *format, ...) {
    va_list myargs;
    va_start(myargs, format);
    vprintf(format, myargs);
    va_end(myargs);
}

__attribute__((weak)) void ei_printf_float(float f) {
    float n = f;

    static double PRECISION = 0.00001;
    static int MAX_NUMBER_STRING_SIZE = 32;

    char s[MAX_NUMBER_STRING_SIZE];

    if (n == 0.0) {
        strcpy(s, "0");
    }
    else {
        int digit, m;
        char *c = s;
        int neg = (n < 0);
        if (neg) {
            n = -n;
        }
        // calculate magnitude
        m = log10(n);
        if (neg) {
            *(c++) = '-';
        }
        if (m < 1.0) {
            m = 0;
        }
        // convert the number
        while (n > PRECISION || m >= 0) {
            double weight = pow(10.0, m);
            if (weight > 0 && !isinf(weight)) {
                digit = floor(n / weight);
                n -= (digit * weight);
                *(c++) = '0' + digit;
            }
            if (m == 0 && n > 0) {
                *(c++) = '.';
            }
            m--;
        }
        *(c) = '\0';
    }


    ei_printf("%s", s);
}

__attribute__((weak)) void *ei_malloc(size_t size) {
    return malloc(size);
}

__attribute__((weak)) void *ei_calloc(size_t nitems, size_t size) {
    return calloc(nitems, size);
}

__attribute__((weak)) void ei_free(void *ptr) {
    free(ptr);
}

#if defined(__cplusplus) && EI_C_LINKAGE == 1
extern "C"
#endif
__attribute__((weak)) void DebugLog(const char* s) {
    ei_printf("%s", s);
}

// C Wrappers

void vprint(const char *fmt, va_list argp) {
	char string[200];
	if(0 < vsprintf(string, fmt, argp)) // build string
	{
		printf(string);
	}
}

extern "C" void ei_c_wrapper_printf(const char *format, ...) {
	va_list myargs;
	va_start(myargs, format);
	vprint(format, myargs);
	va_end(myargs);
}

extern "C" void ei_c_wrapper_printf_float(float f) {
	ei_printf_float(f);
}

EI_IMPULSE_ERROR ei_c_wrapper_run_classifier(
		size_t total_length,
		int (*get_data)(size_t, size_t, float *),
	    ei_impulse_result_t *result,
	    bool debug) {

	signal_t signal;
	signal.total_length = total_length;
	signal.get_data = get_data;

    EI_IMPULSE_ERROR res = run_classifier(&signal, result, debug);

    return res;

}
