#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include "log.h"

#if !defined(__APPLE__) && !defined(DARWIN) && !defined(WIN32)
#include <syscall.h>
#endif
#include <sys/time.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifndef PTHREAD_MUTEX_RECURSIVE 
#define PTHREAD_MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif

typedef struct slog {
    unsigned int n_td_safe:1;
    pthread_mutex_t mutex;
     slog_config config;
} slog_t;

typedef struct slog_context {
    const char *p_format;
     slog_flag e_flag;
    slog_date date;
    uint8_t n_full_color;
    uint8_t n_new_line;
} slog_context;

static slog_t g_slog;

static void slog_sync_init(slog_t *pSlog)
{
    if (!pSlog->n_td_safe) return;
    pthread_mutexattr_t mutexAttr;

    if (pthread_mutexattr_init(&mutexAttr) ||
        pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE) ||
        pthread_mutex_init(&pSlog->mutex, &mutexAttr) ||
        pthread_mutexattr_destroy(&mutexAttr))
    {
        printf("<%s:%d> %s: [ERROR] Can not initialize mutex: %d\n", 
            __FILE__, __LINE__, __FUNCTION__, errno);

        exit(EXIT_FAILURE);
    }
}

static void slog_lock(slog_t *pSlog)
{
    if (pSlog->n_td_safe && pthread_mutex_lock(&pSlog->mutex))
    {
        printf("<%s:%d> %s: [ERROR] Can not lock mutex: %d\n", 
            __FILE__, __LINE__, __FUNCTION__, errno);

        exit(EXIT_FAILURE);
    }
}

static void slog_unlock(slog_t *pSlog)
{
    if (pSlog->n_td_safe && pthread_mutex_unlock(&pSlog->mutex))
    {
        printf("<%s:%d> %s: [ERROR] Can not unlock mutex: %d\n", 
            __FILE__, __LINE__, __FUNCTION__, errno);
                
        exit(EXIT_FAILURE);
    }
}

static const char* slog_get_tag( slog_flag e_flag)
{
    switch (e_flag)
    {
        case SLOG_NOTE: return "note";
        case SLOG_INFO: return "info";
        case SLOG_WARN: return "warn";
        case SLOG_DEBUG: return "debug";
        case SLOG_ERROR: return "error";
        case SLOG_TRACE: return "trace";
        case SLOG_FATAL: return "fatal";
        default: break;
    }

    return NULL;
}

static const char* slog_get_color( slog_flag e_flag)
{
    switch (e_flag)
    {
        case SLOG_NOTE: return SLOG_COLOR_NORMAL;
        case SLOG_INFO: return SLOG_COLOR_GREEN;
        case SLOG_WARN: return SLOG_COLOR_YELLOW;
        case SLOG_DEBUG: return SLOG_COLOR_BLUE;
        case SLOG_ERROR: return SLOG_COLOR_RED;
        case SLOG_TRACE: return SLOG_COLOR_CYAN;
        case SLOG_FATAL: return SLOG_COLOR_MAGENTA;
        default: break;
    }

    return SLOG_COLOR_NORMAL;
}

uint8_t slog_get_usec()
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0) return 0;
    return (uint8_t)(tv.tv_usec / 10000);
}

void slog_get_date(slog_date *p_date)
{
    struct tm timeinfo;
    time_t rawtime = time(NULL);
    #ifdef WIN32
    localtime_s(&timeinfo, &rawtime);
    #else
    localtime_r(&rawtime, &timeinfo);
    #endif

    p_date->n_year = timeinfo.tm_year + 1900;
    p_date->n_month = timeinfo.tm_mon + 1;
    p_date->n_day = timeinfo.tm_mday;
    p_date->n_hour = timeinfo.tm_hour;
    p_date->n_min = timeinfo.tm_min;
    p_date->n_sec = timeinfo.tm_sec;
    p_date->n_usec = slog_get_usec();
}

static uint32_t slog_get_tid()
{
#if defined(__APPLE__) || defined(DARWIN) || defined(WIN32)
    return (uint32_t)pthread_self();
#else
    return syscall(__NR_gettid);
#endif
}

static void slog_create_tag(char *pOut, size_t nSize,  slog_flag e_flag, const char *pColor)
{
     slog_config *p_cfg = &g_slog.config;
    pOut[0] = SLOG_NUL;

    const char *pTag = slog_get_tag(e_flag);
    if (pTag == NULL) return;

    if (p_cfg->color_format != SLOG_COLORING_TAG) snprintf(pOut, nSize, "<%s> ", pTag);
    else snprintf(pOut, nSize, "%s<%s>%s ", pColor, pTag, SLOG_COLOR_RESET);
}

static void slog_create_tid(char *pOut, int nSize, uint8_t n_trace_tid)
{
    if (!n_trace_tid) pOut[0] = SLOG_NUL;
    else snprintf(pOut, nSize, "(%u) ", slog_get_tid());
}

