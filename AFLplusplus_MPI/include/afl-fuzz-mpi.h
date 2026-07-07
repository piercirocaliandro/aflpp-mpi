#pragma once
#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

#include "afl-fuzz-xxhash.h"
#include "afl-fuzz.h"

// #define CACHE_DIM 10

/** MPI cache used to store the top N most interesting messages.
 *  This structure keeps
 *
 *   - the buffer of data
 *   - the length of the data
 *   - the score, using the 63 least significant bits.
 *   	The most significant bit of the score's field is used as `used` value
 * */
typedef struct afl_mpi_cache_entry {
  void *buf;
  size_t len;
  unsigned long score;
  unsigned char used;
} afl_mpi_cache_entry_t;

typedef struct afl_mpi_cache {
  afl_mpi_cache_entry_t *mpi_cache;
  size_t n_entries;
} afl_mpi_cache_t;

// extern afl_mpi_cache_t mpi_cache[CACHE_DIM];
extern afl_mpi_cache_t mpi_cache;

// Stack data structure that will be used by MPI receivers
typedef struct afl_mpi_stack_elem {
  void *buf;
  size_t len;
  int ammuina;
  int rank;
  struct afl_mpi_stack_elem *prev;
} afl_mpi_stack_elem_t;

// typedef struct {
//         afl_mpi_stack_elem_t sp;
// } afl_mpi_stack_t;

extern afl_mpi_stack_elem_t the_stack;

extern void push(void *elem, size_t size, int ammuina, int rank);
extern afl_mpi_stack_elem_t *pop();

// Queue structure that will be used by MPI senders
// typedef struct afl_mpi_queue_elem {
//        void *buf;
//        //MPI_Request request;
//        MPI_Request request[256];
//        //MPI_Request *request;
//        struct afl_mpi_queue_elem *next;
//} afl_mpi_queue_elem_t;

typedef struct afl_mpi_sent_msg {
  void *buf;
  MPI_Request *requests;
  // MPI_Request request;
  struct afl_mpi_sent_msg *next;
  u32 n_requests; // The number of requests associated to this message
  u8 removable;
} afl_mpi_sent_msg_t;

extern afl_mpi_sent_msg_t the_sent_msg_head;

typedef struct mpi_ammuina {
  unsigned long msgs;
  MPI_Request req;
} mpi_ammuina_t;

// typedef struct {
//         afl_mpi_queue_elem_t head;
// } afl_mpi_queue_t;

// extern afl_mpi_queue_elem_t the_queue;

// extern afl_mpi_queue_elem_t *queue_test_and_pop(afl_mpi_queue_elem_t
// **last_seen, afl_mpi_queue_elem_t **prev, u64 is_main_node); extern void
// queue_push(afl_mpi_queue_elem_t *the_queue_elem);

extern void mpi_sent_elem_free(afl_mpi_sent_msg_t *the_sent_msg_head);
extern void mpi_sent_elem_insert(afl_mpi_sent_msg_t *the_sent_elem);

#define MPI_MSG_SIZE 4096
#define CACHE_START_SIZE 1000

// #define SCORE_INIT 0x0000000000000000
// #define ENTRY_FREE 0x7fffffffffffffff
// #define ENTRY_USED 0x8000000000000000

// MPI logging
extern FILE *open_mpi_log();
extern void mpi_write_log(const char *msg);
extern void mpi_write_log_int(uint64_t val);
extern void mpi_write_log_double(double val);
extern uint64_t mpi_hash_wrap(void *src, size_t len, unsigned int change);

// Cache functions
extern void cache_init();
extern void realloc_cache();
extern unsigned long mpi_ammisc_mode_recv(int my_rank, int size);
// extern void mpi_ammisc_mode_send(int my_rank, int size, mpi_ammuina_t *msgs);
extern void mpi_ammuina_send(int my_rank, int size);
extern void save_to_cache(void *buf, size_t len, unsigned long score);
extern uint64_t mpi_hash_wrap_cluster(unsigned char *src, size_t len, int range,
                                      int shift);
extern uint64_t mpi_multi_hash_wrap(unsigned char *src, size_t len, int start,
                                    int end);

#define NODE_TYPE 100
#define MPI_SYNC 101
#define REMOVE_FROM_SEND 102
#define FACIMM_AMMUINA 103

#define NODE_MOPT 23
#define NODE_OLD_QUEUE 24
#define NODE_COE 25
#define NODE_LIN 26
#define NODE_QUAD 27
#define NODE_EXPLORE 28
#define NODE_EXPLOIT 29
#define NODE_RARE 30
#define NODE_FAST 31
#define NODE_OTHER 32

// Heuristics to decide weather to trigger AMMUINA or not
#define DELTA_CVG 20
#define DELTA_TIME 15*60*1000

#define CVG_WINDOW 100
#define CVG_ENTRIES 256
