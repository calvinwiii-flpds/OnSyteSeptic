#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "ssh.h"

int FileCopy(SSH *pssh, char* filename);

int main(void)
{
    SSH my_ssh;
 //   char* password;

    //password = getpass("Password: ");

    if (!my_ssh.OpenSSH((char *)"cwiney@onsyte-raspi-slave.local",(char *)"aardvark"))
    {
        fprintf(stderr, "my_ssh.OpenSSH failed\n");
        exit(-1);
    }

    my_ssh.scp_write_file("testfile.txt",32);


    //stuff...
    my_ssh.show_remote_processes("ls");

    my_ssh.CloseSSH();


    for (int i = 0; i < 15; i++)
    {
        sleep(1000);
    }
}

int FileCopy(SSH* pssh, char* filename)
{

}

