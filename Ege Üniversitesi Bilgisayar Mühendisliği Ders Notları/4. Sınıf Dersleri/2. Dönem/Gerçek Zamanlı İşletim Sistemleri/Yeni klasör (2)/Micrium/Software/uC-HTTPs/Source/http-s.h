/*
*********************************************************************************************************
*                                              uC/HTTPs
*                                 Hypertext Transfer Protocol (server)
*
*                          (c) Copyright 2004-2011; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/HTTPs is provided in source form to registered licensees ONLY.  It is
*               illegal to distribute this source code to any third party unless you receive
*               written permission by an authorized Micrium representative.  Knowledge of
*               the source code may NOT be used to develop a similar product.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                             HTTP SERVER
*
* Filename      : http-s.h
* Version       : V1.97.00
* Programmer(s) : JDH
*                 SR
*                 SL
*********************************************************************************************************
*
*  This server is designed to work well with an embedded system that does not have PERL and the other
*  "niceties" that are on a larger UNIX or NT server environment.  GET and POST requests are supported.
*
*  For GET, if the string is "GET / ", then index.htm is the response, if the string is "GET /FileName",
*  then that file is used as the response.  If the file suffix is .htm or .html, then the server will
*  parse the file prior to sending it to the client.  If the syntax ${TEXT_STRING} is found within the
*  file, then the string TEXT_STRING is passed to an application-specific function and the all characters
*  from the opening '$' to the closing '}' are omitted and replaced with whatever the application-
*  specific code wants to replace it with.
*
*  For POST, (generated by the HTML "form" submission), the input is assumed to be formatted as
*  "POST /action" where 'action' defines what the server is to do with the next incoming TCP stream. This
*  next stream is the data entered through the user interface presented by the HTML file that contains
*  the form mechanism (the cgi name/value pairs).  It can simply be an HTML file that is processed in the
*  same way as was discussed above for the GET command.
*
*  Now a simple example of the above discussion.
*  Assume we have a file called myip.htm on the target that looks like this:
*
*  <html><body><center>
*  This system's IP address is ${My_IP_Address}
*  </center></body></html>
*
*  When a client attaches to this server and requests (via GET) myip.htm, the server will parse the file,
*  find the ${My_IP_Address} syntax and pass the string "My_IP_Address" into an application specific
*  function called HTTPs_ValRx().  That function will then build a replacement text string that the
*  server will give to the client in place of the ${My_IP_Address} text.  The file seen by the client
*  would look something like this:
*
*  <html><body><center>
*  This system's IP address is 135.17.115.215
*  </center></body></html>
*
*  Similarly, if the action of a form is a .htm file, the server will first parse the incoming name/value
*  list and pass each name value pair into an app-specific function called HTTPs_ValRx() to allow the
*  application to properly parse the data entered by the user of the client.  Then the HTML file
*  specified as the action will be processed as discussed above and sent to the client.
*
*  Note that this server does not define any syntax within the ${}.  This is 100% application-specific
*  and can be used for simple variable name conversion or something more elaborate if necessary.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                                 MODULE
*
* Note(s) : (1) This header file is protected from multiple pre-processor inclusion through use of the
*               HTTPs present pre-processor macro definition.
*********************************************************************************************************
*/

#ifndef  HTTPs_PRESENT                                          /* See Note #1.                                         */
#define  HTTPs_PRESENT


/*
*********************************************************************************************************
*                                        HTTPs VERSION NUMBER
*
* Note(s) : (1) (a) The HTTPs module software version is denoted as follows :
*
*                       Vx.yy.zz
*
*                           where
*                                   V               denotes 'Version' label
*                                   x               denotes     major software version revision number
*                                   yy              denotes     minor software version revision number
*                                   zz              denotes sub-minor software version revision number
*
*               (b) The HTTPs software version label #define is formatted as follows :
*
*                       ver = x.yyzz * 100 * 100
*
*                           where
*                                   ver             denotes software version number scaled as an integer value
*                                   x.yyzz          denotes software version number, where the unscaled integer
*                                                       portion denotes the major version number & the unscaled
*                                                       fractional portion denotes the (concatenated) minor
*                                                       version numbers
*********************************************************************************************************
*/

