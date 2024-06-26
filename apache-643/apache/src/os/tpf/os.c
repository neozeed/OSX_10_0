/* ====================================================================
 * Copyright (c) 1998-1999 The Apache Group.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * 4. The names "Apache Server" and "Apache Group" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    apache@apache.org.
 *
 * 5. Products derived from this software may not be called "Apache"
 *    nor may "Apache" appear in their names without prior written
 *    permission of the Apache Group.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the Apache Group
 *    for use in the Apache HTTP server project (http://www.apache.org/)."
 *
 * THIS SOFTWARE IS PROVIDED BY THE APACHE GROUP ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE APACHE GROUP OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Apache Group and was originally based
 * on public domain software written at the National Center for
 * Supercomputing Applications, University of Illinois, Urbana-Champaign.
 * For more information on the Apache Group and the Apache HTTP server
 * project, please see <http://www.apache.org/>.
 *
 */

/*
 * This file will include OS specific functions which are not inlineable.
 * Any inlineable functions should be defined in os-inline.c instead.
 */

#include "httpd.h"
#include "http_core.h"
#include "os.h"
#include "scoreboard.h"
#include "http_log.h"
#include "http_conf_globals.h"
#ifdef __PIPE_
#include "ipc.h"
#include "shm.h"
static TPF_FD_LIST *tpf_fds = NULL;
#endif

void *tpf_shm_static_ptr = NULL;

static FILE *sock_fp;

/* Check the Content-Type to decide if conversion is needed */
int ap_checkconv(struct request_rec *r)
{
    int convert_to_ascii;
    const char *type;

    /* To make serving of "raw ASCII text" files easy (they serve faster 
     * since they don't have to be converted from EBCDIC), a new
     * "magic" type prefix was invented: text/x-ascii-{plain,html,...}
     * If we detect one of these content types here, we simply correct
     * the type to the real text/{plain,html,...} type. Otherwise, we
     * set a flag that translation is required later on.
     */

    type = (r->content_type == NULL) ? ap_default_type(r) : r->content_type;

    /* If no content type is set then treat it as (ebcdic) text/plain */
    convert_to_ascii = (type == NULL);

    /* Conversion is applied to text/ files only, if ever. */
    if (type && (strncasecmp(type, "text/", 5) == 0 ||
		 strncasecmp(type, "message/", 8) == 0)) {
	if (strncasecmp(type, ASCIITEXT_MAGIC_TYPE_PREFIX,
                        sizeof(ASCIITEXT_MAGIC_TYPE_PREFIX)-1) == 0){
	    r->content_type = ap_pstrcat(r->pool, "text/",
                   type+sizeof(ASCIITEXT_MAGIC_TYPE_PREFIX)-1, NULL);
            if (r->method_number == M_PUT)
                   ap_bsetflag(r->connection->client, B_ASCII2EBCDIC, 0);
            }

        else
	    /* translate EBCDIC to ASCII */
	    convert_to_ascii = 1;
    }
    else{
           if (r->method_number == M_PUT)
               ap_bsetflag(r->connection->client, B_ASCII2EBCDIC, 0);
               /* don't translate non-text files to EBCDIC */
    }
    /* Enable conversion if it's a text document */
    ap_bsetflag(r->connection->client, B_EBCDIC2ASCII, convert_to_ascii);

    return convert_to_ascii;
}

int tpf_select(int maxfds, fd_set *reads, fd_set *writes, fd_set *excepts, struct timeval *tv)
{
/* We're going to force our way through select.  We're only interested reads and TPF allows
   2billion+ socket descriptors for we don't want an fd_set that big.  Just assume that maxfds-1
   contains the socket descriptor we're interested in.  If it's 0, leave it alone. */

    int sockets[1];
    int no_reads = 0;
    int no_writes = 0;
    int no_excepts = 0;
    int timeout = 0;
    int rv;
    
    if(maxfds) {
        if(tv)
            timeout = tv->tv_sec * 1000 + tv->tv_usec;
        sockets[0] = maxfds-1;
        no_reads++;
    }
        else
        sockets[0] = 0;
        
    ap_check_signals();
    rv = select(sockets, no_reads, no_writes, no_excepts, timeout);
    ap_check_signals();
    
    return rv;

}

