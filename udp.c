/**
 * @file udp.c
 * @author Matej Keznikl (xkezni01@stud.fit.vutbr.cz)
 * @brief Funkcie pre UDP komunikáciu klienta pre IPK Calculator Protokol
 * @date 2023-03-21
 */

#include "udp.h"

void udp(struct Arguments *args)
{
	int client_socket, port_number, bytestx, bytesrx;
	socklen_t serverlen;
	const char *server_hostname;
	struct hostent *server;
	struct sockaddr_in server_address;
	char buf[BUFSIZE];
	char help_buf[BUFSIZE];
	char temp[BUFSIZE];
	int buffer_length = 0;

	server_hostname = args->host;
	char *text;
	port_number = strtol(args->port, &text, 10);

	/* 2. ziskani adresy serveru pomoci DNS */

	if ((server = gethostbyname(server_hostname)) == NULL)
	{
		fprintf(stderr, "ERROR: no such host as %s\n", server_hostname);
		exit(EXIT_FAILURE);
	}

	/* 3. nalezeni IP adresy serveru a inicializace struktury server_address */
	null_memory((char *)&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	memcpy((char *)&server_address.sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);
	server_address.sin_port = htons(port_number);

	/* tiskne informace o vzdalenem soketu */
	printf("INFO: Server socket: %s : %d \n", inet_ntoa(server_address.sin_addr), ntohs(server_address.sin_port));

	/* Vytvoreni soketu */
	if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
	{
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}

	while (true)
	{
		/* nacteni zpravy od uzivatele */
		null_memory(buf, BUFSIZE);
		null_memory(help_buf, BUFSIZE);
		null_memory(temp, BUFSIZE);

		if (!fgets(temp, BUFSIZE, stdin) || !strchr(temp, '\n'))
		{
			error_exit("Zadaný vstupný text sa nepodarilo načítať");
		}

		strcpy(buf, "0");
		sprintf(help_buf, "%ld", strlen(temp) - 1);
		buffer_length = strlen(temp) - 1 + 2;
		strcat(buf, help_buf);
		strcat(buf, temp);
		buf[0] = '\0';
		buf[strcspn(buf, "\n")] = 0;

		/* odeslani zpravy na server */
		serverlen = sizeof(server_address);
		bytestx = sendto(client_socket, buf, buffer_length, 0, (struct sockaddr *)&server_address, serverlen);
		if (bytestx < 0)
		{
			perror("ERROR: sendto");
		}

		null_memory(buf, BUFSIZE);
		null_memory(help_buf, BUFSIZE);
		null_memory(temp, BUFSIZE);

		/* prijeti odpovedi a jeji vypsani */
		bytesrx = recvfrom(client_socket, buf, BUFSIZE, 0, (struct sockaddr *)&server_address, &serverlen);
		if (bytesrx < 0)
		{
			perror("ERROR: recvfrom");
		}
		if (buf[OPCODE] == 1)
		{
			if (buf[STATUS] == OK)
			{
				strncpy(help_buf, buf + 2, sizeof(help_buf) - 2);
				printf("OK:%s\n", help_buf);
			}
			else if (buf[STATUS] == ERROR)
			{
				strncpy(help_buf, buf + 2, sizeof(help_buf) - 2);
				printf("ERR:%s\n", help_buf);
				exit(ERROR);
			}
			else
			{
				error_exit("Neočakávaný response kód\n");
			}
		}
	}
}
