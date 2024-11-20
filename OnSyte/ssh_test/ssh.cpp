//#include <stat. h>
#include "ssh.h"
#include <stdio.h>
#include <stdlib.h>

bool SSH::OpenSSH(char* hostname, char* password)
{
    int rc;


    // Open session and set options
    my_ssh_session = ssh_new();
    if (my_ssh_session == NULL)
        return false;
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, hostname);

    // Connect to server
    rc = ssh_connect(my_ssh_session);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error connecting to localhost: %s\n",
            ssh_get_error(my_ssh_session));
        ssh_free(my_ssh_session);
        return false;
    }

    // Verify the server's identity
    // For the source code of verify_knownhost(), check previous example
    if (verify_knownhost(my_ssh_session) < 0)
    {
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return false;
    }

    // Authenticate ourselves
   // password = getpass("Password: ");
    rc = ssh_userauth_password(my_ssh_session, NULL, password);
    if (rc != SSH_AUTH_SUCCESS)
    {
        fprintf(stderr, "Error authenticating with password: %s\n",
            ssh_get_error(my_ssh_session));
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return false;
    }
    return true;
}
void SSH::CloseSSH(void)
{

    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
}

int SSH::verify_knownhost(ssh_session session)
{
    enum ssh_known_hosts_e state;
    unsigned char* hash = NULL;
    ssh_key srv_pubkey = NULL;
    size_t hlen;
    char buf[10];
    char* hexa;
    char* p;
    int cmp;
    int rc;

    rc = ssh_get_server_publickey(session, &srv_pubkey);
    if (rc < 0) {
        return -1;
    }

    rc = ssh_get_publickey_hash(srv_pubkey,
        SSH_PUBLICKEY_HASH_SHA1,
        &hash,
        &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0) {
        return -1;
    }

    state = ssh_session_is_known_server(session);
    switch (state) {
    case SSH_KNOWN_HOSTS_OK:
        /* OK */

        break;
    case SSH_KNOWN_HOSTS_CHANGED:
        fprintf(stderr, "Host key for server changed: it is now:\n");
        ssh_print_hexa("Public key hash", hash, hlen);
        fprintf(stderr, "For security reasons, connection will be stopped\n");
        ssh_clean_pubkey_hash(&hash);

        return -1;
    case SSH_KNOWN_HOSTS_OTHER:
        fprintf(stderr, "The host key for this server was not found but an other"
            "type of key exists.\n");
        fprintf(stderr, "An attacker might change the default server key to"
            "confuse your client into thinking the key does not exist\n");
        ssh_clean_pubkey_hash(&hash);

        return -1;
    case SSH_KNOWN_HOSTS_NOT_FOUND:
        fprintf(stderr, "Could not find known host file.\n");
        fprintf(stderr, "If you accept the host key here, the file will be"
            "automatically created.\n");

        /* FALL THROUGH to SSH_SERVER_NOT_KNOWN behavior */

    case SSH_KNOWN_HOSTS_UNKNOWN:
        hexa = ssh_get_hexa(hash, hlen);
        fprintf(stderr, "The server is unknown. Do you trust the host key?\n");
        fprintf(stderr, "Public key hash: %s\n", hexa);
        ssh_string_free_char(hexa);
        ssh_clean_pubkey_hash(&hash);
        p = fgets(buf, sizeof(buf), stdin);
        if (p == NULL) {
            return -1;
        }

        cmp = strncasecmp(buf, "yes", 3);
        if (cmp != 0) {
            return -1;
        }

        rc = ssh_session_update_known_hosts(session);
        if (rc < 0) {
            fprintf(stderr, "Error %s\n", strerror(errno));
            return -1;
        }

        break;
    case SSH_KNOWN_HOSTS_ERROR:
        fprintf(stderr, "Error %s", ssh_get_error(session));
        ssh_clean_pubkey_hash(&hash);
        return -1;
    }

    ssh_clean_pubkey_hash(&hash);
    return 0;
}

