#include "afl-fuzz-mpi.h"
#include "afl-fuzz.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

afl_mpi_cache_t cache;
afl_mpi_sent_msg_t the_sent_msg_head;

FILE *open_mpi_log() {
  char fname[10];
  const char *base = "log";
  memcpy(fname, base, 3);
  int mpi_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  snprintf(fname + 3, sizeof(int), "%d", mpi_rank);
  // printf("%s\n", fname);
  return fopen(fname, "a");
}

void mpi_write_log(const char *msg) {
  FILE *h = open_mpi_log();
  unsigned char buf[256];
  u64 time = get_cur_time();
  fprintf(h, "%lld ", time);
  memcpy(buf, msg, strlen(msg));
  fwrite(buf, strlen(msg), 1, h);
  fflush(h);
  fclose(h);
}

void mpi_write_log_int(uint64_t val) {
  FILE *h = open_mpi_log();
  fprintf(h, "%ld\n", val);
  fflush(h);
  fclose(h);
}

void mpi_write_log_double(double val) {
  FILE *h = open_mpi_log();
  fprintf(h, "%f\n", val);
  fflush(h);
  fclose(h);
}


/** @brief This is just a wrapper around the real xxhash function.
 * Since we need to trim the hash modulus the size of the MPI_WORLD.
 *
 * @param src   Content to hash
 * @param len   Size of the content
 *
 * @return      Trimmed hash
 * */
uint64_t mpi_hash_wrap(void *src, size_t len, unsigned int range) {
  return ((XXH3_64bits(src, len) % range));
}

uint64_t mpi_multi_hash_wrap(unsigned char *src, size_t len, int start,
                             int end) {
  return ((XXH3_64bits(src, len) % (end - start)) + start);
}

/** @brief Send of the AMMISC_MODE protcol. Here, each node
 *  sends asynchronously each of its messages to the others
 *
 *  @param my_rank MPI rank for this process
 * */
void mpi_ammuina_send(int my_rank, int size) {
  // uint64_t num = 0;
  int i = 0;
  while (i < cache.n_entries) {
    if (cache.mpi_cache[i].used) {
      afl_mpi_sent_msg_t *new_node = calloc(1, sizeof(afl_mpi_sent_msg_t));
      if (new_node == NULL) {
        fprintf(stderr, "OOM while getting queue node for AMMISC_SEND\n");
        exit(EXIT_FAILURE);
      }
      new_node->buf = malloc(cache.mpi_cache[i].len * sizeof(unsigned char));
      if (new_node->buf == NULL) {
        fprintf(stderr, "OOM while getting buffer for AMMISC_SEND\n");
        exit(EXIT_FAILURE);
      }
      memcpy(new_node->buf, cache.mpi_cache[i].buf, cache.mpi_cache[i].len);
      new_node->next = NULL;
      new_node->requests = malloc((size - 1) * sizeof(MPI_Request));
      new_node->n_requests = (size - 1);
      int idx = 0;
      for (int j = 0; j < size; j++) {
        if (j != my_rank) {
          MPI_Isend(new_node->buf, cache.mpi_cache[i].len, MPI_UINT8_T, j, 0,
                    MPI_COMM_WORLD, &(new_node->requests[idx]));
          idx++;
        }
      }
      mpi_sent_elem_insert(new_node);
      cache.mpi_cache[i].used = 0;
      cache.mpi_cache[i].score = 0;
      free(cache.mpi_cache[i].buf);
    }
    i++;
  }
  mpi_write_log("Done sending AMMUINA\n");
}


