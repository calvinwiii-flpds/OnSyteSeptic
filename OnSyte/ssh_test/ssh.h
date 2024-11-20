#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "libssh.h"

class SSH
{
public:
	ssh_session my_ssh_session;
	ssh_scp my_read_scp;
	ssh_scp my_write_scp;

	bool OpenSSH(char* hostname, char* password);
	void CloseSSH(void);
	int verify_knownhost(ssh_session session);
	int show_remote_processes( char* process);
	int scp_receive( void);
	int scp_read_open(char* filename);
	int scp_write_open(void);
	int scp_read_close(void);
	int scp_write_close(void);

	int scp_write_file(char* filename, int filelength);


};
