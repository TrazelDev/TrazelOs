#!/bin/bash
set -e

cd /mnt/bin_auxilarys/tarballs
curl -O https://busybox.net/downloads/busybox-1.17.2.tar.bz2

cd /mnt/cross_compiler/lib/gcc/x86_64-trazelos/10.2.0/
ln -sf crtbegin.o crtbeginT.o
ln -sf crtend.o crtendT.o


cd /mnt/bin_auxilarys

# 1. Start fresh
rm -rf busybox-1.17.2
tar -xf tarballs/busybox-1.17.2.tar.bz2
cd busybox-1.17.2

# 2. Configure for minimal static build
make CROSS_COMPILE=x86_64-trazelos- allnoconfig
sed -i 's/# CONFIG_STATIC is not set/CONFIG_STATIC=y/' .config

# 3. Create fake sysroot directory structure
mkdir -p fake_sysroot/sys fake_sysroot/arpa fake_sysroot/net fake_sysroot/netinet

# 4. Populate clean headers inside fake_sysroot/
cat << 'EOF' > fake_sysroot/byteswap.h
#include <stdint.h>
#define bswap_16(x) __builtin_bswap16(x)
#define bswap_32(x) __builtin_bswap32(x)
#define bswap_64(x) __builtin_bswap64(x)
EOF

cat << 'EOF' > fake_sysroot/endian.h
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN 4321
#define __BYTE_ORDER __LITTLE_ENDIAN
#define LITTLE_ENDIAN __LITTLE_ENDIAN
#define BIG_ENDIAN __BIG_ENDIAN
#define BYTE_ORDER __BYTE_ORDER
EOF

cat << 'EOF' > fake_sysroot/dirent.h
#ifndef _CUSTOM_DIRENT_H
#define _CUSTOM_DIRENT_H
typedef struct { int dummy; } DIR;
struct dirent { long d_ino; char d_name[256]; };
static inline DIR *opendir(const char *name) { return (DIR *)0; }
static inline struct dirent *readdir(DIR *dirp) { return (struct dirent *)0; }
static inline int closedir(DIR *dirp) { return 0; }
#endif
EOF

cat << 'EOF' > fake_sysroot/sys/stat.h
#ifndef _CUSTOM_SYS_STAT_H
#define _CUSTOM_SYS_STAT_H
#include <sys/types.h>
struct stat { long st_mode; long st_ino; long st_dev; long st_rdev; long st_size; long st_uid; long st_gid; long st_mtime; };
static inline int lstat(const char *path, struct stat *buf) { return -1; }
static inline int stat(const char *path, struct stat *buf) { return -1; }
static inline int mknod(const char *path, int mode, unsigned long dev) { return -1; }
#endif
EOF
cp fake_sysroot/sys/stat.h fake_sysroot/stat.h

cat << 'EOF' > fake_sysroot/mntent.h
struct mntent { char *mnt_fsname; char *mnt_dir; char *mnt_type; char *mnt_opts; int mnt_freq; int mnt_passno; };
EOF

cat << 'EOF' > fake_sysroot/termios.h
#ifndef _CUSTOM_TERMIOS_H
#define _CUSTOM_TERMIOS_H
typedef unsigned int speed_t;
struct termios {
    unsigned int c_iflag, c_oflag, c_cflag, c_lflag;
    unsigned char c_line, c_cc[32];
};
#define TCIFLUSH 0
#define TCSANOW 0
#define IUCLC 0
#define IXON 0
#define IXOFF 0
#define IXANY 0
#define ECHO 0
#define ECHOE 0
#define ECHOK 0
#define ECHONL 0
#define TOSTOP 0
#define B0 0
#define B9600 13
#define B38400 15
static inline int tcgetattr(int fd, struct termios *p) { return 0; }
static inline int tcsetattr(int fd, int act, const struct termios *p) { return 0; }
static inline int tcflush(int fd, int q) { return 0; }
#endif
EOF
cp fake_sysroot/termios.h fake_sysroot/sys/termios.h