int tpf_accept(int sockfd, struct sockaddr *peer, int *paddrlen)
{
    int socks[1];
    int rv;

    ap_check_signals();
    socks[0] = sockfd;
    rv = select(socks, 1, 0, 0, 1000);
    errno = sock_errno();
    if(rv>0) {
        ap_check_signals();
        rv = accept(sockfd, peer, paddrlen);
        errno = sock_errno();
    }    
    return rv;
}
   
/* the getpass function is not usable on TPF */
char *getpass(const char* prompt)
{
    errno = EIO;
    return((char *)NULL);
}

/* fork and exec functions are not defined on
   TPF due to the implementation of tpf_fork() */
 
pid_t fork(void)
{
    errno = ENOSYS;
    return(-1);
}

int execl(const char *path, const char *arg0, ...)
{
    errno = ENOSYS;
    return(-1);
}

int execle(const char *path, const char *arg0, ...)
{
    errno = ENOSYS;
    return(-1);
}

int execve(const char *path, char *const argv[], char *const envp[])
{
    errno = ENOSYS;
    return(-1);
}

int execvp(const char *file, char *const argv[])
{
    errno = ENOSYS;
    return(-1);
}



int ap_tpf_spawn_child(pool *p, int (*func) (void *, child_info *),
                       void *data, enum kill_conditions kill_how,
                       int *pipe_in, int *pipe_out, int *pipe_err,
                       int out_fds[], int in_fds[], int err_fds[])

{

   int                      i, temp_out, temp_in, temp_err, save_errno, pid, result=0;
   int                      fd_flags_out, fd_flags_in, fd_flags_err;
   struct tpf_fork_input    fork_input;
   TPF_FORK_CHILD           *cld = (TPF_FORK_CHILD *) data;
   array_header             *env_arr = ap_table_elts ((array_header *) cld->subprocess_env);
   table_entry              *elts = (table_entry *) env_arr->elts;

   if (func) {
      if (result=func(data, NULL)) {
          return 0;                    /* error from child function */
      }
   }

   if (pipe_out) {
      fd_flags_out = fcntl(out_fds[0], F_GETFD);
      fcntl(out_fds[0], F_SETFD, FD_CLOEXEC);
      temp_out = dup(STDOUT_FILENO);
      fcntl(temp_out, F_SETFD, FD_CLOEXEC);
      dup2(out_fds[1], STDOUT_FILENO);
   }

   if (pipe_in) {
      fd_flags_in = fcntl(in_fds[1], F_GETFD);
      fcntl(in_fds[1], F_SETFD, FD_CLOEXEC);
      temp_in = dup(STDIN_FILENO);
      fcntl(temp_in, F_SETFD, FD_CLOEXEC);
      dup2(in_fds[0], STDIN_FILENO);
   }

   if (pipe_err) {
      fd_flags_err = fcntl(err_fds[0], F_GETFD);
      fcntl(err_fds[0], F_SETFD, FD_CLOEXEC);
      temp_err = dup(STDERR_FILENO);
      fcntl(temp_err, F_SETFD, FD_CLOEXEC);
      dup2(err_fds[1], STDERR_FILENO);
   }

   if (cld->subprocess_env) {
      for (i = 0; i < env_arr->nelts; ++i) {
           if (!elts[i].key)
               continue;
           setenv (elts[i].key, elts[i].val, 1);
       }
   }

   fork_input.program = (const char*) cld->filename;
   fork_input.prog_type = cld->prog_type;
   fork_input.istream = TPF_FORK_IS_BALANCE;
   fork_input.ebw_data_length = 0;
   fork_input.ebw_data = NULL;
   fork_input.parm_data = NULL;


   if ((pid = tpf_fork(&fork_input)) < 0) {
       save_errno = errno;
       if (pipe_out) {
           close(out_fds[0]);
       }
       if (pipe_in) {
           close(in_fds[1]);
       }
       if (pipe_err) {
           close(err_fds[0]);
       }
       errno = save_errno;
       pid = 0;
   }

   if (cld->subprocess_env) {
       for (i = 0; i < env_arr->nelts; ++i) {
            if (!elts[i].key)
                continue;
            unsetenv (elts[i].key);
       }
   }

   if (pipe_out) {
       close(out_fds[1]);
       dup2(temp_out, STDOUT_FILENO);
       close(temp_out);
       fcntl(out_fds[0], F_SETFD, fd_flags_out);
   }

   if (pipe_in) {
       close(in_fds[0]);
       dup2(temp_in, STDIN_FILENO);
       close(temp_in);
       fcntl(in_fds[1], F_SETFD, fd_flags_in);
   }


   if (pipe_err) {
       close(err_fds[1]);
       dup2(temp_err, STDERR_FILENO);
       close(temp_err);
       fcntl(err_fds[0], F_SETFD, fd_flags_err);
   }


   if (pid) {

       ap_note_subprocess(p, pid, kill_how);

       if (pipe_out) {
          *pipe_out = out_fds[0];
       }
       if (pipe_in) {
          *pipe_in = in_fds[1];
       }
       if (pipe_err) {
          *pipe_err = err_fds[0];
       }
   }

   return pid;

}