void save_to_cache(void *buf, size_t len, unsigned long score) {
  uint8_t inserted = 0;
  uint8_t has_cvg_higher = 0;
  for (int i = 0; i < cache.n_entries; i++) {
    // Entry is free
    if (!cache.mpi_cache[i].used) {
      cache.mpi_cache[i].buf = calloc(len, sizeof(unsigned char));
      memcpy(cache.mpi_cache[i].buf, buf, len);
      cache.mpi_cache[i].len = len;
      cache.mpi_cache[i].score = score;
      cache.mpi_cache[i].used = 1;
      inserted = 1;
      break;
    } else {
      if (cache.mpi_cache[i].score <= score)
        has_cvg_higher = 1;
    }
  }
  if (!inserted && has_cvg_higher) {
    mpi_write_log("Reallocing cache\n");
    cache.mpi_cache =
        realloc(cache.mpi_cache,
                (cache.n_entries * 2) *
                    sizeof(afl_mpi_cache_entry_t)); // double up the cache size

    cache.mpi_cache[cache.n_entries].buf = calloc(len, sizeof(unsigned char));
    memcpy(cache.mpi_cache[cache.n_entries].buf, buf, len);
    cache.mpi_cache[cache.n_entries].len = len;
    cache.mpi_cache[cache.n_entries].score = score;
    cache.mpi_cache[cache.n_entries].used = 1;

    for (size_t i = cache.n_entries + 1; i < cache.n_entries * 2; i++) {
      cache.mpi_cache[i].buf = NULL;
      cache.mpi_cache[i].len = 0;
      cache.mpi_cache[i].score = 0;
      cache.mpi_cache[i].used = 0;
    }
    cache.n_entries *= 2;
    mpi_write_log("Inserted new cache entry\n");
  }
  // mpi_write_log("Done with cache\n");
}

/** @brief Initialize the cache used by MPI send and recv to keep interesting
 *  messages that will be forwarded to other nodes when AMMISC mode
 *  is triggered. By default, the initial cache size is 500 entries
 *
 * */
void cache_init() {
  cache.mpi_cache = calloc(CACHE_START_SIZE, sizeof(afl_mpi_cache_entry_t));
  cache.n_entries = CACHE_START_SIZE;
  mpi_write_log("Initialized cache\n");
}

/** @brief During AMMUINA, if the number of saved messages is
 *  greater than a given threshold, realloc the cache by
 *  doubling up its size.
 *
 * */
void realloc_cache() {
  cache.mpi_cache =
      realloc(cache.mpi_cache,
              (cache.n_entries * 2) *
                  sizeof(afl_mpi_cache_entry_t)); // double up the cache size
  for (size_t i = cache.n_entries; i < cache.n_entries * 2; i++) {
    cache.mpi_cache[i].buf = NULL;
    cache.mpi_cache[i].len = 0;
    cache.mpi_cache[i].score = 0;
    cache.mpi_cache[i].used = 0;
  }
  cache.n_entries *= 2;
}

/** @brief Add a message that was sent via MPI_Isend, so that it can accessed
 * 	later on to check if it was actually received and free the corresponding
 * 	buffer. Adding of the new messages happens in tail
 *
 *  @param the_sent_elem	New elem to add
 * */
void mpi_sent_elem_insert(afl_mpi_sent_msg_t *the_sent_elem) {
  if (the_sent_msg_head.next == NULL) {
    the_sent_msg_head.next = the_sent_elem;
  } else {
    afl_mpi_sent_msg_t *the_index = the_sent_msg_head.next;
    while (the_index->next != NULL)
      the_index = the_index->next;
    the_index->next = the_sent_elem;
  }
}

/** @brief Test if messages sent via MPI_Isend were succesfully received,
 *  if so free the corresponding element in the list.
 *
 *  @param the_sent_msg_head	List head
 * */
void mpi_sent_elem_free(afl_mpi_sent_msg_t *the_sent_msg_head) {
  if (the_sent_msg_head->next == NULL) {
    return;
  }

  else {
    afl_mpi_sent_msg_t *idx = the_sent_msg_head->next;

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    while (idx != NULL) {
      int flag = 1;
      int index;
      while (flag) {
        MPI_Testany(idx->n_requests, idx->requests, &index, &flag,
                    MPI_STATUS_IGNORE);
        if (flag && index == MPI_UNDEFINED) {
          idx->removable = 1;
          break;
        }
      }
      idx = idx->next;
    }
  }
  // Now, try to free the requests that completed
  afl_mpi_sent_msg_t *idx = the_sent_msg_head->next;
  afl_mpi_sent_msg_t *prev = the_sent_msg_head;
  while (idx != NULL) {
    if (idx->removable) {
      afl_mpi_sent_msg_t *to_free;
      to_free = idx;
      prev->next = idx->next;
      idx = to_free->next;

      // Free the node
      free(to_free->buf);
      free(to_free);
    } else {
      prev = idx;
      idx = idx->next;
    }
  }
}