cat << 'EOF' > fake_sysroot/sys/socket.h
#ifndef _CUSTOM_SOCKET_H
#define _CUSTOM_SOCKET_H
#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RDM 3
#define SOCK_SEQPACKET 4
#define SOCK_RAW 5
#define AF_UNSPEC 0
#define AF_INET 2
#define AF_INET6 10
#define AF_UNIX 1
#define INADDR_ANY 0
#define SOL_SOCKET 1
#define SO_REUSEADDR 2
#define SO_BROADCAST 6
typedef unsigned short sa_family_t;
typedef unsigned int socklen_t;
struct sockaddr { unsigned short sa_family; char sa_data[14]; };
struct in_addr { unsigned int s_addr; };
struct sockaddr_in { unsigned short sin_family; unsigned short sin_port; struct in_addr sin_addr; char sin_zero[8]; };
static inline int setsockopt(int sockfd, int level, int optname, const void *optval, unsigned int optlen) { return 0; }
static inline int getsockname(int sockfd, struct sockaddr *addr, unsigned int *addrlen) { return -1; }
static inline int getpeername(int sockfd, struct sockaddr *addr, unsigned int *addrlen) { return -1; }
static inline int connect(int sockfd, const struct sockaddr *addr, unsigned int addrlen) { return -1; }
#endif
EOF

cat << 'EOF' > fake_sysroot/sys/poll.h
#ifndef _CUSTOM_POLL_H
#define _CUSTOM_POLL_H
#define POLLIN  0x0001
#define POLLPRI 0x0002
#define POLLOUT 0x0004
#define POLLERR 0x0008
#define POLLHUP 0x0010
#define POLLNVAL 0x0020
struct pollfd { int fd; short events; short revents; };
typedef unsigned long nfds_t;
static inline int poll(struct pollfd *fds, nfds_t nfds, int timeout) { return 0; }
#endif
EOF
cp fake_sysroot/sys/poll.h fake_sysroot/poll.h

cat << 'EOF' > fake_sysroot/sys/mman.h
#ifndef _CUSTOM_MMAN_H
#define _CUSTOM_MMAN_H
#define PROT_READ  0x1
#define PROT_WRITE 0x2
#define PROT_EXEC  0x4
#define MAP_SHARED 0x01
#define MAP_PRIVATE 0x02
#define MAP_ANONYMOUS 0x20
#define MAP_FAILED ((void *)-1)
static inline void *mmap(void *addr, unsigned long length, int prot, int flags, int fd, long offset) { return MAP_FAILED; }
static inline int munmap(void *addr, unsigned long length) { return -1; }
#endif
EOF

cat << 'EOF' > fake_sysroot/sys/ioctl.h
#ifndef _CUSTOM_IOCTL_H
#define _CUSTOM_IOCTL_H
#define TIOCGWINSZ 0x5413
static inline int ioctl(int fd, unsigned long request, ...) { return -1; }
#endif
EOF
cp fake_sysroot/sys/ioctl.h fake_sysroot/ioctl.h

cat << 'EOF' > fake_sysroot/sys/statfs.h
#ifndef _CUSTOM_STATFS_H
#define _CUSTOM_STATFS_H
struct statfs { long f_type; long f_bsize; };
static inline int statfs(const char *path, struct statfs *buf) { return -1; }
#endif
EOF

cat << 'EOF' > fake_sysroot/arpa/inet.h
#ifndef _CUSTOM_ARPA_INET_H
#define _CUSTOM_ARPA_INET_H
#include <sys/socket.h>
static inline int inet_aton(const char *cp, struct in_addr *inp) { return 0; }
static inline char *inet_ntoa(struct in_addr in) { return (char*)"0.0.0.0"; }
#endif
EOF

cat << 'EOF' > fake_sysroot/netdb.h
#ifndef _CUSTOM_NETDB_H
#define _CUSTOM_NETDB_H
#include <sys/socket.h>
struct servent { char *s_name; char **s_aliases; int s_port; char *s_proto; };
struct hostent { char *h_name; char **h_aliases; int h_addrtype; int h_length; char **h_addr_list; };
static int dummy_h_errno = 0;
#define h_errno dummy_h_errno
static inline const char *hstrerror(int err) { return "Host error"; }
static inline struct hostent *gethostbyname(const char *name) { return (struct hostent *)0; }
#endif
EOF

