#include "threads/synch.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <stdio.h>
#include <string.h>

static void set_donation_priority(struct lock *lock);
static void remove_donation_priority(struct lock *lock);

static bool sema_priority(const struct list_elem *a, const struct list_elem *b, void *aux);
void sema_init(struct semaphore *sema, unsigned value) // 세마포어 초기화
{
    ASSERT(sema != NULL);

    sema->value = value;
    list_init(&sema->waiters);
}

void sema_down(struct semaphore *sema)
{
    enum intr_level old_level;

    ASSERT(sema != NULL);
    ASSERT(!intr_context());

    old_level = intr_disable();
    while (sema->value == 0) // 세마포어가 할당할 수 없으면
    {
        list_insert_ordered(&sema->waiters, &thread_current()->elem, priority_insert_helper, NULL); // 대기리스트에 삽입
        thread_block();                                                                             // 블락
    }
    sema->value--;
    intr_set_level(old_level);
}

bool sema_try_down(struct semaphore *sema)
{
    enum intr_level old_level;
    bool success;

    ASSERT(sema != NULL);

    old_level = intr_disable();
    if (sema->value > 0)
    {
        sema->value--;
        success = true;
    }
    else
        success = false;
    intr_set_level(old_level);

    return success;
}

void sema_up(struct semaphore *sema) // 깨우기함수
{
    enum intr_level old_level;
    bool should_yield = false;
    ASSERT(sema != NULL);

    old_level = intr_disable();
    if (!list_empty(&sema->waiters))
    {
        struct list_elem *e = list_pop_front(&sema->waiters);
        struct thread *semaup_th = list_entry(e, struct thread, elem);
        thread_unblock(semaup_th);
        if (thread_current()->cur_priority < semaup_th->cur_priority)
        {
            should_yield = true;
        }
    }

    sema->value++;
    intr_set_level(old_level);

    if (should_yield)
    {
        thread_yield();
    }
}

static void sema_test_helper(void *sema_);

/* Self-test for semaphores that makes control "ping-pong"
   between a pair of threads.  Insert calls to printf() to see
   what's going on. */
void sema_self_test(void)
{
    struct semaphore sema[2];
    int i;

    printf("Testing semaphores...");
    sema_init(&sema[0], 0);
    sema_init(&sema[1], 0);
    thread_create("sema-test", PRI_DEFAULT, sema_test_helper, &sema);
    for (i = 0; i < 10; i++)
    {
        sema_up(&sema[0]);
        sema_down(&sema[1]);
    }
    printf("done.\n");
}

/* Thread function used by sema_self_test(). */
static void sema_test_helper(void *sema_)
{
    struct semaphore *sema = sema_;
    int i;

    for (i = 0; i < 10; i++)
    {
        sema_down(&sema[0]);
        sema_up(&sema[1]);
    }
}

void lock_init(struct lock *lock)
{
    ASSERT(lock != NULL);

    lock->holder = NULL;
    sema_init(&lock->semaphore, 1);
}

void lock_acquire(struct lock *lock) // 현재스레드가 lock을 잡는다
{
    ASSERT(lock != NULL);
    ASSERT(!intr_context());
    ASSERT(!lock_held_by_current_thread(lock));
    enum intr_level old_level;

    if (lock->holder != NULL) // lock이 소유가 있으면
    {
        thread_current()->wait_lock = lock; // lock을 기다리고 있다.

        old_level = intr_disable();
        list_insert_ordered(&(lock->holder)->donations, &thread_current()->donation_elem,
                            priority_insert_helper_donation, 0);
        // donation <-> donation_elem 양방향연결

        set_donation_priority(lock); // lock->holder의 priority를 바꿈
        intr_set_level(old_level);
    }
    sema_down(&lock->semaphore); // 양도해준 스레드는 sema_down
    thread_current()->wait_lock = NULL;
    lock->holder = thread_current();
}

static void set_donation_priority(struct lock *lock)
{
    ASSERT(lock != NULL);
    ASSERT(lock->holder != NULL);

    struct thread *holder_th = lock->holder; // 홀더

    if (list_empty(&holder_th->donations)) // 만약 비어있다면
    {
        holder_th->cur_priority = holder_th->init_priority; // 나의 우선순위를 설정
    }
    else
    {
        // 대기리스트의 첫번째스레드
        struct thread *first_th = list_entry(list_front(&holder_th->donations), struct thread, donation_elem);
        if (first_th->cur_priority > holder_th->init_priority) // 만약 첫번째가 나보다 크다면
        {
            holder_th->cur_priority = first_th->cur_priority; // 나의 현재는 첫번째의 우선순위
        }
        else // 그게아니라면
        {
            holder_th->cur_priority = holder_th->init_priority; // 내거 사용
        }
    }

    // donation_list에서 가장 priority가 가장 큰 스레드
}

