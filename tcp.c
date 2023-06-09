/**
 * @file tcp.c
 * @author Matej Keznikl (xkezni01@stud.fit.vutbr.cz)
 * @brief Funkcie pre TCP komunikáciu klienta pre IPK Calculator Protokol
 * @date 2023-03-21
 */

#include "tcp.h"

/* Globálne premenné z dôvodu potreby zachytenia CTRL+C (SIGINT) */
char buf[BUFSIZE] = "init";
int client_socket, bytestx, bytesrx;

/**
 * @brief Vynuluje pamäť zadanú ako argument funkcie o zadanej veľkosti
 *
 * @param memory Pamäť na vynulovanie
 * @param memory_size Veľkosť pamäti na vynulovanie
 */
void null_memory(char *memory, int memory_size)
{
	memset(memory, '\0', memory_size);
}

/**
 * @brief V prípade ukončenia programu pomocou signálu SIGINT ukončí komunikáciu so serverom
 *
 */
void catch_sigint()
{
	null_memory(buf, BUFSIZE);
	strcpy(buf, "BYE\n");
	bytestx = send(client_socket, buf, strlen(buf), 0);
	if (bytestx < 0)
	{
		perror("ERROR in send");
	}

	null_memory(buf, BUFSIZE);
	bytesrx = recv(client_socket, buf, BUFSIZE, 0);

	if (bytesrx < 0)
	{
		perror("ERROR in recv");
	}
	printf("%s", buf);
	fflush(stdout);

	close(client_socket);

	exit(SIGINT);
}

/**
 * @brief Vykoná TCP komunikáciu pre zadané argumenty programu
 *
 * @param args Skontrolované argumenty programu
 */
void tcp(struct Arguments *args)
{
	int port_number;
	const char *server_hostname;
	struct hostent *server;
	struct sockaddr_in server_address;

	/* Zachytenie CTRL+C (SIGINT) */
	signal(SIGINT, catch_sigint);

	/* Načítanie spracovaného vstupu */
	server_hostname = args->host;
	char *text;
	port_number = strtol(args->port, &text, 10);

	null_memory((char *)&server_address, sizeof(server_address));

	if ((server = gethostbyname(server_hostname)) == NULL)
	{
		error_exit("Zadaný host neexistuje %s", server_hostname);
	}

	null_memory((char *)&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	memcpy((char *)&server_address.sin_addr.s_addr, (char *)server->h_addr_list[0], server->h_length);
	server_address.sin_port = htons(port_number);

	/* Vytvorenie socketu */
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
	{
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}

	/* Nadviazanie spojenia so serverom */
	if (connect(client_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0)
	{
		perror("ERROR: connect");
		exit(EXIT_FAILURE);
	}

	/* Komunikácia so serverom */
	while (true)
	{
		if (!strcmp(buf, "BYE\n"))
		{
			break;
		}

		/* Načítanie vstupu od užívateľa */
		if (!fgets(buf, BUFSIZE, stdin) || !strchr(buf, '\n'))
		{
			/* Ošetrenie v prípade, že riadok zo vstupu je dlhší ako veľkost bufferu */
			null_memory(buf, BUFSIZE);
			strcpy(buf, "BYE\n");
			printf("%s", buf);
			bytestx = send(client_socket, buf, strlen(buf), 0);
			if (bytestx < 0)
			{
				perror("ERROR: send");
			}

			null_memory(buf, BUFSIZE);
			bytesrx = recv(client_socket, buf, BUFSIZE, 0);

			if (bytesrx < 0)
			{
				perror("ERROR: recv");
			}
			printf("%s", buf);

			close(client_socket);
			error_exit("Zadaný vstupný text sa nepodarilo načítať");
		}

		/* Odoslanie správy od užívateľa na server */
		bytestx = send(client_socket, buf, strlen(buf), 0);
		if (bytestx < 0)
		{
			perror("ERROR: send");
		}

		null_memory(buf, BUFSIZE);

		/* Prijatie správy od servera */
		bytesrx = recv(client_socket, buf, BUFSIZE, 0);

		if (bytesrx < 0)
		{
			perror("ERROR: recv");
		}
		printf("%s", buf);

		/* V prípade obdržania správy "BYE\n" od servera ukončenie komunikácie */
		if (!strcmp(buf, "BYE\n"))
		{
			break;
		}

		null_memory(buf, BUFSIZE);
	}
	/* Ukončenie spojenia */
	close(client_socket);
}