# 5. Create libcrypt.a stub
echo 'char *crypt(const char *key, const char *salt) { return (char *)salt; }' > fake_sysroot/crypt_stub.c
x86_64-trazelos-gcc -c fake_sysroot/crypt_stub.c -o fake_sysroot/crypt_stub.o
x86_64-trazelos-ar rcs fake_sysroot/libcrypt.a fake_sysroot/crypt_stub.o

# 6. Patch utoa/itoa to avoid Newlib collisions
sed -i 's/char \*utoa(unsigned n)/char \*bb_utoa(unsigned n)/' include/libbb.h
sed -i 's/char \*itoa(int n)/char \*bb_itoa(int n)/' include/libbb.h
find libbb -name "*.c" -exec sed -i 's/utoa(/bb_utoa(/g' {} +
find libbb -name "*.c" -exec sed -i 's/itoa(/bb_itoa(/g' {} +

# 6.5
cat << 'EOF' > fake_sysroot/sys/stat.h
#ifndef _CUSTOM_SYS_STAT_H
#define _CUSTOM_SYS_STAT_H
#include <sys/types.h>

struct stat {
    long st_mode;
    long st_ino;
    long st_dev;
    long st_rdev;
    long st_size;
    long st_uid;
    long st_gid;
    long st_mtime;
};

#define S_ISUID  04000
#define S_ISGID  02000
#define S_ISVTX  01000
#define S_IRUSR  00400
#define S_IWUSR  00200
#define S_IXUSR  00100
#define S_IRGRP  00040
#define S_IWGRP  00020
#define S_IXGRP  00010
#define S_IROTH  00004
#define S_IWOTH  00002
#define S_IXOTH  00001

static inline int lstat(const char *path, struct stat *buf) { return -1; }
static inline int stat(const char *path, struct stat *buf) { return -1; }
static inline int mknod(const char *path, int mode, unsigned long dev) { return -1; }
#endif
EOF
cp fake_sysroot/sys/stat.h fake_sysroot/stat.h

#6.6
cat << 'EOF' > fake_sysroot/sys/stat.h
#ifndef _CUSTOM_SYS_STAT_H
#define _CUSTOM_SYS_STAT_H
#include <sys/types.h>

struct stat {
    long st_mode;
    long st_ino;
    long st_dev;
    long st_rdev;
    long st_size;
    long st_uid;
    long st_gid;
    long st_mtime;
};

/* File permission bits */
#define S_ISUID  04000
#define S_ISGID  02000
#define S_ISVTX  01000

#define S_IRWXU  00700
#define S_IRUSR  00400
#define S_IWUSR  00200
#define S_IXUSR  00100

#define S_IRWXG  00070
#define S_IRGRP  00040
#define S_IWGRP  00020
#define S_IXGRP  00010

#define S_IRWXO  00007
#define S_IROTH  00004
#define S_IWOTH  00002
#define S_IXOTH  00001

/* File type masks and macros */
#define S_IFMT   0170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000

#define S_ISREG(m)  (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)  (((m) & S_IFMT) == S_IFDIR)
#define S_ISLNK(m)  (((m) & S_IFMT) == S_IFLNK)
#define S_ISCHR(m)  (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)  (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

/* System call stubs */
static inline int lstat(const char *path, struct stat *buf) { return -1; }
static inline int stat(const char *path, struct stat *buf) { return -1; }
static inline int mknod(const char *path, int mode, unsigned long dev) { return -1; }
static inline int mkdir(const char *path, int mode) { return -1; }

#endif
EOF
cp fake_sysroot/sys/stat.h fake_sysroot/stat.h

# 6.8
cat << 'EOF' > fake_sysroot/features.h
#ifndef _CUSTOM_FEATURES_H
#define _CUSTOM_FEATURES_H
/* Dummy features.h for Newlib / custom toolchain */
#endif
EOF

