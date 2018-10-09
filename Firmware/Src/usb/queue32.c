// Queue32.c
// 4byte ring buffer (32bit version)
//
//     (CC at)2016 by D.F.Mac. @TripArts Music
// 
// 2016.07.27 : D.F.Mac.


#include "queue32.h"
#include "stm32f4xx_hal.h"

#ifdef __cplusplus
extern "C"{
#endif 

void b4arrq_init(stB4Arrq *qp){
  qp->top = 0;
  qp->last = 0;
  qp->num = 0;
  qp->queue_size = B4QUEUE_SIZE;
}

uint32_t b4arrq_next(uint32_t value){
  return (value + 1) % B4QUEUE_SIZE;
}

uint32_t b4arrq_push(stB4Arrq *qp, uint32_t *p)
{
    __disable_irq();
    uint32_t *q = qp->queue;
    uint32_t last = qp->last;
    if(b4arrq_next(last) == qp->top)
    {
        return 0;
    }
    *(q+last) = *p;
    qp->last = b4arrq_next(last);
    ++(qp->num);
    __enable_irq();
    return 1;
}

uint32_t *b4arrq_pop(stB4Arrq *qp)
{
    __disable_irq(); // disable interrupts, so variables wouldn't be changed during reading
    uint32_t *pRes = (uint32_t *)0;
    uint32_t *p = qp->queue;
    uint32_t top = qp->top;
    if(top != qp->last)
    {
        pRes = p+top;
        qp->top = b4arrq_next(top);
        --(qp->num);
    }
    __enable_irq();
    return pRes;
}

#ifdef __cplusplus
} // extern "C"
#endif

