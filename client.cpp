#include "client.h"
#include <bits/stdc++.h>
using namespace std;

#define PORT 8000
#define SERVER_IP "127.0.0.1"
#define MAXSZ 100

struct sockaddr_in serverAddress;

char msg[MAXSZ];
char grid[3][3];

bool turn = false;
int lin, col;

void init() {
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 3; j++) grid[i][j] = ' ';
}

bool check_win(){
  int line[3] = {0}, col[3] = {0}, dig[2] = {0};
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 3; j++){
      if(grid[i][j] != 'X') continue;
      line[i] += (1 << j), col[j] += (1 << i);
      if(i == j) dig[0] += (1 << i);
      if(i != j and i + j == 2) dig[1] += (1 << i);
    }

  for(int i = 0; i < 3; i++){
    if(line[i] == 7 or col[i] == 7) return true;
    if(i < 2 and dig[i] == 7) return true;
  }
  return false;
} 

bool check_full(){
  for(int i = 0; i < 3; i++)
    for(int j = 0; j < 3; j++) if(grid[i][j] == ' ') return false;
  return true;
}

void fill() {
 memset(&serverAddress,0,sizeof(serverAddress));
 serverAddress.sin_family=AF_INET;
 serverAddress.sin_addr.s_addr=inet_addr(SERVER_IP);
 serverAddress.sin_port=htons(PORT);
}

bool verify_input(int l, int c){
  if(l < 1 or l > 3 or c < 1 or c > 3) {
    printf("Fora do limite do grid\n");
    return false;
  }
  if(grid[l-1][c-1] == 'X' or grid[l-1][c-1] == 'O'){
    printf("Posição já ocupada\n");
    return false;
  }
  return true;
}

void play(int l, int c){
  grid[l-1][c-1] = 'O';
}

void play(string s){
  string l, c;
  int cond = 0;
  for(auto x : s){
    if(x == ' ') {
      cond = 1;
      continue;
    }
    cond ? c += x : l += x;
  }
  int line = stoi(l), col = stoi(c);
  grid[line-1][col-1] = 'X';
}

void print(){
  printf("\t %c | %c | %c\n", grid[0][0], grid[0][1], grid[0][2]);
  printf("\t-----------\n");
  printf("\t %c | %c | %c\n", grid[1][0], grid[1][1], grid[1][2]);
  printf("\t-----------\n");
  printf("\t %c | %c | %c\n", grid[2][0], grid[2][1], grid[2][2]);
}

int main() {
  int sockc = socket(AF_INET,SOCK_STREAM,0);
 
  fill();
  init(); 
  
  //memset(&serverAddress, 0, sizeof(serverAddress));
  connect(sockc, (struct sockaddr*) &serverAddress, sizeof(serverAddress));
  printf("Conectado!\n");
  printf("O jogo irá começar\n");
  
  while(1){
    printf("Vez do outro jogador\n");
    recv(sockc, msg, MAXSZ, 0);
    string aux(msg);
    play(aux);
    if(check_win() or check_full()){
      print();
      if(check_win()) printf("Você perdeu\n");
      if(check_full()) printf("Empatou\n");
      printf("Finalizando o jogo\n");
      break;
    }
    printf("Sua vez\n");
    print();
    printf("Forneça a jogada (linha, coluna)\n");
    do{
      scanf("%d%d", &lin, &col);
    } while(!verify_input(lin, col));
    play(lin, col);
    if(check_win() or check_full()){
      print();
      if(check_win())printf("Você ganhou\n");
      if(check_full()) printf("Empatou\n");
      printf("Finalizando o jogo\n");
      break;
    }

    string s = to_string(lin) + " " + to_string(col);
    strcpy(msg, s.c_str());
    send(sockc, msg, MAXSZ, 0);
  }

  close(sockc);
}