# 6.9:
cat << 'EOF' > fake_sysroot/netdb.h
#ifndef _CUSTOM_NETDB_H
#define _CUSTOM_NETDB_H

#include <sys/socket.h>

/* Address info flags */
#define AI_PASSIVE     0x0001
#define AI_CANONNAME   0x0002
#define AI_NUMERICHOST 0x0004
#define AI_V4MAPPED    0x0008
#define AI_ALL         0x0010
#define AI_ADDRCONFIG  0x0020
#define AI_NUMERICSERV 0x0400

/* Name info flags */
#define NI_NAMEREQD    1
#define NI_DGRAM       2
#define NI_NOFQDN      4
#define NI_NUMERICHOST 8
#define NI_NUMERICSERV 16

struct servent {
    char *s_name;
    char **s_aliases;
    int s_port;
    char *s_proto;
};

struct hostent {
    char *h_name;
    char **h_aliases;
    int h_addrtype;
    int h_length;
    char **h_addr_list;
};

struct netent {
    char *n_name;
    char **n_aliases;
    int n_addrtype;
    unsigned int n_net;
};

struct addrinfo {
    int ai_flags;
    int ai_family;
    int ai_socktype;
    int ai_protocol;
    unsigned int ai_addrlen;
    struct sockaddr *ai_addr;
    char *ai_canonname;
    struct addrinfo *ai_next;
};

static int dummy_h_errno = 0;
#define h_errno dummy_h_errno

static inline const char *hstrerror(int err) { return "Host error"; }
static inline struct hostent *gethostbyname(const char *name) { return (struct hostent *)0; }
static inline struct hostent *gethostbyaddr(const void *addr, int len, int type) { return (struct hostent *)0; }
static inline struct netent *getnetbyaddr(unsigned int net, int type) { return (struct netent *)0; }
static inline struct servent *getservbyname(const char *name, const char *proto) { return (struct servent *)0; }
static inline int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res) { return -1; }
static inline void freeaddrinfo(struct addrinfo *res) {}
static inline int getnameinfo(const struct sockaddr *sa, unsigned int salen, char *host, unsigned int hostlen, char *serv, unsigned int servlen, int flags) { return -1; }

#endif
EOF

#6.9.1
cat << 'EOF' > fake_sysroot/sys/utsname.h
#ifndef _CUSTOM_SYS_UTSNAME_H
#define _CUSTOM_SYS_UTSNAME_H

struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
    char domainname[65];
};

static inline int uname(struct utsname *buf) {
    if (buf) {
        /* Provide a mock Linux-compatible version string so BusyBox parses it successfully */
        buf->sysname[0] = 'L'; buf->sysname[1] = 'i'; buf->sysname[2] = 'n'; buf->sysname[3] = 'u'; buf->sysname[4] = 'x'; buf->sysname[5] = '\0';
        buf->release[0] = '2'; buf->release[1] = '.'; buf->release[2] = '6'; buf->release[3] = '.'; buf->release[4] = '3'; buf->release[5] = '2'; buf->release[6] = '\0';
        buf->version[0] = '\0';
        buf->machine[0] = 'x'; buf->machine[1] = '8'; buf->machine[2] = '6'; buf->machine[3] = '_'; buf->machine[4] = '6'; buf->machine[5] = '4'; buf->machine[6] = '\0';
    }
    return 0;
}

#endif
EOF

cp fake_sysroot/sys/utsname.h fake_sysroot/utsname.h

#6.9.2:
cat << 'EOF' > fake_sysroot/sys/sysmacros.h
#ifndef _CUSTOM_SYS_SYSMACROS_H
#define _CUSTOM_SYS_SYSMACROS_H

#define major(dev) ((unsigned int) (((dev) >> 8) & 0xfff))
#define minor(dev) ((unsigned int) ((dev) & 0xff))
#define makedev(ma, mi) (((ma) << 8) | (mi))

#endif
EOF

cp fake_sysroot/sys/sysmacros.h fake_sysroot/sysmacros.h

