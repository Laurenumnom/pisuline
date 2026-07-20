/* Author: Lauren Toivanen */

#include <stdio.h>
#include <stdbool.h>
#include <pwd.h>
#include <sys/select.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <time.h>

void strToDash(char *str);
int handlePS0(void);
void timeFailPrint(void);

char timeFilePath[2014];
time_t timeNow;
struct timespec nowClock;
	
char colorReset[]    = "\e[0m";
char colorUsr[]      = "\e[01;31m";
char colorCwd[]      = "\e[01;34m";
char colorNow[] = "\e[0;90m";
char colorGray[] = "\e[0;00;90m";
char colorTimeNull[] = "\e[00;00;40m";
char colorTimeSucc[] = "\e[00;30;42m";
char colorTimeFail[] = "\e[00;00;41m";
char *colorTime = colorTimeNull;
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
	
	printf("%s%s@%s: %s%s%s$", colorUsr, username, hostname, colorCwd, cwd, colorReset);
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
