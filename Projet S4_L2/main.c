#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "sos_defines.h"

#define MAX_TIMES_LOGIN 3
#define VIRTUAL_DISK_FILE "virtual_disk.bin"
#define LOGIN_FILE "login.bin"
#define MAX_COMMAND 15

typedef file_t file_table_t[INODE_TABLE_SIZE];

// global variables
virtual_disk_t virtual_disk_sos;
session_t session_sos;
file_table_t file_sos;

// function
// function helps to get current time in string
char *timestamp();

// init disk to start system
bool init_disk_sos(const char *);

// shutdown system
void shutdown();

// calculate number of block
uint compute_nblock(const uint);

// write a block
bool write_block(const block_t, const uint);

// read a block
bool read_block(block_t *, const uint);

// write a super block
bool write_super_block(const super_block_t, const uint);

// read a super block
bool read_super_block(super_block_t *, const uint);

// set the first free byte in disk
void first_free_byte(super_block_t *);

// read inodes table
bool read_inodes_table(inode_table_t *, const uint);

// write inodes table
bool write_inodes_table(const inode_table_t, const uint);

// delete an inode by position
bool delete_inode(uint);

// get index of unused inode
uint get_unused_inode();

// init a new inode
inode_t init_inode(const char *, const uint, const uint);

// cmd_dump_inode
void cmd_dump_inode();

// write file object to a file
int write_file(const char *, const file_t);

// read file object from a file
int read_file(const char *, file_t *);
 
// delete a file from system
int delete_file(const char *);

// load a file from host
file_t load_file_from_host(const char *);

// save a file to host
bool store_file_to_host(const char *);

// login
bool login();

// init all command we can use
cmd_t init_command();

// split the input string to command and variable
cmd_t split_command(const char *);

// get permission of a file by uright
char *get_permission(const int);

// find a file is existed or not in systeams
int find_file(const char *);

// find a user is existed or not in systeams
int find_user(const char *);

// find a command is existed or not in list command
int find_command(const char *, const cmd_t);

void ls_command(const bool);
void cat_command(const char *);
void rm_command(const char *);
void cr_command(const char *);
void edit_command(const char *);
void load_command(const char *);
void store_command(const char *);
void chown_command(const char *, const char *);
void chmod_command(const char *, const int);
void listusers_command();
void quit_command();
void adduser_command(const char *, const char *);
void rmuser_command(const char *);

// main method to run project
void run();

// if this is the first time to run, user must login with root role
void first_time_login();

// show all list of command
void help();

// split the string by delimiter
char *strtoke(char *, char *);

int main(int argc, char **argv) {
  run();
  return 0;
}

char *timestamp() {
  time_t current_time;
  char *c_time_string;

  /* Obtain current time. */
  current_time = time(NULL);

  if (current_time == ((time_t)-1)) {
    (void)fprintf(stderr, "Failure to obtain the current time.\n");
    exit(EXIT_FAILURE);
  }

  /* Convert to local time format. */
  c_time_string = ctime(&current_time);

  if (c_time_string == NULL) {
    (void)fprintf(stderr, "Failure to convert the current time.\n");
    exit(EXIT_FAILURE);
  }
  c_time_string[strlen(c_time_string) - 1] = '\0';
  return c_time_string;
}

// function helps to init disk
bool init_disk_sos(const char *filename) {
  virtual_disk_sos.super_block.nb_blocks_used = 0;
  virtual_disk_sos.super_block.first_free_byte = 0;
  virtual_disk_sos.super_block.number_of_files = 0;
  virtual_disk_sos.super_block.number_of_users = 0;

  int i = 0;
  for (i = 0; i < INODE_TABLE_SIZE; ++i) {
    virtual_disk_sos.inodes[i] = init_inode("", 0, 0);
  }

  if (virtual_disk_sos.storage != NULL) {
    fclose(virtual_disk_sos.storage);
  }

  virtual_disk_sos.storage = fopen(filename, "wb+");

  if (virtual_disk_sos.storage == NULL) {
    printf("Can not open file: %s\n", filename);
    return false;
  }

  return true;
}

