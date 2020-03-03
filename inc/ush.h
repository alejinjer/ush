#pragma once

// Includes
#include "libmx.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <termcap.h>
#include <termios.h>
#include <stdarg.h>

#include <sys/types.h>
#include <pwd.h>
#include <uuid/uuid.h>

// Constants
#define MX_BUILTINS_COUNT 9
#define MX_USH_TOK_BUFFSIZE 64
#define MX_USH_TOK_DELIM " \t\r\n\a"
#define MX_DEFAULT_PATH "/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin"

#define MX_ENV_FLAGS "iPu"
#define MX_CD_FLAGS "sP"
#define MX_PWD_FLAGS "LP"
#define MX_WHICH_FLAGS "as"
#define MX_ECHO_FLAGS "neE"

#define RIGHT       4414235
#define LEFT        4479771
#define UP          4283163
#define DOWN        4348699
#define ESC         27
#define BACKSPACE   127
#define ENTER       10

#define MX_SUCCESS     0
#define MX_FAILURE     1

// Structures
typedef struct s_hist t_hist;
typedef struct s_ush t_ush;
typedef struct s_token t_token;
typedef struct s_env t_env;
typedef struct s_cmd t_cmd;
typedef struct s_input t_input;
typedef struct s_builtins t_builtins;
typedef struct s_process t_process;
typedef struct s_job t_job;

struct s_process {
    char **argv;
    int status;
    pid_t pid;
    t_process *next;
};

 struct s_job {
    char *cmd;
    pid_t pgid;
    int stdin;
    int stdout;
    int stderr;
    t_process *processes;
};

struct s_builtins {
    int (*mx_ush_cd)(char **, t_ush *);
    int (*mx_ush_pwd)(char **, t_ush *);
    int (*mx_ush_env)(char **, t_ush *);
    int (*mx_ush_echo)(char **, t_ush *);
    int (*mx_ush_exit)(char **, t_ush *);
    int (*mx_ush_bye)(char **, t_ush *);
    int (*mx_ush_export)(char **, t_ush *);
    int (*mx_ush_unset)(char **, t_ush *);
    int (*mx_ush_local)(char **, t_ush *);
};

struct s_input {
    size_t win_x;
    size_t cur_x;
    size_t cur_y;
    char *line;
    int key;
    size_t winsize;
};

struct s_ush {
    t_job *suspended;
    t_dll *trees;
    t_hist *history;
    t_hist *current;
    struct termios savetty;
    char **builtins;
    char **env;
    char **export;
    char **local_variables;
    bool exit;
    t_input *in;
};

struct s_cmd {
    char **args;
};

struct s_env {
    bool i;
    char *P;
    char *u;
    char **name_val;
    char **util;
    char error;
};

struct s_token {
    char *data;
    int type;
};

struct s_hist {
    char *cmd;
    t_hist *next;
    t_hist *prev;
};

typedef enum e_builtins {
    env,
    cd,
    pwd,
    which,
    echo,
    ext
}            t_blt;

typedef enum e_defaults {
    OLDPWD,
    PWD,
    SHLVL,
    PATH,
    TERM,
    HOME
}            t_def;


// Functions
// Core
int mx_ush_loop(t_ush *ush);
int mx_proccess_commands_list(t_ush *ush);
void mx_traverse_and_execute_tree(t_tree *tree, t_ush *ush, int *status);
char *mx_get_line(t_ush *ush);
t_ush *mx_init_shell(void);
void mx_init_terminal_data(void);
void mx_enable_input_mode(t_ush *ush);
void mx_disable_input_mode(t_ush *ush);

// Parsing
char *mx_proccess_escapings(char *line);
t_dll *mx_split_token(char *token);
char **mx_split_cmd(char *cmd);

// Utils
void mx_create_trees(t_ush *ush, char *line);
void mx_create_tree(t_dll *sub_tokens, t_tree **leaf);
void mx_print_inorder_tree(t_tree *tree);
void mx_add_cmd(t_hist **hist, t_hist *node);
t_hist *mx_create_hist_node(char *cmd);
int mx_printnbr(int i);
void mx_choose_error(char **args, char **env);
void mx_set_default(t_ush *ush, int *not_found);
char **mx_process_home(char **arr);
char **mx_split_key_value(char *str);
int mx_check_identifier_validity(char *str, int ravno);
char **add_var(char **export, char *keyword);
char **mx_create_tmp_env(t_ush *ush, char ***args);
void mx_setup_underscore_env_var(t_ush *ush, char *arg);

// Signals
void mx_init_signal(void);
void mx_signal_dfl(void);

// Job control system
t_job *mx_create_job(char *cmd);
void mx_delete_job(t_job **job);
t_process *mx_create_processes(char *cmd);
void mx_delete_processes(t_process **processes);
int mx_launch_job(t_job *job, t_ush *ush, char **env);
int mx_launch_proccess(t_job *job, t_process *procces, int *fd, t_ush *ush);

// Builtins
char **mx_store_flags(char **argv);
char **mx_store_files(char **argv);
int mx_flags_validation(char **flags, t_blt builtin);
int mx_ush_cd(char **args, t_ush *ush);
int mx_ush_pwd(char **args, t_ush *ush);
int mx_ush_env(char **args, t_ush *ush);
int mx_ush_echo(char **args, t_ush *ush);
int mx_ush_exit(char **args, t_ush *ush);
int mx_ush_export(char **args, t_ush *ush);
int mx_ush_unset(char **args, t_ush *ush);
int mx_ush_local(char **args, t_ush *ush);
void mx_unset_invalid_option(char *option);
void mx_export_invalid_option(char *option);
void mx_invalid_identifier(char *cmd, char *identifier);

    // CD

    // ENV
int mx_env(t_env *env, t_ush *ush);
t_env *mx_parse_env(char **args);
void mx_env_illegal_option(char illegal_option);
void mx_option_requires_an_argument(char option);

    // EXPORT
void mx_export(char **arguments, t_ush *ush);

    //UNSET
void mx_unset(t_ush *ush, char **arg);
void mx_unset_invalid_option(char *option);

// Data clearing
void mx_clear_tokens(t_dll **tokens);
void mx_clear_trees(t_ush *ush);
int mx_clean_data(char **tmp_env);
void mx_reset_env_and_clean_data(t_ush *ush, char **tmp,
                                 int (**builtin_func)(char **, t_ush *));

// Errors
void mx_start_proccess_error(char *process_name);
void mx_command_not_found_error(char *command_name);
void mx_no_such_file_or_directory(char *cmd);
