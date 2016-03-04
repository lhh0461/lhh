
void syslog_err(const char *format, ...);
void syslog_debug(const char *format, ...);
void syslog_info(const char *format, ...);
void log_init(const char *program_name, int facility);
void log_release(void);
