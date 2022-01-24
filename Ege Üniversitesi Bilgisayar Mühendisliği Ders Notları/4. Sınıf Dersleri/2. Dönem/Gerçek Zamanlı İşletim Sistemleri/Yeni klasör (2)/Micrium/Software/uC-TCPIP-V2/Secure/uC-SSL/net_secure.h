/*
*********************************************************************************************************
*                                             uC/TCP-IP V2
*                                      The Embedded TCP/IP Suite
*
*                          (c) Copyright 2003-2011; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*
*               uC/TCP-IP is provided in source form to registered licensees ONLY.  It is
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
*                                     NETWORK SECURITY PORT LAYER
*
*                                               uC/SSL
*
* Filename      : net_secure.h
* Version       : V2.11.01
* Programmer(s) : SL
*********************************************************************************************************
* Note(s)       : (1) Assumes the following versions (or more recent) of software modules are included in
*                     the project build :
*
*                     (a) uC/SSL V1.09.01
*                     (b) uC/Clk V3.09
*
*                     See also 'net.h  Note #1'.
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                               MODULE
*********************************************************************************************************
*/

#ifdef  NET_SECURE_MODULE_PRESENT


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <net_secure_os.h>

#include  <clk.h>

#include  <ssl_cfg.h>
#include  <cyassl_int.h>
#include  <error.h>



/*
*********************************************************************************************************
*                                               EXTERNS
*********************************************************************************************************
*/

#if ((defined(NET_SECURE_MODULE)) && \
     (defined(NET_GLOBALS_EXT)))
#define  NET_SECURE_EXT
#else
#define  NET_SECURE_EXT  extern
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                               DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             DATA TYPES
*********************************************************************************************************
*/

typedef  SSL         NET_SECURE_SESSION;

typedef  SSL_CTX     NET_SECURE_CTX;

typedef  SSL_METHOD  NET_SECURE_METHOD;


typedef  BIO         NET_SECURE_BIO;


/*
*********************************************************************************************************
*                                    uC/SSL BUFFER POOLS DATA TYPE
*********************************************************************************************************
*/

                                                                /* ---------------- NET SECURE POOLS ------------------ */
typedef  struct  net_secure_pools {
    MEM_POOL  SSL_Session;
    MEM_POOL  CA_CertKey;
    MEM_POOL  IssuerCN;
    MEM_POOL  PublicKey;
    MEM_POOL  Signer;
    MEM_POOL  RSA_Key;
} NET_SECURE_POOLS;


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/


/*$PAGE*/
/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

                                                                            /* -------------- INIT FNCTS -------------- */
void                NetSecure_Init             (NET_ERR     *p_err);

void                NetSecure_InitSession      (NET_SOCK    *p_sock,
                                                NET_ERR     *p_err);


                                                                            /* ------------- INSTALL FNCTS ------------ */
CPU_BOOLEAN         NetSecure_InstallBuf       (void        *p_buf,
                                                CPU_INT08U   type,
                                                CPU_INT08U   fmt,
                                                CPU_SIZE_T   size,
                                                NET_ERR     *p_err);

#if (NET_SECURE_CFG_FS_EN == DEF_ENABLED)
CPU_BOOLEAN         NetSecure_InstallFile      (CPU_CHAR    *p_file,
                                                CPU_INT08U   type,
                                                CPU_INT08U   fmt,
                                                NET_ERR     *p_err);
#endif


                                                                            /* ----------- SOCK HANDLER FNCTS --------- */
void                NetSecure_SockClose        (NET_SOCK    *p_sock,
                                                NET_ERR     *p_err);

void                NetSecure_SockCloseNotify  (NET_SOCK    *p_sock,
                                                NET_ERR     *p_err);

void                NetSecure_SockConn         (NET_SOCK    *p_sock,
                                                NET_ERR     *p_err);

void                NetSecure_SockAccept       (NET_SOCK    *p_sock_listen,
                                                NET_SOCK    *p_sock_accept,
                                                NET_ERR     *p_err);


NET_SOCK_RTN_CODE   NetSecure_SockRxDataHandler(NET_SOCK    *p_sock,
                                                void        *p_data_buf,
                                                CPU_INT16U   data_buf_len,
                                                NET_ERR     *p_err);

