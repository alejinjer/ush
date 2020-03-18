#include "ush.h"

static bool check_path(char *line, size_t start_index) {
    char *user_path = NULL;
    char *path = NULL;
    struct stat pstat;
    size_t end_index = strlen(line);

    for (size_t i = start_index; i < strlen(line); i++)
        if (isspace(line[i])) {
            if (i - 1 > 0 && line[i - 1] == '\\')
                continue;
            end_index = i;
        }
    user_path = strndup(line + start_index + 1, end_index);
    path = mx_strjoin("/Users/", user_path);
    mx_strdel(&user_path);
    if (stat(path, &pstat) < 0)
        mx_strdel(&path);
    return path;
}

static char *replace_tilde(t_ush *ush, size_t index, char *postfix) {
    char *tilde = NULL;

    if (postfix[0] == '+') {
        if (strlen(postfix) > 1 && !isspace(postfix[1]))
            return NULL;
        tilde = mx_getenv(ush->hidden, "PWD");
    }
    else if (postfix[0] == '-') {
        if (strlen(postfix) > 1 && !isspace(postfix[1]))
            return NULL;
        tilde = mx_getenv(ush->hidden, "OLDPWD");
    }
    else if (!postfix[0] || isspace(postfix[0]) || postfix[0] == '/')
        tilde = mx_getenv(ush->hidden, "HOME");
    else
        tilde = check_path(ush->in->line, index) ? strdup("/Users/") : NULL;
    return tilde;
}

void mx_expand_tilde(t_ush *ush, size_t index, char *postfix) {
    char *tilde = NULL;
    char *line = ush->in->line;
    char *tmp = mx_strdup(line + index + 1);

    if (index != 0 && !isspace(line[index - 1])) {
        mx_strdel(&tmp);
        return;
    }
    tilde = replace_tilde(ush, index, postfix);
    if (tilde) {
        if (strlen(tilde) + strlen(line) >= malloc_size(line))
            ush->in->line = realloc(line, malloc_size(line) + strlen(tilde));
        memmove(line + index, tilde, strlen(tilde));
        memmove(line + index + strlen(tilde)
                + (tmp[0] == '+' || tmp[0] == '-'), tmp, strlen(tmp));
        mx_strdel(&tilde);
    }
    mx_strdel(&tmp);
}