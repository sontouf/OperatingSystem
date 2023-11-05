## Summary of Last Class

![11_swapping_1](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\11_Swapping\11_swapping_1.png)

- Example

![11_swapping_2](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\11_Swapping\11_swapping_2.png)

## Demand Paging

- OS brings the page into memory when it is accessed
  - On demend
- Page fault
  - If a page is invalid (and not present)
- Perfetching
  - E.g., if a code page P is brought into memory, code page P + 1 will likely soon be accessed and thus should be brought into memory too



# Swapping

## How to Go Beyond Physical Memory

- Supporting many concurrently - running large address spaces
  - We've assumed that all pages reside in physical memory
  - To support large address spaces, OS requires an additional level in the memory hierarchy
    - To stash away portions of address spaces that currently aren't in great demand
    - Should have more capacity than memory (e.g., hard disk drive, SSD)
  - How can the OS make use of a larger, slower device to transparently provide the illusion of a large virtual address space?



## Swap Space

- Swap space
  - Reserved space on the disk for moving pages back and forth
  - Swap pages out of memory to it (swap-out) and swap pages into memory from it (swap-in)
    - OS can read from and write to the swap space, in page-sized units
    - OS needs to remember the dist address of a given page

![11_swapping_3](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\11_Swapping\11_swapping_3.png)

## Present Bit

- TLB miss
  - When the hardware looks in the PTE, it may find that the page is not present in physical
  - Present bit
    - If the present bit is set to one, it means the page is present in physical memory
    - If it is set to zero, the page is not in memory but rather on disk somewhere

## Page Fault

- Page fault
  - The act of accessing a page that is not in physical memory
- Page - fault handler
  - If a page is not present and has been swapped to disk, the handler will need to swap the page into memory
  - How will the OS know where to find the desired page?
    - Uses PFN in PTE for a disk address
  - When the disk I/O completes, the OS will then update the present bit and PFN
    - While the I/O is in flight, the process will be in the blocked state



## Page Fault Control Flow (Hardware)

![11_swapping_4](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\11_Swapping\11_swapping_4.png)

## Page Fault Control Flow (Software)

![11_swapping_5](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\11_Swapping\11_swapping_5.png)

## Swap Entry in Linux / x86-32

![11_swapping_6](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\11_Swapping\11_swapping_6.png)

![11_swapping_7](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\11_Swapping\11_swapping_7.png)

## Page Replacement

- What if memory is full?
  - Page out (swap out) one or more pages th make room for the new page(s) the OS is about to bring in
- When replacements really occur?
  - High Watermark (HW) and Low Watermark (LW)
  - Swap daemon
    - If there are fewer than LW pages available, frees memory
    - Evics pages until there are HW pages avaliable
- How to decide which page to evict?
  - Making the wrong decision can cause a program to run at disk-like speeds



## Page - Replacement Policy

- Optimal replacement policy
  - Replaces the page that will be accessed furthest in the future
  
    ![11_swapping_8](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\11_Swapping\11_swapping_8.png)
  
- FIFO
  
  ![11_swapping_9](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\11_Swapping\11_swapping_9.png)
  
  - Belady's anomaly : hit rate gets worse when the queue size increases
  
- Least-Frequently-Used(LFU) and Least-Recently Used(LRU)
  - Use history (e.g., frequency, recency) as guide
  
    ![11_swapping_10](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\11_Swapping\11_swapping_10.png)
  
- Implementing historical algorithms
  - To keep track of which pages have been least- and most- recently used, the system has to do some accounting work on every memory reference
  - Hardware support for time field
    - e.g., in PTE of some separate array
    - As the number of pages in a system grows, scanning a huge array of times is expensive
  
- Approximation LRU
  - Hardware support for use bit / reference bit
  
  - Clock algorithm
  
    ![11_swapping_11](C:\Ref\ME\2023\건 3-1\OperatingSystem\image\11_Swapping\11_swapping_11.png)
  
- Considering dirty pages
  - If the page has not been modified, it can simply be reused for other purposes without additional I/O