NET_SOCK_RTN_CODE   NetSecure_SockTxDataHandler(NET_SOCK    *p_sock,
                                                void        *p_data_buf,
                                                CPU_INT16U   data_buf_len,
                                                NET_ERR     *p_err);


                                                                            /* -------------- MGMT FNCTS -------------- */
CPU_INT08U         *NetSecure_BlkGet           (CPU_INT08U   type,
                                                CPU_SIZE_T   size,
                                                NET_ERR     *p_err);

void                NetSecure_BlkFree          (CPU_INT08U   type,
                                                void        *p_buf,
                                                NET_ERR     *p_err);


void                NetSecure_TraceOut         (CPU_CHAR    *p_msg);


                                                                            /* ------------ CALLBACK FNCTS ------------ */
CPU_BOOLEAN         NetSecure_ValidateDate     (CPU_INT16U   yr,
                                                CPU_INT08U   month,
                                                CPU_INT16U   day,
                                                CPU_INT08U   hr,
                                                CPU_INT08U   min,
                                                CPU_INT08U   sec,
                                                CPU_INT08U   type);


/*$PAGE*/
/*
*********************************************************************************************************
*                                    CALLBACK FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                        NetSecure_InitSeed()
*
* Description : Generate an initialization seed for the random number generator.
*
* Argument(s) : p_seed      Pointer to the first byte of the seed to generate.
*
*               size        Size of    the seed to generate in bytes. See Note #2.
*
* Return(s)   : none.
*
* Caller(s)   : GenerateSeed().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by
*               application function(s).
*
* Note(s)     : (1) The security level of the connection is directly related to the randomness of the
*                   seed that will be generated by this callback function. If the seed is not random
*                   enough, an attacker could break the encryption and monitor the data exchanged
*                   between the peers.
*
*               (2) If the seed to be generated is 256 bits, this function will be called with the
*                   size param set to 32. On a system that can only generate 8 bits random numbers,
*                   this function MUST set the first random byte at the seed pointer position, and
*                   increment the seed pointer to point on the next byte. This operation must be
*                   repeated 32 times to form the 256 bits seed.
*
*                   On a system that can generate 32 bits random numbers, this function MUST set
*                   the first random word at the seed position, and increment the seed pointer to
*                   point on the next word. This operation must be repeated 8 times to form the
*                   256 bits seed.
*********************************************************************************************************
*/

void  NetSecure_InitSeed(CPU_INT08U  *p_seed,
                         CPU_INT32U   size);


