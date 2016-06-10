#include "config.h"

int parse_commandline(struct parameter *p,int argc,char *argv[])
{
    int ch;
    opterr = 0;

    //host,port可选,-f readfile -o outputfile
    strcpy(p->host,HOST);
    p->port = PORT;
    //-h127.0.01 -p9000 -f xxx -offf
    while((ch=getopt(argc,argv,"h::p::f:o::"))!=-1){
        switch(ch){
            case 'h':
                snprintf(p->host,50,"%s",optarg);
                break;
            case 'p':
                p->port = atoi(optarg);
                break;
            case 'f':
                snprintf(p->infile,MAXLEN,"%s",optarg);
                strcpy(p->outfile,basename(p->infile));//default
                break;
            case 'o':
                snprintf(p->outfile,MAXLEN,"%s",optarg);
                break;
            default:
                printf(" -h host server host[default:localhost]\n -p port server port[default:9000]\n -f input file\n -o output file[default:src filename]\n");
                opterr = 1;
        }
    }
    return opterr;
}
