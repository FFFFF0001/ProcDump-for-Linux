// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License

//--------------------------------------------------------------------
//
// This library reads from the /procfs pseudo filesystem
//
//--------------------------------------------------------------------

#include "Process.h"

bool GetProcessStat(pid_t pid, struct ProcessStat *proc) {
    char pidStatFilePath[32];
    char fileBuffer[1024];
    char *token, *savePtr = NULL;
    int tokenLength;

    FILE *pidStatFile = NULL;

    // Read /proc/[pid]/stat
    if(sprintf(pidStatFilePath, "/proc/%d/stat", pid) < 0){
        return false;
    }
    pidStatFile = fopen(pidStatFilePath, "r");
    
    if(pidStatFile != NULL){
        if(fgets(fileBuffer, sizeof(fileBuffer), pidStatFile) == NULL) {
            Log(error, "Failed to read from %s. Exiting...\n", pidStatFilePath);
            fclose(pidStatFile);
            return false;
        }
        
        // close file after reading this iteration of stats
        fclose(pidStatFile);
    }
    else{
        Log(error, "Failed to open %s.\n", pidStatFilePath);
        return false;
    }

    // Now we have /proc/[pid]/stat (which is one long line)
    if((token = strtok_r(fileBuffer, " ", &savePtr)) == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - PID.");
        return false; 
    }


    // (1) process ID
    proc->pid = (pid_t)atoi(token);

    // (2) comm (process name)
    if((token = strtok_r(NULL, " ", &savePtr)) == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - Process name.");        
        return false; 
    }
    if (proc->comm == NULL) {
        tokenLength = (int)strlen(token);
        proc->comm = (char *)(malloc(sizeof(char) * (tokenLength - 1)));
        if(proc->comm == NULL){
            Trace("GetProcessStat: failed to allocate memory.");
            return false;
        }
        strncpy(proc->comm, token+1, (size_t)tokenLength-2); // copy everything but leading and trailing '(' & ')""'
        proc->comm[tokenLength-2] = '\0'; // null terminate
    }

    // (3) process state
    proc->state = strtok_r(NULL, " ", &savePtr)[0];

    // (4) parent process ID
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - Parent PID.");        
        return false;
    }

    proc->ppid = (pid_t)strtol(token, NULL, 10);

    // (5) process group ID
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - Process group ID.");        
        return false;
    }
    
    proc->pgrp = (gid_t)strtol(token, NULL, 10);

    // (6) session ID
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - Session ID.");        
        return false;
    }

    proc->session = (int)strtol(token, NULL, 10);

    // (7) controlling terminal
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - Controlling terminal.");        
        return false;
    }
    
    proc->tty_nr = (int)strtol(token, NULL, 10);

    // (8) Foreground group ID of controlling terminal
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - Foreground group ID.");        
        return false;
    }
    
    proc->tpgid = (gid_t)strtol(token, NULL, 10);

    // (9) Kernel flags
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - Kernel flags.");        
        return false;
    }
    
    proc->flags = (unsigned int)strtoul(token, NULL, 10);

    // (10) minflt
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - Minflt.");        
        return false;
    }
    
    proc->minflt = strtoul(token, NULL, 10);

    // (11) cminflt
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - cminflt.");        
        return false;
    }
    
    proc->cminflt = strtoul(token, NULL, 10);

    // (12) majflt
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - majflt.");        
        return false;
    }
    
    proc->majflt = strtoul(token, NULL, 10);

    // (13) cmajflt
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - cmajflt.");        
        return false;
    }
    
    proc->cmajflt = strtoul(token, NULL, 10);

    // (14) utime
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - utime.");        
        return false;
    }
    
    proc->utime = strtoul(token, NULL, 10);

    // (15) stime
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - stime.");        
        return false;
    }
    
    proc->stime = strtoul(token, NULL, 10);

    // (16) cutime
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - cutime.");        
        return false;
    }
    
    proc->cutime = strtoul(token, NULL, 10);

    // (17) cstime
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - cstime.");        
        return false;
    }
    
    proc->cstime = strtoul(token, NULL, 10);

    // (18) priority
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - priority.");        
        return false;
    }
    
    proc->priority = strtol(token, NULL, 10);

    // (19) nice
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - nice.");        
        return false;
    }
    
    proc->nice = strtol(token, NULL, 10);

    // (20) num_threads
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - num_threads.");       
        return false;
    }
    
    proc->num_threads = strtol(token, NULL, 10);

    // (21) itrealvalue
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - itrealvalue.");       
        return false;
    }
    
    proc->itrealvalue = strtol(token, NULL, 10);

    // (22) starttime
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - starttime.");       
        return false;
    }
    
    proc->starttime = strtoull(token, NULL, 10);

    // (23) vsize
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - vsize.");       
        return false;
    }
    
    proc->vsize = strtoul(token, NULL, 10);

    // (24) rss
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - rss.");       
        return false;
    }
    
    proc->rss = strtol(token, NULL, 10);

    // (25) rsslim
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - rsslim.");       
        return false;
    }
    
    proc->rsslim = strtoul(token, NULL, 10);

    // (26) startcode
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - startcode.");       
        return false;
    }
    
    proc->startcode = strtoul(token, NULL, 10);

    // (27) endcode
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - endcode.");       
        return false;
    }
    
    proc->endcode = strtoul(token, NULL, 10);

    // (28) startstack
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - startstack.");       
        return false;
    }
    
    proc->startstack = strtoul(token, NULL, 10);

    // (29) kstkesp
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - kstkesp.");       
        return false;
    }
    
    proc->kstkesp = strtoul(token, NULL, 10);

    // (30) kstkeip
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - kstkeip.");       
        return false;
    }
    
    proc->kstkeip = strtoul(token, NULL, 10);

    // (31) signal
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - signal.");       
        return false;
    }
    
    proc->signal = strtoul(token, NULL, 10);

    // (32) blocked
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - blocked.");       
        return false;
    }
    
    proc->blocked = strtoul(token, NULL, 10);

    // (33) sigignore
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - sigignore.");       
        return false;
    }
    
    proc->sigignore = strtoul(token, NULL, 10);

    // (34) sigcatch
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - sigcatch.");       
        return false;
    }
    
    proc->sigcatch = strtoul(token, NULL, 10);

    // (35) wchan
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - wchan.");       
        return false;
    }
    
    proc->wchan = strtoul(token, NULL, 10);

    // (36) nswap
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - nswap.");       
        return false;
    }
    
    proc->nswap = strtoul(token, NULL, 10);

    // (37) cnswap
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - cnswap.");       
        return false;
    }
    
    proc->cnswap = strtoul(token, NULL, 10);

    // (38) exit_signal
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - exit_signal.");       
        return false;
    }
    
    proc->exit_signal = (int)strtol(token, NULL, 10);

    // (39) processor
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - processor.");       
        return false;
    }
    
    proc->processor = (int)strtol(token, NULL, 10);

    // (40) rt_priority
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - rt_priority.");       
        return false;
    }
    
    proc->rt_priority = (unsigned int)strtoul(token, NULL, 10);

    // (41) policy
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - policy.");       
        return false;
    }
    
    proc->policy = (unsigned int)strtoul(token, NULL, 10);

    // (42) delayacct_blkio_ticks
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - delayacct_blkio_ticks.");       
        return false;
    }
    
    proc->delayacct_blkio_ticks = strtoull(token, NULL, 10);

    // (43) guest_time
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - guest_time.");       
        return false;
    }
    
    proc->guest_time = strtoul(token, NULL, 10);

    // (44) cguest_time
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - cguest_time.");       
        return false;
    }
    
    proc->cguest_time = strtol(token, NULL, 10);

    // (45) start_data
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - start_data.");       
        return false;
    }
    
    proc->start_data = strtoul(token, NULL, 10);

    // (46) end_data
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - end_data.");       
        return false;
    }
    
    proc->end_data = strtoul(token, NULL, 10);

    // (47) start_brk
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - start_brk.");       
        return false;
    }
    
    proc->end_data = strtoul(token, NULL, 10);

    // (48) arg_start
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - arg_start.");
        return false;
    }
    
    proc->arg_start = strtoul(token, NULL, 10);

    // (49) arg_end
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - arg_end.");       
        return false;
    }
    
    proc->arg_end = strtoul(token, NULL, 10);

    // (50) env_start
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - env_start.");
        return false;
    }
    
    proc->env_start = strtoul(token, NULL, 10);

    // (52) env_end
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - env_end.");
        return false;
    }
    
    proc->env_end = strtoul(token, NULL, 10);

    // (53) exit_code
    token = strtok_r(NULL, " ", &savePtr);
    if(token == NULL){
        Trace("GetProcessStat: failed to get token from proc/[pid]/stat - exit_code.");
        return false;
    }
    
    proc->exit_code = (int)strtol(token, NULL, 10);

    return true;
}