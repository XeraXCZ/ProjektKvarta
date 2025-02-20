#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <conio.h>

#define ARROW_UP    256 + 72
#define ARROW_DOWN  256 + 80

typedef struct{ //Op
    char name[100];
    int speed;
    bool post;
}Op;

char * booltopost(int z){
    if(z==1)
    return "attacker";
    return "defender";
}

int menu(bool a){
    int ch,y=0;
    char chcs[10][2][100]={ {{"Sign as administrator"},{"Change password"}},
                            {{"Print whole DB"},{"Print whole DB"}},
                            {{"Search in DB"},{"Search in DB"}},
                            {{"End"},{"Add element"}},
                            {{},{"Edit element"}},
                            {{},{"Remove element"}},
                            {{},{"Switch to user"}}};
    printf("\e[?25l");
    while(true){
        system("cls");
        printf("You are logged as ");
        if(a==true)
            printf("administrator\n\n");
        else
            printf("user\n\n");
        for(int i=0;i<10;i++){
            if(i==y)
                printf("\e[7m%s\e[m\n",chcs[i][a]);
            else
                printf("%s\n",chcs[i][a]);
        }
        ch = getch();
        if ( ch == 0 || ch == 224 )
            ch = 256 + getch();
        switch(ch){
            case ARROW_UP:if(y-1>=0)y--;break;
            case ARROW_DOWN:if(((y+1<7)&&(a==1))||(y+1<4))y++;break;
            case 13:if(a==1)return (y+1)%7;return (y+1)%4;
            default:break;
        }
    }
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

bool signin(){
    char passwd[100];
    char cpasswd[100];
    FILE * in = fopen("passwd.txt","r");
    system("cls");
    if(in==NULL||fscanf(in,"%s",cpasswd)!=1){
        fclose(in);
        printf("Create password\n");
        printf("Insert password:");
        createpassword();
        return true;
    }
    else{
        fclose(in);
        printf("Insert password:");
        scanPasswd(passwd);
        if(strcmp(passwd,cpasswd)==0)
            return true;
        printf("The password is incorrect\n\n");
        system("pause");
        return false;
    }
}

void changepasswd(){
    if(signin()==true){
        printf("\nType in new password:\n");
        createpassword();
        printf("Password successfully changed\n\n");
        system("pause");
    }
}

void printall(Op ops[], int n){
    system("cls");
    printf("%-15s %-10s %-10s %-10s\n\n","Name","Speed","Health","Post");
        for(int i=0;i<n;i++){
        printf("%-15s %-10d %-10d %-10s\n",ops[i].name,ops[i].speed,4-ops[i].speed,booltopost(ops[i].post));
    }
    system("pause");
}

int read(FILE * in, Op ops[],int n){
    int i=0,tmp;
    while(i<n&&fscanf(in,"%s %d %d\n",ops[i].name,&ops[i].speed,&tmp)==3){
        ops[i].post=tmp;
        i++;
    }
    rewind(in);
    return i;
}

int main()
{
    int n=100;
    Op ops[n];
    DWORD mode = 0;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleMode(hConsole, &mode);
    mode |= 0x1 | 0x4;
    SetConsoleMode(hConsole, mode);
    SetConsoleTitle("Projekt");
    FILE * in = fopen("ops.txt","r");
    FILE * passwd = fopen("passwd.txt","a");
    if(in==NULL||passwd==NULL)return -1;
    fclose(passwd);
    int choice;
    n=read(in,ops,n);
    bool admin=false;
    while(true){
        choice = menu(admin);
        switch(choice){
            case 1:if(admin==false){admin = signin();}else{ changepasswd();}break;
            case 2:printall(ops,n);break;
            case 3:break;
            case 4:break;
            case 5:break;
            case 6:break;
            case 0:if(admin==false){fclose(in);return 0;}admin=false;break;
            default:break;
        }
    }
}
