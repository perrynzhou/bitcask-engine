
#define __LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <pthread.h>

/* SLog version information */
#define SLOG_VERSION_MAJOR      1
#define SLOG_VERSION_MINOR      8
#define SLOG_BUILD_NUM          25

/* Supported colors */
#define SLOG_COLOR_NORMAL       "\x1B[0m"
#define SLOG_COLOR_RED          "\x1B[31m"
#define SLOG_COLOR_GREEN        "\x1B[32m"
#define SLOG_COLOR_YELLOW       "\x1B[33m"
#define SLOG_COLOR_BLUE         "\x1B[34m"
#define SLOG_COLOR_MAGENTA      "\x1B[35m"
#define SLOG_COLOR_CYAN         "\x1B[36m"
#define SLOG_COLOR_WHITE        "\x1B[37m"
#define SLOG_COLOR_RESET        "\033[0m"

/* Trace source location helpers */
#define SLOG_TRACE_LVL1(LINE) #LINE
#define SLOG_TRACE_LVL2(LINE) SLOG_TRACE_LVL1(LINE)
#define SLOG_THROW_LOCATION "[" __FILE__ ":" SLOG_TRACE_LVL2(__LINE__) "] "

/* SLog limits (To be safe while avoiding dynamic allocations) */
#define SLOG_MESSAGE_MAX        8196
#define SLOG_VERSION_MAX        128
#define SLOG_PATH_MAX           2048
#define SLOG_INFO_MAX           512
#define SLOG_NAME_MAX           256
#define SLOG_DATE_MAX           64
#define SLOG_TAG_MAX            32
#define SLOG_COLOR_MAX          16

#define SLOG_FLAGS_CHECK(c, f) (((c) & (f)) == (f))
#define SLOG_FLAGS_ALL          255

#define SLOG_NAME_DEFAULT       "slog"
#define SLOG_NEWLINE            "\n"
#define SLOG_EMPTY              ""
#define SLOG_NUL                '\0'

typedef struct log_date {
    uint16_t n_year;
    uint8_t n_month;
    uint8_t n_day;
    uint8_t n_hour;
    uint8_t n_min;
    uint8_t n_sec;
    uint8_t n_usec;
} slog_date;

uint8_t slog_get_usec();
void slog_get_date(slog_date *pDate);

/* Log level flags */
typedef enum
{
    SLOG_NOTAG = (1 << 0),
    SLOG_NOTE = (1 << 1),
    SLOG_INFO = (1 << 2),
    SLOG_WARN = (1 << 3),
    SLOG_DEBUG = (1 << 4),
    SLOG_TRACE = (1 << 5),
    SLOG_ERROR = (1 << 6),
    SLOG_FATAL = (1 << 7)
} slog_flag;

typedef int(*slog_cb)(const char *p_log, size_t n_len, slog_flag flag, void *p_ctx);

/* Output coloring control flags */
typedef enum
{
    SLOG_COLORING_DISABLE = 0,
    SLOG_COLORING_TAG,
    SLOG_COLORING_FULL
} slog_coloring;

typedef enum
{
    SLOG_TIME_DISABLE = 0,
    SLOG_TIME_ONLY,
    SLOG_DATE_FULL
} slog_date_ctrl;

#define slog(...) \
    slog_display(SLOG_NOTAG, 1, __VA_ARGS__)

#define slogwn(...) \
    slog_display(SLOG_NOTAG, 0, __VA_ARGS__)

#define slog_note(...) \
    slog_display(SLOG_NOTE, 1, __VA_ARGS__)

#define slog_info(...) \
    slog_display(SLOG_INFO, 1, __VA_ARGS__)

#define slog_warn(...) \
    slog_display(SLOG_WARN, 1, __VA_ARGS__)

#define slog_debug(...) \
    slog_display(SLOG_DEBUG, 1, __VA_ARGS__)

#define slog_error(...) \
    slog_display(SLOG_ERROR, 1, __VA_ARGS__)

#define slog_trace(...) \
    slog_display(SLOG_TRACE, 1, SLOG_THROW_LOCATION __VA_ARGS__)

#define slog_fatal(...) \
    slog_display(SLOG_FATAL, 1, SLOG_THROW_LOCATION __VA_ARGS__)

/* Short name definitions */
#define slogn(...) slog_note(__VA_ARGS__)
#define slogi(...) slog_info(__VA_ARGS__)
#define slogw(...) slog_warn(__VA_ARGS__)
#define slogd(...) slog_debug( __VA_ARGS__)
#define sloge(...) slog_error( __VA_ARGS__)
#define slogt(...) slog_trace(__VA_ARGS__)
#define slogf(...) slog_fatal(__VA_ARGS__)

typedef struct log_config {
    slog_date_ctrl date_control;     // Display output with date format
    slog_coloring color_format;      // Output color format control
    slog_cb log_callback;             // Log callback to collect logs
    void* p_callback_ctx;                // Data pointer passed to log callback

    uint8_t n_trace_tid:1;                // Trace thread ID and display in output
    uint8_t n_to_screen:1;                // Enable screen logging
    uint8_t n_use_heap:1;                 // Use dynamic allocation
    uint8_t n_to_file:1;                  // Enable file logging
    uint8_t n_flush:1;                   // Flush stdout after screen log
    uint16_t n_flags;                    // Allowed log level flags

    char s_separator[SLOG_NAME_MAX];     // Separator between info and log
    char s_file_name[SLOG_NAME_MAX];      // Output file name for logs
    char s_file_path[SLOG_PATH_MAX];      // Output file path for logs
} slog_config;

size_t slog_version(char *pDest, size_t nSize, uint8_t n_min);
void slog_config_get(slog_config *pCfg);
void slog_config_set(slog_config *pCfg);

void slog_separator_set(const char *pFormat, ...);
void slog_callback_set(slog_cb callback, void *pContext);

void slog_enable(slog_flag eFlag);
void slog_disable(slog_flag eFlag);

void slog_init(const char* pName, uint16_t n_flags, uint8_t nTdSafe);
void slog_display(slog_flag eFlag, uint8_t nNewLine, const char *pFormat, ...);
void slog_destroy(); // Needed only if the slog_init() function argument nTdSafe > 0

#ifdef __cplusplus
}
#endif