pid_t os_fork(server_rec *s, int slot)
{
    struct tpf_fork_input fork_input;
    APACHE_TPF_INPUT input_parms;
    int count;
    listen_rec *lr;

    fflush(stdin);
    if (dup2(fileno(sock_fp), STDIN_FILENO) == -1)
        ap_log_error(APLOG_MARK, APLOG_CRIT, s,
        "unable to replace stdin with sock device driver");
    fflush(stdout);
    if (dup2(fileno(sock_fp), STDOUT_FILENO) == -1)
        ap_log_error(APLOG_MARK, APLOG_CRIT, s,
        "unable to replace stdout with sock device driver");
    input_parms.generation = ap_my_generation;
#ifdef SCOREBOARD_FILE
    input_parms.scoreboard_fd = scoreboard_fd;
#else /* must be USE_TPF_SCOREBOARD or USE_SHMGET_SCOREBOARD */
    input_parms.scoreboard_heap = ap_scoreboard_image;
#endif

    lr = ap_listeners;
    count = 0;
    do {
        input_parms.listeners[count] = lr->fd;
        lr = lr->next;
        count++;
    } while(lr != ap_listeners);

    input_parms.slot = slot;
    input_parms.restart_time = ap_restart_time;
    input_parms.shm_static_ptr = tpf_shm_static_ptr;
    input_parms.tpf_fds = tpf_fds;
    fork_input.ebw_data = &input_parms;
    fork_input.program = ap_server_argv0;
    fork_input.prog_type = TPF_FORK_NAME;
    fork_input.istream = TPF_FORK_IS_BALANCE;
    fork_input.ebw_data_length = sizeof(input_parms);
    fork_input.parm_data = "-x";
    return tpf_fork(&fork_input);
}

int os_check_server(char *server) {
    #ifndef USE_TPF_DAEMON
    int rv;
    int *current_acn;
    if((rv = inetd_getServerStatus(server)) == INETD_SERVER_STATUS_INACTIVE)
        return 1;
    else {
        current_acn = (int *)cinfc_fast(CINFC_CMMACNUM);
        if(ecbp2()->ce2acn != *current_acn)
            return 1;
    }
    #endif
    return 0;
}

void os_note_additional_cleanups(pool *p, int sd) {
    char sockfilename[50];
    /* write the socket to file so that TPF socket device driver
       will close socket in case we happen to abend. */
    sprintf(sockfilename, "/dev/tpf.socket.file/%.8X", sd);
    sock_fp = fopen(sockfilename, "r+");
    /* arrange to close on exec or restart */
    ap_note_cleanups_for_file(p, sock_fp);
    fcntl(sd,F_SETFD,FD_CLOEXEC);
}