/*
*********************************************************************************************************
*                                    NetSecure_TrustInvalidSig()
*
* Description : Callback function that defines the action to be taken if a certificate sent by a server
*                   during the connect handshake process is not trusted by any of the installed CAs.
*
* Argument(s) : p_cn    Common name of the untrusted certificate.
*
* Return(s)   : DEF_TRUE,  if connection is to be established with the untrusted certificate,
*
*               DEF_FALSE, if the connection should be closed.
*
* Caller(s)   : NetSecure_SockConn().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by
*               application function(s).
*
* Note(s)     : (1) This function will also be called if no CA is installed.
*
*               (2) If this function returns DEF_TRUE, it can be considered as a user clicking the
*                   "I Understand the Risks" button on Firefox to continue with the untrustred
*                   certificate. If the application understand what is going on, it can trust this
*                   server’s identification even if it is not trusted by any of the installed CAs.
*                   However, it could mean that an attacker is tempering with the connection.
*
*               (3) This function must ONLY return DEF_TRUE if the application knows there is a good
*                   reason why the server does not use trusted identification.
*
*               (4) This function MUST return DEF_FALSE if the application requires a high security
*                   level.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetSecure_TrustInvalidSig(CPU_CHAR  *p_cn);


/*$PAGE*/
/*
*********************************************************************************************************
*                                    NetSecure_TrustExpiredDate()
*
* Description : (1) Callback function to defines the action to be taken if a certificate is expired :
*
*                   (a) During certificate installation
*                   (b) During SSL connect handshake to validate server's public key certificate
*
*
* Argument(s) : none.
*
* Return(s)   : DEF_TRUE,  if the application decides     to trust the expired certificate.
*
*               DEF_FALSE, if the application decides NOT to trust the expired certificate.
*
* Caller(s)   : NetSecure_SockConn(),
*               NetSecure_InstallBuf(),
*               NetSecure_InstallFile().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by
*               application function(s).
*
* Note(s)     : (2) This function must ONLY return DEF_TRUE if the application knows there is a good
*                   reason why the certificate is expired.
*
*               (3) This function MUST return DEF_FALSE if the application requires a high security
*                   level.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetSecure_TrustExpiredDate(void);


/*
*********************************************************************************************************
*                                    NetSecure_TrustInvalidDate()
*
* Description : (1) Callback function to defines the action to be taken if the certificate has been
*                       created after the current local time :
*
*                   (a) During certificate installation
*                   (b) During SSL connect handshake to validate server's public key certificate
*
*
* Argument(s) : none.
*
* Return(s)   : DEF_TRUE,  if the application decides     to trust the bad date certificate.
*
*               DEF_FALSE, if the application decides NOT to trust the bad date certificate.
*
* Caller(s)   : NetSecure_SockConn(),
*               NetSecure_InstallBuf(),
*               NetSecure_InstallFile().
*
*               This function is an INTERNAL network protocol suite function & MUST NOT be called by
*               application function(s).
*
* Note(s)     : (2) It is technically impossible that a certificate is created in the future so the
*                   function might be be called because the system local time is wrong.
*
*               (3) This function must ONLY return DEF_TRUE if the application knows there is a good
*                   reason why the current local time is wrong.
*
*               (4) This function MUST return DEF_FALSE if the application requires a high security
*                   level.
*********************************************************************************************************
*/

CPU_BOOLEAN  NetSecure_TrustInvalidDate(void);


/*$PAGE*/
/*
*********************************************************************************************************
*                                              TRACING
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

#define  SSL_TRACE_LEVEL_OFF                 TRACE_LEVEL_OFF
#define  SSL_TRACE_LEVEL_INFO                TRACE_LEVEL_INFO
#define  SSL_TRACE_LEVEL_DBG                 TRACE_LEVEL_DBG

#ifndef  SSL_TRACE_LEVEL
#define  SSL_TRACE_LEVEL                     SSL_CFG_TRACE_LEVEL
#endif


#ifndef  SSL_TRACE
#define  SSL_TRACE                           APP_TRACE
#endif

#define  SSL_TRACE_INFO(x)                    ((SSL_TRACE_LEVEL >= TRACE_LEVEL_INFO) ? (void)(SSL_TRACE x) : (void)0)
#define  SSL_TRACE_DBG(x)                     ((SSL_TRACE_LEVEL >= TRACE_LEVEL_DBG)  ? (void)(SSL_TRACE x) : (void)0)


/*$PAGE*/
/*
*********************************************************************************************************
*                                        CONFIGURATION ERRORS
*********************************************************************************************************
*/

                                                                                /* See this 'net_secure.h  Note #1a'.   */
#if     (SSL_VERSION < 10901u)
#error  "SSL_VERSION                      [SHOULD be >= V1.09.01]"
#endif




#if    ((NET_SECURE_CFG_VER != NET_SECURE_SSL_V3_0) && \
        (NET_SECURE_CFG_VER != NET_SECURE_TLS_V1_0) && \
        (NET_SECURE_CFG_VER != NET_SECURE_TLS_V1_1) && \
        (NET_SECURE_CFG_VER != NET_SECURE_TLS_V1_2))
#error  "NET_SECURE_CFG_VER               illegally #define'd in 'net_cfg.h'"
#error  "                                 [MUST be  NET_SECURE_SSL_V3_0]    "
#error  "                                 [     ||  NET_SECURE_TLS_V1_0]    "
#error  "                                 [     ||  NET_SECURE_TLS_V1_1]    "
#error  "                                 [     ||  NET_SECURE_TLS_V1_2]    "
#endif




