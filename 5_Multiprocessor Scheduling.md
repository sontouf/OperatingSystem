## Sunmmary of Last Class

- Multi-Level Feedback Queue

  - Rule 1 : If Priority(A) > Priority(B), A runs (B doesn't)
  - Rule 2 : If Priority(A) = Priority(B), A & B in RR
  - Rule 3 : When a job enters the system, it is placed at the highest priority
  - Rule 4 : Once a job uses up its time allotment at a given level (regardless of how many times it gives up the CPU), its priority is reduced
  - Rule 5 : After some time period S, move all the jobs in the system to the topmost queue

  

# Multiprocessor Scheduling

## Single - Queue Scheduling

- Single - queue multiprocessor scheduling (SQMS)
  - Simply reuse the basic framework for single processor scheduling
  - Pick the best n jobs to run (n = number of CPUs)
  - Lack of scalability due to synchronization overheads
  - cache affinity

| Queue | A -> | B -> | C -> | D -> | E -> | NULL |
| :---: | :--: | :--: | :--: | :--: | :--: | :--: |

| CPU  | 0    | 1    | 2    | 3    | 4    |
| ---- | ---- | ---- | ---- | ---- | ---- |
| CPU0 | A    | E    | D    | C    | B    |
| CPU1 | B    | A    | E    | D    | C    |
| CPU2 | C    | B    | A    | E    | D    |
| CPU3 | D    | C    | B    | A    | E    |



## Multi - Queue Scheduling

- Multi - queue multiprocessor scheduling (MQMS)

  - One per CPU
  - When a job enters the system, it is placed on exactly one scheduling queue
    - Random, shorter, queue, etc
  - Then it is scheduled essentially independently
    - Can avoid synchronization and provide cache affinity

  | Q0 -> A -> C |      | Q1 -> B -> D |
  | ------------ | ---- | ------------ |

  | CPU  | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   |
  | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
  | CPU0 | A    | A    | C    | C    | A    | A    | C    | C    | A    | A    | C    | C    |
  | CPU1 | B    | B    | D    | D    | B    | B    | D    | D    | B    | B    | D    | D    |

  

- Load imbalance

  | Q0 -> A |      | Q1 -> B -> D |
  | ------- | ---- | ------------ |

  | CPU  | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   |
  | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
  | CPU0 | A    | A    | A    | A    | A    | A    | A    | A    | A    | A    | A    | A    |
  | CPU1 | B    | B    | D    | D    | B    | B    | D    | D    | B    | B    | D    | D    |

  

  

  | Q0 -> |      | Q1 -> B -> D |
  | ----- | ---- | ------------ |

  | CPU  | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   |
  | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
  | CPU0 |      |      |      |      |      |      |      |      |      |      |      |      |
  | CPU1 | B    | B    | D    | D    | B    | B    | D    | D    | B    | B    | D    | D    |

- How to deal with load imbalance

  - Migration
  - Work stealing
    - a (source) queue that is low on jobs will occasionally peek at another (target) queue
    - If the target queue is more full than the source queue, the source will "steal" one or more jobs from the target to help balance load

  | Q0 -> A |      | Q1 -> B -> D |
  | ------- | ---- | ------------ |

  | CPU  | 0    | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   | 11   |
  | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
  | CPU0 | A    | A    | A    | A    | B    | A    | B    | A    | B    | B    | B    | B    |
  | CPU1 | B    | D    | B    | D    | D    | D    | D    | D    | A    | D    | A    | D    |



## Linux CPU Schedulers

> red-black tree 에서 왼쪽 아래로 갈 수록 vruntime이 작다.

- Completely Fair Scheduler (CFS)  | 가중치를 부여해서 resource를 준다

  - SCHED_NORMAL

    (traditionally called SCHED_OTHER)

  - Weighted fair scheduling

- Real-Time Schedulers | 우선순위 기준으로 준다.

  - SCHED_FIFO and SCHED_RR
  - Priority-based scheduling
  - sched_setattr()

- Deadline Scheduler | 젤 급한 애부터 처리

  - SCEHD_DEADLINE
  - EDF-like periodic scheduler
  - sched_setattr()

> priority 변환에는 루트 권한이 필요하다.



## Completely Fair Scheduler (CFS)

- vruntime

  - Each task is stored in red-black tree based on its virtualruntime

  - Weighted runtime calculated based on the nice value (-20 ~ 19) of each process
    $$
    vruntime = vruntime + DeltaExec * {Weight_0 \over weight_p}
    $$

    > 시간으로 관리한다. 얼마나 CPU를 많이 썼냐로 구분짓는다. 가중치를 부여해서 프로세스마다 같은 시간을 CPU를 쓰더라도 다르게 계산한다. vruntime을 적게 쓸수록 prio 가 올라간다.
    >
    > DeltaExec = 최근에 CPU를 사용한 시간

    | Nice        | Weight_p (미리 정의됨) | Weight_0 / Weight_p |
    | ----------- | ---------------------- | ------------------- |
    | -10         | 9548                   | 0.107               |
    | -5          | 3121                   | 0.328               |
    | 0 (default) | 1024 (Weight_0)        | 1                   |
    | 5           | 335                    | 3.057               |
    | 10          | 110                    | 9.309               |

    > CPU는 최대한 공평하게 동작하려고 하기 때문에 Nice 값을 설정해서 프로세스에게 resource를 적절하게 주려고 한다. Nice 가 낮으면 weight_p가 높은데 CPU를 더 많이 쓰고자 한다는 의미이다. CPU를 계속 차지하려고 하면 안되기 때문에 prio는 낮아지게 된다.
    >
    > fair 를 위해 nice value가 낮더라도 아예 CPU를 안 주지는 않는다.

## /proc/\<pid>/sched

```shell
root@user: /usr/cgroups/cpuset/1# cat /proc/2474/sched
loop(2474, #threads: 1)
-------------------------------
...
prio : 120
...
```

## Priority

- prio = nice + 120 (nice : -20 ~ 19)
  - default prio : 120
  - CFS : 100 ~ 139
  - 0 ~ 99 are reserved for real-time schedulers 우선순위가 기준으로 작동해서 양보가 잘 안된다.
- renice command
  - Users cannot change nice to -1 ~ -20
    - Only the root can do 권한이 필요하다.



## Multiprocessor Scheduling in CFS

- Load metric
  - Load of thread : average CPU utilization ( weighted by the thread's priority)
  - Load of core : sum of the loads of the threads
- Tries to even out the load of cores
  - For every 4ms
- Topology awareness
  - Cache
  - NUMA
    - If the load diff. b/w nodes < 25%, no load balancing



## Diving into Linux Kernel v5.11.8

- Scheduling classes (/kernel/sched/sched.h)

```c
struct sched_class {
    void (*enqueue_task) (strunct rq *rq, struct task_struct *p, int flags);
    void (*dequeue_task) (strunct rq *rq, struct task_struct *p, int flags);
    ...
    struct task_struct *(*pick_next_task) (struct rq *rq);
    ...
    int (*balance) (struct rq *rq, struct task_struct *prev, struct rq_flags *rf);
}
```

```c
extern const struct sched_class dl_sched_class;
extern const struct sched_class rt_sched_class;
extern const struct sched_class fair_sched_class;
extern const struct sched_class idle_sched_class;
```



- CFS scheduling class (/kernel/sched/fair.c)

```c
DEFINE_SCHED_CLASS(fair) = {
    .enqueue_task = enqueue_task_fair,
    .dequeue_task = dequeue_task_fair,
    ...
    .pick_next_task = __pick_next_task_fair, 
    // 누구를 골라서 CPU 줄까, vruntime이 적은거, red-black트리에서 가장 왼쪽아래
    ...
    .balance = balance_fair,// balance가 필요할 때 코드가 실행된다.
    ...
}
```

> 초기화해주고 있는 코드 부분



- Per - CPU runqueue (/kernel/sched/sched.h)

```c
struct rq {
    raw_spinlock_t		lock; // 실행중에 오류 안나게 lock을 걸어준다.
    ...
    struct cfs_rq		cfs;
    struct rt_rq		rt;
    struct dl_rq		dl;
    ...
    struct task_struct	*curr; // context swith 할 대상. PCB 값을 가진다.
    struct task_struct	*idle;
}
```



- CPU scheduler (/kernel/sched/core.c)

  - schedule () -> __schedule() 

  > 스케줄 해야할때 필요한 코드가 들어있다.
  >
  > 감시 메커니즘이 필요하기에 timer 를 셋팅하고 interrupt를 발생시킨다. - handler - trap table

```c
static void __sched notrace __schedule(bool preempt)
{
    ...
    prev = rq->curr; //process PCB 저장
    ...
    next = pick_next_task(rq, prev, &rf);
    ...
    rq = context_switch(rq, prev, next, &rf);
}
```



- - schedule () -> __schedule() -> pick_next_task()

  > 다음에 실행할 프로세스를 찾는다.

```c
static inline struct task_struct *
    pick_next_task(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
    ...
    put_prev_task_balance(rq, prev, rf);
    ...
    for_each_class(class) {
        p = class->pick_next_task(rq);
        if (p)
            return p;
    }
    // prio 가 가장 높은 task를 봄. 없으면 다음번 프로세스를 찾으러 감. 
    // 프로세스간의 우선순위 반영
}
```



