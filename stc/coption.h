// ### BEGIN_FILE_INCLUDE: coption.h
#ifndef COPTION_H_INCLUDED
#define COPTION_H_INCLUDED

#include <string.h>
#include <stdbool.h>

typedef enum {
    coption_no_argument,
    coption_required_argument,
    coption_optional_argument
} coption_type;

typedef struct {
    const char *name;
    coption_type type;
    int val;
} coption_long;

typedef struct {
    int ind;            /* equivalent to optind */
    int opt;            /* equivalent to optopt */
    const char *optstr; /* points to the option string */
    const char *arg;    /* equivalent to optarg */
    int _i, _pos, _nargs;
    char _optstr[4];
} coption;

static inline coption coption_init(void) {
     coption opt = {1, 0, NULL, NULL, 1, 0, 0, {'-', '?', '\0'}};
     return opt;
}

/* move argv[j] over n elements to the left */
static void coption_permute_(char *argv[], int j, int n) {
    int k;
    char *p = argv[j];
    for (k = 0; k < n; ++k)
        argv[j - k] = argv[j - k - 1];
    argv[j - k] = p;
}

static int coption_get(coption *opt, int argc, char *argv[],
                       const char *shortopts, const coption_long *longopts) {
    int optc = -1, i0, j, posixly_correct = (shortopts && shortopts[0] == '+');
    if (!posixly_correct) {
        while (opt->_i < argc && (argv[opt->_i][0] != '-' || argv[opt->_i][1] == '\0'))
            ++opt->_i, ++opt->_nargs;
    }
    opt->opt = 0, opt->optstr = NULL, opt->arg = NULL, i0 = opt->_i;
    if (opt->_i >= argc || argv[opt->_i][0] != '-' || argv[opt->_i][1] == '\0') {
        opt->ind = opt->_i - opt->_nargs;
        return -1;
    }
    if (argv[opt->_i][0] == '-' && argv[opt->_i][1] == '-') { /* "--" or a long option */
        if (argv[opt->_i][2] == '\0') { /* a bare "--" */
            coption_permute_(argv, opt->_i, opt->_nargs);
            ++opt->_i, opt->ind = opt->_i - opt->_nargs;
            return -1;
        }
        optc = '?', opt->_pos = -1;
        if (longopts) { /* parse long options */
            int k, n_exact = 0, n_partial = 0;
            const coption_long *o = 0, *o_exact = 0, *o_partial = 0;
            for (j = 2; argv[opt->_i][j] != '\0' && argv[opt->_i][j] != '='; ++j) {} /* find the end of the option name */
            for (k = 0; longopts[k].name != 0; ++k)
                if (strncmp(&argv[opt->_i][2], longopts[k].name, (size_t)(j - 2)) == 0) {
                    if (longopts[k].name[j - 2] == 0) ++n_exact, o_exact = &longopts[k];
                    else ++n_partial, o_partial = &longopts[k];
                }
            opt->optstr = argv[opt->_i];
            if (n_exact > 1 || (n_exact == 0 && n_partial > 1)) return '?';
            o = n_exact == 1? o_exact : n_partial == 1? o_partial : 0;
            if (o) {
                opt->opt = optc = o->val;
                if (o->type != coption_no_argument) {
                    if (argv[opt->_i][j] == '=') 
                        opt->arg = &argv[opt->_i][j + 1];
                    else if (argv[opt->_i][j] == '\0' && opt->_i < argc - 1 && (o->type == coption_required_argument ||
                                                                                argv[opt->_i + 1][0] != '-'))
                        opt->arg = argv[++opt->_i];
                    else if (o->type == coption_required_argument)
                        optc = ':'; /* missing option argument */
                }
            }
        }
    } else if (shortopts) { /* a short option */
        const char *p;
        if (opt->_pos == 0) opt->_pos = 1;
        optc = opt->opt = argv[opt->_i][opt->_pos++];
        opt->_optstr[1] = optc, opt->optstr = opt->_optstr;
        p = strchr(shortopts, optc);
        if (p == 0) {
            optc = '?'; /* unknown option */
        } else if (p[1] == ':') {
            if (argv[opt->_i][opt->_pos] != '\0')
                opt->arg = &argv[opt->_i][opt->_pos];
            else if (opt->_i < argc - 1 && (p[2] != ':' || argv[opt->_i + 1][0] != '-')) 
                opt->arg = argv[++opt->_i];
            else if (p[2] != ':')
                optc = ':';
            opt->_pos = -1;
        }
    }
    if (opt->_pos < 0 || argv[opt->_i][opt->_pos] == 0) {
        ++opt->_i, opt->_pos = 0;
        if (opt->_nargs > 0) /* permute */
            for (j = i0; j < opt->_i; ++j)
                coption_permute_(argv, j, opt->_nargs);
    }
    opt->ind = opt->_i - opt->_nargs;
    return optc;
}

#endif
// ### END_FILE_INCLUDE: coption.h
