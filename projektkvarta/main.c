/*copy*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x4
#define ARROW_UP    256 + 72
#define ARROW_DOWN  256 + 80
#define ARROW_LEFT  256 + 75
#define ARROW_RIGHT 256 + 77


typedef struct{ //Op
    char name[100];
    int speed;
    bool post;
    bool removed;
}Op;

void spacing(){
    printf("\n  ");
}

char * booltopost(int z){
    if(z==1)
    return "attacker";
    return "defender";
}

int getcharrow(){
    int ch = getch();
    if ( ch == 0 || ch == 224 )
        ch = 256 + getch();
    return ch;
}

int ytoreal(Op ops[],int y){
    int n=0;
    for(int i=0;i<y;i++){
        if(ops[i].removed==false)
            n++;
    }
    return n;
}

int menu(char chcs[][100],int cc,bool vertical,int y){/*choices count*/
    int x=0,ch;
    while(true){
        if(vertical==true){ system("cls");printf("\n");}
        else printf("\e[%d;3H",y+2);
        printf("\e[?25l");
        for(int i=0;i<cc;i++){
            if((i==y&&vertical==true)||(vertical==false&&i==x)){
                if(vertical==true)printf("  \e[7m%s\e[m\n",chcs[i]);
                else printf("\e[7m%s \e[m",chcs[i]);
            }
            else{
                if(vertical==true)printf("  %s\n",chcs[i]);
                else printf("%s ",chcs[i]);
            }
        }
        ch = getch();
        if ( ch == 0 || ch == 224 )
            ch = 256 + getch();
        switch(ch){
            case ARROW_UP:if(y-1>=0&&vertical==true)y--;break;
            case ARROW_DOWN:if(y+1<cc&&vertical==true)y++;break;
            case ARROW_LEFT:if(x-1>=0&&vertical==false)x--;break;
            case ARROW_RIGHT:if(x+1<cc&&vertical==false)x++;break;
            case 13:if(vertical==true)return y;return x;
            case 27:/*if(vertical==false)*/return -2;
            default:break;
        }
    }
}

int mainMenu(bool a){
    char chcs[2][10][100]={ {{"Sign as administrator"},{"Print whole DB"},{"Search in DB"},{"End"}},
                            {{"Change password"},{"Print whole DB"},{"Search in DB"},{"Add element"},{"Edit element"},{"Remove element"},{"Switch to user"}}};
    if(a==0)
        return (menu(chcs[a],4,true,0)+1)%4;
    return (menu(chcs[a],7,true,0)+1)%7;
}

void scanPasswd(char passwd[]){
    printf("\e[30m");
    scanf("%s",passwd);
    printf("\e[m");
}

void createpassword(){
    char passwd[100];
    FILE * out = fopen("passwd.txt","w+");
    scanPasswd(passwd);
    fprintf(out,"%s",passwd);
    fclose(out);
}

bool signin(char inserpasswd[]){
    char passwd[100];
    char cpasswd[100];
    FILE * in = fopen("passwd.txt","r");
    system("cls");
    spacing();
    if(fscanf(in,"%s",cpasswd)!=1){
        fclose(in);
        printf("Create password\n");
        printf("  Insert password:");
        createpassword();
        return true;
    }
    else{
        fclose(in);
        printf("%s",inserpasswd);
        scanPasswd(passwd);
        if(strcmp(passwd,cpasswd)==0)
            return true;
        printf("  The password is incorrect\n");
        spacing();
        system("pause");
        return false;
    }
}

void changepasswd(){

    if(signin("Insert old password:")==true){
        spacing();
        printf("Type in new password:");
        createpassword();
        spacing();
        printf("Password successfully changed");
        spacing();
        system("pause");
    }
}

void printall(Op ops[], int n){
    system("cls");
    spacing();
    printf("%-15s %-10s %-10s %-10s\n\n","Name","Speed","Health","Post");
        for(int i=0;i<n;i++){
        printf("  %-15s %-10d %-10d %-10s\n",ops[i].name,ops[i].speed,4-ops[i].speed,booltopost(ops[i].post));
    }
    spacing();
    system("pause");
}

int reade(FILE * in, Op ops[],int n){
    int i=0,tmp;
    while(i<n&&fscanf(in,"%s %d %d\n",ops[i].name,&ops[i].speed,&tmp)==3){
        ops[i].post=tmp;
        ops[i].removed=false;
        i++;
    }
    rewind(in);
    return i;
}