#define  HTTPs_VERSION                                 19700u   /* See Note #1.                                         */


/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#ifdef   HTTPs_MODULE
#define  HTTPs_EXT
#else
#define  HTTPs_EXT  extern
#endif


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*
* Note(s) : (1) The HTTPs module files are located in the following directories :
*
*               (a) \<Your Product Application>\app_cfg.h
*
*               (b) (1) \<Network Protocol Suite>\Source\net_*.*
*
*                   (2) If network security manager is to be used:
*
*                       (A) \<Network Protocol Suite>\Secure\net_secure_mgr.*
*
*                       (B) \<Network Protocol Suite>\Secure\<Network Security Suite>\net_secure.*
*
*               (c) (1) \<HTTPs>\Source\http-s.h
*                                      \http-s.c
*
*                   (2) \<HTTPs>\OS\<os>\http-s_os.*
*
*                   (3) \<HTTPs>\FS\<fs>\apps_fs.*
*
*                       where
*                               <Your Product Application>      directory path for Your Product's Application
*                               <Network Protocol Suite>        directory path for network protocol suite
*                               <HTTPs>                         directory path for HTTPs module
*                               <os>                            directory name for specific operating system (OS)
*                               <fs>                            directory name for specific file      system (FS)
*
*           (2) CPU-configuration software files are located in the following directories :
*
*               (a) \<CPU-Compiler Directory>\cpu_*.*
*               (b) \<CPU-Compiler Directory>\<cpu>\<compiler>\cpu*.*
*
*                       where
*                               <CPU-Compiler Directory>        directory path for common CPU-compiler software
*                               <cpu>                           directory name for specific processor (CPU)
*                               <compiler>                      directory name for specific compiler
*
*           (3) NO compiler-supplied standard library functions SHOULD be used.
*
*               (a) Standard library functions are implemented in the custom library module(s) :
*
*                       \<Custom Library Directory>\lib_*.*
*
*                           where
*                                   <Custom Library Directory>      directory path for custom library software
*
*           (4) Compiler MUST be configured to include as additional include path directories :
*
*               (a) '\<Your Product Application>\'                                          See Note #1a
*
*               (b) (1) '\<Network Protocol Suite>\                                         See Note #1b1
*
*                   (2) (A) '\<Network Protocol Suite>\Secure\'                             See Note #1b2A
*                       (B) '\<Network Protocol Suite>\Secure\<Network Security Suite>\'    See Note #1b2B
*
*               (c) '\<HTTPs>\' directories                                                 See Note #1c
*
*               (d) (1) '\<CPU-Compiler Directory>\'                                        See Note #2a
*                   (2) '\<CPU-Compiler Directory>\<cpu>\<compiler>\'                       See Note #2b
*
*               (e) '\<Custom Library Directory>\'                                          See Note #3a
*********************************************************************************************************
*/

#include  <cpu.h>                                               /* CPU Configuration              (see Note #2b)        */
#include  <cpu_core.h>                                          /* CPU Core Library               (see Note #2a)        */

#include  <lib_def.h>                                           /* Standard        Defines        (see Note #3a)        */
#include  <lib_str.h>                                           /* Standard String Library        (see Note #3a)        */
#include  <lib_ascii.h>                                         /* Standard ASCII  Library        (see Note #3a)        */

#include  <app_cfg.h>                                           /* Application Configuration File (see Note #1a)        */
#include  <apps_fs.h>                                           /* File System Interface          (see Note #1c3)       */

#include  <net.h>                                               /* Network Protocol Suite         (see Note #1b)        */


/*
*********************************************************************************************************
*                                                DEFINES
*********************************************************************************************************
*/

                                                                    /* ---- HTTPs Module Configuration -------------------- */
#define  HTTPs_CONN_Q_SIZE                                 3
#define  HTTPs_MAX_PKT_SIZE                             1024

#define  HTTPs_TOK_VAR_START                             "${"
#define  HTTPs_TOK_VAR_STOP                               "}"

#define  HTTPs_TOK_DFLT                                "NULL"

