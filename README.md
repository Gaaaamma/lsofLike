# lsofLike
1102_UNIX_Programming Class HW1

# Homework #1
Advanced Programming in the UNIX Environment

Due: March 21, 2021

Read carefully before you implement and submit your homework.

## Implement a 'lsof'-like program
In this homework, you have to implement the 'lsof' tool by yourself. 'lsof' is a tool to list open files. It can be used to list all the files opened by processes running in the system. The output of your homework is required to follow the spec strictly. The TAs will use the 'diff' tool to compare your output directly against our prepared sample test data.

## Program Arguments

Your program should work without any arguments. In the meantime, your program has to handle the following arguments properly:

-c REGEX:<br>
a regular expression (REGEX) filter for filtering command line. For example -c sh would match bash, zsh, and share.

-t TYPE:<br> 
a TYPE filter. Valid TYPE includes  REG,  CHR,  DIR,  FIFO,  SOCK, and  unknown. TYPEs other than the listed should be considered invalid. For invalid types, your program has to print out an error message Invalid TYPE option. in a single line and terminate your program.

-f REGEX:<br> 
a regular expression (REGEX) filter for filtering filenames.

A sample output from this homework is demonstrated as follows:<br>
$ ./hw1 | head -n 20<br>
COMMAND         PID             USER            FD              TYPE            NODE            NAME<br>
systemd         1               root            cwd             unknown                         /proc/1/cwd (Permission denied)<br>
systemd         1               root            rtd             unknown                         /proc/1/root (Permission denied)<br>
systemd         1               root            txt             unknown                         /proc/1/exe (Permission denied)<br>
systemd         1               root            NOFD                                            /proc/1/fd (Permission denied)<br>
kthreadd                2               root            cwd             unknown                         /proc/2/cwd (Permission denied)<br>
kthreadd                2               root            rtd             unknown                         /proc/2/root (Permission denied)<br>
kthreadd                2               root            txt             unknown                         /proc/2/exe (Permission denied)<br>
kthreadd                2               root            NOFD                                            /proc/2/fd (Permission denied)<br>
rcu_gp          3               root            cwd             unknown                         /proc/3/cwd (Permission denied)<br>
rcu_gp          3               root            rtd             unknown                         /proc/3/root (Permission denied)<br>
rcu_gp          3               root            txt             unknown                         /proc/3/exe (Permission denied)<br>
rcu_gp          3               root            NOFD                                            /proc/3/fd (Permission denied)<br>
rcu_par_gp              4               root            cwd             unknown                         /proc/4/cwd (Permission denied)<br>
rcu_par_gp              4               root            rtd             unknown                         /proc/4/root (Permission denied)<br>
rcu_par_gp              4               root            txt             unknown                         /proc/4/exe (Permission denied)<br>
rcu_par_gp              4               root            NOFD                                            /proc/4/fd (Permission denied)<br>
kworker/0:0H-events_highpri             6               root            cwd             unknown                         /proc/6/cwd (Permission denied)<br>
kworker/0:0H-events_highpri             6               root            rtd             unknown                         /proc/6/root (Permission denied)<br>
kworker/0:0H-events_highpri             6               root            txt             unknown                         /proc/6/exe (Permission denied)<br>
...<br>

The detailed spec of this homework is introduced as follows. Your program has to output the following fields (columns) for each file opened by a running process. Each line presents the information for a single file. The required fields include COMMAND, PID, USERM, FD, TYPE, NODE, and NAME. The meaning of each field (column) is explained below.

### COMMAND:
The executable filename of a running process.
DO NOT show arguments.

