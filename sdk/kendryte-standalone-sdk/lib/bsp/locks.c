#include <stdlib.h>
#include <sys/lock.h>
#include "bsp.h"

#define LOCK_MAX_NUM (1024)

// TODO: currently reculock release from lock is disabled as a workaround
// due to some instabilities
// Because of that, all locks should be allocated statically for time being


#ifdef DEBUG_LOCKS

typedef struct unlock_entry {
  int mode;
  int core;
  void* lock;
  void* reclock;
} unlock_entry;

#define ELOG_SIZE 32
static struct unlock_entry unlock_log[ELOG_SIZE];
static uint32_t count = 0;
static volatile uint32_t stop = 0;
static int safe_count = 0;

static void lock_save(int mode, int core,void* lock, void* reclock){
  unlock_entry* log = &unlock_log[++count%ELOG_SIZE];
  log->mode = mode;
  log->core = core;
  log->lock = lock;
  log->reclock = reclock;
}

static void lock_print(){
  printk("DEBUG LOCKS: count %u \n", count%ELOG_SIZE);
  for(int i = 0; i < ELOG_SIZE; i++){
    uint32_t c = count++%ELOG_SIZE;
    unlock_entry* log = &unlock_log[c];
    printk("mode %i, core %i, lock 0x%08x, reclock 0x%08x c %i,  \n", log->mode, log->core, log->lock, log->reclock, c);
  }
}

#else

static void lock_save(int mode, int core,void* lock, void* reclock){}
static void lock_print(){}

#endif

typedef long _lock_t;

static _lock_t _reculock_barrier;

typedef struct
{
    _lock_t *lock;
    long counter;
    unsigned long core;
} reculock_t;

reculock_t reculock[LOCK_MAX_NUM];

void show_error(void)
{
    register unsigned long a7 asm("a7") = 93;
    register unsigned long a0 asm("a0") = 0;
    register unsigned long a1 asm("a1") = 0;
    register unsigned long a2 asm("a2") = 0;

    printk("lock broke \n");
    while(1);

    asm volatile("scall"
                 : "+r"(a0)
                 : "r"(a1), "r"(a2), "r"(a7));
}

static inline long lock_trylock(_lock_t *lock)
{
    long res = atomic_swap(lock, -1);
    /* Use memory barrier to keep coherency */
    mb();
    return res;
}

static inline void lock_lock(_lock_t *lock)
{
    while(lock_trylock(lock))
        ;
}

static inline void lock_unlock(_lock_t *lock)
{
    /* Use memory barrier to keep coherency */
    mb();
    atomic_swap(lock, 0);
    asm volatile("nop");
}

static reculock_t *get_reculock(_lock_t *lock)
{
    lock_lock(&_reculock_barrier);
    reculock_t *ret = NULL;
    for(uint32_t i = 0; i < LOCK_MAX_NUM; i++)
    {
      if(reculock[i].lock == lock){
        ret = &reculock[i];
        break;
      }
    }

    lock_unlock(&_reculock_barrier);
    return ret;
}

static reculock_t *get_free_reculock(void)
{
    reculock_t *ret = NULL;
    for(uint32_t i = 0; i < LOCK_MAX_NUM; i++)
      {
        if(reculock[i].lock == NULL){
          ret = &reculock[i];
          break;
        }
      }

    return ret;
}

static reculock_t *reculock_init(_lock_t *lock)
{
    lock_lock(&_reculock_barrier);
    reculock_t *v_reculock = get_free_reculock();
    if(v_reculock == NULL)
    {
        lock_unlock(&_reculock_barrier);
        return NULL;
    }
    *v_reculock = (reculock_t){
        .lock = lock,
        .counter = 0,
        .core = 0,
    };
    lock_unlock(&_reculock_barrier);
    return v_reculock;
}

