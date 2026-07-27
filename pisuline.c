/* Author: Lauren Toivanen */

#include <stdio.h>
#include <stdbool.h>
#include <pwd.h>
#include <sys/select.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <git2.h>
#include <git2/branch.h>
#include <git2/errors.h>
#include <git2/global.h>
#include <git2/refs.h>
#include <git2/repository.h>
#include <git2/types.h>

void strToDash(char *str);
int handlePS0(void);
void timeFailPrint(void);
void getGitStatus(void);

char timeFilePath[2014];
time_t timeNow;
struct timespec nowClock;
	
char colorReset[]    = "\e[0m";
char colorUsr[9];
char colorCwd[]      = "\e[01;34m";
char colorNow[] = "\e[0;90m";
char colorGray[] = "\e[0;00;90m";
char *colorTimeNull = colorGray;
char colorTimeSucc[] = "\e[00;30;42m";
char colorTimeFail[] = "\e[00;00;41m";
char colorGitOk[] = "\e[00;00;32m";
char colorGitW[] = "\e[00;00;33m";
char colorGitWW[] = "\e[00;00;31m";
char *colorGit = colorGitW;
char *colorTime = colorGray;
char gitStatus[66];
char cwd[1024];

// argv[1]: 0:ps0 1:ps1
// argv[2]: resultcode (bash $?)
int main(int argc, char **argv) {

	char username[257];
	struct passwd *pwd = getpwuid(getuid());
	if (pwd) {
		strcpy(username, pwd->pw_name);
	} else {
		strToDash(username);
	}
	
	timeNow = time(NULL);
	snprintf(timeFilePath, sizeof(timeFilePath), "/dev/shm/%s.pisutime.%d", username, getppid());
	if (clock_gettime(CLOCK_MONOTONIC, &nowClock) == -1) {
		return -1;
	}
	
	if (argc > 1 && strcmp(argv[1], "0") == 0) {
		return handlePS0();
	}

	char hostname[HOST_NAME_MAX + 1];
	if (gethostname(hostname, sizeof(hostname)) != 0) {
		strToDash (hostname);
	}

	if (getuid() == 0) {
		strcpy(colorUsr, "\e[00;41m");
	} else {
		unsigned char userhostColorNum = 0;
		for (unsigned int i = 0; i< sizeof(username); i++) {
			if (username[i] == 0)
				break;
			else
				userhostColorNum = (userhostColorNum+username[i])%6;
		}
		for (unsigned int i = 0; i< sizeof(hostname); i++) {
			if (hostname[i] == 0)
				break;
			else
				userhostColorNum = (userhostColorNum+hostname[i])%6;
		}
		userhostColorNum++;
		sprintf(colorUsr, "\e[01;3%um", userhostColorNum);
	}

	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		strToDash(cwd);
	}

	if (argc > 2) {
		if (strcmp(argv[2], "0") == 0) {
			colorTime=colorTimeSucc;
		} else {	
			colorTime=colorTimeFail;
		}
	}		

	FILE *f;
	unsigned long secs;
	long nsecs;
	bool timeFail = false;
	unsigned int cuteMsecs = 0;
	unsigned int cuteSecs = 0;
	unsigned int cuteMins = 0;
	unsigned long cuteHours = 0;
	f = fopen(timeFilePath, "r");
	if (f) {
		if (fscanf(f, "%lu.%li", &secs, &nsecs) < 2) {
			timeFail = true;
		}
		fclose(f);
	} else {
		timeFail = true;
	}
	unlink(timeFilePath);
	if (timeFail) {
		colorTime = colorTimeNull;
		timeFailPrint();
	} else {
		secs = nowClock.tv_sec - secs;
		if (nowClock.tv_nsec >= nsecs) {
			nsecs = nowClock.tv_nsec - nsecs;
		} else {
			nsecs = nsecs - nowClock.tv_nsec;
			secs -= 1;
		}
		
		cuteMsecs = (nsecs+500)/1000000;
		cuteSecs = secs % 60;
		cuteMins = secs / 60 % 60;
		cuteHours = secs / 60 / 60;

		printf("%s%02luh %02um %02u.%03us%s ", colorTime, cuteHours, cuteMins, cuteSecs, cuteMsecs, colorReset);
	}

	getGitStatus();
	
	printf("%s%s@%s%s: %s%s%s%s$", colorUsr, username, hostname, colorReset, colorCwd, cwd, gitStatus, colorReset);
	return 0;
}

int handlePS0(void) {
	printf("%s@%s%s", colorNow, ctime(&timeNow), colorReset);

	FILE *f;
	f = fopen(timeFilePath, "w");
	if (f == NULL) {
		// TODO: fail handling
		return -1;
	}
	if (fprintf(f, "%lld.%ld", nowClock.tv_sec, nowClock.tv_nsec) < 0) {
		// TODO: fail handling;
		fclose(f);
		return -1;
	}
	fclose(f);
	return 0;
}

void strToDash(char *str) {
	str[0] = '-';
	str[1] = 0;
	return;
}

void timeFailPrint() {
	printf("%s--h --m --.---s%s ", colorTime, colorReset);
}

void getGitStatus(void)
{
	char strbuf[64];
	git_libgit2_init();
	git_repository *repo;
	if (git_repository_open_ext(&repo, cwd, 0, NULL) < 0) {
		return;
	}

	git_reference *ref;
	switch (git_repository_head(&ref, repo)) {
	case GIT_EUNBORNBRANCH:
		strcpy(strbuf, "<unborn>");
		break;
	case GIT_ENOTFOUND:
		strcpy(strbuf, "<missing head>");
		break;
	case 0:
		strncpy(strbuf, git_reference_shorthand(ref), sizeof(strbuf));
		break;
	default:
		strcpy(strbuf, "<HEAD REF ERR>");
	}
	
	snprintf(gitStatus, sizeof(gitStatus), " %s[%s]", colorGit, strbuf);
	
	git_reference_free(ref);
	return;
}
