/**
 * @file logging.h
 * @author SOPE Group 
 * @date 20 March 2020
 * @brief File containing logging' API headers and constants
 */

#ifndef LOGGING_H
#define LOGGING_H

typedef enum{CREATE, EXIT, RECV_SIGNAL, SEND_SIGNAL,RECV_PIPE, SEND_PIPE, ENTRY} action;

struct timeval start;

/**
 * @brief Appends @p info to log file at (path) LOG_FILENAME env variable
 * @param a the action performed to be logged onto the log file
 * @param info additional info of the action performed
 * @return OK if successful, ERROR if not
*/
int appendLogInfo(action a, char * info);

/**
 * @brief Logging on CREATE action
 * @param argc number of main args
 * @param args main args
 * @return OK if successful, ERROR if not
*/
int logCREATE(int argc, char* args[]);

/**
 * @brief Logging on EXIT action
 * @param termCode exit code 
 * @return OK if successful, ERROR if not
*/
int logEXIT(int termCode);

/**
 * @brief Logging on RECV SIGNAL action
 * @param SIG signal received
 * @return OK if successful, ERROR if not
*/
int logRECV_SIGNAL(int SIG);

/**
 * @brief Logging on SEND SIGNAL action
 * @param SIG signal sent
 * @return OK if successful, ERROR if not
*/
int logSEND_SIGNAL(int SIG, int pid);

/**
 * @brief Logging on RECV PIPE action
 * @param msg pipe message received
 * @return OK if successful, ERROR if not
*/
int logRECV_PIPE(char * msg);

/**
 * @brief Logging on SEND PIPE action
 * @param msg pipe message sent
 * @return OK if successful, ERROR if not
*/
int logSEND_PIPE(char * msg);

/**
 * @brief Logging on ENTRY action
 * @param bytes current file/path size
 * @param path current file/path
 * @return OK if successful, ERROR if not
*/
int logENTRY(int bytes, char * path);

#endif