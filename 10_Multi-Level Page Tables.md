## Summary of Last Class

- Segmentation
  - Variable - sized memory pieces
- Paging
  - View physical memory as an array of fixed-sized slots called page frames
  - Page table
- TLB
  - Address-translation cache
  - If the desired translation is held in TLB, the translation is performed quickly without having to consult the page table



# Multi-Level Page Tables

## Paging

- Linear page table
  - Page tables are too big and thus consume too much memory
  - Example
    - 32bit address space with 4KB pages
    - 4B PTE
    - Page table is 4MB in size

- How to make page tables smaller?
  - What are the key ideas?
  - What inefficiencies arise as a result of these new data structures?

## Example 

- 16KB address space with 1KB pages

![10_Multi_level PT_1](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_1.png)

> 내용이 안 적혀있어도 4MB 필요. 정보가 일단은 다 적혀있다.
>
> VPN 을 index로 활용한다.

## Simple Approach

- Bigger pages
  - Example
    - 32 - bit address space with 16KB pages
    - 1MB per page table

- Big pages lead to waste within each page
  - Internal fragmentation
- Most systems use relatively small page sizes in the common case
  - 4KB in x86
  - 8KB in SPARCv9



## Hybrid Approach

- Paging and segments

  - One page table per segment
  - Three base/bounds pairs > heap, stack, code
  - Base register : holds the physical address of the page table of that segment
    - does not point to the segment itself
  - Bounds register : holdsd the value of the maximum valid page in the segment
  - Significant memory savings compared to the linear page table
    - Unallocated pages between the stack and the heap no longer take up space in a page table

- Example

  - 32-bit virtual address space with 4KB pages

  - Four segments

    - 00 : unused
    - 01 : code
    - 10 : heap
    - 11 : stack

    ![10_Multi_level PT_2](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_2.png)

- Cons

  - Large but sparsely-used heap -> a lot of table waste
  - External fragmentation
  - Page tables now can be of arbitrary size -> finding free space for them in memory is more complicated

  > hybrid로 섞어봤지만 안 쓰는 정보도 들어가 아직도 overhead가 존재한다.





## Multi-Level Page Tables

- Page table
  - Chop up the page table into page-sized units
  - If an entire page of PTEs is invalid, don't allocate that page of the page table at all
- Page directory
  - Used to tell you where a page of the page table is, or that the entire page of the page table contains no valid pages





## Address Translation

![10_Multi_level PT_3](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_3.png)

![10_Multi_level PT_4](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_4.png)

![10_Multi_level PT_5](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_5.png)

- Time-space trade - off
  - On a TLB miss, two loads from memory will be required to get the right translation information from the page table
    - One for the page directory, and one for the PTE itself
- Complexity
  - Whether it is the hardware of OS handing the page - table lookup ( on a TLB miss ), doing so is more involved than a simple linear page-table lookup





## Example

- Address space

  - Address space of size 16KB, with 64-byte pages

    - 14-bit addressing : 8 bits for the VPN and 6 bits for the offset ( 2^14 = 16KB)

  - Each PTE is 4 bytes

    > 한 page 에 16개 entry, PTE 존재



- Page table
  - Linear page table
    - 2^8(=256) entries
    - 256 x 4B = 1KB
  - Multi-level page tables
    - Each page can hold 16 PTEs

![10_Multi_level PT_6](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_6.png)

![10_Multi_level PT_7](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_7.png)

## x86 - 32 (2 - Level Paging)

![10_Multi_level PT_8](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_8.png)



## PDE and PTE

![10_Multi_level PT_9](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_9.png)

## x86 - 32 Address Space

![10_Multi_level PT_10](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_10.png)

> 32 bit / 2^32bytes = 4GB

## Sharing Kernel Address Space

![10_Multi_level PT_11](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_11.png)

> 1 page 4KB
>
> 1024 pageTE -> 4MB mapping 
>
> 1024개 4GB

## x86 - 64 (4 - Level Paging)

![10_Multi_level PT_12](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_12.png)

> 2^64는 메모리양 너무 크다. 그래서 48bit 정도만 쓴다.

## x86 - 64 Address Space

![10_Multi_level PT_13](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\10_Multi-Level Page Tables\10_Multi_level PT_13.png)