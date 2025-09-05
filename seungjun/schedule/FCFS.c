#include <stdio.h>
#include <stdlib.h>

typedef struct process
{
    int pid;
    int run_time;
    int arrive_time;
    struct process *next;
} Process;

typedef struct _queue
{
    Process *head;
    Process *tail;
    int size;
} Queue;

Queue *init_queue();
void enqueue_ps(Process *ps, Queue *q);
Process *dequeue_ps(Queue *q);
void execute_ps(Queue *q);

int main()
{
    Queue *q = init_queue();

    // 프로세스 3개 생성
    Process *p1 = malloc(sizeof(Process));
    p1->pid = 1;
    p1->run_time = 24;
    p1->arrive_time = 0;

    Process *p2 = malloc(sizeof(Process));
    p2->pid = 2;
    p2->run_time = 3;
    p2->arrive_time = 1;

    Process *p3 = malloc(sizeof(Process));
    p3->pid = 3;
    p3->run_time = 3;
    p3->arrive_time = 2;

    // 큐에 추가 후 실행
    enqueue_ps(p1, q);
    enqueue_ps(p2, q);
    enqueue_ps(p3, q);

    while (q->size > 0)
    {
        execute_ps(q);
    }

    free(q);
    return 0;
}

Queue *init_queue()
{
    Queue *q = malloc(sizeof(Queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;

    return q;
}

void enqueue_ps(Process *ps, Queue *q)
{
    ps->next = NULL;

    if (q->head == NULL)
    {
        q->head = ps;
        q->tail = ps;
        q->size++;
        return;
    }

    q->tail->next = ps;
    q->tail = ps;
    q->size++;
    return;
}

Process *dequeue_ps(Queue *q)
{
    if (q->head == NULL)
    {
        return NULL;
    }

    Process *ps = q->head;
    if (q->head == q->tail)
    {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
        return ps;
    }

    q->head = ps->next;
    q->size--;
    return ps;
}

void execute_ps(Queue *q)
{
    Process *ps = dequeue_ps(q);
    if (ps)
    {
        printf("실행 pid %d, 시간: %d\n", ps->pid, ps->run_time);
        free(ps);
    }

    return;
}
