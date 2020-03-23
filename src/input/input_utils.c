#include "ush.h"

void mx_init_line(t_ush *ush) {
    if (ush->in->line)
        mx_strdel(&ush->in->line);
    ush->in->line = mx_memalloc(1024);
    ush->in->key = 0;
    ush->in->cur_x = 0;
    ush->in->win_x = 5;
    ush->in->cur_y = 1;
}

void mx_shuffle_text(t_input *in) {
    if (in->key == MX_BACKSPACE) {
        for (size_t i = in->cur_x; i <= strlen(in->line); i++)
            in->line[i] = in->line[i + 1];
    } else {
        for (size_t i = strlen(in->line); i > in->cur_x; i--)
            in->line[i] = in->line[i - 1];
        in->line[in->cur_x] = in->key;
    }
    in->line[strlen(in->line)] = 0;
}

void mx_update_cursor(t_input *in) {
    int j = 0;

    tputs(tgetstr("cr", NULL), 1, mx_printnbr);
    for (int i = 0; i < in->win_x; i++)
        tputs(tgetstr("nd", NULL), 1, mx_printnbr);
    if (((strlen(in->line) + 4) / in->winsize) + 1 > in->cur_y)
        while (j++ < ((strlen(in->line) + 4) / in->winsize + 1 - in->cur_y))
            tputs(tgetstr("up", NULL), 1, mx_printnbr);
    if (in->win_x == in->winsize) {
        tputs(tgetstr("do", NULL), 1, mx_printnbr);
        tputs(tgetstr("cr", NULL), 1, mx_printnbr);
        ++in->cur_y;
        in->win_x = 0;
    }
}
