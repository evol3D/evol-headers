#ifndef EV_HEADERS_LOG_H
#define EV_HEADERS_LOG_H

#define EV_LOG_USE_COLOR

#include "ev_internal.h"

#include "stdarg.h"
#include "stdbool.h"
#include "stdio.h"
#include "time.h"

typedef struct {
  va_list ap;
  const char* fmt;
  const char* file;
  struct tm* time;
  void* udata;
  int line;
  int level;
} ev_log_event_t;

typedef void (*ev_log_log_fn)(ev_log_event_t* ev);
typedef void (*ev_log_lock_fn)(bool lock, void* udata);

typedef enum {
  EV_LOG_TRACE,
  EV_LOG_DEBUG,
  EV_LOG_INFO ,
  EV_LOG_WARN ,
  EV_LOG_ERROR,
  EV_LOG_FATAL,
} ev_log_level;

#define ev_log_trace(...) ev_log(EV_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_debug(...) ev_log(EV_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_info(...)  ev_log(EV_LOG_INFO , __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_warn(...)  ev_log(EV_LOG_WARN , __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_error(...) ev_log(EV_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_fatal(...) ev_log(EV_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

const char* ev_log_level_string(ev_log_level level);
void ev_log_set_lock(ev_log_lock_fn fn, void* udata);
void ev_log_set_level(ev_log_level level);
void ev_log_set_quiet(bool enable);
i32 ev_log_add_callback(ev_log_log_fn fn, void* udata, ev_log_level level);
i32 ev_log_add_fp(FILE *fp, ev_log_level level);

void ev_log(ev_log_level level, const char* file, u32 line, const char* fmt, ...);

#ifdef EV_LOG_IMPLEMENTATION
#undef EV_LOG_IMPLEMENTATION

#define MAX_CALLBACKS 32

typedef struct {
  ev_log_log_fn fn;
  void* udata;
  ev_log_level level;
} ev_log_callback_t;

struct {
  void* udata;
  ev_log_lock_fn lock;
  ev_log_level level;
  bool quiet;
  ev_log_callback_t callbacks[MAX_CALLBACKS];
} G;

static const char* level_strings[] = {
  "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
};

static const char* level_colors[] = {
  "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"
};

static void ev_log_stdout_callback(ev_log_event_t* ev)
{
  char buf[16];
  buf[strftime(buf, sizeof(buf), "%H:%M:%S", ev->time)] = '\0';
#ifdef EV_LOG_USE_COLOR
  fprintf(
    ev->udata, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ",
    buf, level_colors[ev->level], level_strings[ev->level],
    ev->file, ev->line);
#else
  fprintf(
    ev->udata, "%s %-5s %s:%d: ",
    buf, level_strings[ev->level], ev->file, ev->line);
#endif
  vfprintf(ev->udata, ev->fmt, ev->ap);
  fprintf(ev->udata, "\n");
  fflush(ev->udata);
}

static void ev_log_file_callback(ev_log_event_t* ev) 
{
  char buf[64];
  buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
  fprintf(
    ev->udata, "%s %-5s %s:%d: ",
    buf, level_strings[ev->level], ev->file, ev->line);
  vfprintf(ev->udata, ev->fmt, ev->ap);
  fprintf(ev->udata, "\n");
  fflush(ev->udata);
}

static void lock(void)
{
  if(G.lock)
    G.lock(true, G.udata);
}

static void unlock(void)
{
  if(G.lock)
    G.lock(false, G.udata);
}

const char* ev_log_level_string(ev_log_level level) 
{
  return level_strings[level];
}

void ev_log_set_lock(ev_log_lock_fn fn, void* udata) 
{
  G.lock = fn;
  G.udata = udata;
}


void ev_log_set_level(ev_log_level level) 
{
  G.level = level;
}

void ev_log_set_quiet(bool enable) 
{
  G.quiet = enable;
}

i32 ev_log_add_callback(ev_log_log_fn fn, void* udata, ev_log_level level) 
{
  for (i32 i = 0; i < MAX_CALLBACKS; i++) {
    if (!G.callbacks[i].fn) {
      G.callbacks[i] = (ev_log_callback_t) { fn, udata, level };
      return 0;
    }
  }
  return -1;
}


i32 log_add_fp(FILE *fp, ev_log_level level) 
{
  return ev_log_add_callback(ev_log_file_callback, fp, level);
}


static void init_event(ev_log_event_t* ev, void* udata) 
{
  if (!ev->time) {
    time_t t = time(NULL);
    ev->time = localtime(&t);
  }
  ev->udata = udata;
}

void ev_log(ev_log_level level, const char* file, u32 line, const char* fmt, ...) 
{
  ev_log_event_t ev = {
    .fmt   = fmt,
    .file  = file,
    .line  = line,
    .level = level,
  };

  lock();

  if (!G.quiet && level >= G.level) {
    init_event(&ev, stderr);
    va_start(ev.ap, fmt);
    ev_log_stdout_callback(&ev);
    va_end(ev.ap);
  }

  for (i32 i = 0; i < MAX_CALLBACKS && G.callbacks[i].fn; i++) {
    ev_log_callback_t* cb = &G.callbacks[i];
    if (level >= cb->level) {
      init_event(&ev, cb->udata);
      va_start(ev.ap, fmt);
      cb->fn(&ev);
      va_end(ev.ap);
    }
  }

  unlock();
}

#endif
#endif
