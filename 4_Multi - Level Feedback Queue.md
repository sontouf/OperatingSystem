# Multi - Level Feedback Queue

## Scheduling Metrics

- Turnaround time
  $$
  T_{turnaround} = T_{completion} - T_{arrival}
  $$

  - OS generally doesn't know how long a job will run for
    - SJF and STCF assume that the run-time of each job is known

- Reponse TIme
  $$
  T_{response} = T_{firsttime} - T_{arrival}
  $$

  - OS like to make a system feel responsive to interactive users
    - RR reduces response time but is terrible for turnaround time



## Crux(요점) of the Problem

- How can we design a scheduler that both
  - Minimizes response time for interactive jobs
  - Minimizes turnaround time
  - Without a priori knowledge of job length



## Multi-Level Feedback Queue(MLFQ)

- Multiple queues
  - Each has a different priority level
  - At any given time, a job that is ready to run is on a single
- Basic rules
  - Rule 1 : If Priority(A) > Priority(B), A runs (B doesn't)
  - Rule 2 : If Priority(A) = Priority(B), A & B run in RR

> 상위 우선순위를 가진 프로세스가 끝나거나 I/O 입력을 받았을 경우 후위 우선순위를 가진 프로세스가 실행된다. 같은 우선순위라면 time slice 를 통해 RR 방식으로 동작한다.



## How the Scheduler Sets Priorities

- Giving a fixed priority to each job | 고정값 쓰기
- Varying the priority of  a job based on its observed behavior
  - MLFQ uses the history of the job to predict its future behavior
    - If a job repeatedly relinquishes(포기하다) the CPU (for I/O), MLFQ keeps its priority high
    - If a job uses the CPU intensively for long periods of time, MLFQ reduces its priority



## How to change Priority

- Workload
  - I/O - bound jobs
    - Interactive and short-running
  - CPU - bound jobs
    - Compute intensive and longer-running

- Priority adjustment algorithm

  - Rule 3 : When a job enters the system, it is placed at the highest priority

  - Rule 4a : If a job uses up an entire time slice while running, its priority is reduces

    > CPU bound

  - Rule 4b : If a job gives ip the CPU before the time slice is up, it stays at the same priority level

    > I/O bound





## Problems

- Starvations

  - If there are too many interative jobs in the system

  > 작업이 너무 많으면 할당을 아예 못 받을수도 있다.

- Gaming the scheduler

  - Issuing an I/O operation just before the time slice is over

  > time slice 가 끝나기 거의 직전에 i/o를 요청하면서 i/o bound 라고 착각하게 만든다.

- Changing behavior

  - CPU-bound -> I/O-bound

  > 나중에 i/o-bound 가 됐는데 너무 우선순위가 낮아져서 복구가 안된다.



## Priority Boost - [Starvations Problem]

- Periodically boost the priority of all the jobs in system
  - Rule 5 : After some time period S, move all the jobs in the system to the topmost queue
  - Can resolve the starvation and changing-behavior problems



## Better Accounting - [Gaming the scheduler]

- Recall rules 4a and 4b

  - Rule 4a : If a job uses up an entire time slice while running, its priority is reduces

  - Rule 4b : If a job gives ip the CPU before the time slice is up, it stays at the same priority level

- Gaming tolerance

  - Rule 4: Once a job uses up its time allotment at a given level (regardless of how many times it gives up the CPU), its priority is reduced

  > 누적해서 사용한 CPU 양이 Time Slice 만큼인지 확인한다.



## Accounting for Changes in Behavior - [Changing behavior]

- Priority boost

  - How often should prority be boosted in order to avoid starvation and account for changes in behavior?

    - ex ) Default value in Solaris: 1s

    > 1s 정도 간격으로 프로세스의 우선순위가 다 올라감

- How big should the time slice be per queue?

  - Short time slices for high - priority queues
    - Default value in Solaris 20 ms
  - Longer time slices for low-  priority queues
    - Default value in Solaris : few hundred ms

  > 우선순위가 낮은 프로세스에는 timeslice가 좀더 길게 할당된다.