static void slog_display_message(const slog_context *pCtx, const char *pInfo, const char *pInput)
{
    const char *p_reset = pCtx->n_full_color ? SLOG_COLOR_RESET : SLOG_EMPTY;
    const char *p_new_line = pCtx->n_new_line ? SLOG_NEWLINE : SLOG_EMPTY;
    const char *p_message = pInput != NULL ? pInput : SLOG_EMPTY;

     slog_config *p_cfg = &g_slog.config;
    int nCbVal = 1;

    if (p_cfg->log_callback != NULL)
    {
        size_t nLength = 0;
        char *pLog = NULL;

        nLength += asprintf(&pLog, "%s%s%s%s", pInfo, p_message, p_reset, p_new_line);
        if (pLog != NULL)
        {
            nCbVal = p_cfg->log_callback(pLog, nLength, pCtx->e_flag, p_cfg->p_callback_ctx);
            free(pLog);
        }
    }

    if (p_cfg->n_to_screen && nCbVal > 0)
    {
        printf("%s%s%s%s", pInfo, p_message, p_reset, p_new_line);
        if (p_cfg->n_flush) fflush(stdout);
    }

    if (!p_cfg->n_to_file || nCbVal < 0) return;
    const slog_date *p_date = &pCtx->date;

    char s_file_path[SLOG_PATH_MAX + SLOG_NAME_MAX + SLOG_DATE_MAX];
    snprintf(s_file_path, sizeof(s_file_path), "%s/%s-%04d-%02d-%02d.log", 
        p_cfg->s_file_path, p_cfg->s_file_name, p_date->n_year, p_date->n_month, p_date->n_day);

    FILE *pFile = fopen(s_file_path, "a");
    if (pFile == NULL) return;

    fprintf(pFile, "%s%s%s%s", pInfo, p_message, p_reset, p_new_line);
    fclose(pFile);
}

static void slog_create_info(const slog_context *pCtx, char* pOut, size_t nSize)
{
     slog_config *p_cfg = &g_slog.config;
    const slog_date *p_date = &pCtx->date;

    char s_date[SLOG_DATE_MAX + SLOG_NAME_MAX];
    s_date[0] = SLOG_NUL;

    if (p_cfg->date_control == SLOG_TIME_ONLY)
    {
        snprintf(s_date, sizeof(s_date),
            "%02d:%02d:%02d.%03d%s",
            p_date->n_hour,p_date->n_min,
            p_date->n_sec, p_date->n_usec,
            p_cfg->s_separator);
    }
    else if (p_cfg->date_control == SLOG_DATE_FULL)
    {
        snprintf(s_date, sizeof(s_date),
            "%04d.%02d.%02d-%02d:%02d:%02d.%03d%s",
            p_date->n_year, p_date->n_month,
            p_date->n_day, p_date->n_hour,
            p_date->n_min, p_date->n_sec,
            p_date->n_usec, p_cfg->s_separator);
    }

    char sTid[SLOG_TAG_MAX], sTag[SLOG_TAG_MAX];
    const char *pColorCode = slog_get_color(pCtx->e_flag);
    const char *pColor = pCtx->n_full_color ? pColorCode : SLOG_EMPTY;

    slog_create_tid(sTid, sizeof(sTid), p_cfg->n_trace_tid);
    slog_create_tag(sTag, sizeof(sTag), pCtx->e_flag, pColorCode);
    snprintf(pOut, nSize, "%s%s%s%s", pColor, sTid, s_date, sTag); 
}

static void slog_display_heap(const slog_context *pCtx, va_list args)
{
    size_t n_bytes = 0;
    char *p_message = NULL;
    char s_log_info[SLOG_INFO_MAX];

    n_bytes += vasprintf(&p_message, pCtx->p_format, args);
    va_end(args);

    if (p_message == NULL)
    {
        printf("<%s:%d> %s<error>%s %s: Can not allocate memory for input: errno(%d)\n", 
            __FILE__, __LINE__, SLOG_COLOR_RED, SLOG_COLOR_RESET, __FUNCTION__, errno);

        return;
    }

    slog_create_info(pCtx, s_log_info, sizeof(s_log_info));
    slog_display_message(pCtx, s_log_info, p_message);
    if (p_message != NULL) free(p_message);
}

static void slog_display_stack(const slog_context *pCtx, va_list args)
{
    char s_message[SLOG_MESSAGE_MAX];
    char s_log_info[SLOG_INFO_MAX];

    vsnprintf(s_message, sizeof(s_message), pCtx->p_format, args);
    slog_create_info(pCtx, s_log_info, sizeof(s_log_info));
    slog_display_message(pCtx, s_log_info, s_message);
}