#define  HTTPs_TOK_PARSE_TBL_SIZE_MIN                      1
#define  HTTPs_TOK_PARSE_TBL_SIZE_TERM_NULL                2

                                                                /* ---- HTTP Version Definitions ---------------------- */
#define  HTTP_VER_UNSUPPORTED                              0
#define  HTTP_VER_09                                       1
#define  HTTP_VER_10                                       2
#define  HTTP_VER_11                                       3

                                                                /* ---- HTTP Incoming Methods Definitions ------------- */
#define  HTTP_METHOD_UNSUPPORTED                           0
#define  HTTP_METHOD_GET                                   1
#define  HTTP_METHOD_HEAD                                  2
#define  HTTP_METHOD_POST                                  3

                                                                /* ---- HTTP Status Definitions ----------------------- */
#define  HTTP_STATUS_OK                                    0
#define  HTTP_STATUS_CREATED                               1
#define  HTTP_STATUS_ACCEPTED                              2
#define  HTTP_STATUS_NO_CONTENT                            3
#define  HTTP_STATUS_MOVED_PERMANENTLY                     4
#define  HTTP_STATUS_MOVED_TEMPORARILY                     5
#define  HTTP_STATUS_NOT_MODIFIED                          6
#define  HTTP_STATUS_BAD_REQUEST                           7
#define  HTTP_STATUS_UNAUTHORIZED                          8
#define  HTTP_STATUS_FORBIDDEN                             9
#define  HTTP_STATUS_NOT_FOUND                            10
#define  HTTP_STATUS_INTERNAL_SERVER_ERROR                11
#define  HTTP_STATUS_NOT_IMPLEMENTED                      12
#define  HTTP_STATUS_BAD_GATEWAY                          13
#define  HTTP_STATUS_SERVICE_UNAVAILABLE                  14

                                                                /* ---- HTTP Header ----------------------------------- */
#define  HTTPs_STR_CONTENT_TYPE                 "Content-Type: "
#define  HTTPs_STR_CONTENT_TYPE_LEN                       14
#define  HTTPs_STR_CONTENT_LEN                  "Content-Length: "
#define  HTTPs_STR_CONTENT_LEN_LEN                        16

#define  HTTPs_STR_SPACE                        " "
#define  HTTPs_STR_SPACE_LEN                               1
#define  HTTPs_STR_CRLF                         "\x0D\x0A"
#define  HTTPs_STR_CRLF_LEN                                2

#define  HTTPs_PATH_SEP_CHAR                    '/'
#define  HTTPs_ROOT_PATH                        "/"

/*
*********************************************************************************************************
*                                               FSM STATES
*
* Note(s)     : (1) See function HTTPs_ProcessDynHTMLFile() for details on this state machine.
*********************************************************************************************************
*/

#define  HTTPs_STATE_COPY_DATA                             1
#define  HTTPs_STATE_START_1_FOUND                         2
#define  HTTPs_STATE_START_2_FOUND                         3
#define  HTTPs_STATE_REPLACE_VAL                           4
#define  HTTPs_STATE_REVERT                                5
#define  HTTPs_STATE_COPY_OVER                             6


/*
*********************************************************************************************************
*                                              DATA TYPES
*********************************************************************************************************
*/

                                                                /* HTTP_Status[]:                                       */
                                                                /* Status Code and Reason Phrase table (taken from      */
                                                                /* RFC2068).  This array is indexed by the              */
                                                                /* HTTP_STATUS_YYY values.                              */
typedef  struct  HTTPs_StatusStruct {
           CPU_INT32U   Code;
    const  CPU_CHAR    *Reason;
} HTTPs_STATUS_STRUCT;

                                                                /* HTTPs_MimeType[]:                                    */
                                                                /* Mapping between some popular web file extensions and */
                                                                /* their Mime Types.  Other types can be added.  The    */
                                                                /* NULL element at the end of the list MUST remain the  */
                                                                /* last element.                                        */
typedef  struct  HTTPs_MimeTypeStruct {
    const  CPU_CHAR  *FileExt;
    const  CPU_CHAR  *MimeType;
} HTTPs_MIME_TYPE_STRUCT;

                                                                /* HTTPs_ConnInfo[]:                                    */
                                                                /* Stores the state of each HTTP connection.            */