#ifndef  SSL_CFG_TRACE_LEVEL
#error  "SSL_CFG_TRACE_LEVEL                    not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  SSL_TRACE_LEVEL_OFF ]   "
#error  "                                 [     ||  SSL_TRACE_LEVEL_INFO]   "
#error  "                                 [     ||  SSL_TRACE_LEVEL_DBG ]   "

#elif  ((SSL_CFG_TRACE_LEVEL != SSL_TRACE_LEVEL_OFF)  && \
        (SSL_CFG_TRACE_LEVEL != SSL_TRACE_LEVEL_INFO) && \
        (SSL_CFG_TRACE_LEVEL != SSL_TRACE_LEVEL_DBG))
#error  "SSL_CFG_TRACE_LEVEL              illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  SSL_TRACE_LEVEL_OFF ]   "
#error  "                                 [     ||  SSL_TRACE_LEVEL_INFO]   "
#error  "                                 [     ||  SSL_TRACE_LEVEL_DBG ]   "
#endif




#ifndef  SSL_CFG_OPENSSL_EN
#error  "SSL_CFG_OPENSSL_EN                     not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif  ((SSL_CFG_OPENSSL_EN != DEF_DISABLED) && \
        (SSL_CFG_OPENSSL_EN != DEF_ENABLED ))
#error  "SSL_CFG_OPENSSL_EN               illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "


#elif   (SSL_CFG_OPENSSL_EN == DEF_ENABLED)


#if     (SSL_CFG_DH_EN      != DEF_ENABLED)
#error  "SSL_CFG_DH_EN                    illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "
#endif

#if     (SSL_CFG_AES_EN     != DEF_ENABLED)
#error  "SSL_CFG_AES_EN                   illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "
#endif


#endif




#ifndef  SSL_CFG_MULTI_THREAD_EN
#error  "SSL_CFG_MULTI_THREAD_EN                not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif  ((SSL_CFG_MULTI_THREAD_EN != DEF_DISABLED) && \
        (SSL_CFG_MULTI_THREAD_EN != DEF_ENABLED ))
#error  "SSL_CFG_MULTI_THREAD_EN          illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "
#endif



/*$PAGE*/
#ifndef  SSL_CFG_RC4_EN
#error  "SSL_CFG_RC4_EN                         not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif  ((SSL_CFG_RC4_EN != DEF_DISABLED) && \
        (SSL_CFG_RC4_EN != DEF_ENABLED ))
#error  "SSL_CFG_RC4_EN                   illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "
#endif



#ifndef  SSL_CFG_3DES_EN
#error  "SSL_CFG_3DES_EN                        not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif  ((SSL_CFG_3DES_EN != DEF_DISABLED) && \
        (SSL_CFG_3DES_EN != DEF_ENABLED ))
#error  "SSL_CFG_3DES_EN                  illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "
#endif



#ifndef  SSL_CFG_DH_EN
#error  "SSL_CFG_DH_EN                          not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif  ((SSL_CFG_DH_EN != DEF_DISABLED) && \
        (SSL_CFG_DH_EN != DEF_ENABLED ))
#error  "SSL_CFG_DH_EN                    illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "
#endif



#ifndef  SSL_CFG_AES_EN
#error  "SSL_CFG_AES_EN                         not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif  ((SSL_CFG_AES_EN != DEF_DISABLED) && \
        (SSL_CFG_AES_EN != DEF_ENABLED ))
#error  "SSL_CFG_AES_EN                   illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "
#endif



#ifndef  SSL_CFG_RABBIT_EN
#error  "SSL_CFG_RABBIT_EN                      not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif  ((SSL_CFG_RABBIT_EN != DEF_DISABLED) && \
        (SSL_CFG_RABBIT_EN != DEF_ENABLED ))
#error  "SSL_CFG_RABBIT_EN                illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "
#endif



#ifndef  SSL_CFG_HC128_EN
#error  "SSL_CFG_HC128_EN                       not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif  ((SSL_CFG_HC128_EN != DEF_DISABLED) && \
        (SSL_CFG_HC128_EN != DEF_ENABLED ))
#error  "SSL_CFG_HC128_EN                 illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "
#endif



