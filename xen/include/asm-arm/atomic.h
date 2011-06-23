#ifndef __ASM_ATOMIC_H__
#define __ASM_ATOMIC_H__

typedef struct {
	volatile int counter;
} atomic_t;


#define ATOMIC_INIT(i)	{ (i) }

#define atomic_read(v)		((v)->counter)
#define atomic_set(v,i)		(((v)->counter) = (i))

#define _atomic_read(v)		atomic_read(&v)
#define _atomic_set(v,i)	atomic_set(&v,i)

static inline int atomic_add_return(int i, atomic_t *v)
{
	unsigned long flags;
	int val;

	local_irq_save(flags);
	val = v->counter;
	v->counter = val += i;
	local_irq_restore(flags);

	return val;
}

static inline int atomic_sub_return(int i, atomic_t *v)
{
	unsigned long flags;
	int val;

	local_irq_save(flags);
	val = v->counter;
	v->counter = val -= i;
	local_irq_restore(flags);

	return val;
}

static inline void atomic_clear_mask(unsigned long mask, unsigned long *addr)
{
	unsigned long flags;

	local_irq_save(flags);
	*addr &= ~mask;
	local_irq_restore(flags);
}


static inline atomic_t atomic_cmpxchg(atomic_t *v, atomic_t old, atomic_t new)
{
        atomic_t ret; // by PCJ
        unsigned long flags;

        local_irq_save(flags);
        ret.counter  = v->counter;
        if (likely(ret.counter == old.counter))
                v->counter = new.counter;
        local_irq_restore(flags);

        return ret;
}

static inline atomic_t atomic_compareandswap(
        atomic_t old, atomic_t new, atomic_t *v)
{
        atomic_t rc;
        rc = atomic_cmpxchg( (atomic_t *)v, old, new);  // by PCJ
        return rc;
}

#define atomic_add(i, v)	(void) atomic_add_return(i, v)
#define atomic_inc(v)		(void) atomic_add_return(1, v)
#define atomic_sub(i, v)	(void) atomic_sub_return(i, v)
#define atomic_dec(v)		(void) atomic_sub_return(1, v)

#define atomic_inc_and_test(v)	(atomic_add_return(1, v) == 0)
#define atomic_dec_and_test(v)	(atomic_sub_return(1, v) == 0)
#define atomic_inc_return(v)    (atomic_add_return(1, v))
#define atomic_dec_return(v)    (atomic_sub_return(1, v))

#define atomic_add_negative(i,v) (atomic_add_return(i, v) < 0)

#endif
