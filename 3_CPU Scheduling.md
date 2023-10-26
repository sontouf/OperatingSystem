# CPU Scheduling

## Workload Assumptions

- Each job runs for the same amount of time
- All jobs arrive at the same times
- Once started, each job runs to completion
- All jobs only uses the CPU (i.e no I/O)
- The run-time of each job is known

> I know, it's unrealistic



## Scheduling Metrics

- Turn around time
  $$
  T_{turnaround} = T_{completion} - T_{arrival}
  $$

- First In, First Out (FIFO)

  - = First come, First served (FCFS)

    Simple and easy to implement

    먼저 실행하는 프로세스가 끝날 때까지 대기해야함.

    | Time            | 0             | 10         | 20      | 30   | 40   |
    | --------------- | ------------- | ---------- | ------- | ---- | ---- |
    | running process | p0(run),p1,p2 | p1(run),p2 | p2(run) |      |      |

    - Average turnaround time = 20

    $$
    {{10 + 20 + 30} \over 3} = 20
    $$

## Workload Assumptions

- ~~Each job runs for the same amount of time~~

  실행시점이 같을 수는 없다.

- All jobs arrive at the same times
- Once started, each job runs to completion
- All jobs only uses the CPU (i.e no I/O)
- The run-time of each job is known



## First In, First Out (FIFO)

- = First come, First served (FCFS)

  Simple and easy to implement

  먼저 실행하는 프로세스가 끝날 때까지 대기해야함.

  | Time            | 0             | 100        | 110     |      |      |
  | --------------- | ------------- | ---------- | ------- | ---- | ---- |
  | running process | p0(run),p1,p2 | p1(run),p2 | p2(run) |      |      |

  - Average turnaround time = 110
    $$
    {{100 + 110 + 120} \over 3} = 110
    $$

- How does FIFO perform now?

  - What kind of workload could you construct to make FIFO perform poorly?
  - Convoy effect - 작업시간이 긴 프로세스가 앞쪽에 위치하면 뒤에 프로세스들에게 계속적으로 영향을 준다



## Shortest Job First (SJF)

FIFO에서 실행시간이 긴 프로세스가 뒤쪽에 다 영향을 주는게 문제라면 실행시간이 짧은 것부터 처리하면 어떨까?

- Short job First, then the next shortest, and so on
- time 0에 p0,p1,p2가 다 도착했다고 가정하자.

| Time            | 0    | 10   | 20   | 40   | 60   | 80   | 100  | 120  |
| --------------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| running process | p1   | p2   | p0   |      |      |      |      |      |

- Average turnaround = 50

$$
{{10 + 20 + 120} \over 3} = 50
$$

- Optimal when assuming all jobs arrive at the same time



## Workload Assumptions

- ~~Each job runs for the same amount of time~~

  실행시점이 같을 수는 없다.

- ~~All jobs arrive at the same times~~

  모든 작업이 같은 시점에 시작된다면 SJF가 최적일 수 있지만 언제나 그럴수는 없다.

- Once started, each job runs to completion
- All jobs only uses the CPU (i.e no I/O)
- The run-time of each job is known



## Shortest Job First (SJF)

- Short job First, then the next shortest, and so on

| Time            | 0    | 20           | 40   | 60   | 80   | 100  | 110  | 120  |
| --------------- | ---- | ------------ | ---- | ---- | ---- | ---- | ---- | ---- |
| running process | p0   | p1,p2 arrive |      |      |      | p1   | p2   |      |

- Average turnaround = 103.33

$$
{{100 + (110-10) + (120-10)} \over 3} = 103.33
$$

긴 프로세스가 먼저 들어와버리면 convey effect 에 영향을 받는다.





## Workload Assumptions

- ~~Each job runs for the same amount of time~~

  실행시점이 같을 수는 없다.

- ~~All jobs arrive at the same times~~

  모든 작업이 같은 시점에 시작된다면 SJF가 최적일 수 있지만 언제나 그럴수는 없다.

