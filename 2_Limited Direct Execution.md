# Limited Direct Execution

scheduling policy 에 따라 process 가 state를 왔다갔다하며 잘 진행되면 좋은데 가끔 OS가 간섭을 해야 할 때가 있다.  언제 어떻게 하는 지 알아보자.



## Direct Execution

아래와 같은 순서로 진행된다. 

| OS                                                           | Program                                                      |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| `OS >> 새로운 process 를 실행시키기 위해 작업공간을 새롭게 만든다.`<br/>create entry for process list<br/>Allocate memory for program<br/>Load program into memory<br/>Set up stack with argc/argv<br/>Clear registers<br/>Execute call main() |                                                              |
|                                                              | `Program >> main() 이 실행된다.`<br/>Execute return from main() |
| `OS >> 프로세스를 다 썼으면 할당된 모든 메모리공간에서 삭제한다.`<br/>Free memory of process<br/>Remove from process list |                                                              |







- Without "limits" on running programs, the OS wouldn't be in control of anything and thus would be just a library
  - How can the OS make sure the program doesn't do anything that we don't want it to do, while still running it efficienty?
  - How does the OS stop it from running and switch to another process (i.e time sharing)? 

> process 가 실행되는 동안 심각한 오류를 범하지 않도록 계속 감시를 해줘야 한다. 예를 들어 main() 함수가 작동중에 잘못된 index에 접근해서 메모리공간을 망치고 있는데 OS가 그것을 관리하지 않고 그냥 return 0; 신호만 받아서 context  switch만 관리한다면 결국 큰 문제를 야기하고 말것이다.

> 근데 OS 또한 프로그램으로 결국 메모리공간에 올라와 실행되는 process 이므로 본인도 policy에 따라 resource를 할당받아야 한다. 언제 어떻게 해야 할까?



## Problem # 1. Restricted Operations

- How to Perform restricted operations?
  - Restricted operations (privileged operations(우선순위가 높음))
    - Issuing an I/O request to a disk
    - Gaining access to more system resources, such as CPU or memory
  - Applications must be able to perform restricted operations, but without giving the process complete control over the system

> 프로세스들이 공평하게 CPU 자원이 분배가 되는지, 입출력 같은 부분이 잘 처리되고 있는지, 권한이 있는지 검사.



## Processor Modes

- User mode
  - Code that runs in user mode is restricted in what it can do
  - Restricted operations would result in the processor raising an exeption
- Kernel mode
  - In this mode, code that runs can do what it likes
  - OS runs in kernel mode

> kernel mode 에서 실행되는 것이 우선순위가 높다. user mode 에서는 어느정도 제약이 걸린채 실행되지만 kernel mode면 그냥 실행되어진다. user mode 에서 kernel 로 실행시킬 수 있다.



## System calls

What should a user process do when it wishes to perform a privileged operations?

- Trap instructions
  - Jumps into the kernel
  - Raises the privilege level to kernel mode
- Return - from - trap instruction
  - Returns into the calling user program
  - Reduces the privilege level back to user mode

How does the trap know which code to run inside the OS?

- The calling process can't specify an adress to jump to

> 프로세스가 trap 을 처리할 수는 없기 떄문에 Trap table 이 사용된다.

Trap table

- When the machine boots up, the OS informs the hardware of the locations of trap handlers
  - Privileged operation
- A system - call number is assigned to each system call
- User code cannot specify an exact address to jump to, but rather must request a particular service via number

> 미리 정해진 번호를 통해서만 restrict operation 을 처리한다. hadler 에 기록되어 있다.
>
> 시스템 콜 -> trap handler -> table -> 해당 시스템콜 호출

