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



