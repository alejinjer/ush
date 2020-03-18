#include "ush.h"

static void print_suspended_job(t_job *job, int *count, int size) {
    ++(*count); // ???? delete
    mx_printchar('[');
    mx_printint(job->index);
    mx_printchar(']');
    if (*count == size)
        mx_printstr("+  ");
    else if (*count == size - 1)
        mx_printstr("-  ");
    else
        mx_printchar_ntimes(' ', 3);
    mx_printstr("Stopped");
    mx_printchar_ntimes(' ', 17);
    mx_printstr_endl(job->cmd);
}

int mx_ush_jobs(char **args, t_ush *ush) {
    t_job *tmp = ush->suspended;
    int size = mx_suspended_jobs_list_size(ush->suspended);
    int count = 0;

    ush->delete_suspended = true;
    if (!size)
        return MX_SUCCESS;
    while (tmp->next)
        tmp = tmp->next;
    while (tmp) {
            print_suspended_job(tmp, &count, size);
        tmp = tmp->prev;
    }
    return MX_SUCCESS;
}