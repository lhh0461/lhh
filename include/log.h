#ifndef __LOG_H__
#define __LOG_H__

void log_error(const char *format, ...);
void log_debug(const char *format, ...);
void log_info(const char *format, ...);
void log_init(const char *program_name, int facility);
void log_release(void);

#endif
