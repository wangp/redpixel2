#ifndef __included_svintern_h
#define __included_svintern_h


typedef char *string_t;

#define string_init(var)	(var = NULL)
#define string_set(var,str)	({ free (var); var = ustrdup (str); })
#define string_free(var)	({ free (var); var = NULL; })


typedef struct server_interface {
    void(*init) (void);
    void (*shutdown) (void);
    void (*add_log) (const char *prefix, const char *text);
    void (*set_status) (const char *text);
    const char *(*poll) (void);
} server_interface_t;


typedef enum {
    /* Keep this in sync with server_state_procs in server.c. */
    SERVER_STATE_LOBBY,
    SERVER_STATE_GAME,
    SERVER_STATE_QUIT
} server_state_t;

typedef struct server_state_procs {
    int (*init) (void);
    void (*poll) (void);
    void (*shutdown) (void);
} server_state_procs_t;


extern string_t server_current_map_file;
extern string_t server_next_map_file;


void server_log (const char *fmt, ...);

void server_set_next_state (server_state_t state);
void server_interface_set_status (const char *text);


#endif