typedef  struct  HTTPs_ConnInfo {
    CPU_INT32U     ConnNbr;                                     /* Connection number.                                   */
    NET_SOCK_ID    SockID;                                      /* Socket ID of connection.                             */
    CPU_INT32U     ClientVerNo;                                 /* Client HTTP version number.                          */
    CPU_CHAR       ClientVer[32];                               /* Client HTTP version string.                          */
    CPU_CHAR       ServerVer[32];                               /* Server HTTP version response string.                 */
    NET_SOCK_ADDR  ClientAddr;                                  /* Client socket information.                           */
} HTTPs_CONN_INFO;


/*
*********************************************************************************************************
*                                           GLOBAL VARIABLES
*********************************************************************************************************
*/

                                                                /* Used to keep track of the total number of HTTP       */
                                                                /* accesses that have been made on the server.          */
HTTPs_EXT  CPU_INT32U  HTTPs_ReqCnt;

                                                                /* Table used to get tokens' values when dealing with   */
                                                                /* dynamic content HTML pages.                          */
#if HTTPs_CFG_TOK_PARSE_EN == DEF_ENABLED
HTTPs_EXT  CPU_CHAR    HTTPs_TokParseTbl[HTTPs_CFG_TOK_PARSE_TBL_SIZE + HTTPs_TOK_PARSE_TBL_SIZE_TERM_NULL];
#endif


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

CPU_BOOLEAN  HTTPs_Init(CPU_BOOLEAN   secure);
void         HTTPs_Task(void         *p_arg);


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*                               DEFINED IN USER'S APPLICATION (CALLBACKS)
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            HTTPs_ValReq()
*
* Description : Called for each ${TEXT_STRING} embedded variable found in a HTML file.
*
* Argument(s) : ptok       string containing the name of the HTML embedded token.
*               pval       pointer to the string containing the value of the HTML embedded token.
*
* Return(s)   : DEF_FAIL   Token doesn't exist.
*               DEF_OK     Value returned successfully.
*
* Caller(s)   : HTTPs_ProcessDynHTMLFile().
*
* Note(s)     : This function is called by the server as it is parsing an outgoing HTML file.  If it
*               detects the ${TEXT_STRING} syntax, it calls this function with tok name pointing to the
*               TEXT_STRING.  Application should set the value pointer to a application-allocated buffer
*               containing the value.  The replacement text used here is 100% application specific.
*               Buffer is not modified.
*********************************************************************************************************
*/

CPU_BOOLEAN  HTTPs_ValReq(CPU_CHAR   *ptok,
                          CPU_CHAR  **pval);


/*
*********************************************************************************************************
*                                             HTTPs_ValRx()
*
* Description : Called for each variable received from a HTTP POST command.
*
* Argument(s) : pvar        string containing the name  of the HTTP POST variable.
*               pval        string containing the value of the HTTP POST variable.
*
* Return(s)   : Application should set the return value to:
*                   DEF_FAIL        data process failed.
*                   DEF_OK          data process successful.
*
* Caller(s)   : HTTPs_ProcessCGIList().
*
* Note(s)     : This function is called by the server as it is processing any list of name/value pairs
*               in a form response.  This is where the application puts all of the "actions" that must
*               occur as a result of a client interacting with a form.  The name and value are incoming
*               ASCII and it is 100% application specific code that deals with these name/value pairs.
*********************************************************************************************************
*/

CPU_BOOLEAN  HTTPs_ValRx(CPU_CHAR  *pvar,
                         CPU_CHAR  *pval);


/*
*********************************************************************************************************
*                                       RTOS INTERFACE FUNCTIONS
*                                           (see http-s_os.c)
*********************************************************************************************************
*/

CPU_BOOLEAN  HTTPs_OS_Init       (void  *p_arg);
void         HTTPs_OS_TaskSuspend(void);


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

                                                                /* Trace level, default to TRACE_LEVEL_OFF              */
#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                                   0
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                                  1
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                                   2
#endif

