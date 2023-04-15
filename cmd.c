#include <stdlib.h>

#include "lmkd.h"

/*
 * LMK_PROCPRIO packet payload
 *
 * struct lmk_procprio {
 *     pid_t pid;
 *     uid_t uid;
 *     int oomadj;
 *     enum proc_type ptype;
 * };
 */
int
cmd_add_proc(int argc, char *argv[], char *payload)
{
    struct lmk_procprio pprio;

    if (argc < 3)
        return -1;

    pprio.pid = atoll(argv[0]);
    pprio.uid = atoll(argv[1]);
    pprio.oomadj = atoi(argv[2]);
    pprio.ptype = PROC_TYPE_APP;

    return lmkd_pack_set_procprio((void *)payload, &pprio);
}

/*
 * LMK_PROCREMOVE packet payload
 *
 * struct lmk_procremove {
 *     pid_t pid;
 * };
 */
int
cmd_del_proc(int argc, char *argv[], char *payload)
{
    struct lmk_procremove prmv;

    if (argc < 1)
        return -1;

    prmv.pid = atoll(argv[0]);

    return lmkd_pack_set_procremove((void *)payload, &prmv);
}

