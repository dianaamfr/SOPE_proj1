#pragma once

typedef enum{CREATE, EXIT, RECV_SIGNAL, SEND_SIGNAL,RECV_PIPE, SEND_PIPE, ENTRY} action;

struct timeval start;

int appendLogInfo(action a, char * info);

int logCREATE(int argc, char* args[]);

int logEXIT(int termCode);

int logRECV_SIGNAL(int SIG);

int logSEND_SIGNAL(int SIG, int pid);

int logRECV_PIPE(char * msg);

int logSEND_PIPE(char * msg);

int logENTRY(int bytes, char * path);