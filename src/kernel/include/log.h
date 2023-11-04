void debug_info(const char *file, i32 line, const char *fmt, ...);
void trace_info(const char *file, i32 line, const char *fmt, ...);
void info_info(const char *file, i32 line, const char *fmt, ...);
void warn_info(const char *file, i32 line, const char *fmt, ...);
void error_info(const char *file, i32 line, const char *fmt, ...);

#define debug(fmt, ...) debug_info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define trace(fmt, ...) trace_info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define info(fmt, ...) info_info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define warn(fmt, ...) warn_info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define error(fmt, ...) error_info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)