void slog_display( slog_flag e_flag, uint8_t n_new_line, const char *p_format, ...)
{
    slog_lock(&g_slog);
     slog_config *p_cfg = &g_slog.config;

    if ((SLOG_FLAGS_CHECK(g_slog.config.n_flags, e_flag)) &&
       (g_slog.config.n_to_screen || g_slog.config.n_to_file))
    {
        slog_context ctx;
        slog_get_date(&ctx.date);

        ctx.e_flag = e_flag;
        ctx.p_format = p_format;
        ctx.n_new_line = n_new_line;
        ctx.n_full_color = p_cfg->color_format == SLOG_COLORING_FULL ? 1 : 0;

        void(*slog_display_args)(const slog_context *pCtx, va_list args);
        slog_display_args = p_cfg->n_use_heap ? slog_display_heap : slog_display_stack;

        va_list args;
        va_start(args, p_format);
        slog_display_args(&ctx, args);
        va_end(args);
    }

    slog_unlock(&g_slog);
}

size_t slog_version(char *pDest, size_t nSize, uint8_t n_min)
{
    size_t nLength = 0;

    /* Version short */
    if (n_min) nLength = snprintf(pDest, nSize, "%d.%d.%d", 
        SLOG_VERSION_MAJOR, SLOG_VERSION_MINOR, SLOG_BUILD_NUM);

    /* Version long */
    else nLength = snprintf(pDest, nSize, "%d.%d build %d (%s)", 
        SLOG_VERSION_MAJOR, SLOG_VERSION_MINOR, SLOG_BUILD_NUM, __DATE__);

    return nLength;
}

void slog_config_get( slog_config *p_cfg)
{
    slog_lock(&g_slog);
    *p_cfg = g_slog.config;
    slog_unlock(&g_slog);
}

void slog_config_set( slog_config *p_cfg)
{
    slog_lock(&g_slog);
    g_slog.config = *p_cfg;
    slog_unlock(&g_slog);
}

void slog_enable( slog_flag e_flag)
{
    slog_lock(&g_slog);

    if (!SLOG_FLAGS_CHECK(g_slog.config.n_flags, e_flag))
        g_slog.config.n_flags |= e_flag;

    slog_unlock(&g_slog);
}

void slog_disable( slog_flag e_flag)
{
    slog_lock(&g_slog);

    if (SLOG_FLAGS_CHECK(g_slog.config.n_flags, e_flag))
        g_slog.config.n_flags &= ~e_flag;

    slog_unlock(&g_slog);
}

void slog_separator_set(const char *p_format, ...)
{
    slog_lock(&g_slog);
     slog_config *p_cfg = &g_slog.config;

    va_list args;
    va_start(args, p_format);

    if (vsnprintf(p_cfg->s_separator, sizeof(p_cfg->s_separator), p_format, args) <= 0)
    {
        p_cfg->s_separator[0] = ' ';
        p_cfg->s_separator[1] = '\0';
    }

    va_end(args);
    slog_unlock(&g_slog);
}

void slog_callback_set( slog_cb callback, void *pContext)
{
    slog_lock(&g_slog);
     slog_config *p_cfg = &g_slog.config;
    p_cfg->p_callback_ctx = pContext;
    p_cfg->log_callback = callback;
    slog_unlock(&g_slog);
}

void slog_init(const char* pName, uint16_t n_flags, uint8_t n_td_safe)
{
    /* Set up default values */
     slog_config *p_cfg = &g_slog.config;
    p_cfg->color_format = SLOG_COLORING_TAG;
    p_cfg->date_control = SLOG_TIME_ONLY;
    p_cfg->p_callback_ctx = NULL;
    p_cfg->log_callback = NULL;
    p_cfg->s_separator[0] = ' ';
    p_cfg->s_separator[1] = '\0';
    p_cfg->s_file_path[0] = '.';
    p_cfg->s_file_path[1] = '\0';
    p_cfg->n_trace_tid = 0;
    p_cfg->n_to_screen = 1;
    p_cfg->n_use_heap = 0;
    p_cfg->n_to_file = 0;
    p_cfg->n_flush = 0;
    p_cfg->n_flags = n_flags;

    const char *pFileName = (pName != NULL) ? pName : SLOG_NAME_DEFAULT;
    snprintf(p_cfg->s_file_name, sizeof(p_cfg->s_file_name), "%s", pFileName);

#ifdef WIN32
    // Enable color support
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOutput, &dwMode);
    dwMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOutput, dwMode);
#endif

    /* Initialize mutex */
    g_slog.n_td_safe = n_td_safe;
    slog_sync_init(&g_slog);
}

void slog_destroy()
{
    g_slog.config.p_callback_ctx = NULL;
    g_slog.config.log_callback = NULL;

    if (g_slog.n_td_safe)
    {
        pthread_mutex_destroy(&g_slog.mutex);
        g_slog.n_td_safe = 0;
    }
}