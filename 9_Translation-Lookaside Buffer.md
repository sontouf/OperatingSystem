## Summary of Last Class

- Paging

  > segmentation 은 길이가 가변적이다.
  >
  > 고정된 크기로 짜르고 mapping 4KB

  - View physical memory as an array of `fixed-sized` slots called page frames

  - Page table

    - Per-process data structure to store address translations for each of the virtual pages of the address space

  - 6 - bit addressing

    - 64-byte address space

    - 16-byte page frame

      ![9_TLB_1](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\9_TLB\9_TLB_1.png)

- Questions

  - How big are the tables ? 

    > 32 -bit 기준으로 process 당 4MB 필요

  - Where are these page tables stored?

    > 메모리 상에 존재, page table 의 base를 가리키는 레지스터도 필요하다.

  - What are typical contents of the page table?

    > 페이지 속성에 관한 여러가지 정보를 담고 있다. 권한 등등

  - Does paging making the system (too) slow?

    > 메모리 접근 하는 속도정도.



# Translation - Lookaside Buffer

## Paging

- Paging

  - Address space is chopped into small, `fixed-sized` units (i.e., pages)
  - Requires `a large amount of` mapping information
    - Stored in physical memory
    - Requires an extra memory lookup for each virtual address

- How to spped up address translation

  - What hardware support is required?
  - What OS involvement is needed?

- TLB

  - Part of the chip's (CPU) memory management unit (MMU) 

    >  CPU 가 해줘야 하는 부분.

  - Address - translation cache

    - If the desired translation is held in TLB, the translation is performed quickly without having to consult the page table

      > 메모리까지 가지 않고 미리 자주 접근하는 부분을 알려주는 역할을 해주자.



## Basic Algorithm

- Assuming a simple linear page table

![9_TLB_2](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\9_TLB\9_TLB_2.png)

> TLB hit 면 바로 찾아서 실행
>
> 없으면 miss 면 메모리까지 가서 찾아야 함.

## Example

![9_TLB_3](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\9_TLB\9_TLB_3.png)

- A simple virtual address space

  - 8 bit addressing (VPN 4, offset 4)
  - 16 byte pages

- An array of 10 4 - byte integers

  - Starting at virtual address 100

- A simple loop

  ```c
  int sum = 0;
  for (i = 0; i < 10; i++)
  {
      sun += a[i];
  }
  ```

  - hit rate = 70%
  - Spatial and temporal locality



## Who Handles the TLB Miss?

- Hardware - managed TLB

  - CISC (e.g., x86)

  - Hardware has to know where the page tables are located in memory (via PTBR), as well as their exact format

    - In x86, CR3 and multi - level page table

      > 스스로 처리한다.

- Software (OS) - managed TLB

  - RISC

    > 스스로는 처리 못해서 except을 발생시켜 운영체제가 처리하게 한다.

    ![9_TLB_4](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\9_TLB\9_TLB_4.png)

  - Trap handler uses privileged instructions to update the TLB

    - Must resume execution at the instruction that caused the trap
    - Need to be careful not to cause an infinite chain of TLB misses

  - OS can use any data structure it wants to implement the page table



## TLB Contents

- Fully associative  - cache 와 유사
  - Any given translation can be anywhere in the TLB
  - The hardware searches the entire TLB in parallel to find the desired translation
- VPN | PFN | other bits
  - Valid bit : says whether the entry has a valid translation or not
  - Protection bits : determine how a page can be accessed as in the page table
  - Address - space identifier, dirty bit, etc.



## Context Switches

- TLB contains virtual - to - physical tranlations that are only valid for the currently running process

  ![9_TLB_5](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\9_TLB\9_TLB_5.png)

  > VPN 정보로는 구별이 어려워서 더 정보가 필요하다.

- How to manage TLB contents on a context switch

  - Flush the TLB on context switches - 오버헤드 발생

    - If the OS switches between proesses frequently, this cost may be high

  - Address space identifier (ASID)

    > 중복된 VPN - ASID 로 구별하자.

    ![9_TLB_6](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\9_TLB\9_TLB_6.png)

## Sharing a Page

- Sharing of pages

  - Reduce number of physical pages in use, tus reducing memory overheads

    - Binaries, shared libaries, fork()

  - Shared memory IPC

    > 가상페이지 넘버는 다른데 프레임넘버는 같다. 즉 공유하고 있다는 것이다. process address 가 용량이 커서 쓸 때만 카피하자.

    ![9_TLB_7](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\9_TLB\9_TLB_7.png)



## Replacement Policy

![스크린샷 2023-11-01 174305](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\9_TLB\스크린샷 2023-11-01 174305.png)

- How to design TLB replacement policy

  - Which TLB entry should be replaced when we add a new TLB entry?
  - Aim to minimize the miss rate

  > 모두 valid 상태에서 새로운 page 발생하면 하나 쫒아내고 새로 넣어야 하는데, TLB miss를 최소한 하는 방법으로 결정해야 한다.

- Basic policies

  - Least - Recently - Used (LRU)

    - Assumes that an entry that has not recently been used is a good candidate for eviction
    - Unreasonable when a program loops over n + 1 pages with a TLB of size n

    > 최악의 경우는 방금 내보낸 친구에 접근해야 하는 경우가 발생하면 계속 miss 가 뜰수있다.

  - Random

    - Evicts a TLB mapping at random

      > 평타는 친다.





## Example

- MIPS TLB entry

  - 32 - bit address space with 4KB page
    - 12 - bit offset
    - 20 - bit VPN

  ![스크린샷 2023-11-01 174322](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\9_TLB\스크린샷 2023-11-01 174322.png)

  - PFN

    - 24 bits -> 64GB main memory (2^24 * 4KB)

  - Global bit (G)

    - Used for pages that are globally- shared among processes

  - ASID bits

    - Used to distinguish between address spaces
    - Fewer bits than PID
      - Refuse to create more processes than the number of ASIDs
      - Behave as if ASIDs are not supported
      - Allocate ASIDs to processes dynamically

    > process 개수와 CPU가 제공하는 ASID 랑 같도록 하자
    >
    > ASID 가 없는 것처럼 동작하자 (PID > ASID), context overhead 발생
    >
    > 1대 1로 매칭 동적으로 할당하자. Flush 횟수가 min이 되도록 고민하자.
    >
    > 최대한 ASID 쓰자.

  - Coherence bits (C) : 캐시정책

    - Determine how a page is cached by the hardware

  - Dirty bit (D) : write operaion 발생했냐.

  - Valid bit (V) : valid 하냐