#6.9.3:
cat << 'EOF' > fake_sysroot/sys/utmp.h
#ifndef _CUSTOM_SYS_UTMP_H
#define _CUSTOM_SYS_UTMP_H

#define EMPTY 0
#define RUN_LVL 1
#define BOOT_TIME 2
#define NEW_TIME 3
#define OLD_TIME 4
#define INIT_PROCESS 5
#define LOGIN_PROCESS 6
#define USER_PROCESS 7
#define DEAD_PROCESS 8

struct utmp {
    short ut_type;
    int ut_pid;
    char ut_line[32];
    char ut_id[4];
    char ut_user[32];
    char ut_host[256];
    struct {
        long tv_sec;
        long tv_usec;
    } ut_tv;
};

#define _PATH_UTMP "/var/run/utmp"
#define _PATH_WTMP "/var/log/wtmp"
#define UTMP_FILE "/var/run/utmp"
#define WTMP_FILE "/var/log/wtmp"

#endif
EOF
cp fake_sysroot/sys/utmp.h fake_sysroot/utmp.h


#6.9.4:
cat << 'EOF' > fake_sysroot/signal.h
#ifndef _CUSTOM_SIGNAL_H
#define _CUSTOM_SIGNAL_H

#include <sys/types.h>

/* Define Newlib's expected internal function pointer type first */
#ifndef _SIG_FUNC_PTR
#define _SIG_FUNC_PTR
typedef void (*_sig_func_ptr)(int);
#endif

/* Pull in Newlib's base signal header safely */
#include_next <signal.h>

/* Ensure standard signal numbers */
#ifndef SIGHUP
#define SIGHUP    1
#endif
#ifndef SIGINT
#define SIGINT    2
#endif
#ifndef SIGQUIT
#define SIGQUIT   3
#endif
#ifndef SIGILL
#define SIGILL    4
#endif
#ifndef SIGTRAP
#define SIGTRAP   5
#endif
#ifndef SIGABRT
#define SIGABRT   6
#endif
#ifndef SIGBUS
#define SIGBUS    7
#endif
#ifndef SIGFPE
#define SIGFPE    8
#endif
#ifndef SIGKILL
#define SIGKILL   9
#endif
#ifndef SIGUSR1
#define SIGUSR1   10
#endif
#ifndef SIGSEGV
#define SIGSEGV   11
#endif
#ifndef SIGUSR2
#define SIGUSR2   12
#endif
#ifndef SIGPIPE
#define SIGPIPE   13
#endif
#ifndef SIGALRM
#define SIGALRM   14
#endif
#ifndef SIGTERM
#define SIGTERM   15
#endif
#ifndef SIGCHLD
#define SIGCHLD   17
#endif
#ifndef SIGCONT
#define SIGCONT   18
#endif
#ifndef SIGSTOP
#define SIGSTOP   19
#endif
#ifndef SIGTSTP
#define SIGTSTP   20
#endif
#ifndef SIGXCPU
#define SIGXCPU   24
#endif
#ifndef SIGXFSZ
#define SIGXFSZ   25
#endif
#ifndef SIGVTALRM
#define SIGVTALRM 26
#endif
#ifndef SIGPROF
#define SIGPROF   27
#endif
#ifndef SIGWINCH
#define SIGWINCH  28
#endif

/* POSIX Signal Types & Constants */
#ifndef _SIGSET_T_DECLARED
#define _SIGSET_T_DECLARED
typedef unsigned long sigset_t;
#endif

#ifndef _SIGACTION_DECLARED
#define _SIGACTION_DECLARED
struct sigaction {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, void *, void *);
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
};
#endif

#ifndef SA_RESTART
#define SA_NOCLDSTOP 1
#define SA_NOCLDWAIT 2
#define SA_SIGINFO   4
#define SA_RESTART   0x10000000
#define SA_NODEFER   0x40000000
#define SA_RESETHAND 0x80000000
#endif

#ifndef SIG_BLOCK
#define SIG_BLOCK   0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2
#endif

