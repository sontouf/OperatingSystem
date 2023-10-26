# Address Spaces

## Early Systems

- Didn't provide much of an abstraction

  - There would be one running program that currently sat in physical memory

  > 추상화가 구현을 안했어서 그냥 메모리공간에 하나의 프로세스를 위한 공간이었다. 일단 다 실행시키고 다음 프로세스한테 넘기는 식이었다. 여러 프로세스가 대기해도 메모리공간 전체를 하나의 프로세스를 위해서만 썼기에 효율성이 굉장히 떨어졌다.

## Multiprogramming and Time Sharing

- Multiprogramming

  - Multiple processes were ready to run at a given time, and the OS would switch between them
  - Increases the effective utilization of the CPU

  > 메모리공간을 쪼개서 각 프로세스에게 할당해준다. CPU 활용도가 높아짐.

- Time sharing

  - Crude sharing of the 'machine'. saving entire contents of memory to disk is costly (previous slide)
  - Leave processes in memory while switching between them

  > 메모리를 할당해주고 다 쓰면 switch 해준다.

- Protection issue

  - A process should not be able to read or write some other process's memory

  > 다른 프로세스 공간에 침범하지 않게 검사하는 장치가 필요하다.



## Address Spaces

- Address space
  - Easy to use abstraction of physical memory
  - Running program's view of memory in the system
    - Contains all of the memory state of the running program
    - Code, stack, and heap
  - Example
    - 64KB address space starting at 0
    - The program really isn't in memory at physical addresses 0 through 64KB

> 주소공간을 추상화로 활용한다. 프로세스에 필요한 모든 정보가 메모리에 저장되는데 실제 메모리공간위치에 그 값이 있는 것은 아니다.

## How to Virtualize Memory

- How can the OS build this abstraction of a private, potentially large address space for multiple running processes on top of a single, physical memory



## Virtual Memory

- Running program thinks it is loaded into memory at a particular address (0) and has a potentially very large address space (32 - bits or 64 - bits)

  > 가상적으로 굉장히 무한한 공간을 가진다고 가정. 실제로 그렇진 않음.

  - Transperency

    - The program behaves as if it has its own private physical memory

  - Effciency

    - Time : not making programs run much more slowly
    - Space : not using too much memory for structures needed to support virtualization

    > 메모리를 찾아가는 데 시간이 조금 걸리고 메모리의 위치를 저장할 새로운 자료구조가 필요

  - Protection

    - Isolation among processes
    - The OS should make sure to protect processes from one another as well as the OS itself from processes

    > OS 가 할당된 공간 외를 벗어나지 않도록 잘 관리해준다.



## Type of Memory

- Stack
  - Allocations and deallocations of it are managed implicitly by the compiler
  - Sometimes called automatic memory
- Heap
  - Allocations and deallocations of it are managed explicitly by the programmer
  - Live beyound the call invocation
  - Presents more challenges to both users and systems





## Memory API

- void *malloc (size_t size)
  - A library call ( not a system call)
  - Built on top of some system calls which call into the OS to ask for more memory
    - brk : change the location of the program's break (the location of the end of the heap)
    - sbrk
- void free(void *)



## Common Errors

- Forgetting to allocate memory

  - Segmentation fault

  ```c
  char *src = "hello";
  char *dst; // unallocated
  strcpy(dst, src); // segfault and die
  ```

  ```c
  char *src = "hello";
  char *dst = (char *)malloc(strlen(src) + 1); // 널문자 공간까지 생각.
  strcpy(dst, src);
  ```

- Not allocating enough memory

  - Buffers overflow

  ```c
  char *src = "hello";
  char *dst = (char *)malloc(strlen(src)); // too small
  strcpy(dst, src); // 널공간까지 추가로 넣어줘야함
  ```

- Forgetting to initialize allocated memory

- Forgetting to free memory

  - Memory leak

- Freeing memory before you are done with it

  - Dangling pointer

- Freeing memory repeatedly

  - Double free

- Calling free() incorrectly

  - Invalid free



## Address Spaces

- Address space
  - Beautiful illusion that the program has its own private memory, where its own code and data reside
- Ugly physical truth
  - Many programs are actually sharing memory at the same time
  - CPU switches between running one program and the next



## Address Translation

- Address translation

  - Changing the virtual address provided by the instruction to a physical address where the desired information is actually located

    - Fetch(가져오기), Load, Store

  - Example

    ```c
    void func() {
        int x = 3000;
        x = x + 3;
    ...
    }
    ```

    ```
    128: movl 0x0(%ebx), %eax
    132: addl $0x03, %eax
    135: movl %eax, 0x0(%eax)
    ```

    ```
    Fetch the instruction at address 128
    Execute this instruction (load from address 15KB)
    Fetch the instruction at address 132
    Execute this instruction (no memory reference)
    Fetch the instruction at address 135
    Execute this instruction (store to address 15KB)
    ```

## Hardware - based Address Translation

- On each and every memory reference, an address translation is performed by the hardware
  - Hardware alone cannot virtualize memory
    - OS must get involved at key points to set up the hardware
- How to effciently and flexible virtualize memory
  - How do we provide the flexibility needed by applications?
    - To be able to use programs address spaces in whatever way they would like
  - How do we maintain control over whick memory locations an application can access?
    - To ensure that no application is allowed to access any memory but its own
- How can we build an efficient virtualization of memory



## Assumptions for Simple Memory Virtualization

- User's address space must be placed contiguously in physical memory
- Size of address space is not too big
  - Less than the size of physical memory
- Each address space is exactly the same size



## Dynamic (Hardware-based) Relocation

- Base and bounding
  - base and bounds registers
    - base register allows us to place the address space anywhere in physical memory
    - bounds register ensures that the process can only access its own address space
- Address translation
  - Physical address = Base + Virtual address
  - Example
    - 128 : movl 0x0 (%eax), %eax
    - PC : 128 -> 32KB(32768B) + 128B = 32896B
    - x : 15KB -> 32KB + 15KB = 47KB
- It is dynamic
  - Relocation happens at runtime and can be changed even after the process has started running
- Protection
  - Check that the memory reference is within `bounds`
  - If a virtual address is greater than the `bounds` or negative, the CPU will raise an exception





## Hardware Support

| Hardware Requirements                                        | Notes                                                        |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| Privileged mode                                              | Needed to prevent user-mode processes from executing privileged operations |
| base/bounds registers                                        | Need pair of registers per CPU to support address translation and bounds checks |
| Ability to translate virtual addresses and check if within bounds | Circuitry to do translations and check limits (in this case, quite simple) |
| Privileged instruction(s) to update base/ bounds             | OS must be able to set these values before letting a user program run |
| Privileged instruction(s) to register exception handlers     | OS must be able to tell hardware what codeto run if exception occurs |
| Ability to raise exceptions                                  | When processes try to access privileged instructions or out-of-bounds memory |



## Operating System Issues

| OS Requirements          | Notes                                                        |
| ------------------------ | ------------------------------------------------------------ |
| Memory management        | Need to allocate memory for new provesses; Reclaim memory from terminated processes; Generally manage via free list |
| base / bounds management | Must set base/bounds properly upon context switch or moving a provess's address space |
| Exception handling       | Code to run when exceptions arise; likely action is to terminate offending process |

*) Variable -sized address spaces and Larger address spaces than the size of physical memory are more difficult to handle

