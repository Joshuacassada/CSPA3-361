//---------------------------------------------------------------------
// Assignment : PA-03 UDP Single-Threaded Server
// Date       :
// Author     : WRITE YOUR  NAME(S)  HERE  ... or risk losing points
// File Name  : factory.c
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

#define MAXSTR     200
#define IPSTRLEN    50

typedef struct sockaddr SA ;

int minimum( int a , int b)
{
    return ( a <= b ? a : b ) ; 
}

void subFactory( int factoryID , int myCapacity , int myDuration ) ;

void factLog( char *str )
{
    printf( "%s" , str );
    fflush( stdout ) ;
}

/*-------------------------------------------------------*/

// Global Variable for Future Thread to Shared
int   remainsToMake , // Must be protected by a Mutex
      actuallyMade ;  // Actually manufactured items

int   numActiveFactories = 1 , orderSize ;

int   sd ;      // Server socket descriptor
struct sockaddr_in  
             srvrSkt,       /* the address of this server   */
             clntSkt;       /* remote client's socket       */

//------------------------------------------------------------
//  Handle Ctrl-C or KILL 
//------------------------------------------------------------
void goodbye(int sig) 
{
    /* Mission Accomplished */
    printf( "\n### I (%d) have been nicely asked to TERMINATE. "
           "goodbye\n\n" , getpid() );  
    close(sd);
    exit(0);

    // missing code goes here

}

/*-------------------------------------------------------*/
int main( int argc , char *argv[] )
{

    
    char  *myName = "Joshua Cassada" ; 
    unsigned short port = 50015 ;      /* service port number  */
    int    N = 1 ;                     /* Num threads serving the client */

    printf("\nThis is the FACTORY server developed by %s\n\n" , myName ) ;
    char myUserName[30] ;
    char    ipStr[ IPSTRLEN ] ;    /* dotted-dec IP addr. */

    getlogin_r ( myUserName , 30 ) ;
    time_t  now;
    time( &now ) ;
    fprintf( stdout , "Logged in as user '%s' on %s\n\n" , myUserName ,  ctime( &now)  ) ;
    fflush( stdout ) ;

	switch (argc) 
	{
      case 1:
        break ;     // use default port with a single factory thread
      
      case 2:
        N = atoi( argv[1] ); // get from command line
        port = 50015;            // use this port by default
        break;

      case 3:
        N    = atoi( argv[1] ) ; // get from command line
        port = atoi( argv[2] ) ; // use port from command line
        break;

      default:
        printf( "FACTORY Usage: %s [numThreads] [port]\n" , argv[0] );
        exit( 1 ) ;
    }


    // missing code goes here
    msgBuf msg1;
    
    sd = Socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0){
        err_sys;
    }
    memset((void *) &srvrSkt, 0, sizeof(srvrSkt));
    srvrSkt.sin_family = AF_INET;
    srvrSkt.sin_addr.s_addr = htonl(INADDR_ANY);
    srvrSkt.sin_port = htons(port);
    if (bind(sd, (const struct sockaddr *)&srvrSkt, sizeof(srvrSkt)) < 0) {
        perror("bind failed");
        close(sd);
        exit(EXIT_FAILURE);
    }

    inet_ntop( AF_INET, (void *) & srvrSkt.sin_addr.s_addr , ipStr, IPSTRLEN ) ;
    printf( "Bound socket %d to IP %s Port %d\n" , sd , ipStr , ntohs( srvrSkt.sin_port ) );    


    signal(SIGINT, goodbye);
    signal(SIGTERM, goodbye);

    int forever = 1;
    while ( forever )
    {
        printf( "\nFACTORY server waiting for Order Requests\n" ) ; 

        // missing code goes here
        
        unsigned int client = sizeof(clntSkt);

        recvfrom(sd, &msg1, sizeof(msg1), 0, (SA *)&clntSkt, &client);


        printf("\n\nFACTORY server received: " ) ;
        printMsg( & msg1 );  
        puts("");


        // missing code goes here
        msg1.purpose = ORDR_CONFIRM;
        msg1.numFac = 1;

        Sendto(sd, &msg1, sizeof(msg1), 0, (SA *)&clntSkt, client);



        printf("\n\nFACTORY sent this Order Confirmation to the client " );
        printMsg(  & msg1 );  puts("");
        
        remainsToMake = msg1.orderSize;
        subFactory( 1 , 50 , 350 ) ;  // Single factory, ID=1 , capacity=50, duration=350 ms
    }


    return 0 ;
}

void subFactory( int factoryID , int myCapacity , int myDuration )
{
    char    strBuff[ MAXSTR ] ;   // snprint buffer
    int     partsImade = 0 , myIterations = 0 ;
    msgBuf  msg;

    while ( 1 )
    {
        // See if there are still any parts to manufacture
        if ( remainsToMake <= 0 )
            break ;   // Not anymore, exit the loop
        

        int toMake = minimum(myCapacity, remainsToMake);
        remainsToMake -= toMake;
        partsImade += toMake;
        myIterations++;


        // missing code goes here

        Usleep(myDuration * 1000);

        msg.purpose = PRODUCTION_MSG;
        msg.facID = factoryID;
        msg.capacity = myCapacity;
        msg.partsMade = toMake;
        msg.duration = myDuration;
        sendto(sd, &msg, sizeof(msg), 0, (SA *)&clntSkt, sizeof(clntSkt));


        // Send a Production Message to Supervisor


        // missing code goes here

    }

    // Send a Completion Message to Supervisor


    // missing code goes here

    msg.purpose = COMPLETION_MSG;
    msg.facID = factoryID;
    msg.capacity = myCapacity;
    msg.partsMade = partsImade;
    msg.duration = myDuration;
    sendto(sd, &msg, sizeof(msg), 0, (SA *)&clntSkt, sizeof(clntSkt));


    snprintf( strBuff , MAXSTR , ">>> Factory # %-3d: Terminating after making total of %-5d parts in %-4d iterations\n" 
          , factoryID, partsImade, myIterations);
    factLog( strBuff ) ;
    
}