int SSH::show_remote_processes( char* process)
{
    ssh_channel channel;
    int rc;
    char buffer[256];
    int nbytes;

    fprintf(stderr, "ssh_channel_new\n");
    channel = ssh_channel_new(my_ssh_session);
    if (channel == NULL)
        return SSH_ERROR;


    fprintf(stderr, "ssh_channel_open_session\n");
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return rc;
    }

    fprintf(stderr, "ssh_channel_request_exec\n");
    //rc = ssh_channel_request_exec(channel, "ps aux");
    rc = ssh_channel_request_exec(channel, process);
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }

    fprintf(stderr, "ssh_channel_read\n");
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    while (nbytes > 0)
    {
        if (write(1, buffer, nbytes) != (unsigned int)nbytes)
        {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return SSH_ERROR;
        }
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }

    if (nbytes < 0)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return SSH_ERROR;
    }

    fprintf(stderr, "ssh_channel_send_eof\n");
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    return SSH_OK;
}
int SSH::scp_receive( void)
{
    int rc;
    int size, mode;
    char* filename, * buffer;

    rc = ssh_scp_pull_request(my_read_scp);
    if (rc != SSH_SCP_REQUEST_NEWFILE)
    {
        fprintf(stderr, "Error receiving information about file: %s\n",
            ssh_get_error(my_ssh_session));
        return SSH_ERROR;
    }

    size = ssh_scp_request_get_size(my_read_scp);
    filename = strdup(ssh_scp_request_get_filename(my_read_scp));
    mode = ssh_scp_request_get_permissions(my_read_scp);
    printf("Receiving file %s, size %d, permissions 0%o\n",
        filename, size, mode);
    free(filename);

    buffer = (char *) malloc(size);
    if (buffer == NULL)
    {
        fprintf(stderr, "Memory allocation error\n");
        return SSH_ERROR;
    }

    ssh_scp_accept_request(my_read_scp);
    rc = ssh_scp_read(my_read_scp, buffer, size);
    if (rc == SSH_ERROR)
    {
        fprintf(stderr, "Error receiving file data: %s\n",
            ssh_get_error(my_ssh_session));
        free(buffer);
        return rc;
    }
    printf("Done\n");

    write(1, buffer, size);
    free(buffer);

    rc = ssh_scp_pull_request(my_read_scp);
    if (rc != SSH_SCP_REQUEST_EOF)
    {
        fprintf(stderr, "Unexpected request: %s\n",
            ssh_get_error(my_ssh_session));
        return SSH_ERROR;
    }

    return SSH_OK;
}

int SSH::scp_write_open(void)
{
    int rc;

    my_write_scp = ssh_scp_new
    (my_ssh_session, SSH_SCP_WRITE | SSH_SCP_RECURSIVE, ".");
    if (my_write_scp == NULL)
    {
        fprintf(stderr, "Error allocating scp session: %s\n",
            ssh_get_error(my_ssh_session));
        return SSH_ERROR;
    }

    rc = ssh_scp_init(my_write_scp);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error initializing scp session: %s\n",
            ssh_get_error(my_ssh_session));
        ssh_scp_free(my_write_scp);
        return rc;
    }

    return SSH_OK;
}

int SSH::scp_read_open(char* filename)
{
    int rc;

    my_read_scp = ssh_scp_new
    (my_ssh_session, SSH_SCP_READ, filename);
    if (my_read_scp == NULL)
    {
        fprintf(stderr, "Error allocating scp session: %s\n",
            ssh_get_error(my_ssh_session));
        return SSH_ERROR;
    }

    rc = ssh_scp_init(my_read_scp);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Error initializing scp session: %s\n",
            ssh_get_error(my_ssh_session));
        ssh_scp_free(my_read_scp);
        return rc;
    }
    return SSH_OK;
}


int SSH::scp_write_close(void)
{

    ssh_scp_close(my_write_scp);
    ssh_scp_free(my_write_scp);
    return SSH_OK;
}

int SSH::scp_read_close(void)
{

    ssh_scp_close(my_read_scp);
    ssh_scp_free(my_read_scp);
    return SSH_OK;
}

#define S_IRUSR 0400
#define S_IWUSR 0200

int SSH::scp_write_file(char* filename, int filelength)
{
    int rc;

    rc = scp_write_open();

    //if new directory is needed
   // rc = ssh_scp_push_directory(scp, "helloworld", S_IRWXU);
   // if (rc != SSH_OK)
   // {
   //     fprintf(stderr, "Can't create remote directory: %s\n",
    //        ssh_get_error(session));
    //    return rc;
    //}

    rc = ssh_scp_push_file
    (my_write_scp, filename, filelength, S_IRUSR | S_IWUSR);
    if (rc != SSH_OK)
    {
        fprintf(stderr, "Can't open remote file: %s\n",
            ssh_get_error(my_ssh_session));
        return rc;
    }

    FILE* infile;
    char buffer[128];
    char numstr[32];
    infile = fopen(filename, "r");

    while (!feof(infile))
    {
        fgets(buffer, 127, infile);
        fprintf(stderr, buffer);
        fprintf(stderr, "%d",strlen(buffer));

        rc = ssh_scp_write(my_write_scp, buffer, strlen(buffer));
        sleep(10);
        if (rc != SSH_OK)
        {
            fprintf(stderr, "Can't write to remote file: %s\n",
                ssh_get_error(my_ssh_session));
            return rc;
        }

    }


    fclose(infile);

    rc = scp_write_close();

    return SSH_OK;
}