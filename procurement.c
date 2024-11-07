//---------------------------------------------------------------------
// Assignment : PA-03 UDP Single-Threaded Server
// Date       :
// Author     : WRITE YOUR  NAME(S)  HERE  ... or risk losing points
// File Name  : procurement.c
//---------------------------------------------------------------------

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "wrappers.h"
#include "message.h"

#define MAXFACTORIES    20

typedef struct sockaddr SA ;

/*-------------------------------------------------------*/
int main( int argc , char *argv[] )
{
    int     numFactories ,      // Total Number of Factory Threads
            activeFactories ,   // How many are still alive and manufacturing parts
            iters[ MAXFACTORIES+1 ] = {0} ,  // num Iterations completed by each Factory
            partsMade[ MAXFACTORIES+1 ] = {0} , totalItems = 0;

    char  *myName = "Joshua Cassada and Thomas Cantrell" ; 
    printf("\nPROCUREMENT: Started. Developed by %s\n\n" , myName );    

    char myUserName[30] ;
    getlogin_r ( myUserName , 30 ) ;
    time_t  now;
    time( &now ) ;
    fprintf( stdout , "Logged in as user '%s' on %s\n\n" , myUserName ,  ctime( &now)  ) ;
    fflush( stdout ) ;
    
    if ( argc < 4 )
    {
        printf("PROCUREMENT Usage: %s  <order_size> <FactoryServerIP>  <port>\n" , argv[0] );
        exit( -1 ) ;  
    }

    unsigned        orderSize  = atoi( argv[1] ) ;
    char	       *serverIP   = argv[2] ;
    unsigned short  port       = (unsigned short) atoi( argv[3] ) ;
 

    /* Set up local and remote sockets */

    struct sockaddr_in myAddr, serverAddr;

    int sd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sd < 0){
        perror("Socket address failed");
        exit(EXIT_FAILURE);
    }
    memset((void *) &myAddr, 0, sizeof(myAddr));
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(0);

    if (bind(sd, (const struct sockaddr *)&myAddr, sizeof(myAddr)) < 0) {
        perror("bind failed");
        close(sd);
        exit(EXIT_FAILURE);
    }

    
    // missing code goes here


    // Prepare the server's socket address structure

    memset((void *) &serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIP, (void *) & serverAddr.sin_addr.s_addr);



    // missing code goes here



    // Send the initial request to the Factory Server
    msgBuf  msg1;

    msg1.purpose = REQUEST_MSG;
    msg1.orderSize = orderSize;
    
    sendto(sd, &msg1, sizeof(msg1), 0, (SA *)&serverAddr, sizeof(serverAddr));

    // missing code goes here


    printf("\nPROCUREMENT Sent this message to the FACTORY server: "  );
    printMsg( & msg1 );  puts("");


    /* Now, wait for oreder confirmation from the Factory server */
    msgBuf  msg2;
    printf ("\nPROCUREMENT is now waiting for order confirmation ...\n" );


    // missing code goes here
    socklen_t len = sizeof(serverAddr);

    recvfrom(sd, &msg2, sizeof(msg2), 0, (SA *)&serverAddr, &len);



    printf("PROCUREMENT received this from the FACTORY server: "  );
    printMsg( & msg2 );  puts("\n");


    numFactories = msg2.numFac;
    activeFactories = numFactories;

    // missing code goes here


    // Monitor all Active Factory Lines & Collect Production Reports
    while ( activeFactories > 0 ) // wait for messages from sub-factories
    {

        msgBuf msg;

        recvfrom(sd, &msg, sizeof(msg), 0, (SA *)&serverAddr, &len);

        if (msg.purpose == PRODUCTION_MSG){
            iters[msg.facID]++;
            partsMade[msg.facID] += msg.partsMade;
            printf("Received production update from Factory #%d: made %d items in %d ms\n",
                   msg.facID, msg.partsMade, msg.duration);
        } else if (msg.purpose == COMPLETION_MSG){
            activeFactories--;
            printf("Factory #%d has completed production\n", msg.facID);

        }

        // missing code goes here


       // Inspect the incoming message

       // missing code goes here

    } 

    // Print the summary report
    totalItems  = 0 ;
    printf("\n\n****** PROCUREMENT Summary Report ******\n");


    // missing code goes here
    for (int i = 1; i <= numFactories; i++){
        printf("Factory #%d: Made %d items in %d iterations\n",
               i, partsMade[i], iters[i]);
        totalItems += partsMade[i];
    }

    printf("==============================\n") ;
    printf("Total items made: %d\n", totalItems);
    printf("Original order size: %d\n", orderSize);


    // missing code goes here

    if (totalItems == orderSize){
        printf("Order completed successfully");
    }else{
        printf("THese do not match this is incorrect");
    }


    printf( "\n>>> Supervisor Terminated\n");



    // missing code goes here
    close(sd);


    return 0 ;
}
