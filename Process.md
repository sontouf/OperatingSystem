# Process

## Program vs Process

- Program
  - bunch of instructions and static data in the disk 
- Process
  - A running program
  - Machine state
    - Memory : instructions and data
    - Registers : program counter, stack pointer etc
    - Others : list of the files the process currently has open
- APIs
  - Create, destroy, wait, miscellaneous(여러가지) control, and status



## fork() System Call

```c
// fork system call

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid: %d)\n", (int)getpid());
    int rc = fork();
    // rc에 저장 이후 
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("I am child (pid : %d)\n", (int)getpid());
    } else {
        printf("I am parent of %d (pid : %d)\n", rc, (int)getpid());
    }
    return 0;
}
```

```shell
// 실행 순서는 달라질 수 있다. 운영체제 맘.

hello world (pid: 66)
I am parent of 67 (pid : 66)
I am child (pid : 67)
```

```c
#include <unistd.h>

// pid_t 같은 자료형들은 primitive 자료형이라고 한다. primitive 자료형들은 sys/types.h 헤더파일에 정의되어 있다. 시스템이 변화함에 따라 자료형들을 계속 바꿔줘야 하는데 헤더파일에 정의해놓고 쓰면 바꿀 필요가 있을 때 헤더파일 내용만 수정해주면 된다. 보통 int 정도의 값을 가진다.


pid_t fork(void);
// 성공 시 : 부모 프로세스에서는 자식 프로세스의 PID 값을 반환받음
// 자식 프로세스에서는 0 값을 반환 받음
// 실패 시 : 음수 값 (-1) 반환
```

> fork 함수를 호출하는 프로세스는 부모 프로세스가 되고 새롭게 생성되는 프로세는 자식 프로세스가 된다.
>
> fork 함수에 의해 생성된 자식 프로세스는 부모 프로세스의 메모리를 그대로 복사하여 가지게 된다.



## wait() System Call

```c
// wait system call

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid: %d)\n", (int)getpid());
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("I am child (pid : %d)\n", (int)getpid());
    } else {
        int wc = wait(NULL);
        printf("I am parent of %d (wc : %d) (pid : %d)\n", rc, wc, (int)getpid());
    }
    return 0;
}
```

```shell
// 자식 프로세스가 반환된 이후에 부모 프로세스가 실행되므로 무조건 자식 이후에 부모가 출력된다.

hello world (pid: 82)
I am child (pid : 83)
I am parent of 83 (wc : 83) (pid : 82)
```

```c
#include <sys/wait.h>

pid_t wait(int *status);
// 인수 : 자식 프로세스 종료 상태
// 반환 : 종료된 자식 프로세스 id
```

> 자식 프로세스가 반환될 때까지 부모 프로세스의 이후 실행을 대기 한다.



## exec() System Call

```c
// exec system call

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("hello world (pid: %d)\n", (int)getpid());
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {
        printf("I am child (pid : %d)\n", (int)getpid());
        char *myargs[3];
        myargs[0] = strdup("wc");
        myargs[1] = strdup("process_exec.c");
        myargs[2] = NULL;
        execvp(myargs[0], myargs);
        printf("this shouldn't print out");
        
    } else {
        int wc = wait(NULL);
        printf("I am parent of %d (wc : %d) (pid : %d)\n", rc, wc, (int)getpid());
    }
    return 0;
}
```

```shell
hello world (pid: 93)
I am child (pid : 94)
 29  94 789 process_exec.c
I am parent of 94 (wc : 94) (pid : 93)
```

```c
#include <unistd.h>

int execvp(const char* command, char *argv[]);
// 첫번째인자로 명령이 들어간다.
```



자식 프로세스와 부모프로세스 등 여러 프로세스는 한정된 자원을 차지하려고 애를 쓰는데 OS는 이것을 어떻게 처리하는 지 알아보자.



## Process State

- Running
  - A process is running on a processor
- Ready
  - A process is ready to run, but the OS has chosen not to run it at this given moment
