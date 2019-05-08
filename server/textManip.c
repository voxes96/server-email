int cutWord(char* line, char* word, int* idx)
{
    int i=0;
    while(line[(*idx)]!='\0'&&line[(*idx)]!=' '&&line[(*idx)]!='\n'&&(int)line[(*idx)]!=13)
        word[i++]=line[(*idx)++];
    if(word[i]!='\0')word[i]='\0';
    while(line[(*idx)]==' ')
        (*idx)++;

    if(line[(*idx)]=='\0')return 0;
    else return 1;
}
