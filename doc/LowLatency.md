Low Latency {#LowLatency}
===========

Clock
-----

* NTP
* PTP

CPU Isolation
-------------

Isolate CPUs from the load-balancing and scheduling algorithms using the `isolcpus` boot
parameter. The `taskset` command can then be used to run processes on the isolated CPUs.

Interrupts
----------

The `irqbalance` allows specific CPUs to be skipped when rebalancing IRQs. See the
`IRQBALANCE_BANNED_CPUS` option for more details.

Use the following command to check the interrupts that have occurred on each CPU:

    $ cat /proc/interrupts

Numa
----

Show distances between Numa nodes:

    $ numactl --hardware

Bind process to specific CPU and Numa nodes:

    $ numactl --cpunodebind=0 --membind=0 echo test

Perf
----

Install performance analysis tools:

    # pacman -S perf

Scheduler
---------

* `SCHED_FIFO`
* `SCHED_RR`
* `SCHED_OTHER`