- ~~Once started, each job runs to completion~~

  혼자서 끝날 때까지 하는 건 안된다. 일전에 time sharing 에서도 양보를 할 수 있게 한다.

- All jobs only uses the CPU (i.e no I/O)

- The run-time of each job is known



## Preemptive Scheduler

- Non-preemptive scheduler
  - Run each job to completion before considering whether to run a new job
  - Batch computing in the old days
- Preemptive scheduler 
  - Stop one process from running in order to run another
  - Context switch

> 양보하자!



## Shortest Job First (SJF)

- Short job First, then the next shortest, and so on

| Time            | 0    | 10                     | 20   | 30   | 40   | 60   | 80   | 100  | 120  |
| --------------- | ---- | ---------------------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| running process | p0   | p1,p2 arrive / p1(run) | p2   | p0   |      |      |      |      |      |

- Average turnaround = 50

$$
{{120 + (20-10) + (30-10)} \over 3} = 50
$$



## Scheduling Metrics

- Turnaround time
  $$
  T_{turnaround} = T_{completion} - T_{arrival}
  $$

- Response time
  $$
  T_{response} = T_{firstrun} - T_{arrival}
  $$
  

## Shortest Time - to -Completion First (STCF)

- = Preemptive Shortest Job First (PSJF)

| Time            | 0    | 10                     | 20   | 30   | 40   | 60   | 80   | 100  | 120  |
| --------------- | ---- | ---------------------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| running process | p0   | p1,p2 arrive / p1(run) | p2   | p0   |      |      |      |      |      |

- Average response time = 3.33
- While great for turnaround time, this approach is quite bad for response time and interactivity

$$
{{0 + 0 + (20-10)} \over 3} = 3.33
$$

> 완료까지 가장 짧게 나은 것들을 먼저 처리해주자.



## Round Robin (RP)

- = Time slicing
  - Runs a job for a time slice (aka scheduling quantum)

- Average response time = 2, but worst for turnaround time

$$
{{0 + 2 + 4} \over 3} = 2
$$

- Time slice : 2
  - Trade - off between response time and cost of context switching

> 시간 간격을 적당히 적게 줄여서 배분하자.



## Workload Assumptions

- ~~Each job runs for the same amount of time~~

  실행시점이 같을 수는 없다.

- ~~All jobs arrive at the same times~~

  모든 작업이 같은 시점에 시작된다면 SJF가 최적일 수 있지만 언제나 그럴수는 없다.

- ~~Once started, each job runs to completion~~

  혼자서 끝날 때까지 하는 건 안된다. 일전에 time sharing 에서도 양보를 할 수 있게 한다.

- ~~All jobs only uses the CPU (i.e no I/O)~~

  I/O 같이 작업시간을 알수없게 하는 이벤트들이 발생할 수 있다.

- The run-time of each job is known



## Incorporating I/O

- The scheduler has a decision to make when a job initiates an I/O request
  - The job is blocked waiting for I/O completion
  - The scheduler also has to make a desicion when the I/O completes

> 입력 받을 동안 다른 작업이 그 시간에 작업하면 시간을 절약할 수 있다.

## Workload Assumptions

- ~~Each job runs for the same amount of time~~

  실행시점이 같을 수는 없다.

- ~~All jobs arrive at the same times~~

  모든 작업이 같은 시점에 시작된다면 SJF가 최적일 수 있지만 언제나 그럴수는 없다.

- ~~Once started, each job runs to completion~~

  혼자서 끝날 때까지 하는 건 안된다. 일전에 time sharing 에서도 양보를 할 수 있게 한다.

- ~~All jobs only uses the CPU (i.e no I/O)~~

  I/O 같이 작업시간을 알수없게 하는 이벤트들이 발생할 수 있다.

- ~~The run-time of each job is known~~

  - How can we build an approach that behaves like SJF/STCF without such a priori knowledge?

> Round Rodin 쓰자.





