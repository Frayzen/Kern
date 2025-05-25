# NVME

NVMe controllers can be found as PCI devices with class code 1 and subclass code 8.
The controller processes commands submitted to it from "submission queues". The driver prepares commands in the queue's circular buffer in memory, and then updates the tail pointer register for the queue.
When the controller has finished processing a command, it appends an entry to a "completion queue". The completion queue to use is specified when a submission queue is created. The controller sends an interrupt when a completion queue has available commands. The driver processes all new entries in the queue's circular buffer, and then updates the head pointer register for the queue.

## Admin queues

At reset, only one submission queue and one completion queue exists. These are the admin queues. The admin queues can process admin commands, such as creating IO queues (used to submit IO commands, like read/write sectors), and query information about the controller and drives (called "namespaces") connected to it. The admin queues are unique and have identifiers 0. The associated MSI(-X) vector is also always the 0 one.
The admin commands include:
* Create IO submission queue
* Create IO completion queue
* Identify

## IO queues

For common purposes, you will need one Admin completion queue and one Admin submission queue, and you will need at least one IO completion queue and one IO submission queue. IO queues are used to perform IO actions on your NVMe controller (Such as reading or writing sectors)

The IO commands include:
* Reqd
* Write

