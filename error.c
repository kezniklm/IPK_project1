/**
 * @file error.c
 * @author Matej Keznikl (xkezni01@stud.fit.vutbr.cz)
 * @brief Implementácia klienta pre IPK Calculator Protokol
 * @date 2023-03-21
 * Prelozene: GCC 11.3.0
 */

#include "error.h"

void warning_msg(const char *fmt, ...)
{
    va_list arguments;

    va_start(arguments, fmt);
    fprintf(stderr, "CHYBA:\n");
    vfprintf(stderr, fmt, arguments);
    va_end(arguments);
}
void error_exit(const char *fmt, ...)
{
    va_list arguments;

    va_start(arguments, fmt);
    fprintf(stderr, "CHYBA:\n");
    vfprintf(stderr, fmt, arguments);
    va_end(arguments);
    exit(EXIT_FAILURE);
}