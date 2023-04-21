#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define err_and_ret(ret, fmt, ...) \
    do { \
        int rerrno = ret; \
        fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
        if (sockfd != -1); \
            close(sockfd); \
        exit(rerrno); \
    } while(0)

#define argv_cur(...) \
    ({ char *__t = argv[0]; argv++; argc--; __t; })
#define argv_next(...) \
    { argv++; argc--; }

/* cmd.c */
extern int cmd_add_proc(int argc, char *argv[], char *payload);
extern int cmd_del_proc(int argc, char *argv[], char *payload);

struct _cmd {
    char *cmd;
    int len;
    int (*handler)(int argc, char *argv[], char *payload);
} lmkd_cmd[] = {
    { "add", sizeof "add" - 1, cmd_add_proc },
    { "del", sizeof "del" - 1, cmd_del_proc },
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

int
main(int argc, char *argv[])
{
    char *sckname;
    char *cmd;
#define BUFSZ 1024
    char payload[BUFSZ];
    int i, nbytes = -1;
    /* sock */
    int sockfd = -1, ret;
    struct sockaddr_un addr;

    if (argc < 3)
        err_and_ret(-1, "Usage: %s <sock path> <cmd> [opts...]", basename(argv[0]));

    argv_next();

    sckname = argv_cur();
    cmd = argv_cur();

    for (i = 0; i < ARRAY_SIZE(lmkd_cmd); i++)
        if (!strncmp(cmd, lmkd_cmd[i].cmd, lmkd_cmd[i].len)) {
            nbytes = lmkd_cmd[i].handler(argc, argv, payload);
            break;
        }

    if (nbytes <= 0)
        err_and_ret(nbytes, "error on lmkd_cmd (%s)", cmd);

    /* open sock and send */
    sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    if (sockfd < 0)
        err_and_ret(errno, "error on socket()");

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, sckname, sizeof(addr.sun_path) - 1);

    ret = connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
        err_and_ret(errno, "error on connect() to %s", sckname);

    ret = send(sockfd, payload, nbytes, 0);
    if (ret < 0)
        err_and_ret(errno, "error on write()");

    /* prevent EPOLLHUP on lmkd */
    sleep(1);

    /* done */
    close(sockfd);

    return 0;
}
