#ifndef WAIT_H
#define WAIT_H		1


#define WNOHANG			0

#define WAIT_EXITED		0x001
#define	WAIT_SIGNALLED	0x010

#define WIFEXITED(x)	(x & WAIT_EXITED)
#define WIFSIGNALED(x)	(x & WAIT_SIGNALLED)

#define WEXITSTATUS(x) (HIBYTE(HIWORD(x)))
#define WTERMSIG(x)	(LOBYTE(LOWORD(x)))

int waitpid(pid_t pid, int *stat_loc, int options);

#endif