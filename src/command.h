#ifndef __COMMAND_H
#define __COMMAND_H

#define ARRAY_CHUNK_SIZE 1024

#include "mbuf.h"
#include "parser.h"

#define CMD_MAP_LEN 1024

struct context;

enum {
    CMD_REQ,
    CMD_REP,

    CMD_ERR,
    CMD_ERR_MOVED,
    CMD_ERR_ASK,
};

STAILQ_HEAD(cmd_tqh, command);

struct command {
    STAILQ_ENTRY(command) cmd_next;
    STAILQ_ENTRY(command) ready_next;
    STAILQ_ENTRY(command) waiting_next;
    STAILQ_ENTRY(command) retry_next;
    STAILQ_ENTRY(command) sub_cmd_next;
    struct mhdr buf_queue;
    struct mhdr rep_queue;
    struct context *ctx;
    struct reader *reader;
    int parse_done;
    int cmd_done;
    int cmd_fail;
    int cmd_done_count;
    int cmd_fail_count;

    int slot;
    int cmd_type;
    int request_type;

    int cmd_count;
    struct cmd_tqh sub_cmds;
    struct command *parent;

    struct redis_data *req_data;
    struct redis_data *rep_data;

    struct buf_ptr req_buf[2];
    struct buf_ptr rep_buf[2];

    struct connection *client;
    struct connection *server;
};

struct redirect_info {
    uint16_t slot;
    char *addr;
    int type;
};

struct iov_data {
    struct iovec *data;
    void *ptr;
    int len;
    size_t max_size;
};


void cmd_queue_init(struct cmd_tqh *cmd_queue);
struct command *cmd_get_lastest(struct context *ctx, struct cmd_tqh *q);
int cmd_read(struct command *cmd, int fd, int type);
int cmd_read_reply(struct command *cmd, struct connection *server);
int cmd_read_request(struct command *cmd, int fd);
void init_command_map();
void cmd_mark_done(struct command *cmd);
void cmd_parse_redirect(struct command *cmd, struct redirect_info *info);
void cmd_make_iovec(struct command *cmd, struct iov_data *iov);
void cmd_mark_fail(struct command *cmd);
void cmd_free(struct command *cmd);
struct command *cmd_create(struct context *ctx);
void cmd_create_iovec(struct command *cmd, struct buf_ptr *start,
        struct buf_ptr *end, struct iov_data *iov);

#endif /* end of include guard: __COMMAND_H */