void os_tpf_child(APACHE_TPF_INPUT *input_parms) {
    tpf_child = 1;
    ap_my_generation = input_parms->generation;
    ap_restart_time = input_parms->restart_time;
    tpf_fds = input_parms->tpf_fds;
    tpf_shm_static_ptr = input_parms->shm_static_ptr;
}

#ifndef __PIPE_

int pipe(int fildes[2])
{
    errno = ENOSYS;
    return(-1);
}

API_EXPORT(piped_log *) ap_open_piped_log(pool *p, const char *program)

{
    fprintf(stderr, "Pipes not supported on this TPF system\n");
    exit (1);
}

#else

void ap_tpf_detach_shared_mem(void *address)
{
    if (*((void **)address)) {
        shmdt(*((void **)address));
        *((void **)address) = NULL;
    }
}

static void *ap_tpf_get_shared_mem(size_t size)
{
    key_t shmkey = IPC_PRIVATE;
    int shmid = -1;
    void *result;

    if ((shmid = shmget(shmkey, size, IPC_CREAT | SHM_R | SHM_W)) == -1) {
        perror("shmget failed in ap_tpf_get_shared_mem funciton");
        exit(1);
    }
#define BADSHMAT ((void *)(-1))
    if ((result = shmat(shmid, 0, 0)) == BADSHMAT) {
        perror("shmat failed in ap_tpf_get_shared_mem");
    }
    if (shmctl(shmid, IPC_RMID, NULL) != 0) {
        perror("shmctl(IPC_RMID) failed in ap_tpf_get_shared_mem");
    }
    if (result == BADSHMAT) {   /* now bailout */
        exit(1);
    }

    return result;
}

int ap_tpf_fd_lookup(enum FILE_TYPE file_type, const char *fname)
/* lookup a fd in the fd inheritance table */
{
    if (tpf_fds) {
        int i;
        TPF_FD_ITEM *fd_item = &tpf_fds->first_item;

        for (i = 1; i <= tpf_fds->nbr_of_items; i++, fd_item++) {
            /* check for an fd with the same type and name */
            if ((file_type == fd_item->file_type) &&
                (strcmp(fname, fd_item->fname) == 0) ) {
                /* we've got a match, check that fd is still open */
                struct stat stbuf;

                if (fstat(fd_item->fd, &stbuf) == 0) {
                    return(fd_item->fd);
                }
                else {
                    /* fd is not open - the entire fd table is suspect */
                    fprintf(stderr, "fstat failed in ap_tpf_fd_lookup "
                                    "for fd %i (filename/pipe to %s): %s\n",
                            fd_item->fd, fname, strerror(errno));
                    ap_tpf_detach_shared_mem(&tpf_fds);
                    return(-1);
                }
            }
        }
    }
    return(-1);
}

void ap_tpf_add_fd(pool *p, int fd, enum FILE_TYPE file_type, const char *fname)
/* add a newly opened fd to the fd inheritance table */
{
    int fname_size;

    if (tpf_child) {
        return; /* no kids allowed */
    }
    if (tpf_fds == NULL) {
        /* get shared memory if necssary */
        tpf_fds = ap_tpf_get_shared_mem((size_t)TPF_FD_LIST_SIZE);
        if (tpf_fds) {
            ap_register_cleanup(p, (void *)&tpf_fds,
                                ap_tpf_detach_shared_mem, ap_null_cleanup);
            tpf_fds->nbr_of_items = 0;
            tpf_fds->next_avail_byte = &tpf_fds->first_item;
            tpf_fds->last_avail_byte = (char *)tpf_fds + TPF_FD_LIST_SIZE;
        }
    }
    /* add fd */
    if (tpf_fds) {
        TPF_FD_ITEM *fd_item;

        /* make sure there's room */
        fname_size = strlen(fname) + 1;
        if (sizeof(TPF_FD_ITEM) + fname_size >
            (char *)tpf_fds->last_avail_byte -
            (char *)tpf_fds->next_avail_byte) {
            fprintf(stderr, "fd inheritance table out of room, increase "
                    "TPF_FD_LIST_SIZE in os.h and recompile Apache\n");
            exit(1);
        }
        /* add the new item */
        fd_item = tpf_fds->next_avail_byte;
        tpf_fds->next_avail_byte = fd_item + 1;
        tpf_fds->last_avail_byte
            = (char *)tpf_fds->last_avail_byte - fname_size;
        fd_item->fname = tpf_fds->last_avail_byte;
        strcpy(fd_item->fname, fname);
        fd_item->fd = fd;
        fd_item->file_type = file_type;
        tpf_fds->nbr_of_items++;
    }
}