static void remove_donation_priority(struct lock *lock)
{
    // 순회하면서 wait_lock == lock 이면 아웃!
    ASSERT(lock != NULL);
    ASSERT(lock->holder != NULL);
    struct list_elem *e;
    struct list_elem *next;
    struct list *donations = &lock->holder->donations;
    // ASSERT(list != NULL);
    // ASSERT(elem != NULL);
    // ASSERT(less != NULL);
    if (list_empty(donations))
    {
        return;
    }
    enum intr_level old_level = intr_disable();
    for (e = list_begin(donations); e != list_end(donations); e = next)
    {
        next = list_next(e);
        if (list_entry(e, struct thread, donation_elem)->wait_lock == lock)
        {
            list_remove(e);
        }
    }
    intr_set_level(old_level);
}

/* Tries to acquires LOCK and returns true if successful or false
   on failure.  The lock must not already be held by the current
   thread.

   This function will not sleep, so it may be called within an
   interrupt handler. */
bool lock_try_acquire(struct lock *lock) // 대기리스트 없이 lock획득
{
    bool success;

    ASSERT(lock != NULL);
    ASSERT(!lock_held_by_current_thread(lock));

    success = sema_try_down(&lock->semaphore);
    if (success)
        lock->holder = thread_current();
    return success;
}

void lock_release(struct lock *lock) // lock 해제
{
    ASSERT(lock != NULL);
    ASSERT(lock_held_by_current_thread(lock));
    remove_donation_priority(lock); // 홀더가 가진 기부리스트에서 해당 lock을 기다리는 스레드를 삭제
    set_donation_priority(lock);    // 홀더가 가진 기부리스트중에서 다시 높은 우선순위를 현재우선순위로 변경

    lock->holder = NULL;
    sema_up(&lock->semaphore);
}

bool lock_held_by_current_thread(const struct lock *lock)
{
    ASSERT(lock != NULL);

    return lock->holder == thread_current();
}

/* One semaphore in a list. */
struct semaphore_elem
{
    struct list_elem elem;      /* List element. */
    struct semaphore semaphore; /* This semaphore. */
};

void cond_init(struct condition *cond)
{
    ASSERT(cond != NULL);

    list_init(&cond->waiters);
}

void cond_wait(struct condition *cond, struct lock *lock)
{
    struct semaphore_elem waiter;

    ASSERT(cond != NULL);
    ASSERT(lock != NULL);
    ASSERT(!intr_context());
    ASSERT(lock_held_by_current_thread(lock));

    sema_init(&waiter.semaphore, 0);
    // list_push_back(&cond->waiters, &waiter.elem);
    list_insert_ordered(&cond->waiters, &waiter.elem, sema_priority, NULL);
    lock_release(lock);
    sema_down(&waiter.semaphore);
    lock_acquire(lock);
}

static bool sema_priority(const struct list_elem *a, const struct list_elem *b, void *aux)
{
    struct semaphore_elem *sema_a = list_entry(a, struct semaphore_elem, elem);
    struct semaphore_elem *sema_b = list_entry(b, struct semaphore_elem, elem);

    int prio_a = -1;
    int prio_b = -1;

    if (!list_empty(&sema_a->semaphore.waiters))
    {
        struct thread *th_a = list_entry(list_front(&sema_a->semaphore.waiters), struct thread, elem);
        prio_a = th_a->cur_priority;
    }

    if (!list_empty(&sema_b->semaphore.waiters))
    {
        struct thread *th_b = list_entry(list_front(&sema_b->semaphore.waiters), struct thread, elem);
        prio_b = th_b->cur_priority;
    }

    return prio_a > prio_b;
}

void cond_signal(struct condition *cond, struct lock *lock UNUSED)
{
    ASSERT(cond != NULL);
    ASSERT(lock != NULL);
    ASSERT(!intr_context());
    ASSERT(lock_held_by_current_thread(lock));

    if (!list_empty(&cond->waiters))
    {
        list_sort(&cond->waiters, sema_priority, 0);
        sema_up(&list_entry(list_pop_front(&cond->waiters), struct semaphore_elem, elem)->semaphore);
    }
}

void cond_broadcast(struct condition *cond, struct lock *lock)
{
    ASSERT(cond != NULL);
    ASSERT(lock != NULL);

    while (!list_empty(&cond->waiters))
        cond_signal(cond, lock);
}
