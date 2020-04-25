#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <dirent.h>
#include <libgen.h>
#include <pwd.h>
#include <unistd.h>
#include <time.h> 
#include <sys/stat.h>

int for_threads(char* path)
{
    struct stat task_stat;

    if (stat(path, &task_stat))
        return -1;

    return task_stat.st_nlink - 2;
}
 
const char *getUserName(int userID)
{
  struct passwd *pword = getpwuid(userID);
  if (pword)
  {
    return pword->pw_name;
  }

  return "";
}
int is_number (char *str)
{
  int idx;
  for (idx=0; str[idx] != '\0'; idx++)
  {
    if (!isdigit (str[idx]))
    {
      return 0;
    }
  }
  return 1;
}

void myfunc_pidaux ()
{
  DIR *DIRPATH;
  FILE *fp;


  const char *user;

  char *command;
  size_t len=0;


  struct dirent *entry;

  char path[100], read_buf[1024],temp_buf[100];
  char uid_int_str[100]={0},*line;
  char uptime_str[100];



  DIRPATH = opendir ("/proc/");
  strcpy(path,"/proc/");
  strcat(path,"uptime");
 
  fp=fopen(path,"r");
  if(fp!=NULL)
  {
	getline(&line,&len,fp);
	sscanf(line,"%s ",uptime_str);
  }
  long uptime=atof(uptime_str);
  strcpy(path,"/proc/");
  strcat(path,"meminfo");

  fp=fopen(path,"r");
  unsigned long long ull_total_memory;
  unsigned long long ull_free_memory;
  if(fp!=NULL)
  {
	getline(&line,&len,fp);
	sscanf(line,"MemTotal:        %llu kB",&ull_total_memory);
  }

  while ((entry = readdir (DIRPATH)) != NULL)
  {
    if (is_number (entry->d_name))
    {
	
	unsigned long long memory_rss =0;
	unsigned long long vmsize =0;
	unsigned long long vmsize2 = 0;
	unsigned long long memory_rss2=0;

	strcpy(path,"/proc/");
	strcat(path,entry->d_name);
	strcat(path,"/statm");

	fp = fopen(path,"r");

	getline(&line,&len,fp);

	sscanf(line, "%llu %llu", &vmsize, &memory_rss);
	strcpy(path,"/proc/");
	strcat(path,entry->d_name);
	strcat(path,"/status");
	fp=fopen(path,"r");
	unsigned long long un_ll_vmlock;

	if(fp!=NULL)
	{
		un_ll_vmlock =0;
		while(getline(&line,&len,fp)!=-1)
		{
			sscanf(line,"Uid:    %s ",uid_int_str);
			sscanf(line,"VmLck:     %llu kB", &un_ll_vmlock);
			if(vmsize!=0)
			{
				sscanf(line,"VmSize:    %llu kB",&vmsize2);
			}
			if(memory_rss !=0)
			{
				sscanf(line,"VmRSS:     %llu kB",&memory_rss2);
			}
		}

	}
	else
	{
		fprintf(stdout,"FP is NULL\n");
	}
	float memory_usage=100.0*memory_rss2/ull_total_memory;
	strcpy(path,"/proc/");
	strcat(path,entry->d_name);
	strcat(path,"/stat");
	fp=fopen(path,"r");
	getline(&line,&len,fp);
	char comm[100];
	char state[10] = "";
	unsigned int flags;
	int pid,ppid,pgrp,session,tty_nr,tpgid;
	unsigned long ul_minflt,cul_minflt,ul_majflt,cul_majflt,utime,stime;
	unsigned long long starttime;
	long cutime,cstime,priority,nice,num_threads,itreavalue;


	sscanf(line,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld  %ld %llu",&pid,comm,&state,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,&ul_minflt,&cul_minflt,&ul_majflt,&cul_majflt,&utime,&stime,&cutime,&cstime,&priority,&nice,&num_threads,&itreavalue,&starttime);
	
	if(nice==19)
		strcat(state, "N");
	if(nice==-20)
		strcat(state, "<");
	if(un_ll_vmlock!=0)
		strcat(state, "L");
	if(session ==pid)
		strcat(state,"s");
	strcpy(path,"/proc/");
	strcat(path,entry->d_name);
	strcat(path,"/task");
	int threads = for_threads(path);
	if(threads>1)
		strcat(state,"l");
	if(pgrp==tpgid)
		strcat(state,"+");
	
	unsigned long ul_total_time = utime+stime;

	float seconds=uptime-(starttime/sysconf(_SC_CLK_TCK));
	float cpu_usage=100*((ul_total_time/sysconf(_SC_CLK_TCK))/seconds);

	if(isnan(cpu_usage))
	{
		cpu_usage=0.0;
	}
	if(isnan(memory_usage))
	{
		memory_usage=0.0;
	}

	struct timeval current_time;
	int process_time;
	process_time = (utime+stime)/sysconf(_SC_CLK_TCK);
	int m = (process_time%3600)/60;
	int s = process_time%60;

	strcpy (path, "/proc/");
	strcat (path, entry->d_name);
	strcat (path, "/cmdline");



    fp = fopen (path, "r");


	read_buf[0] = 0;

      	if (fp != NULL)
      	{
        //	fscanf (fp, "%s", read_buf);
		fgets(read_buf,sizeof(read_buf),fp);
		fclose(fp);
      	}
	if(read_buf[0]==0)
	{
		char middle[1024];
		strcpy (path, "/proc/");
       		strcat (path, entry->d_name);
        	strcat (path, "/comm");
		fp = fopen(path,"r");


		fscanf (fp, "%s", middle);
		read_buf[0] = '\0';

		strcat(read_buf, "[");
		strcat(read_buf, middle);
		strcat(read_buf, "]");
	}
      	const char *userName=getUserName(atoi(uid_int_str));
     	if(strlen(userName)<9)
      	{
		user=userName;	
      	}
      	else
      	{
		user=uid_int_str;
      	}

	time_t t = time(NULL);
        struct tm tm = *localtime(&t);
	float days = (seconds/60)/60;
	char *question = "?";
	if((float)tm.tm_hour-days<0)
	{

		int day = tm.tm_mday;
		int imonth = tm.tm_mon;
		days = (days/24);

		while(days>0)
		{
			if(day==1)
			{
				if(imonth ==4||imonth ==6||imonth ==9||imonth ==11)
					day =30;
				else if(imonth ==2)
					day =28;
				else
					day = 31;
				if(imonth==0)
					imonth = 11;
				else
					imonth--;
			}
			else
			{
				day--;
			}
			days--;
		}
		char *month;
		if(imonth==0)
			month = "Jan";
		else if(imonth==1)
			month = "Feb";
                else if(imonth==2)
                        month = "Mar";
                else if(imonth==3)
                        month = "Apr";
                else if(imonth==4)
                        month = "May";
                else if(imonth==5)
                        month = "Jun";
                else if(imonth==6)
                        month = "Jul";
                else if(imonth==7)
                        month = "Aug";
                else if(imonth==8)
                        month = "Sep";
                else if(imonth==9)
                        month = "Oct";
                else if(imonth==10)
                        month = "Nov";
                else if(imonth==11)
                        month = "Dec";
		fprintf(stdout,"%-7s %6s %5.1f %2.1f %9llu %6llu %-7s %-4s %d%s %6d:%02d %s\n",user,entry->d_name,cpu_usage,memory_usage,vmsize2,memory_rss2,question,state,day,month,m,s,read_buf);

	}
	else
	{
		time_t t = time(NULL)-seconds;
		tm = *localtime(&t);
	fprintf(stdout,"%-7s %6s %5.1f %2.1f %9llu %6llu %-7s %4s %02d:%02d %6d:%02d %s\n",user,entry->d_name,cpu_usage,memory_usage,vmsize2,memory_rss2,question,state,tm.tm_hour,tm.tm_min,m,s,read_buf);
	}
    }
  } 
 
  closedir (DIRPATH);
}
 
int main (int argc, char *argv[])
{
	printf("USER       PID %%CPU %%MEM       VSZ    RSS TTY     STAT START      TIME COMMAND\n");
	myfunc_pidaux();

	return 0;
}