- Blocked (Waiting)
  - A process is not ready to run until some other event takes place

| Time | P0      | P1      | Notes                                 |
| ---- | ------- | ------- | ------------------------------------- |
| 1    | Running | Ready   |                                       |
| 2    | Running | Ready   |                                       |
| 3    | Running | Ready   | P0 initiates I/O                      |
| 4    | Blocked | Running | P0 is blocked, so P1 runs             |
| 5    | Blocked | Running |                                       |
| 6    | Blocked | Running |                                       |
| 7    | Ready   | Running | I/O done, 인터럽트로 인해 state 변경. |
| 8    | Ready   | Running | P1 now done                           |
| 9    | Running | -       |                                       |
| 10   | Running | -       | P0 now done                           |

>  CPU가 하나 있다고 가정
>
> I/O 가 CPU보다 느리고 언제 I/O 입력이 들어올 지 모르므로 계속 실행시키지 않고 block(wait) state 로 바꾼다.



| Time | 1    | 2    | 3    | 4    | 5    | 6    | 7    | 8    | 9    | 10   |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| CPU0 | 0    | 0    | 0    | 1    | 1    | 1    | 1    | 1    | 0    | 0    |



> process는 스스로 움직이지는 않고 process 에 관한 모든 정보를 담고 있는 PCB(Process control blocks)를 통해서 운영체제가 관리해준다.

```c
// PCB process에 관한 모든 정보를 담고 있다.
struct task_struct {
    volatile long		state; // TASK_RUNNING, TASK_INTERRUPTIBLE ... 상태를 탐는다.
    void				*stack; // Pointer to the kernel-mode stack
    ...
    unsigned int		cpu; // 몇 번 cpu에서 작동하는 지 기록
    struct mm_struck	*mm; // memory virtualization 정보가 포함된 구조체. (페이징 등)
    ...
    struct task_struct	*parent; // parent 의 PCB 를 가리킴
    struct list_head	children; // 프로세스를 list 형태로 관리
    ...
    struct files_struct	*files; // 프로세스가 오픈한 파일의 정보를 갖고 있음.
    ...
}

```

- Each user process has both a user-mode stack and kernel-mode stack
- When a thread enters the kernel, the contents of all registers used by a process in user-mode are saved on the kernel-mode stack (i.e Linux saves the context information of a process into its kernel-mode stack)



## Scheduling queues

- OS manages three types of the queue
  - Run queue
  - Ready queue
  - Waiting queue

> 같은 상태를 갖는 프로세스가 여러개 있을 수 있기 때문에 큐로 관리함으로써  효과적으로 원하는 프로세스를 찾는다.

| Time | P0      | P1      | Notes                                 | Run queue | Ready queue | Waiting queue |
| ---- | ------- | ------- | ------------------------------------- | --------- | ----------- | ------------- |
| 1    | Running | Ready   |                                       | P0        | P1          |               |
| 2    | Running | Ready   |                                       | P0        | P1          |               |
| 3    | Running | Ready   | P0 initiates I/O                      | P0        | P1          |               |
| 4    | Blocked | Running | P0 is blocked, so P1 runs             | P1        |             | P0            |
| 5    | Blocked | Running |                                       | P1        |             | P0            |
| 6    | Blocked | Running |                                       | P1        |             | P0            |
| 7    | Ready   | Running | I/O done, 인터럽트로 인해 state 변경. | P1        | P0          |               |
| 8    | Ready   | Running | P1 now done                           | P1        | P0          |               |
| 9    | Running | -       |                                       | P0        |             |               |
| 10   | Running | -       |                                       | P0        |             |               |

> P0 프로세스가 완전히 작업을 끝내기 전에 P1에 작업을 넘겨주는 등 모든 프로세스가 작업을 끝날 때까지 running state는 시간에 따라 계속 변화하는 데 이를 Context switch 라고 한다. 꽤나 비싼 작업이다.
>
> 그리고 이렇게 모든 state 에 scheduling policy 에 따라 관리된다. process <= cpu면 굳이 할 필요는 없다.