static void reculock_deinit(_lock_t *lock)
{
    lock_lock(lock);
    // TODO: workaround
    /* reculock_t *v_reculock = get_reculock(lock); */
    /* if(v_reculock) */
    /* { */
    /*     *v_reculock = (reculock_t){ */
    /*         .lock = NULL, */
    /*         .counter = 0, */
    /*         .core = 0, */
    /*     }; */
    /* } */
    lock_unlock(lock);
}

static inline int reculock_trylock(_lock_t *lock)
{
    int res = 0;
    unsigned long core;

    asm volatile("csrr %0, mhartid;"
                 : "=r"(core));
    if(lock_trylock(lock))
    {
        return -1;
    }

    reculock_t *v_reculock = get_reculock(lock);
    if(v_reculock == NULL)
    {
        v_reculock = reculock_init(lock);
        if(v_reculock == NULL)
        {
            lock_unlock(lock);
            show_error();
        }
    }

    lock_save(1, v_reculock->core, v_reculock->lock, v_reculock);

    if(v_reculock->counter == 0)
    {
        /* First time get lock */
        v_reculock->counter++;
        v_reculock->core = core;
        res = 0;
    } else if(v_reculock->core == core)
    {
        /* Same core get lock */
        v_reculock->counter++;
        res = 0;
    } else
    {
        /* Different core get lock */
        res = -1;
    }
    lock_unlock(lock);

    return res;
}

static inline void reculock_lock(_lock_t *lock)
{
    unsigned long core;

    asm volatile("csrr %0, mhartid;"
                 : "=r"(core));
    lock_lock(lock);

    reculock_t *v_reculock = get_reculock(lock);
    if(v_reculock == NULL)
    {
        v_reculock = reculock_init(lock);
        if(v_reculock == NULL)
        {
            lock_unlock(lock);
            show_error();
        }
    }

    lock_save(1, core, v_reculock->lock, v_reculock);

    if(v_reculock->counter == 0)
    {
        /* First time get lock */
        v_reculock->counter++;
        v_reculock->core = core;
    }
    else if(v_reculock->core == core)
    {
        /* Same core get lock */
        v_reculock->counter++;
    }
    else
    {
        /* Different core get lock */
        lock_unlock(lock);
        do
        {
            while(atomic_read(&reculock->counter))
                ;
        } while(reculock_trylock(lock));
        return;
    }
    lock_unlock(lock);
}

static inline void reculock_unlock(_lock_t *lock)
{
    unsigned long core;

    asm volatile("csrr %0, mhartid;"
                 : "=r"(core));
    lock_lock(lock);

    reculock_t *v_reculock = get_reculock(lock);
    if(v_reculock == NULL)
    {
        lock_print();
        lock_unlock(lock);
        show_error();
    }

    if(v_reculock->core == core)
    {
        /* Same core release lock */
        v_reculock->counter--;
        if(v_reculock->counter <= 0)
        {
            lock_save(0, v_reculock->core, v_reculock->lock, v_reculock);
            v_reculock->core = 0;
            v_reculock->counter = 0;
            //v_reculock->lock = NULL; // TODO: workaround
        }
    } else
    {
        /* Different core release lock */
        lock_unlock(lock);
        show_error();
    }
    lock_unlock(lock);
}

void _lock_init(_lock_t *lock)
{
    *lock = 0;
}

void _lock_init_recursive(_lock_t *lock)
{
    reculock_init(lock);
}

void _lock_close(_lock_t *lock)
{
    lock_unlock(lock);
}

void _lock_close_recursive(_lock_t *lock)
{
    reculock_deinit(lock);
}

void _lock_acquire(_lock_t *lock)
{
    lock_lock(lock);
}

void _lock_acquire_recursive(_lock_t *lock)
{
    reculock_lock(lock);
}

int _lock_try_acquire(_lock_t *lock)
{
    return lock_trylock(lock);
}

int _lock_try_acquire_recursive(_lock_t *lock)
{
    return reculock_trylock(lock);
}

void _lock_release(_lock_t *lock)
{
    lock_unlock(lock);
}

void _lock_release_recursive(_lock_t *lock)
{
    reculock_unlock(lock);
}