/* Inline stubs for missing POSIX signal functions */
static inline int sigemptyset(sigset_t *set) { if (set) *set = 0; return 0; }
static inline int sigfillset(sigset_t *set) { if (set) *set = ~0UL; return 0; }
static inline int sigaddset(sigset_t *set, int signum) { if (set) *set |= (1UL << (signum % 32)); return 0; }
static inline int sigdelset(sigset_t *set, int signum) { if (set) *set &= ~(1UL << (signum % 32)); return 0; }
static inline int sigismember(const sigset_t *set, int signum) { return set ? (*set & (1UL << (signum % 32))) != 0 : 0; }
static inline int sigprocmask(int how, const sigset_t *set, sigset_t *oldset) { return 0; }
static inline int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) { return 0; }

#endif
EOF
cp fake_sysroot/signal.h fake_sysroot/sys/signal.h


#6.9.5:
cat << 'EOF' > fake_sysroot/termios.h
#ifndef _CUSTOM_TERMIOS_H
#define _CUSTOM_TERMIOS_H

typedef unsigned int speed_t;

struct termios {
    unsigned int c_iflag, c_oflag, c_cflag, c_lflag;
    unsigned char c_line, c_cc[32];
};

/* Terminal flags and settings */
#define TCIFLUSH 0
#define TCSANOW  0
#define IUCLC    0
#define IXON     0
#define IXOFF    0
#define IXANY    0
#define ECHO     0
#define ECHOE    0
#define ECHOK    0
#define ECHONL   0
#define TOSTOP   0

/* Standard Baud Rates */
#define B0       0
#define B50      1
#define B75      2
#define B110     3
#define B134     4
#define B150     5
#define B200     6
#define B300     7
#define B600     8
#define B1200    9
#define B1800    10
#define B2400    11
#define B4800    12
#define B9600    13
#define B19200   14
#define B38400   15
#define B57600   16
#define B115200  17
#define B230400  18
#define B460800  19
#define B500000  20
#define B576000  21
#define B921600  22
#define B1000000 23
#define B1152000 24
#define B1500000 25
#define B2000000 26
#define B2500000 27
#define B3000000 28
#define B3500000 29
#define B4000000 30

/* Inline stubs for terminal control functions */
static inline int tcgetattr(int fd, struct termios *p) { return 0; }
static inline int tcsetattr(int fd, int act, const struct termios *p) { return 0; }
static inline int tcflush(int fd, int q) { return 0; }

#endif
EOF
cp fake_sysroot/termios.h fake_sysroot/sys/termios.h



#6.9.6:
mkdir -p fake_sysroot/netinet

cat << 'EOF' > fake_sysroot/netinet/in.h
#ifndef _CUSTOM_NETINET_IN_H
#define _CUSTOM_NETINET_IN_H

#include <sys/socket.h>

/* Standard IP Protocols */
#define IPPROTO_IP   0
#define IPPROTO_ICMP 1
#define IPPROTO_TCP  6
#define IPPROTO_UDP  17
#define IPPROTO_IPV6 41
#define IPPROTO_RAW  255

/* Internet addresses */
#define INADDR_ANY       ((unsigned int)0x00000000)
#define INADDR_BROADCAST ((unsigned int)0xffffffff)
#define INADDR_NONE      ((unsigned int)0xffffffff)

/* Byte swapping inline helpers */
static inline unsigned short htons(unsigned short x) { return x; }
static inline unsigned short ntohs(unsigned short x) { return x; }
static inline unsigned int   htonl(unsigned int x)   { return x; }
static inline unsigned int   ntohl(unsigned int x)   { return x; }

#endif
EOF

#6.9.7:
mkdir -p fake_sysroot/net

cat << 'EOF' > fake_sysroot/net/if.h
#ifndef _CUSTOM_NET_IF_H
#define _CUSTOM_NET_IF_H

#include <sys/socket.h>

#define IFNAMSIZ 16

struct ifreq {
    char ifr_name[IFNAMSIZ]; /* Interface name, e.g., "eth0" */
    union {
        struct sockaddr ifr_addr;
        short ifr_flags;
        int ifr_ifindex;
        int ifr_metric;
        int ifr_mtu;
        char ifr_data[24];
    } ifr_ifru;
};

