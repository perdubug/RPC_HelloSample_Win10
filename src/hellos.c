// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.


/****************************************************************************
						Microsoft RPC
          
                        hello Example

    FILE:       hellos.c

    USAGE:      hellos  -p protocol_sequence
                        -e endpoint
                        -a server principal name
                        -m max calls
                        -n min calls
                        -f flag for RpcServerListen

    PURPOSE:    Server side of RPC distributed application hello

    FUNCTIONS:  main() - registers server as RPC server

    COMMENTS:   This version of the distributed application that prints
                "hello, world" (or other string) on the server features
                a client that manages its connection to the server.
                It uses the binding handle hello_IfHandle, defined in
                the file hello.h.

****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "hello.h"    // header file generated by MIDL compiler
#include "string.h"
#include "spn.h"

#define PURPOSE \
"This Microsoft RPC Version sample program demonstrates\n\
the use of the [string] attribute. For more information\n\
about the attributes and the RPC API functions, see the\n\
RPC programming guide and reference.\n\n"

void Usage(char * pszProgramName)
{
    fprintf_s(stderr, "%s", PURPOSE);
    fprintf_s(stderr, "Usage:  %s\n", pszProgramName);
    fprintf_s(stderr, " -p protocol_sequence\n");
    fprintf_s(stderr, " -e endpoint\n");
    fprintf_s(stderr, " -a server principal name\n");	
    fprintf_s(stderr, " -m maxcalls\n");
    fprintf_s(stderr, " -n mincalls\n");
    fprintf_s(stderr, " -f flag_wait_op\n");
   

    exit(1);
}

// main:  register the interface, start listening for clients 
void __cdecl main(int argc, char * argv[])
{
    RPC_STATUS status;
    unsigned char * pszProtocolSequence = "ncacn_ip_tcp";
    unsigned char * pszSecurity         = NULL;
    unsigned char * pszEndpoint         = "8865";
    unsigned char * pszSpn              = NULL;
    unsigned int    cMinCalls           = 1;
    unsigned int    cMaxCalls           = RPC_C_LISTEN_MAX_CALLS_DEFAULT;
    unsigned int    fDontWait           = FALSE;
    int i;

    // allow the user to override settings with command line switches 
    for (i = 1; i < argc; i++) {
        if ((*argv[i] == '-') || (*argv[i] == '/')) {
            switch (tolower(*(argv[i]+1))) {
            case 'p':  // protocol sequence
                pszProtocolSequence = argv[++i];
                break;
            case 'e':
                pszEndpoint = argv[++i];
                break;
            case 'a':
                pszSpn = argv[++i];
                break;
            case 'm':
                cMaxCalls = (unsigned int) atoi(argv[++i]);
                break;
            case 'n':
                cMinCalls = (unsigned int) atoi(argv[++i]);
                break;
            case 'f':
                fDontWait = (unsigned int) atoi(argv[++i]);
                break;	   

            case 'h':
            case '?':
            default:
                Usage(argv[0]);
            }
        }
        else
            Usage(argv[0]);
    }

    status = RpcServerUseProtseqEp(pszProtocolSequence,
                                   cMaxCalls,
                                   pszEndpoint,
                                   pszSecurity);  // Security descriptor
    if (status != RPC_S_OK)
    {
		printf_s("Error at RpcServerUseProtseqEp that returns 0x%x\n", status);
        exit(status);
    }
	
    // User did not specify spn, construct one.
    if (pszSpn == NULL) {
        MakeSpn(&pszSpn);
    }

    // Using Negotiate as security provider.
    status = RpcServerRegisterAuthInfo(pszSpn,
                                       RPC_C_AUTHN_GSS_NEGOTIATE,
                                       NULL,
                                       NULL);
	if (status != RPC_S_OK) {
		printf_s("Error at RpcServerRegisterAuthInfo that returns 0x%x\n", status);
        exit(status);
    }	

    status = RpcServerRegisterIfEx(hello_v1_0_s_ifspec, NULL, NULL, 0, RPC_C_LISTEN_MAX_CALLS_DEFAULT, NULL );

    printf_s("RpcServerRegisterIfEx returned 0x%x\n", status);   

    if (status) {
        exit(status);
    }

    printf_s("Calling RpcServerListen\n");
    status = RpcServerListen(cMinCalls,
                             cMaxCalls,
                             fDontWait);
    printf_s("RpcServerListen returned: 0x%x\n", status);
    if (status) {
        exit(status);
    }

    if (fDontWait) {
        printf_s("Calling RpcMgmtWaitServerListen\n");
        status = RpcMgmtWaitServerListen();  // wait operation
        printf_s("RpcMgmtWaitServerListen returned: 0x%x\n", status);
        if (status) {
            exit(status);
        }
    }

}  // end main()


/*********************************************************************/
/*                MIDL allocate and free                             */
/*********************************************************************/

void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
    return(malloc(len));
}

void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
    free(ptr);
}

// end file hellos.c 
