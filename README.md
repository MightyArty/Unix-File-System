# Unix File System
## Why do we need file system?
* Disks are huge block devices
* We want to store data and read it
* We want to know where data is sotred and what is free space
* We want to sync different application stroring modules
* So we create file system to manage all that

## What is Unix File System?
>Unix file system is a logical method of organizing and storing large amounts of information in a way that makes it easy to manage. A file is a smallest unit in which the information is stored. Unix file system has several important features. All data in Unix is organized into files. All files are organized into directories. These directories are organized into a tree-like structure called the file system.

>Files in Unix System are organized into multi-level hierarchy structure known as a directory tree. At the very top of the file system is a directory called “root” which is represented by a “/”. All other files are “descendants” of root.

## File System properties
### Block:
>* Place to store data
>* Each block is identified by unique address it can be used or not
>* Files are saved on discrete number of blocks.
>* Each block is identified by unique identifier

### Inode:
>* Reference to a file (one inode per file)
>* Points directly and inderectly to blocks
>* Contain the OS info on a file
>* Each Inode is identified by unique number

### Super block:
>* The file system catalog
>* General information about the file system such as: number of Inodes, number of blocks, number of used and free Inodes and blocks.
