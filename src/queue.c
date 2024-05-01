#include "queue.h"

#include <stdio.h>
#include <stdlib.h>

int empty(struct queue_t *q) {
  if (q == NULL) return 1;
  return (q->size == 0);
}

void enqueue(struct queue_t *q, struct pcb_t *proc) {
  /* TODO: put a new process to queue [q] */
  #ifdef MLQ_SCHED
  if (q != NULL && q->size < MAX_QUEUE_SIZE) {
      q->proc[q->size] = proc;
      ++q->size;
  }
  #else
  for (int i = 0; i < q->size; ++i) {
    if (q->proc[i]->priority <= proc->priority) {
      for (int j = q->size - 1; j >= i; --j)
         q->proc[j + 1] = q->proc[j];

         q->proc[i] = proc;
         ++q->size;
         return;
     }
  }

  q->proc[q->size] = proc;
  ++q->size;
  #endif
}

struct pcb_t *dequeue(struct queue_t *q) {
  /* TODO: return a pcb whose prioprity is the highest
   * in the queue [q] and remember to remove it from q
   * */
  if (empty(q)) return NULL;

  struct pcb_t *p = q->proc[0];
  for (int i = 0; i + 1 < q->size; ++i) q->proc[i] = q->proc[i + 1];

  q->proc[q->size - 1] = NULL;
  --q->size;
  return p;
}