void shutdown() {
  if (virtual_disk_sos.storage != NULL) {
    fclose(virtual_disk_sos.storage);
  }

  FILE *stream = fopen(VIRTUAL_DISK_FILE, "wb+");
  fwrite(&virtual_disk_sos, sizeof(virtual_disk_t), 1, stream);
  fclose(stream);
  stream = fopen(LOGIN_FILE, "wb+");
  fwrite(&virtual_disk_sos.users_table, sizeof(users_table_t), 1, stream);
  fclose(stream);
}

uint compute_nblock(const uint nbOctets) {
  return (nbOctets + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

bool write_block(const block_t block, const uint pos) {
  if (fseek(virtual_disk_sos.storage, pos, SEEK_SET) == 0) {
    fwrite(&block, sizeof(block_t), 1, virtual_disk_sos.storage);
    rewind(virtual_disk_sos.storage);
    return true;
  }

  return false;
}

bool read_block(block_t *block, const uint pos) {
  if (fseek(virtual_disk_sos.storage, pos, SEEK_SET) == 0) {
    block = malloc(sizeof(block_t));

    fread(block, sizeof(block_t), 1, virtual_disk_sos.storage);
    rewind(virtual_disk_sos.storage);
    return true;
  }

  return false;
}

bool write_super_block(const super_block_t super_block, const uint pos) {
  if (fseek(virtual_disk_sos.storage, pos, SEEK_SET) == 0) {
    fwrite(&super_block, sizeof(super_block), 1, virtual_disk_sos.storage);
    rewind(virtual_disk_sos.storage);
    return true;
  }

  return false;
}

bool read_super_block(super_block_t *super_block, const uint pos) {
  if (fseek(virtual_disk_sos.storage, pos, SEEK_SET) == 0) {
    super_block = malloc(sizeof(super_block_t));

    fread(super_block, sizeof(super_block_t), 1, virtual_disk_sos.storage);
    rewind(virtual_disk_sos.storage);
    return true;
  }

  return false;
}

void first_free_byte(super_block_t *super_block) {
  super_block->first_free_byte = 0;
}

bool read_inodes_table(inode_table_t *inode_table, const uint pos) {
  if (fseek(virtual_disk_sos.storage, pos, SEEK_SET) == 0) {
    inode_table = malloc(sizeof(inode_table_t));

    fread(inode_table, sizeof(inode_table_t), 1, virtual_disk_sos.storage);
    rewind(virtual_disk_sos.storage);
    return true;
  }

  return false;
}

bool write_inodes_table(const inode_table_t inode_table, const uint pos) {
  if (fseek(virtual_disk_sos.storage, pos, SEEK_SET) == 0) {
    fwrite(&inode_table, sizeof(inode_table_t), 1, virtual_disk_sos.storage);
    rewind(virtual_disk_sos.storage);
    return true;
  }

  return false;
}

bool delete_inode(uint pos) {
  if (pos < 0 || pos >= INODE_TABLE_SIZE) {
    return false;
  }

  // if first byte >= 0, means it was allocated
  // otherwise, it  not allocated
  virtual_disk_sos.inodes[pos].first_byte = -1;
  return true;
}

uint get_unused_inode() {
  uint i;
  for (i = 0; i < INODE_TABLE_SIZE; ++i) {
    if (virtual_disk_sos.inodes[i].first_byte == -1) {
      return i;
    }
  }

  return -1;
}

inode_t init_inode(const char *filename, const uint size, const uint nblock) {
  inode_t inode;

  strcpy(inode.filename, filename);
  inode.size = size;
  strcpy(inode.ctimestamp, timestamp());
  strcpy(inode.mtimestamp, inode.ctimestamp);
  inode.nblock = nblock;
  inode.first_byte = -1;
  inode.uid = session_sos.userid;
  inode.uright = rw;
  inode.oright = rw;

  return inode;
}

// return size of file
int write_file(const char *fileName, const file_t file) {
  FILE *stream = fopen(fileName, "w+");
  if (stream == NULL) {
    printf("Error: Can not edit file %s\n", fileName);
    return -1;
  } else {
    fwrite(&file, sizeof(file_t), 1, stream);
    fseek(stream, 0, SEEK_END);
    int res = ftell(stream);
    fclose(stream);
    return res;
  }
}

int read_file(const char *fileName, file_t *file) {
  file->size = 0;

  FILE *stream = fopen(fileName, "r");
  if (stream == NULL) {
    printf("Error: Unable to read file %s\n", fileName);
    return 0;
  } else {
    char c;
    while ((c = getc(stream)) != EOF) {
      file->data[file->size++] = c;
      if (file->size == MAX_FILE_SIZE) {
        break;
      }
    }
  }
  fclose(stream);
  return 1;
}

int delete_file(const char *fileName) {
  int index = find_file(fileName);
  if (index == -1) {
    printf("File: %s not exist!\n", fileName);
    return 0;
  }

  // root have role to do anything
  if (session_sos.userid == ROOT_UID) {
    // remove from inodes
    virtual_disk_sos.inodes[index].first_byte = -1;
    virtual_disk_sos.super_block.number_of_files--;
    
    return 1;
  }

  // other user
  // check permission
  if (get_permission(virtual_disk_sos.inodes[index].uright)[1] == 'W') {
    // remove from inodes
    virtual_disk_sos.inodes[index].first_byte = -1;
    virtual_disk_sos.super_block.number_of_files--;
    return 1;
  } else {
    printf("You do not have permission to delete %s!\n", fileName);
    return 0;
  }
}

file_t load_file_from_host(const char *fileName) {
  file_t file;
  file.size = 0;

  if (read_file(fileName, &file) == 1) {
    return file;
  }

  file.size = -1;
  return file;
}

bool store_file_to_host(const char *fileName) {
  int index = find_file(fileName);
  if (index == -1) {
    printf("File: %s not exist!\n", fileName);
    return false;
  }

  file_sos[index].size = strlen((char *)file_sos[index].data);
  if (write_file(fileName, file_sos[index]) != -1) {
    return true;
  }

  return false;
}

bool login() {
  printf("Login:\n");
  int i, j;
  char userName[FILENAME_MAX_SIZE];
  char passWord[SHA256_BLOCK_SIZE * 2 + 1];
  for (i = 0; i < MAX_TIMES_LOGIN; ++i) {
    printf("Enter username: ");
    scanf("%s", userName);
    printf("Enter password: ");
    scanf("%s", passWord);
    sha256ofString((BYTE *)passWord, passWord);
    for (j = 0; j < NB_USERS; ++j) {
      if (strcmp(userName, virtual_disk_sos.users_table[j].login) == 0 &&
          strcmp(passWord, virtual_disk_sos.users_table[j].passwd) == 0) {
        session_sos.userid = j;
        return true;
      }
    }
  }
  return false;
}

cmd_t init_command() {
  cmd_t *cmd = malloc(sizeof(cmd_t));

  cmd->nbArgs = MAX_COMMAND;
  cmd->tabArgs = malloc(sizeof(char *) * cmd->nbArgs);
  int i;
  for (i = 0; i < cmd->nbArgs; ++i) {
    cmd->tabArgs[i] = malloc(sizeof(char) * MAX_MSG);
  }

  // init command line
  strcpy(cmd->tabArgs[0], "ls");
  strcpy(cmd->tabArgs[1], "ls -l");
  strcpy(cmd->tabArgs[2], "cat");
  strcpy(cmd->tabArgs[3], "rm");
  strcpy(cmd->tabArgs[4], "cr");
  strcpy(cmd->tabArgs[5], "edit");
  strcpy(cmd->tabArgs[6], "load");
  strcpy(cmd->tabArgs[7], "store");
  strcpy(cmd->tabArgs[8], "chown");
  strcpy(cmd->tabArgs[9], "chmod");
  strcpy(cmd->tabArgs[10], "listusers");
  strcpy(cmd->tabArgs[11], "quit");
  strcpy(cmd->tabArgs[12], "adduser");
  strcpy(cmd->tabArgs[13], "rmuser");
  strcpy(cmd->tabArgs[14], "help");

  return *cmd;
}

cmd_t split_command(const char *command) {
  cmd_t *cmd = malloc(sizeof(cmd_t));
  cmd->nbArgs = 0;
  cmd->tabArgs = malloc(sizeof(char *) * FILENAME_MAX_SIZE); // max 32 arguments
  int i;
  for (i = 0; i < FILENAME_MAX_SIZE; ++i) {
    cmd->tabArgs[i] = malloc(sizeof(char) * MAX_MSG);
  }
  char input[FILENAME_MAX_SIZE];
  strcpy(input, command);
  const char *tok;
  char *delim = " ";
  tok = strtoke(input, delim);
  while (tok != NULL) {
    strcpy(cmd->tabArgs[cmd->nbArgs++], tok);
    if (cmd->nbArgs == FILENAME_MAX_SIZE) {
      break;
    }
    tok = strtoke(NULL, delim);
  }

  return *cmd;
}

char *get_permission(const int permission) {
  if (permission == rw) {
    return "rw";
  } else if (permission == rW) {
    return "rW";
  } else if (permission == Rw) {
    return "Rw";
  } else {
    return "RW";
  }
}

void ls_command(const bool containOption) {
  int i;
  if (containOption) {
    printf("Total: %d\n", virtual_disk_sos.super_block.number_of_files);
    printf("%3s %5s %20s %26s %26s %32s\n", "   ", "Size", "User",
           "Created time", "Modified time", "File name");
    for (i = 0; i < INODE_TABLE_SIZE; ++i) {
      if (virtual_disk_sos.inodes[i].first_byte != -1) {
        printf(
            "%3s %5d %20s %26s %26s %32s\n",
            get_permission(virtual_disk_sos.inodes[i].uright),
            virtual_disk_sos.inodes[i].size,
            virtual_disk_sos.users_table[virtual_disk_sos.inodes[i].uid].login,
            virtual_disk_sos.inodes[i].ctimestamp,
            virtual_disk_sos.inodes[i].mtimestamp,
            virtual_disk_sos.inodes[i].filename);
      }
    }
  } else {
    printf("Total: %d\n", virtual_disk_sos.super_block.number_of_files);
    printf("File name\n");
    for (i = 0; i < INODE_TABLE_SIZE; ++i) {
      if (virtual_disk_sos.inodes[i].first_byte != -1) {
        printf("%s\n", virtual_disk_sos.inodes[i].filename);
      }
    }
  }
}

int find_file(const char *fileName) {
  int i;
  for (i = 0; i < INODE_TABLE_SIZE; ++i) {
    if (virtual_disk_sos.inodes[i].first_byte != -1 &&
        strcmp(virtual_disk_sos.inodes[i].filename, fileName) == 0) {
      return i;
    }
  }

  return -1;
}

void cat_command(const char *fileName) {
  file_t file;

  int index = find_file(fileName);
  if (index == -1) {
    printf("File: %s not exist!\n", fileName);
    return;
  }

  // root have role to do anything
  if (session_sos.userid == ROOT_UID) {
    // if (read_file(fileName, &file) == 1) {
    file = file_sos[index];
    int i;
    for (i = 0; i < file.size; ++i) {
      printf("%c", file.data[i]);
    }
    printf("\n");
    // }
    return;
  }

  // other user
  // check permission
  if (get_permission(virtual_disk_sos.inodes[index].uright)[1] == 'R') {
    file = file_sos[index];
    int i;
    for (i = 0; i < file.size; ++i) {
      printf("%c", file.data[i]);
    }
    printf("\n");
  } else {
    printf("You do not have permission to read %s!\n", fileName);
    return;
  }
}

void rm_command(const char *fileName) {
  if (delete_file(fileName) == 1) {
    printf("Delete file %s successfully!\n", fileName);
  }
}

void quit_command() {
  shutdown();
  exit(0);
}

void cr_command(const char *fileName) {
  int index = get_unused_inode();
  if (index == -1) {
    printf("Disk is full!\n");
  } else {
    virtual_disk_sos.inodes[index] = init_inode(fileName, 0, 0);
    printf("Create file %s successfully!\n", fileName);
    virtual_disk_sos.inodes[index].first_byte = index;
    virtual_disk_sos.super_block.number_of_files++;
  }
}

void edit_command(const char *fileName) {
  int index = find_file(fileName);
  if (index == -1) {
    printf("Error: File %s not exist!\n", fileName);
    return;
  } else {
    // root have role to do anything
    if (session_sos.userid == ROOT_UID) {
      char data[MAX_FILE_SIZE];
      printf("Enter new data: ");
      fflush(stdin);
      scanf("%[^\n]s", data);
      file_t file;
      file.size = strlen((char *)data);
      strcpy((char *)file.data, data);
      printf("Edit file %s successfully!\n", fileName);
      virtual_disk_sos.inodes[index].size = strlen(data);
      strcpy(virtual_disk_sos.inodes[index].mtimestamp, timestamp());
      file_sos[index] = file;
      return;
    }

    // other user
    if (get_permission(virtual_disk_sos.inodes[index].uright)[1] == 'W') {
      char data[MAX_FILE_SIZE];
      printf("Enter new data: ");
      fflush(stdin);
      scanf("%[^\n]s", data);
      file_t file;
      file.size = strlen(data);
      strcpy((char *)file.data, data);
      printf("Edit file %s successfully!\n", fileName);
      virtual_disk_sos.inodes[index].size = strlen(data);
      strcpy(virtual_disk_sos.inodes[index].mtimestamp, timestamp());
      file_sos[index] = file;
    } else {
      printf("You do not have permission to edit %s!\n", fileName);
      return;
    }
  }
}

void load_command(const char *fileName) {
  file_t file = load_file_from_host(fileName);
  if (file.size != -1) {
    int index = get_unused_inode();
    if (index == -1) {
      printf("Error: Disk is full!\n");
      return;
    }
    virtual_disk_sos.inodes[index] = init_inode(fileName, file.size, 0);
    virtual_disk_sos.inodes[index].first_byte = file.size;
    file_sos[index] = file;
    printf("Load file %s successfully\n", fileName);
  }
}

void store_command(const char *fileName) {
  if (store_file_to_host(fileName)) {
    printf("Store file %s successfully\n", fileName);
  }
}

int find_user(const char *userName) {
  int i;
  for (i = 0; i < NB_USERS; ++i) {
    if (strcmp(userName, virtual_disk_sos.users_table[i].login) == 0) {
      return i;
    }
  }

  return -1;
}

void chown_command(const char *fileName, const char *userName) {
  int indexFile = find_file(fileName);
  if (indexFile == -1) {
    printf("Error: file %s not exists!\n", fileName);
    return;
  }

  int indexUser = find_user(userName);
  if (indexUser == -1) {
    printf("Error: user %s not exists!\n", userName);
    return;
  }

  // root have role to do anything
  if (session_sos.userid == ROOT_UID) {
    virtual_disk_sos.inodes[indexFile].uid = indexUser;
    strcpy(virtual_disk_sos.inodes[indexFile].mtimestamp, timestamp());
    printf("Change user of file %s successfully!\n", fileName);
    return;
  }

  // other user
  if (get_permission(virtual_disk_sos.inodes[indexFile].uright)[1] == 'W') {
    virtual_disk_sos.inodes[indexFile].uid = indexUser;
    strcpy(virtual_disk_sos.inodes[indexFile].mtimestamp, timestamp());
    printf("Change user of file %s successfully!\n", fileName);
  } else {
    printf("You do not have permission to edit %s!\n", fileName);
    return;
  }
}

int find_command(const char *command, const cmd_t cmd) {
  int i;
  for (i = 0; i < cmd.nbArgs; ++i) {
    if (strcmp(cmd.tabArgs[i], command) == 0) {
      return i;
    }
  }

  return -1;
}

void chmod_command(const char *fileName, const int role) {
  int indexFile = find_file(fileName);
  if (indexFile == -1) {
    printf("Error: file %s not exists!\n", fileName);
    return;
  }

  if (role < rw || role > RW) {
    printf("Error: role %d not exists!\n", role);
    return;
  }

  // root have role to do anything
  if (session_sos.userid == ROOT_UID) {
    virtual_disk_sos.inodes[indexFile].uright = role;
    strcpy(virtual_disk_sos.inodes[indexFile].mtimestamp, timestamp());
    printf("Change role of file %s successfully!\n", fileName);
    return;
  }

  // other user
  if (get_permission(virtual_disk_sos.inodes[indexFile].uright)[1] == 'W') {
    virtual_disk_sos.inodes[indexFile].uright = role;
    strcpy(virtual_disk_sos.inodes[indexFile].mtimestamp, timestamp());
    printf("Change role of file %s successfully!\n", fileName);
  } else {
    printf("You do not have permission to edit %s!\n", fileName);
  }
}

void listusers_command() {
  int i;
  printf("User\n");
  for (i = 0; i < virtual_disk_sos.super_block.number_of_users; ++i) {
    printf("%s\n", virtual_disk_sos.users_table[i].login);
  }
}

void adduser_command(const char *userName, const char *passWord) {
  // check root right
  if (session_sos.userid == ROOT_UID) {
    int indexUser = find_user(userName);
    if (indexUser != -1) {
      printf("Error: user %s exists!\n", userName);
      return;
    }

    if (virtual_disk_sos.super_block.number_of_users >= NB_USERS) {
      printf("Error: list user full!\n");
      return;
    }

    user_t user;
    strcpy(user.login, userName);
    char hashRes[SHA256_BLOCK_SIZE * 2 + 1];
    sha256ofString((BYTE *)passWord, hashRes);
    strcpy(user.passwd, hashRes);
    virtual_disk_sos
        .users_table[virtual_disk_sos.super_block.number_of_users++] = user;
    printf("Add user successfully!\n");
  } else {
    printf("You do not have permission to add user!\n");
    return;
  }
}

void rmuser_command(const char *userName) {
  // check root right
  if (session_sos.userid == ROOT_UID) {
    int indexUser = find_user(userName);
    if (indexUser == -1) {
      printf("Error: user %s not exists!\n", userName);
      return;
    } else if (indexUser == ROOT_UID) {
      printf("Error: can not delete root account!\n");
      return;
    }

    int i;
    for (i = indexUser; i < virtual_disk_sos.super_block.number_of_users - 1;
         ++i) {
      virtual_disk_sos.users_table[i] = virtual_disk_sos.users_table[i + 1];
    }
    for (i = virtual_disk_sos.super_block.number_of_users - 1; i < NB_USERS;
         ++i) {
      strcpy(virtual_disk_sos.users_table[i].login, "");
    }
    virtual_disk_sos.super_block.number_of_users--;
    printf("Remove user %s successfully!\n", userName);
  } else {
    printf("You do not have permission to remove user!\n");
    return;
  }
}

void help() {
  printf("Command line: \n");
  printf("ls: list file\n");
  printf("ls -l: list detail file\n");
  printf("cat <file>: view file\n");
  printf("rm <file>: delete file\n");
  printf("cr <file>: create file\n");
  printf("edit <file>: edit file\n");
  printf("load <file>: load file from host\n");
  printf("store <file>: store file to host\n");
  printf("chown <file> <user>: change author\n");
  printf("chmod <file> <mod>: change mod\n");
  printf("listusers: list users\n");
  printf("quit: exit\n");
  printf("adduser <user name> <password>: add user\n");
  printf("rmuser <user name>: delete user\n");
  printf("help: print info\n");
}

void run() {
  if (!init_disk_sos(VIRTUAL_DISK_FILE)) {
    printf("Error: Can not run systems!\n");
    return;
  }

  FILE *login_stream = fopen(LOGIN_FILE, "r");
  if (login_stream == NULL) {
    printf("This is the first time you use our system. You must create an "
           "account to continue:\n");
    first_time_login();
    login_stream = fopen(LOGIN_FILE, "wb+");
    fwrite(&virtual_disk_sos.users_table, sizeof(users_table_t), 1,
           login_stream);
    fclose(login_stream);
  } else {
    fread(&virtual_disk_sos.users_table, sizeof(users_table_t), 1,
          login_stream);
    virtual_disk_sos.super_block.number_of_users++;
    fclose(login_stream);
  }
  if (!login()) {
    printf("Failed to login!\n");
    return;
  }

  // run the program forever
  char command[MAX_MSG];
  cmd_t cmd, split_cmd;
  int index;

  cmd = init_command();

  while (true) {
    fflush(stdin);
    printf("%s: ", virtual_disk_sos.users_table[session_sos.userid].login);
    scanf("%[^\n]s", command);
    split_cmd = split_command(command);

    // check case ls and ls -l
    index = find_command(command, cmd);
    if (index == 0) { // case ls
      ls_command(false);
    } else if (index == 1) { // case ls -l
      ls_command(true);
    } else {
      index = find_command(split_cmd.tabArgs[0], cmd);
      switch (index) {
      case 2:
        if (split_cmd.nbArgs != 2) {
          printf("Error: wrong format!. 'help' for more infomation!\n");
        } else {
          cat_command(split_cmd.tabArgs[1]);
        }
        break;
      case 3:
        if (split_cmd.nbArgs != 2) {
          printf("Error: wrong format!. 'help' for more infomation!\n");
        } else {
          rm_command(split_cmd.tabArgs[1]);
        }
        break;
      case 4:
        if (split_cmd.nbArgs != 2) {
          printf("Error: wrong format!. 'help' for more infomation!\n");
        } else {
          cr_command(split_cmd.tabArgs[1]);
        }
        break;
      case 5:
        if (split_cmd.nbArgs != 2) {
          printf("Error: wrong format!. 'help' for more infomation!\n");
        } else {
          edit_command(split_cmd.tabArgs[1]);
        }
        break;
      case 6:
        if (split_cmd.nbArgs != 2) {
          printf("Error: wrong format!. 'help' for more infomation!\n");
        } else {
          load_command(split_cmd.tabArgs[1]);
        }
        break;
      case 7:
        if (split_cmd.nbArgs != 2) {
          printf("Error: wrong format!. 'help' for more infomation!\n");
        } else {
          store_command(split_cmd.tabArgs[1]);
        }
        break;
      case 8:
        if (split_cmd.nbArgs != 3) {
          printf("Error: wrong format!. 'help' for more infomation!\n");
        } else {
          chown_command(split_cmd.tabArgs[1], split_cmd.tabArgs[2]);
        }
        break;
      case 9:
        if (split_cmd.nbArgs != 3) {
          printf("Error: wrong format!. 'help' for more infomation!\n");
        } else {
          chmod_command(split_cmd.tabArgs[1], atoi(split_cmd.tabArgs[2]));
        }
        break;
      case 10:
        if (split_cmd.nbArgs != 1) {
          printf("Error: wrong format!. 'help' for more infomation!\n");
        } else {
          listusers_command();
        }
        break;
      case 12:
        if (split_cmd.nbArgs != 3) {
          printf("Error: wrong format!. 'help' for more infomation!\n");
        } else {
          adduser_command(split_cmd.tabArgs[1], split_cmd.tabArgs[2]);
        }
        break;
      case 13:
        if (split_cmd.nbArgs != 2) {
          printf("Error: wrong format!. 'help' for more infomation!\n");
        } else {
          rmuser_command(split_cmd.tabArgs[1]);
        }
        break;
      case 11:
        printf("Good bye!\n");
        quit_command();
      case 14:
        help();
        break;
      default:
        printf("Error: Invalid command!. 'help' for more infomation!\n");
        break;
      }
    }
  }
}

void first_time_login() {
  user_t user;
  char passwd[SHA256_BLOCK_SIZE * 2 + 1];

  printf("Enter root username: ");
  scanf("%s", user.login);
  printf("Enter root password: ");
  scanf("%s", user.passwd);
  sha256ofString((BYTE *)user.passwd, passwd);
  strcpy(user.passwd, passwd);

  // add to root
  virtual_disk_sos.users_table[ROOT_UID] = user;
  virtual_disk_sos.super_block.number_of_users++;
}


char *strtoke(char *str, char *delim) {
  static char *start = NULL; 
  char *token = NULL;        
  if (str)
    start = str;
  if (start == NULL)
    return NULL;
  token = start;
  start = strpbrk(start, " ");
  if (start)
    *start++ = '\0';
  return token;
}