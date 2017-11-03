#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <netdb.h>

#define DAP_LOCAL_ADDR "/tmp/DapVpnService"


void critical_error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, const char*argv[])
{
    if(argc<2){
        printf("[DapVpnCli] DiveVPN command line interface. Usage:  DapVpnCli <command> [<arguments>]\n");
    }else{
        size_t cmd_length=0;
        int i;
        char * cmd;
        // Calculate total length of command
        for(i=1;i<argc;i++){
            cmd_length+=strlen(argv[i])+1;
        }

        // Concatinate all the arguments into the one big long fucking string
        cmd=malloc(cmd_length+1);
        cmd[0]='\0';
        for(i=1;i<argc;i++){
            strcat(cmd,argv[i]);
            if(i!=(argc-1))
                strcat(cmd," ");
        }

        printf ("[DapVpnCli] Executing \"%s\" command...\n",cmd);

        cmd[cmd_length-1]='\n';
        cmd[cmd_length]='\0';
        int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
        if(sockfd<0)
            critical_error("[DapVpnCli][CRITICAL] Can't create socket!");
        struct sockaddr_un saddr ={0};
        saddr.sun_family = AF_LOCAL;
        strncpy(saddr.sun_path,DAP_LOCAL_ADDR,sizeof(saddr.sun_path)-1);
       // {AF_LOCAL, DAP_LOCAL_ADDR};
        socklen_t saddrlen = sizeof(struct sockaddr) + strlen(DAP_LOCAL_ADDR);

        int conn;
        if( (conn = connect(sockfd, (const struct sockaddr *) &saddr, sizeof(struct sockaddr_un) ) ) == 0  ){
            printf("[DapVpnCli] Connected to %s\n",DAP_LOCAL_ADDR);
            ssize_t sent_data=0;
            size_t sent_data_total=0;
            do{
                sent_data=send(sockfd,cmd+sent_data_total,cmd_length+1-sent_data_total,0);
                if(sent_data>0)
                    sent_data_total += (size_t) sent_data;
                else
                    critical_error("[DapVpnCli][CRITICAL] Error with command sending");
            } while (sent_data_total < (cmd_length+1) );
            printf("[DapVpnCli] Command sent successfuly\n");
        }else{
            printf("[DapVpnCli][ERROR] connect() returned %d, err str '%s'\n",conn, strerror(errno));
            critical_error("[DapVpnCli][CRITICAL] Can't connect to the local " DAP_LOCAL_ADDR " socket!");
        }

    }
    return 0;
}