#ifndef  HTTPs_TRACE_LEVEL
#define  HTTPs_TRACE_LEVEL                   TRACE_LEVEL_OFF
#endif

#ifndef  HTTPs_TRACE
#define  HTTPs_TRACE                                  printf
#endif

#define  HTTPs_TRACE_INFO(x)                  ((HTTPs_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(HTTPs_TRACE x) : (void)0)
#define  HTTPs_TRACE_DBG(x)                   ((HTTPs_TRACE_LEVEL >= TRACE_LEVEL_DBG)  ? (void)(HTTPs_TRACE x) : (void)0)


/*
*********************************************************************************************************
*                                         CONFIGURATION ERRORS
*********************************************************************************************************
*/

#ifndef  HTTPs_CFG_IPPORT
#error  "HTTPs_CFG_IPPORT                  illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif

#ifndef  HTTPs_CFG_IPPORT_SECURE
#error  "HTTPs_CFG_IPPORT_SECURE                 not #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif



#ifndef  HTTPs_CFG_MAX_ACCEPT_TIMEOUT_MS
#error  "HTTPs_CFG_MAX_ACCEPT_TIMEOUT_MS   illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif

#ifndef  HTTPs_CFG_MAX_RX_TIMEOUT_MS
#error  "HTTPs_CFG_MAX_RX_TIMEOUT_MS       illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif

#ifndef  HTTPs_CFG_MAX_TX_TIMEOUT_MS
#error  "HTTPs_CFG_MAX_TX_TIMEOUT_MS       illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif

#ifndef  HTTPs_CFG_MAX_ACCEPT_RETRY
#error  "HTTPs_CFG_MAX_ACCEPT_RETRY        illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif

#ifndef  HTTPs_CFG_MAX_RX_RETRY
#error  "HTTPs_CFG_MAX_RX_RETRY            illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif

#ifndef  HTTPs_CFG_MAX_TX_RETRY
#error  "HTTPs_CFG_MAX_TX_RETRY            illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif



#ifndef  HTTPs_CFG_FILE_RD_BUF_LEN
#error  "HTTPs_CFG_FILE_RD_BUF_LEN         illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif

#ifndef  HTTPs_CFG_FILE_TX_BUF_LEN
#error  "HTTPs_CFG_FILE_TX_BUF_LEN         illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif



#ifndef  HTTPs_CFG_TOK_PARSE_EN
#error  "HTTPs_CFG_TOK_PARSE_EN            illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif

#ifndef  HTTPs_CFG_TOK_PARSE_TBL_SIZE
#error  "HTTPs_CFG_TOK_PARSE_TBL_SIZE      illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#elif   (HTTPs_CFG_TOK_PARSE_TBL_SIZE < HTTPs_TOK_PARSE_TBL_SIZE_MIN)
#error  "HTTPs_CFG_TOK_PARSE_TBL_SIZE      illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif



#ifndef  HTTPs_CFG_FS_ROOT
#error  "HTTPs_CFG_FS_ROOT                 illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif

#ifndef  HTTPs_CFG_DFLT_FILE
#error  "HTTPs_CFG_DFLT_FILE               illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif

#ifndef  HTTPs_CFG_ERR_MSG_HTML_NOT_FOUND
#error  "HTTPs_CFG_ERR_MSG_HTML_NOT_FOUND  illegally #define'd in 'app_cfg.h'"
#error  "                                  see template file in package      "
#error  "                                  named 'http-s_cfg.h'              "
#endif


/*
*********************************************************************************************************
*                                    NETWORK CONFIGURATION ERRORS
*********************************************************************************************************
*/

#if     (NET_CFG_TRANSPORT_LAYER_SEL != NET_TRANSPORT_LAYER_SEL_UDP_TCP)
#error  "NET_CFG_TRANSPORT_LAYER_SEL       illegally #define'd in 'net_cfg.h'        "
#error  "                                  [MUST be  NET_TRANSPORT_LAYER_SEL_UDP_TCP]"
#endif


/*
*********************************************************************************************************
*                                             MODULE END
*********************************************************************************************************
*/

#endif                                                          /* End of HTTPs module include.                         */