API_EXPORT(piped_log *) ap_open_piped_log(pool *p, const char *program)
{
    int log_fd;
    piped_log *pl;

    /* check fd inheritance table to see if this log is already open */
    log_fd = ap_tpf_fd_lookup(PIPE_OUT, program);
    if (log_fd < 0) {
        /* this is a new log - open it */
        FILE *dummy;
        TPF_FORK_CHILD cld;
        cld.filename = (char *)program;
        cld.subprocess_env = NULL;
        cld.prog_type = FORK_NAME;

        if (ap_spawn_child(p, NULL, &cld, kill_after_timeout,
            &dummy, NULL, NULL)) {
            log_fd = fileno(dummy);
            /* add this log to the fd inheritance table */
            ap_tpf_add_fd(p, log_fd, PIPE_OUT, program);
        }
        else {
            perror("ap_spawn_child");
            fprintf(stderr, "Couldn't fork child for piped log process\n");
            exit (1);
        }
    }

    pl = ap_palloc(p, sizeof (*pl));
    pl->p = p;
    pl->fds[1] = log_fd;

    return pl;
}

#endif /* __PIPE_ */

/*   The following functions are used for the tpf specific module called
     mod_tpf_shm_static.  This module is a clone of Apache's mod_mmap_static.
     Because TPF doesn't support the system call mmap(), it is replaced by
     shared memory, but uses the mmap directives, etc.   */

union align{

   /* Types which are likely to have the longest RELEVANT alignment
    * restrictions...     */

   char *cp;
   void (*f) (void);
   long l;
   FILE *fp;
   double d;
};

#define CLICK_SZ (sizeof(union align))
union block_hdr {
    union align a;

    /* Actual header... */

    struct {
        char *endp;
        union block_hdr *next;
        char *first_avail;
 #ifdef POOL_DEBUG
        union block_hdr *global_next;
        struct pool *owning_pool;
 #endif
     } h;
};

struct pool {
    union block_hdr *first;
    union block_hdr *last;
    struct cleanup *cleanups;
    struct process_chain *subprocesses;
    struct pool *sub_pools;
    struct pool *sub_next;
    struct pool *sub_prev;
    struct pool *parent;
    char *free_first_avail;
#ifdef ALLOC_USE_MALLOC
    void *allocation_list;
#endif
#ifdef POOL_DEBUG
    struct pool *joined;
#endif
};

#include "ap_alloc.h"
#define POOL_HDR_CLICKS (1 + ((sizeof(struct pool) - 1) / CLICK_SZ))
#define POOL_HDR_BYTES (POOL_HDR_CLICKS * CLICK_SZ)

pool * ap_get_shared_mem_pool(size_t size)
{
    pool *new_pool;
    union block_hdr *blok;

    blok = (union block_hdr *) ap_tpf_get_shared_mem(size);
    /* if shm fails, it will exit  blok will be valid here */
    memset((char *) blok, '\0', size);
    blok->h.next = NULL;
    blok->h.first_avail = (char *) (blok + 1);
    blok->h.endp = size + blok->h.first_avail;
    new_pool = (pool *) blok->h.first_avail;
    blok->h.first_avail += POOL_HDR_BYTES;
    new_pool->free_first_avail = blok->h.first_avail;
    new_pool->first = new_pool->last = blok;

    return new_pool;
}

int ap_check_shm_space(struct pool *a, int size)
{
    union block_hdr *blok = a->last;
    char *first_avail = blok->h.first_avail;
    char *new_first_avail;

    new_first_avail = first_avail + size;
    if (new_first_avail <= blok->h.endp) {
         return (1);
    }
    else
         return (0);
}