void adde(){

}

void OpstoString(Op ops[],int n,char opstring[][100]){
    int y=0;
    for(int i=0;i<n;i++){
        if(ops[i].removed==false){
            sprintf(opstring[y], "%-15s %-10d %-10d %-10s",ops[i].name,ops[i].speed,4-ops[i].speed,booltopost(ops[i].post));
            y++;
        }
    }
}

void OptoStrings(Op op,char opstring[][100]){
    sprintf(opstring[0],"%-15s",op.name);
    sprintf(opstring[1],"%-10d",op.speed);
    sprintf(opstring[2],"%-10d",4-op.speed);
    sprintf(opstring[3],"%-10s",booltopost(op.post));
}

int changeopname(char string[],int y,Op ops[],int n){
    int no=0;
    for(int i=0;i<n;i++){
        if(strcmp(ops[i].name,string)==0){
            no=1;
            break;
        }
    }
    if(no==0){
        strcpy(ops[y].name,string);
        return 0;
    }
    return 1;
}

int changeopspeed(int num, Op * op, bool speed){
    if(!(num>=1&&num<=3)){
        if(speed==1)
            (*op).speed=num;
        else
            (*op).speed=4-num;
        return 0;
    }
    return -1;
}

int changeoppost(char string[],Op * op){
    if(strcmp("attacker",string)==0||strcmp("1",string)==0){
        (*op).post=1;
        return 0;
    }
    else if(strcmp("defender",string)==0||strcmp("0",string)==0){
        (*op).post=0;
        return 0;
    }
    return 1;
}

void editElement(int x,int y, Op ops[],int n){
    char tmp[100];
    int tmpi;
    switch(x){
        case 0:printf("\e[%d;%dH\e[7m               \e[%d;%dH",y+2,3,y+2,3);scanf("%15s",tmp);changeopname(tmp,y,ops,n);break;
        case 1:printf("\e[%d;%dH\e[7m          \e[%d;%dH",y+2,16+3,y+2,16+3);scanf("%d",&tmpi);changeopspeed(tmpi,&ops[y],1);break;
        case 2:printf("\e[%d;%dH\e[7m          \e[%d;%dH",y+2,27+3,y+2,27+3);scanf("%d",&tmpi);changeopspeed(tmpi,&ops[y],0);break;
        case 3:printf("\e[%d;%dH\e[7m          \e[%d;%dH",y+2,38+3,y+2,38+3);scanf("%10s",tmp);changeoppost(tmp,&ops[y]);break;
        default:break;
    }
    printf("\e[m");

}

void edite(Op ops[],int n){
    char opsstring[n][100],opstring[4][100];
    int y=0,x;
    OpstoString(ops,n,opsstring);
    y = menu(opsstring,n,true,y);
    while(y!=-2){
        OptoStrings(ops[y],opstring);
        x = menu(opstring,4,false,y);
        if(x>=0&&x<=3){
            editElement(x,y,ops,n);
            OpstoString(ops,n,opsstring);
        }
        y = menu(opsstring,n,true,y);
    }
}

void rme(Op ops[], int n){
    char opsstring[n][100];
    int y=0,c=n;
    while(y!=-2){
        OpstoString(ops,c,opsstring);
        y=menu(opsstring,c,true,y);
        ops[ytoreal(ops,y+c-n)].removed=true;
        n--;
    }
}

int main(){
    int n=100;
    Op ops[n];
    /*TEMNO*/
    DWORD mode = 0;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hConsole, &mode);
    mode |= ENABLE_PROCESSED_INPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hConsole, mode);
    /*SVITANI*/
    FILE * in = fopen("ops.txt","r");
    FILE * passwd = fopen("passwd.txt","a");
    if(in==NULL||passwd==NULL)return -1;
    fclose(passwd);
    int choice;
    n=reade(in,ops,n);
    bool admin=true;
    while(true){
        choice = mainMenu(admin);
        switch(choice){
            case 1:if(admin==false){if((admin = signin("Insert password:"))==true){spacing();printf("Login successfull");spacing();system("pause");};}else{changepasswd();}break;
            case 2:printall(ops,n);break;
            case 3:break;
            case 4:adde();break;
            case 5:edite(ops,n);break;
            case 6:rme(ops,n);break;
            case 0:if(admin==false){fclose(in);return 0;}admin=false;break;
            default:break;
        }
    }
}