/*$PAGE*/
#ifndef  SSL_CFG_DSA_EN
#error  "SSL_CFG_DSA_EN                         not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif  ((SSL_CFG_DSA_EN != DEF_DISABLED) && \
        (SSL_CFG_DSA_EN != DEF_ENABLED ))
#error  "SSL_CFG_DSA_EN                   illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "
#endif



#ifndef  SSL_CFG_PSK_EN
#error  "SSL_CFG_PSK_EN                         not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "

#elif  ((SSL_CFG_PSK_EN != DEF_DISABLED) && \
        (SSL_CFG_PSK_EN != DEF_ENABLED ))
#error  "SSL_CFG_PSK_EN                   illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#error  "                                 [     ||  DEF_ENABLED ]           "
#endif



#ifndef  SSL_CFG_USER_RNG_SEED_EN
#error  "SSL_CFG_USER_RNG_SEED_EN               not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "

#elif   (SSL_CFG_USER_RNG_SEED_EN != DEF_ENABLED)
#error  "SSL_CFG_PSK_EN                   illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "
#endif



#ifndef  SSL_CFG_USER_IO_EN
#error  "SSL_CFG_USER_IO_EN                     not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "

#elif   (SSL_CFG_USER_IO_EN != DEF_ENABLED)
#error  "SSL_CFG_USER_IO_EN               illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "
#endif



#ifndef  SSL_CFG_DER_LOAD_EN
#error  "SSL_CFG_DER_LOAD_EN                    not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "

#elif   (SSL_CFG_DER_LOAD_EN != DEF_ENABLED)
#error  "SSL_CFG_DER_LOAD_EN              illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "
#endif



#ifndef  SSL_CFG_FAST_MATH_EN
#error  "SSL_CFG_FAST_MATH_EN                   not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "

#elif   (SSL_CFG_FAST_MATH_EN != DEF_ENABLED)
#error  "SSL_CFG_FAST_MATH_EN             illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "
#endif



#ifndef  SSL_CFG_TFM_TIMING_RESISTANT_EN
#error  "SSL_CFG_TFM_TIMING_RESISTANT_EN        not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "

#elif   (SSL_CFG_TFM_TIMING_RESISTANT_EN != DEF_ENABLED)
#error  "SSL_CFG_TFM_TIMING_RESISTANT_EN  illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_ENABLED]            "
#endif



/*$PAGE*/
#ifndef  SSL_CFG_MD4_EN
#error  "SSL_CFG_MD4_EN                         not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "

#elif   (SSL_CFG_MD4_EN != DEF_DISABLED)
#error  "SSL_CFG_MD4_EN                   illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#endif



#ifndef  SSL_CFG_WRITEV_EN
#error  "SSL_CFG_WRITEV_EN                      not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "

#elif   (SSL_CFG_WRITEV_EN != DEF_DISABLED)
#error  "SSL_CFG_WRITEV_EN                illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#endif



#ifndef  SSL_CFG_DTLS_EN
#error  "SSL_CFG_DTLS_EN                        not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "

#elif   (SSL_CFG_DTLS_EN != DEF_DISABLED)
#error  "SSL_CFG_DTLS_EN                  illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#endif



#ifndef  SSL_CFG_DYNAMIC_BUFFERS_EN
#error  "SSL_CFG_DYNAMIC_BUFFERS_EN             not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "

#elif   (SSL_CFG_DYNAMIC_BUFFERS_EN != DEF_DISABLED)
#error  "SSL_CFG_DYNAMIC_BUFFERS_EN       illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#endif



#ifndef  SSL_CFG_CALLBACKS_EN
#error  "SSL_CFG_CALLBACKS_EN                   not #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "

#elif   (SSL_CFG_CALLBACKS_EN != DEF_DISABLED)
#error  "SSL_CFG_CALLBACKS_EN             illegally #define'd in 'ssl_cfg.h'"
#error  "                                 [MUST be  DEF_DISABLED]           "
#endif


/*$PAGE*/
/*
*********************************************************************************************************
*                                             MODULE END
*
* Note(s) : (1) See 'net_secure.h  MODULE'.
*********************************************************************************************************
*/

#endif                                                          /* End of net secure port include (see Note #1).        */

