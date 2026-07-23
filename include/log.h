#ifndef LOG_H
#define LOG_H

typedef enum {
	LOG_DEBUG, // 0
	LOG_INFO,  // 1
	LOG_WARN,  // 2
	LOG_ERROR  // 3
} nivel_log;

void game_log(nivel_log level, const char *mensaje, ...);

#endif