### PID:
Process id of a running process.
Only need to handle opened files in process level (check /proc/[pid]. No need to handle opened files at thread level (that would be in /proc/[pid]/task/[tid]).

### USER:
The username who runs the process.
Please show username instead of UID.

### FD:
The file descriptor. The value shown in the FD field can be one of the following cases.
cwd:<br>
The current working directory, which can be read from /proc/[pid]/cwd.<br>

rtd:<br>
root directory, which can be read from /proc/[pid]/root.<br>

txt:<br>
program file of this process, can be read from /proc/[pid]/exe.<br>

mem:<br>
memory mapping information, which can be read from /proc/[pid]/maps.<br>

If /proc/<pid>/maps is not accessible, you don't need to show any information about mapped files.
A memory-mapped file may have multiple segments or be mapped multiple times. You only need to output the first one for duplicated files, i.e., files having the same i-node or filename.
You don't need to handle memory segments that do not associate with a file. For example, [heap] or anonymously mapped memory segments. Those memory segments should have an i-node number of zero.

DEL:<br>
indicate a memory-mapped file has been deleted. You should show this value if there is a (deleted) mark right after the filename in memory maps.

[0-9]+[rwu]:<br>
file descriptor and opened mode.<br>
The numbers show the file descriptor number of the opened file.<br>
The mode "r" means the file is opened for reading.<br>
The mode "w" means the file is opened for writing.<br>
The mode "u" means the file is opened for reading and writing.<br>

NOFD:<br>
if /proc/[pid]/fd is not accessible. In this case, the values for TYPE and NODE fields can be left empty.

### TYPE:
The type of the opened file. The value shown in TYPE can be one of the following cases.

DIR:<br>
a directory. cwd and rtd are also classified as this type.

REG:<br>
a regular file

CHR:<br>
a character special file, for example<br>
crw-rw-rw- 1 root root 1, 3 Mar 17 17:31 /dev/null

FIFO:<br>
a pipe, for example<br>
A link to a pipe, e.g.,<br>
lr-x------ 1 root root 64 Mar 17 19:55 5 -> 'pipe:[138394]'<br>
A file with p type (FIFO)<br>
prw------- 1 root root 0 Mar 17 19:54 /run/systemd/inhibit/11.ref<br>

SOCK:<br>
a socket, for example<br>
lrwx------ 1 root root 64 Mar 17 19:55 1 -> 'socket:[136975]'<br>

unknown:<br>
Any other unlisted types. Alternatively, if a file has been deleted or is not accessible (e.g., permission denied), this column can show unknown.

### NODE:
The i-node number of the file
It can be blank or empty if and only if /proc/[pid]/fd is not accessible.

### NAME:
Show the opened filename if it is a typical file or directory.<br>
Show pipe:[node number] if it is a symbolic file to a pipe, e.g.,<br>
l-wx------ 1 ta ta 64 三 8 02:11 91 -> 'pipe:[2669735]'<br>
Show socket:[node number] if it is a symbolic file to a socket, e.g.,<br>
lrwx------ 1 ta ta 64 三 8 02:11 51 -> 'socket:[2669792]'<br>
Append  (Permission denied) if the access to /proc/pid/fd or /proc/pid/(cwd|root|exe) is failed due to permission denied.<br>
If the filename you read from /proc file system contains a  (deleted), please remove it from the filename before you print it out.<br>

## Additional Notes on REGEX

If you plan to test REGEX feature with the lsof package that comes with Linux distributions, you should run it with the option -c /REGEX/.

For students who programming with C++, consider working with regex_search() instead of regex_match(). Please work with regcomp() and regexec() for students who are programming with C to implement this feature.

# Grading
We will test your program in Ubuntu 20.04 with some simple test cases. You may not have to consider peaky test cases.
All continuous spaces and tabs in the output are considered as a single space. You may not have to worry about the differences between space characters in program outputs.<br>
	
For each test case, if more than 90% of your output lines are equivalent to our sample output, you will get full scores for that test case.
If your program crashes in a test case, you will only get part or no scores. It is case-by-case.
All the required program options and arguments can be passed to the program simultaneously. But we will not run it with ambiguous or redundant program options like ./hw1 -c A -c B.<br>
	
Because a running process in the system could be terminated at any time, your program may encounter a race condition that an earlier check is successful for /proc/<pid>,, but latter accesses to files in /proc/<pid> directory are failed. Your program should be able to handle cases like this properly.
We have provided a sample implementation with three test cases. Please access our online sandbox (tested only with Chrome, Firefox, and Edge; Safari is not supported). You can find everything in the directory ~/uphw1. Note that you can only access the service within campus networks or via a valid VPN network.

# Homework Submission
We will compile your homework by simply typing 'make' in your homework directory. You have to ensure your Makefile produces the executable hw1. Please ensure your Makefile works and the output executable name is correct before submitting your homework.<br>

Please pack your C/C++ code and Makefile into a zip archive. The directory structure should follow the below illustration. The id is your student id. Please note that you don't need to enclose your id with the braces.<br>

{id}_hw1.zip<br>
└── {id}_hw1/<br>
	├── Makefile<br>
	├── hw1.cpp<br>
	└── (any other c/c++ files if needed)<br>
You have to submit your homework via the E3 system. Scores will be graded based on the completeness of your implementation.<br>

# Remarks
Please implement your homework in C or C++.<br>
Using any non-standard libraries and any external binaries (e.g., via system()) are not allowed.<br>
No copycats. Please do not use codes from others (even open-source projects).<br>
We will test your program in Ubuntu 20.04 LTS Linux with the default gcc version (9.3.0).<br>

# More Outputs
Your program has to order the output lines by performing a numeric sort against process ID (PIDs). We will test your program with and without root permission. If an operation does not have sufficient permission to perform, you have to print out Permission denied message.<br>

## Run the command without root permission

$ ./hw1| head -n 20<br>
COMMAND		PID		USER		FD		TYPE		NODE		NAME<br>
systemd		1		root		cwd		unknown				/proc/1/cwd (Permission denied)<br>
systemd		1		root		rtd		unknown				/proc/1/root (Permission denied)<br>
systemd		1		root		txt		unknown				/proc/1/exe (Permission denied)<br>
systemd		1		root		NOFD						/proc/1/fd (Permission denied)<br>
kthreadd		2		root		cwd		unknown				/proc/2/cwd (Permission denied)<br>
kthreadd		2		root		rtd		unknown				/proc/2/root (Permission denied)<br>
kthreadd		2		root		txt		unknown				/proc/2/exe (Permission denied)<br>
kthreadd		2		root		NOFD						/proc/2/fd (Permission denied)<br>
kworker/0:0H		4		root		cwd		unknown				/proc/4/cwd (Permission denied)<br>
kworker/0:0H		4		root		rtd		unknown				/proc/4/root (Permission denied)<br>
kworker/0:0H		4		root		txt		unknown				/proc/4/exe (Permission denied)<br>
kworker/0:0H		4		root		NOFD						/proc/4/fd (Permission denied)<br>
mm_percpu_wq		6		root		cwd		unknown				/proc/6/cwd (Permission denied)<br>
mm_percpu_wq		6		root		rtd		unknown				/proc/6/root (Permission denied)<br>
mm_percpu_wq		6		root		txt		unknown				/proc/6/exe (Permission denied<br>
mm_percpu_wq		6		root		NOFD						/proc/6/fd (Permission denied)<br>
ksoftirqd/0		7		root		cwd		unknown				/proc/7/cwd (Permission denied)<br>
ksoftirqd/0		7		root		rtd		unknown				/proc/7/root (Permission denied)<br>
ksoftirqd/0		7		root		txt		unknown				/proc/7/exe (Permission denied)<br>

## Run the command with root permission

$ sudo ./hw1| head -n 20<br>
COMMAND		PID		USER		FD		TYPE		NODE		NAME<br>
systemd		1		root		cwd		DIR		2		/<br>
systemd		1		root		rtd		DIR		2		/<br>
systemd		1		root		txt		REG		23734219		/lib/systemd/systemd<br>
systemd		1		root		DEL		REG		23724387		/lib/x86_64-linux-gnu/libm-2.27.so<br>
systemd		1		root		mem		REG		23724289		/lib/x86_64-linux-gnu/libudev.so.1.6.9<br>
systemd		1		root		mem		REG		23724380		/lib/x86_64-linux-gnu/libgpg-error.so.0.22.0<br>
systemd		1		root		mem		REG		23724264		/lib/x86_64-linux-gnu/libjson-c.so.3.0.1<br>
systemd		1		root		mem		REG		24775614		/usr/lib/x86_64-linux-gnu/libargon2.so.0<br>
systemd		1		root		mem		REG		23724307		/lib/x86_64-linux-gnu/libdevmapper.so.1.02.1<br>
systemd		1		root		mem		REG		23725490		/lib/x86_64-linux-gnu/libattr.so.1.1.0<br>
systemd		1		root		mem		REG		23725195		/lib/x86_64-linux-gnu/libcap-ng.so.0.0.0<br>
systemd		1		root		mem		REG		23724192		/lib/x86_64-linux-gnu/libuuid.so.1.3.0<br>
systemd		1		root		DEL		REG		23724382		/lib/x86_64-linux-gnu/libdl-2.27.so<br>
systemd		1		root		mem		REG		23724639		/lib/x86_64-linux-gnu/libpcre.so.3.13.3<br>
systemd		1		root		DEL		REG		23724459		/lib/x86_64-linux-gnu/libpthread-2.27.so<br>
systemd		1		root		mem		REG		24772961		/usr/lib/x86_64-linux-gnu/liblz4.so.1.7.1<br>
systemd		1		root		mem		REG		23725142		/lib/x86_64-linux-gnu/liblzma.so.5.2.2<br>
systemd		1		root		mem		REG		23724150		/lib/x86_64-linux-gnu/libidn.so.11.6.16<br>
systemd		1		root		mem		REG		24772626		/usr/lib/x86_64-linux-gnu/libip4tc.so.0.1.0<br>
