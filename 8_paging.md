## Summary of Last Class

- Segmentation

  ![8_paging1](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging1.png)

  > 큰 address space는 처리하기가 힘듦기 때문에 공간을 segment(code, heap, stack)로 나누게 되었고 메모리 관리를 효율적으로 하게 되었다.
  >
  > 00 code, 01 heap, 11 stack

- Address translation

  ![8_paging2](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging2.png)

  > 변환을 위해서는 base, bounds 값이 필요하다. 물리메모리에 translation 하기 위해 offset을 구해 bounds 넘는지 확인하고 변환하면 된다. offset은 heap이면 positive direction, stack은 negtive direction으로 메모리에 적재된다.

  > 어떤 segment 인가 -> offset 계산 -> physical address 를 구한다.

- OS support

  - Context switch

    > I/O 요청처럼 process 끼리 context switch가 일어날 때 register 정보를 PCB에 저장하고 restore한다.

  - Managing free space

    - Best-fit
    - Worst-fit
    - First- fit

    > 빈공간이 너무 많아져 낭비되는 공간이 많아질때 한번 정렬하고자 한다. 이 때 free-list에 정렬되는 방식이 달라진다.



# Paging

## Space management

- Segmentation

  - Variable - sized memory pieces

  - External fragmetation

    ![8_paging3](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging3.png)

    > segment 의 단점 : 공간을 모아줄 떄 overhead가 너무 크다.

- Paging

  - View physical memory as an array of fixed-sized slots called page frames
  - Can support the abstraction of an address space effectively, regardless of how a process uses the address space
    - No need of direction information of heap and stack
  - Simplicity of free-space management ( ex / buddy algorithm )

  > segment 은 해당 공간을 base, bounds 등등 기억해야 하는 register가 많이 필요했지만 paging에서는 이미 공간을 다 쪼개놓고 필요한 양만큼 공간을 주기 때문에 메모리가 쓰고 있는 공간만 기억하면 된다.

## Simple Example

![8_paging4](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging4.png)

- 물리적으로 linear 처럼 처리한다.

## Address Translation

- Page table

  - Per-process data structure to store address tanslations for each of the virtual pages of the address space

    ![8_paging5](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging5.png)

- Virtual address

  - Virtual page number + offset

    > segmentation 처럼 상위 두 비트는 segment 확인
    >
    > offset은 운영체재 마다 할당하는 bit수가 다르다.

  - Simple example

    ![8_paging6](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging6.png)

    > 2^6 만큼의 주소를 가질 수 있다.
    >
    > VPN : Virtual page number

- Address : 21

![8_paging7](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging7.png)

> offset은 그대로 가져간다.
>
> address translation을 통해 page number를 frame number로 바꾼다.

## Questions you May Have

- How big are the tables?

  > page table 은 너무 크지 않을까. 너무 크면 register로 쓰기 힘들다.

- Where are these page tables stored?

  > 메모리공간에 같이 저장된다.

- What are the typical contents of the page table?

  > 필요한 공간만 저장한다.

- Does paging make the system (too) slow?

  > 메모리 관리는 좋은데 속도는 어떨까?



## Where Are Page Tables Stored?

- Page tables can get terribly large

  - 32-bit address space with 4KB pages

    - 20-bit VPN implies that there are 2^20 translations for each process

      - 4 - bytes per page table entry (PTE)

      - 4MB of memory needed for each page table

        ![8_paging8](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging8.png)

        > 32 bits * 2^20 = 4MB

- Store the page table for each process in physical memory that the OS manages

  - Too big to store in any special on - chip hardware

    > segmentation 은 register로 저장해둔다
    >
    > page table은 메모리상에 저장되는 데 너무 용량이 크다. 또한 OS가 관리하기 때문에 address tranlation은 CPU가 책임진다.

- Page table

  - Data structure that is used to map virtual addresses to physical addresses

    - VPN -> PFN

  - Linear page table

    - indexes the array by VPN, and looks up the PTE at that index

    - We will see more advanced adta structures in later chapters

      ![8_paging9](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging9.png)



## What's Actually in the Page Table?

- PTE

  > 일반적으로 들어가는 정보는 OS 마다 조금씩 다르다.

  - Valid bit

    - Indicates whether the particular translation is valid

      - All the unused space is marked invalid

    - Can support a sparse address space

      > 현재 프로그램을 실행하면서 필요한 정보만 매핑되면 된다.

  - Protection bits

    - Indicates whether the page could be read from, written to, or executed from

  - Present bit

    > 지금 필요한, 현재 메모리에 있는지. / 안 켜져있으면 지금 당장은 안 필요하다.

    - Indicated whether this page is in physical memory or on disk (i.e., swapped out)

  - Dirty bit

    > mapping 후에 해당 page 내용이 써진다. write operation 

  - Reference bit

    - Used by page replacement policy

      > 메모리 부족상황에서는 최대한 자주 접근하는 정보는 안 빼도록 하는 정보

- Example

  - -x86 - 32 PTE
    - P : present bit
    - R/W : read / write bit
    - U/S : user / supervisor bit
    - A : accessed bit
    - D : Dirty bit
    - PWT, PCD, PAT, G : hardware caching policy
    - PFN : page frame number

  ![8_paging10](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging10.png)



## Isn't It Too Slow?

- The Hardware must know where the page table is for the currently-running process

- Page-table base register

- Address translation

  ![8_paging11](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging11.png)

> 하위 bit 4개를 없앤다.
>
> Offset_Mask = 001111

## Buddy Algorithm

- Pro : fast
- Con : internal fragmentation

![8_paging12](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging12.png)

> 단위보다 작은 프로세스용량이 들어와도 정해진 크기를 무조건 할당해줌.



## Q&A

![8_paging13](C:\rep\ME\2023\건 3-1\OperatingSystem\image\8_paging\8_paging13.png)

- Program 의 code 와 data 는 물리메모리에 적재되는 데, 각 process 마다, 독립된 address space 를 가상적으로 생성해서 물리 메모리에 있는 code 및 data 들을 address space에 매핑하는 것이다.