| OS                                                           | Hardware                                                     | Program                                                     |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ----------------------------------------------------------- |
| Initialize trap table                                        | Remember address of syscall handler                          |                                                             |
| create entry for process list<br/>Allocate memory for program<br/>Load program into memory<br/>Set up stack with argc/argv<br/>FIll kernel Stack with regs/PC<br />Return - from - trap<br />< kernel -> User > |                                                              |                                                             |
|                                                              | Restore regs from kernel stack<br />move to User mode<br />Jump to main |                                                             |
|                                                              |                                                              | Run main()<br />...<br />Call system call<br />Trap into OS |
|                                                              | Save regs/PC to kernel stack<br />move to kernel mode<br />Jump to trap handler |                                                             |
| Handler trap<br />Return - from - trap                       |                                                              |                                                             |
|                                                              | Restore regs from kernel stack<br />Move to user mode<br />Jump to PC after trap |                                                             |
|                                                              |                                                              | Return from main()<br />Trap ( via exit())                  |
| Free Memory of process<br />Remove from porcess list         |                                                              |                                                             |

> Hardware도 같이 움직이는 이유는 현재상황을 최신화하고  context 의 다음 위치를 계속 저장하기 위해서다.



- return - from - trap
  1. k -> u change process mode
  2. kernel stack 맨 위에 kernel mode 에 저장된 usercontext 정보를 restore 한다.
  3. 하지만 처음에는 main() 부터 시작하는 user context 정보가 아예 없다. 그래서 roader 가 kernel 안에 restore 할 user context 정보를 인위적으로 만들어준다.





## Problem #2 : Switching Between Processes

- How to regain control of the CPU?
  - If a process is running on the CPU, this means the OS is not running
  - If the OS is not running, how can it do anything at all?



>CPU 또한 프로세스이므로 CPU가 프로세스를 관리를 하려면 CPU 또한 resoure를 얻어야한다. 어떤 프로세스가 자원을 얻은 상태에서 CPU가 어떻게 관리를 해야 하는가? 즉 어느 시점에 CPU가 관여를 할 것인가?



## Cooperative Approach

- Wait for system calls
  - Processes that run for too long are asummed to periodically give up the CPU
    - Most processes transfer control of the CPU to the OS quite frequently by making system calls
    - Explicit yield system call

- Applications also transfer control to the OS when they do an illegal operation
  - Dividing by zero
  - Segmentation fault

> 시스템콜을 기다리며 OS가 접근하려고 하면 다른 프로세스들이 자원을 차지하기 위해 계속 OS가 자원을 못 얻는 방향으로 시스템콜을 적게 호출할 것이기 때문에 OS 는 그 떄문에 별로 실행되지 못할 것이다.



## Non - Cooperative Approach

- The OS takes control
  - Timer interrupt
    - A timer device can be programmed to raise an interrupt periodically
    - When the interruped is raised, the currently running process is halted, and a pre-configured interrupt handler runs

> 시스템콜이 올때까지 기다리기 보다는 강제적으로 일정 시간마다 검사를 하는 것이 낫다. 4ms  정도마다 검사를 한다. 응용프로그램의 system call로 운영체제로 가거나 timer가 울릴때까지 검사를 한다.



## Context switch

- Saving and restoring context
  - Save a few register values for the currently - executing process (onto its kernel stack)
  - Restore a few for the soon-to-be- executing process (from its kernel stack)
  - Ensures that when the return - from - trap - instruction is finally executed, the system resumes execution of another process

> 검사하기 위해 필요한 값들을 커널스택에 저장하고 이후에 쓸 값을 커널스택으로부터 저장하고 이어서 일을 처리할수 있게 환경을 계속 유지해준다.



| OS                                                           | Hardware                                                     | Program   |
| ------------------------------------------------------------ | ------------------------------------------------------------ | --------- |
| Initialize trap table                                        |                                                              |           |
|                                                              | Remember address of syscall handler and timer handler        |           |
| Start interrupt timer                                        |                                                              |           |
|                                                              | Start timer<br />(Interrupt CPU in X ms)                     |           |
|                                                              |                                                              | Process A |
|                                                              | Timer interrupt<br />save Uregs(A) to k-stack(A)<br />move to kernel mode<br />Jump to trap handler |           |
| Handle the trap<br />Call switch() routine<br />   save Kregs(A) to k-stack(A)<br />   restore Kregs(B) from k-stack(B)<br />   switch to k-stack(B)<br />Return - from - trap (intoB) |                                                              |           |
|                                                              | Restore Uregs(B) from k-stack(B)<br />move to user mode<br />Jump to B's PC |           |
|                                                              |                                                              | Process B |