/* Standard access macros */
#define ifr_addr    ifr_ifru.ifr_addr
#define ifr_flags   ifr_ifru.ifr_flags
#define ifr_metric  ifr_ifru.ifr_metric
#define ifr_mtu     ifr_ifru.ifr_mtu
#define ifr_ifindex ifr_ifru.ifr_ifindex
#define ifr_data    ifr_ifru.ifr_data

#endif
EOF


#6.9.8:
cat << 'EOF' > fake_sysroot/sys/un.h
#ifndef _CUSTOM_SYS_UN_H
#define _CUSTOM_SYS_UN_H

#include <sys/socket.h>

struct sockaddr_un {
    unsigned short sun_family; /* AF_UNIX */
    char sun_path[108];        /* Pathname */
};

#endif
EOF

#6.9.9.0
cat << 'EOF' > fake_sysroot/sys/ioctl.h
#ifndef _CUSTOM_SYS_IOCTL_H
#define _CUSTOM_SYS_IOCTL_H

/* Common terminal ioctl request codes */
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
#define TCGETS     0x5401
#define TCSETS     0x5402

struct winsize {
    unsigned short ws_row;
    unsigned short ws_col;
    unsigned short ws_xpixel;
    unsigned short ws_ypixel;
};

/* Safe stub for ioctl */
static inline int ioctl(int fd, unsigned long request, ...) {
    return -1;
}

#endif
EOF
cp fake_sysroot/sys/ioctl.h fake_sysroot/ioctl.h

#6.9.9.1:
cat << 'EOF' > fake_sysroot/sys/param.h
#ifndef _CUSTOM_SYS_PARAM_H
#define _CUSTOM_SYS_PARAM_H

#ifndef MAXSYMLINKS
#define MAXSYMLINKS 20
#endif

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

#endif
EOF
cp fake_sysroot/sys/param.h fake_sysroot/param.h



#6.9.9.2:




# 7. Create os_stubs.c using weak symbols
cat << 'EOF' > fake_sysroot/os_stubs.c
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>

#define WEAK __attribute__((weak))

WEAK uid_t getuid(void) { return 0; }
WEAK uid_t geteuid(void) { return 0; }
WEAK gid_t getgid(void) { return 0; }
WEAK gid_t getegid(void) { return 0; }
WEAK int setgid(gid_t gid) { errno = ENOSYS; return -1; }
WEAK int setuid(uid_t uid) { errno = ENOSYS; return -1; }
WEAK int dup2(int oldfd, int newfd) { errno = ENOSYS; return -1; }
WEAK unsigned int sleep(unsigned int seconds) { return 0; }
WEAK int fcntl(int fd, int cmd, ...) { errno = ENOSYS; return -1; }
WEAK pid_t waitpid(pid_t pid, int *status, int options) { errno = ENOSYS; return -1; }
WEAK int pipe(int pipefd[2]) { errno = ENOSYS; return -1; }
WEAK int chdir(const char *path) { errno = ENOSYS; return -1; }
WEAK int chroot(const char *path) { errno = ENOSYS; return -1; }
WEAK int ttyname_r(int fd, char *buf, size_t buflen) { errno = ENOSYS; return -1; }
WEAK int socket(int domain, int type, int protocol) { errno = ENOSYS; return -1; }
WEAK int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { errno = ENOSYS; return -1; }
WEAK int listen(int sockfd, int backlog) { errno = ENOSYS; return -1; }
WEAK ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) { errno = ENOSYS; return -1; }
EOF

x86_64-trazelos-gcc -c fake_sysroot/os_stubs.c -I$(pwd)/fake_sysroot -o fake_sysroot/os_stubs.o

# 8. Final Build
make CROSS_COMPILE=x86_64-trazelos- \
     EXTRA_CFLAGS="-I$(pwd)/fake_sysroot" \
     EXTRA_LDFLAGS="$(pwd)/fake_sysroot/os_stubs.o -L$(pwd)/fake_sysroot -lcrypt -lm"

echo "SUCCESS: busybox_unstripped generated!"
