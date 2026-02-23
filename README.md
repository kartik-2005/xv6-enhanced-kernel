# xv6 Kernel Extensions

This repository serves as a monorepo for advanced operating system feature implementations built upon the xv6 RISC-V kernel.

> **Note:** Because these features involve deep, mutually exclusive modifications to core kernel subsystems (Process Scheduler, Virtual Memory, Trap Handling), they are isolated in separate **Git Branches**.

## Implementation Modules

To view the code for a specific feature, please **switch to the corresponding branch** using the dropdown menu above or the links below.

### 1. [Branch: Weighted Round Robin Scheduler](https://github.com/kartik-2005/xv6-enhanced-kernel/tree/feature/weighted-scheduler)
**Focus:** Process Scheduling & CPU Resource Management
*   **Algorithm:** Implemented a **Weighted Round Robin (WRR)** policy where CPU time slices are proportional to user-defined process priorities.
*   **Starvation Avoidance:** Designed the dispatcher to ensure low-priority processes receive minimal CPU guarantees, preventing indefinite blocking.
*   **System Calls:** Added `set_priority(pid, n)` and `get_priority()` to allow dynamic userspace control over process execution weights.
*   **Concurrency:** Secured the process control block (PCB) with spinlocks to prevent race conditions during priority updates in a multi-core environment.

### 2. [Branch: Copy-on-Write (COW) Fork](https://github.com/kartik-2005/xv6-enhanced-kernel/tree/feature/copy-on-write)
**Focus:** Virtual Memory Optimization
*   **Mechanism:** Implemented **Atomic Reference Counting** for physical pages (`kalloc.c`).
*   **Logic:** Modified `fork()` to map parent pages as read-only in the child. Upon a write attempt (Page Fault), the kernel allocates a new page, copies data, and updates permissions.
*   **Impact:** Significantly reduces memory overhead and `fork()` latency for large processes.

### 3. [Branch: Demand Paging & MRU Replacement](https://github.com/kartik-2005/xv6-enhanced-kernel/tree/feature/demand-paging)
**Focus:** Memory Virtualization & Swapping
*   **Page Replacement:** Implemented **Most Recently Used (MRU)** eviction policy to manage resident pages.
*   **Disk Swapping:** Enabled swapping victim pages to a simulated disk interface when physical RAM is exhausted.
*   **Monitoring:** Extended the kernel to track page faults, swap-ins, and swap-outs via the `getpagestat()` system call.

---

## Tech Stack
*   **Kernel:** C (RISC-V Architecture)
*   **User Space:** C
*   **Assembly:** RISC-V (`trampoline.S`, `entry.S`)
*   **Tools:** QEMU Emulator, GDB Debugger

## Usage
To run any specific implementation:
1. Clone the repo:
   ```bash
   git clone https://github.com/kartik-2005/xv6-enhanced-kernel.git
   cd xv6-enhanced-kernel
   ```
2. Switch to the desired feature branch:
    ```bash
    git checkout feature/weighted-scheduler
    ```

    ```bash
    git checkout feature/copy-on-write
    ```

    ```bash
    git checkout feature/demand-paging
    ````
3. Compile and run:
    ```bash
    make clean
    make qemu
    ```

---
