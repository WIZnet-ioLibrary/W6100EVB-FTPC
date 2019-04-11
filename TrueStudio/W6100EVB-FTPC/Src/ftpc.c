#include "ftpc.h"


static uint8_t gMsgBuf[20]={0,};
uint8_t remote_ip[16];
uint8_t connect_state_control_ftpc = 0;
uint8_t connect_state_data_ftpc = 0;
uint8_t gModeActivePassiveflag = 0;
uint8_t gMenuStart = 0;
uint8_t gDataSockReady = 0;
uint8_t gDataPutGetStart = 0;
uint16_t    remote_port;
uint16_t    local_port;
uint16_t    ctrl_port;
char dat[50];
struct ftpc ftpc;
struct Command Command;


uint8_t ftpc_init(uint8_t * FTP_destip, uint16_t FTP_destport, uint8_t astype)
{
    uint8_t i;
    uint8_t status;
    ftpc.as_type=astype;
    ftpc.is_active = 1;


    ftpc.destip[0]=FTP_destip[0];
    ftpc.destip[1]=FTP_destip[1];
    ftpc.destip[2]=FTP_destip[2];
    ftpc.destip[3]=FTP_destip[3];
    ftpc.destip[4]=FTP_destip[4];
    ftpc.destip[5]=FTP_destip[5];
    ftpc.destip[6]=FTP_destip[6];
    ftpc.destip[7]=FTP_destip[7];
    ftpc.destip[8]=FTP_destip[8];
    ftpc.destip[9]=FTP_destip[9];
    ftpc.destip[10]=FTP_destip[10];
    ftpc.destip[11]=FTP_destip[11];
    ftpc.destip[12]=FTP_destip[12];
    ftpc.destip[13]=FTP_destip[13];
    ftpc.destip[14]=FTP_destip[14];
    ftpc.destip[15]=FTP_destip[15];

    ftpc.destport=FTP_destport;


    local_port = 35000;
    strcpy(ftpc.workingdir, "/");
    for(i = 0; i < SOCK_MAX_NUM; i++)
    {
        getsockopt(i, SO_STATUS, &status);
        if(status == SOCK_CLOSED)
        {
            ftpc.control = i++;
            break;
        }
    }
    for(; i < SOCK_MAX_NUM; i++)
    {
        getsockopt(i, SO_STATUS, &status);
        if(status == SOCK_CLOSED)
        {
            ftpc.data = i;
            break;;
        }
    }
    if(i == SOCK_MAX_NUM) return 0;
    return 1;
}


uint8_t ftpc_run(uint8_t * dbuf)
{
#ifndef Need_UARTGetCharBlocking_func
    uint16_t size = 0;
    int ret = 0;
    uint8_t tmp;
    uint16_t msgsize = 0;
    int8_t status;

    getsockopt(ftpc.control, SO_STATUS, &status);
    switch(status)
    {
        case SOCK_ESTABLISHED :
            if(!connect_state_control_ftpc){
                printf("%d:FTP Connected\r\n", ftpc.control);
                strcpy(ftpc.workingdir, "/");
                connect_state_control_ftpc = 1;
            }
            if((size = getSn_RX_RSR(ftpc.control)) > 0)// Don't need to check SOCKERR_BUSY because it doesn't not occur.
            {
                msgsize = peeksockmsg(ftpc.control, "\r\n" ,2);
                msgsize+=2;
                if(msgsize > _FTP_MAX_MSG_SIZE_) msgsize = _FTP_MAX_MSG_SIZE_ - 1;
                ret = recv(ftpc.control,dbuf,msgsize);
                dbuf[ret] = '\0';
                if(ret != msgsize)
                {
                    if(ret==SOCK_BUSY) return 0;
                    if(ret < 0){
                    printf("%d:recv() error:%d\r\n",ftpc.control,ret);
                    close(ftpc.control);
                    return ret;
                    }
                }
                printf("Rcvd Command: %s\r\n", dbuf);
                proc_ftpc((char *)dbuf);
            }

            if(gMenuStart) ftpc_menu(dbuf);
            if(gDataSockReady) ftpc_ready(dbuf);

            break;
        case SOCK_CLOSE_WAIT :
            printf("%d:CloseWait\r\n",ftpc.control);
            if((ret=disconnect(ftpc.control)) != SOCK_OK) return ret;
            printf("%d:Closed\r\n",ftpc.control);
            break;
        case SOCK_CLOSED :
            printf("%d:FTPStart\r\n",ftpc.control);
            if((ret=socket(ftpc.control, Sn_MR_TCPD, ctrl_port, 0x0)) != ftpc.control)
            {
                printf("%d:socket() error:%d\r\n", ftpc.control, ret);
                close(ftpc.control);
                return ret;
            }
            break;
        case SOCK_INIT :
            printf("%d:Opened\r\n",ftpc.control);
            if(ftpc.as_type==AS_IPV6)
                ret=connect(ftpc.control, ftpc.destip, ftpc.destport, 16);
            else
                ret=connect(ftpc.control, ftpc.destip, ftpc.destport, 4);
            if(ret  != SOCK_OK)
            {
                printf("%d:Connect error:%d\r\n",ftpc.control, ret);
                return ret;
            }


            connect_state_control_ftpc = 0;
            printf("%d:Connectting...\r\n",ftpc.control);
            break;
        default :
            break;
    }

    switch(tmp=getSn_SR(ftpc.data)){
        case SOCK_ESTABLISHED :
        if(!connect_state_data_ftpc)
        {
            printf("%d:FTP Data socket Connected\r\n", ftpc.data);
            connect_state_data_ftpc = 1;
        }
        if(gDataPutGetStart) ftpc_putget(dbuf);
        break;

        case SOCK_CLOSE_WAIT :
            printf("%d:CloseWait\r\n",ftpc.data);
            if(gDataPutGetStart) ftpc_putget(dbuf);
            if((ret=disconnect(ftpc.data)) != SOCK_OK) return ret;
            printf("%d:Closed\r\n",ftpc.data);
            break;
        case SOCK_CLOSED :
            if(ftpc.dsock_state == DATASOCK_READY)
            {
                if(ftpc.is_active == 0)
                {
                    printf("%d:FTPDataStart, port : %d\r\n",ftpc.data, local_port);
                    if((ret=socket(ftpc.data, Sn_MR_TCPD, local_port, 0x0)) != ftpc.data)
                    {
                        printf("%d:socket() error:%d\r\n", ftpc.data, ret);
                        close(ftpc.data);
                        return ret;
                    }
                    local_port++;
                    if(local_port > 50000)
                    local_port = 35000;
                }
                else
                {
                    printf("%d:FTPDataStart, port : %d\r\n",ftpc.data, local_port);
                    if((ret=socket(ftpc.data, Sn_MR_TCPD, local_port, 0x0)) != ftpc.data)
                    {
                        printf("%d:socket() error:%d\r\n", ftpc.data, ret);
                        close(ftpc.data);
                        return ret;
                    }
                    local_port++;
                    if(local_port > 50000)
                    local_port = 35000;
                }
                ftpc.dsock_state = DATASOCK_START;
            }
            break;

        case SOCK_INIT :
            printf("%d:Opened\r\n",ftpc.data);
            if(ftpc.is_active == 1)
            {
                if( (ret = listen(ftpc.data)) != SOCK_OK)
                {
                    printf("%d:Listen error\r\n",ftpc.data);
                    return ret;
                }
                gDataSockReady = 1;
                printf("%d:Listen ok\r\n",ftpc.data);
            }
            else
            {
                if(ftpc.as_type==AS_IPV6)
                    ret = connect(ftpc.data, ftpc.destip, remote_port, 16);
                else
                    ret = connect(ftpc.data, remote_ip, remote_port, 4);
                if(ret  != SOCK_OK)
                {
                    printf("%d:Connect error\r\n", ftpc.data);
                    return ret;
                }
                gDataSockReady = 1;
            }
            connect_state_data_ftpc = 0;
            break;
        default :
            break;
    }
#endif
    return 0;
}

void ftpc_menu(uint8_t *dbuf)
{
    uint8_t msg_c;

    gMenuStart = 0;
    printf("\r\n----------------------------------------\r\n");
    printf("Press menu key\r\n");
    printf("----------------------------------------\r\n");
    printf("1> View FTP Server Directory\r\n");
    printf("2> View My Directory\r\n");
    printf("3> Sets the type of file to be transferred. Current state : %s\r\n", (ftpc.is_ascii==1)?"Ascii":"Binary");
    printf("4> Sets Data Connection. Current state : %s\r\n", (ftpc.is_active==1)?"Active":"Passive");
    printf("5> Put File to Server\r\n");
    printf("6> Get File from Server\r\n");
    printf("----------------------------------------\r\n");
    while(1)
    {
        msg_c=ftp_getc();
        if(msg_c=='1'){
            if(ftpc.is_active==0){
                if(ftpc.as_type==AS_IPV6)
                {
                    sprintf(dat,"EPSV ALL\r\n");
                }
                else
                {
                    sprintf(dat,"PASV\r\n");
                }
                send(ftpc.control, (uint8_t *)dat, strlen((const char *)dat));
                Command.First = cmd_dir;
                break;
            }
            else{

                wiz_NetInfo gWIZNETINFO;
                ctlnetwork(CN_GET_NETINFO, (void*) &gWIZNETINFO);
                if(ftpc.as_type==AS_IPV6)
                {
                    sprintf(dat,"EPRT |2|%.2x%.2x:%.2x%.2x:", gWIZNETINFO.lla[0], gWIZNETINFO.lla[1], gWIZNETINFO.lla[2], gWIZNETINFO.lla[3]);
                    sprintf(dat+strlen(dat),"%.2x%.2x:%.2x%.2x:", gWIZNETINFO.lla[4], gWIZNETINFO.lla[5], gWIZNETINFO.lla[6],gWIZNETINFO.lla[7]);
                    send(ftpc.control, (uint8_t *)dat, strlen(dat));
                    sprintf(dat,"%.2x%.2x:%.2x%.2x:", gWIZNETINFO.lla[8],gWIZNETINFO.lla[9], gWIZNETINFO.lla[10], gWIZNETINFO.lla[11]);
                    sprintf(dat+strlen(dat),"%.2x%.2x:%.2x%.2x|%d|\r\n",gWIZNETINFO.lla[12], gWIZNETINFO.lla[13], gWIZNETINFO.lla[14], gWIZNETINFO.lla[15], local_port);
                    send(ftpc.control, (uint8_t *)dat, strlen(dat));
                }

                else{
                    sprintf(dat,"PORT %d,%d,%d,%d,%d,%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3], (uint8_t)(local_port>>8), (uint8_t)(local_port&0x00ff));
                    send(ftpc.control, (uint8_t *)dat, strlen(dat));
                }


                Command.First = cmd_dir;

                gModeActivePassiveflag = 1;
                break;
            }
            break;
        }
        else if(msg_c=='5'){
            if(ftpc.is_active==0){
                if(ftpc.as_type==AS_IPV6)
                {
                    sprintf(dat,"EPSV ALL\r\n");
                }
                else
                {
                    sprintf(dat,"PASV\r\n");
                }
                send(ftpc.control, (uint8_t *)dat, strlen(dat));
                Command.First = cmd_put;
                break;

            }
            else{
                wiz_NetInfo gWIZNETINFO;
                ctlnetwork(CN_GET_NETINFO, (void*) &gWIZNETINFO);
                if(ftpc.as_type==AS_IPV6)
                {
                    sprintf(dat,"EPRT |2|%.2x%.2x:%.2x%.2x:", gWIZNETINFO.lla[0], gWIZNETINFO.lla[1], gWIZNETINFO.lla[2], gWIZNETINFO.lla[3]);
                    sprintf(dat+strlen(dat),"%.2x%.2x:%.2x%.2x:", gWIZNETINFO.lla[4], gWIZNETINFO.lla[5], gWIZNETINFO.lla[6],gWIZNETINFO.lla[7]);
                    send(ftpc.control, (uint8_t *)dat, strlen(dat));
                    sprintf(dat,"%.2x%.2x:%.2x%.2x:", gWIZNETINFO.lla[8],gWIZNETINFO.lla[9], gWIZNETINFO.lla[10], gWIZNETINFO.lla[11]);
                    sprintf(dat+strlen(dat),"%.2x%.2x:%.2x%.2x|%d|\r\n",gWIZNETINFO.lla[12], gWIZNETINFO.lla[13], gWIZNETINFO.lla[14], gWIZNETINFO.lla[15], local_port);
                    send(ftpc.control, (uint8_t *)dat, strlen(dat));
                }

                else{
                    sprintf(dat,"PORT %d,%d,%d,%d,%d,%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3], (uint8_t)(local_port>>8), (uint8_t)(local_port&0x00ff));
                    send(ftpc.control, (uint8_t *)dat, strlen(dat));
                }
                Command.First = cmd_put;

                gModeActivePassiveflag = 1;
                break;
            }
        }
        else if(msg_c=='6'){
            if(ftpc.is_active==0){
                if(ftpc.as_type==AS_IPV6)
                {
                    sprintf(dat,"EPSV ALL\r\n");
                }
                else
                {
                    sprintf(dat,"PASV\r\n");
                }
                send(ftpc.control, (uint8_t *)dat, strlen(dat));
                Command.First = cmd_get;
                break;

            }
            else{
                wiz_NetInfo gWIZNETINFO;
                ctlnetwork(CN_GET_NETINFO, (void*) &gWIZNETINFO);
                if(ftpc.as_type==AS_IPV6)
                {
                    sprintf(dat,"EPRT |2|%.2x%.2x:%.2x%.2x:", gWIZNETINFO.lla[0], gWIZNETINFO.lla[1], gWIZNETINFO.lla[2], gWIZNETINFO.lla[3]);
                    sprintf(dat+strlen(dat),"%.2x%.2x:%.2x%.2x:", gWIZNETINFO.lla[4], gWIZNETINFO.lla[5], gWIZNETINFO.lla[6],gWIZNETINFO.lla[7]);
                    send(ftpc.control, (uint8_t *)dat, strlen(dat));
                    sprintf(dat,"%.2x%.2x:%.2x%.2x:", gWIZNETINFO.lla[8],gWIZNETINFO.lla[9], gWIZNETINFO.lla[10], gWIZNETINFO.lla[11]);
                    sprintf(dat+strlen(dat),"%.2x%.2x:%.2x%.2x|%d|\r\n",gWIZNETINFO.lla[12], gWIZNETINFO.lla[13], gWIZNETINFO.lla[14], gWIZNETINFO.lla[15], local_port);
                    send(ftpc.control, (uint8_t *)dat, strlen(dat));
                }

                else{
                    sprintf(dat,"PORT %d,%d,%d,%d,%d,%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3], (uint8_t)(local_port>>8), (uint8_t)(local_port&0x00ff));
                    send(ftpc.control, (uint8_t *)dat, strlen(dat));
                }

                Command.First = cmd_get;

                gModeActivePassiveflag = 1;
                break;
            }
        }
        else if(msg_c=='2'){
            if (strncmp(ftpc.workingdir, "/$Recycle.Bin", sizeof("/$Recycle.Bin")) != 0)
                sprintf((char*)dbuf, "drwxr-xr-x 1 ftp ftp 0 Dec 31 2014 $Recycle.Bin\r\n-rwxr-xr-x 1 ftp ftp 512 Dec 31 2014 test.txt\r\n");
            printf("\r\n%s\r\n", dbuf);

            gMenuStart = 1;
            break;
        }
        else if(msg_c=='3'){
            printf("1> ASCII\r\n");
            printf("2> BINARY\r\n");
            while(1){
                msg_c=ftp_getc();
                if(msg_c=='1'){
                    sprintf(dat,"TYPE %c\r\n", TransferAscii);
                    ftpc.is_ascii = 1;
                    send(ftpc.control, (uint8_t *)dat, strlen(dat));
                    break;
                }
                else if(msg_c=='2'){
                    sprintf(dat,"TYPE %c\r\n", TransferBinary);
                    ftpc.is_ascii = 0;
                    send(ftpc.control, (uint8_t *)dat, strlen(dat));
                    break;
                }
                else{
                    printf("\r\nRetry...\r\n");
                }
            }
            break;
        }
        else if(msg_c=='4'){
            printf("1> ACTIVE\r\n");
            printf("2> PASSIVE\r\n");
            while(1){
                msg_c=ftp_getc();
                if(msg_c=='1'){
                    ftpc.is_active=1;
                    break;
                }
                else if(msg_c=='2'){
                    ftpc.is_active=0;
                    break;
                }
                else{
                    printf("\r\nRetry...\r\n");
                }
            }
            gMenuStart = 1;
            break;
        }

        else{
            printf("\r\nRetry...\r\n");
        }

    }
}

void ftpc_ready(uint8_t* dbuf)
{
    gDataSockReady = 0;
    switch(Command.First){
        case cmd_dir:

            sprintf(dat,"LIST\r\n");
            send(ftpc.control, (uint8_t *)dat, strlen(dat));
            break;
        case cmd_put:
            printf(">put file name\r\n?");
            sprintf(dat,"STOR %s\r\n", User_Keyboard_MSG());
            send(ftpc.control, (uint8_t *)dat, strlen(dat));
            break;
        case cmd_get:
            printf(">get file name?\r\n");
            sprintf(dat,"RETR %s\r\n", User_Keyboard_MSG());
            send(ftpc.control, (uint8_t *)dat, strlen(dat));
            break;
        default:
            printf("Command.First = default\r\n");
            break;
    }
}


int ftpc_putget(uint8_t* dbuf)
{
    uint16_t size;
    uint32_t remain_filesize;
    uint32_t remain_datasize;
    uint32_t  recv_byte;
    uint32_t blocklen;
    int ret;

    switch(Command.Second){
        case cmd_dir:
            printf("dir waiting...\r\n");

            if((size = getSn_RX_RSR(ftpc.data)) > 0){ // Don't need to check SOCKERR_BUSY because it doesn't not occur.
                printf("ok\r\n");
                memset(dbuf, 0, _FTP_MAX_MSG_SIZE_);
                if(size > _FTP_MAX_MSG_SIZE_) size = _FTP_MAX_MSG_SIZE_ - 1;
                ret = recv(ftpc.data,dbuf,size);
                dbuf[ret] = '\0';
                if(ret != size){
                    if(ret==SOCK_BUSY) return 0;
                    if(ret < 0){
                        printf("%d:recv() error:%d\r\n",ftpc.control,ret);
                        close(ftpc.data);
                        return ret;
                    }
                }
                printf("Rcvd Data:\n\r%s\n\r", dbuf);
                gDataPutGetStart = 0;
                Command.Second = cmd_nocmd;
            }
            break;
        case cmd_put:
            printf("put waiting...\r\n");
            if(strlen(ftpc.workingdir) == 1)
                sprintf(ftpc.filename, "/%s", (uint8_t *)gMsgBuf);
            else
                sprintf(ftpc.filename, "%s/%s", ftpc.workingdir, (uint8_t *)gMsgBuf);

            remain_filesize = strlen(ftpc.filename);
            do{
                memset(dbuf, 0, _FTP_MAX_MSG_SIZE_);
                blocklen = sprintf((char*)dbuf, "%s", ftpc.filename);
                printf("########## dbuf:%s\r\n", dbuf);
                send(ftpc.data, dbuf, blocklen);
                remain_filesize -= blocklen;
            }while(remain_filesize != 0);

            gDataPutGetStart = 0;
            Command.Second = cmd_nocmd;
            disconnect(ftpc.data);
            break;
        case cmd_get:
            printf("get waiting...\r\n");
            if(strlen(ftpc.workingdir) == 1)
                sprintf(ftpc.filename, "/%s", (uint8_t *)gMsgBuf);
            else
                sprintf(ftpc.filename, "%s/%s", ftpc.workingdir, (uint8_t *)gMsgBuf);


            while(1){
                if((remain_datasize = getSn_RX_RSR(ftpc.data)) > 0){
                    while(1){
                        memset(dbuf, 0, _FTP_MAX_MSG_SIZE_);
                        if(remain_datasize > _FTP_MAX_MSG_SIZE_)
                            recv_byte = _FTP_MAX_MSG_SIZE_;
                        else
                            recv_byte = remain_datasize;
                        ret = recv(ftpc.data, dbuf, recv_byte);
                        printf("########## dbuf:%s\r\n", dbuf);
                        remain_datasize -= ret;
                        if(remain_datasize <= 0)
                            break;
                    }
                }else{
                    if(getSn_SR(ftpc.data) != SOCK_ESTABLISHED)
                        break;
                }
            }
            gDataPutGetStart = 0;
            Command.Second = cmd_nocmd;

            break;
        default:
            printf("Command.Second = default\r\n");
            break;
    }
    return 1;
}

uint8_t proc_ftpc(char * buf)
{
    uint16_t Responses;
    char dat[30]={0,};

    Responses =(buf[0]-'0')*100+(buf[1]-'0')*10+(buf[2]-'0');

    switch(Responses){
        case R_220:    /* Service ready for new user. */
            printf("%s",buf+4);
            if(buf[3] == ' ')
            {
                printf("\r\nInput your User ID > \r\n");
                sprintf(dat,"USER %s\r\n", User_Keyboard_MSG());
                printf("\r\n");
                send(ftpc.control, (uint8_t *)dat, strlen(dat));
            }
            break;

        case R_331:    /* User name okay, need password. */
            printf("\r\nInput your Password > \r\n");
            sprintf(dat,"PASS %s\r\n", User_Keyboard_MSG());
            printf("\r\n");
            send(ftpc.control, (uint8_t *)dat, strlen(dat));
            break;
        case R_230:    /* User logged in, proceed */
            printf("\r\nUser logged in, proceed\r\n");

            sprintf(dat,"TYPE %c\r\n", TransferAscii);
            ftpc.is_ascii = 1;
            send(ftpc.control, (uint8_t *)dat, strlen(dat));
            break;
        case R_200:
            if((ftpc.is_active==1)&&gModeActivePassiveflag){
                ftpc.dsock_state = DATASOCK_READY;
                gModeActivePassiveflag = 0;
            }
            else{
                gMenuStart = 1;
            }
            break;
        case R_125:
        case R_150:
            switch(Command.First){
            case cmd_dir:
                Command.First = cmd_nocmd;
                Command.Second = cmd_dir;
                gDataPutGetStart = 1;
                break;
            case cmd_get:
                Command.First = cmd_nocmd;
                Command.Second = cmd_get;
                gDataPutGetStart = 1;
                break;
            case cmd_put:
                Command.First = cmd_nocmd;
                Command.Second = cmd_put;
                gDataPutGetStart = 1;
                break;
            default :
                printf("Command.First = default\r\n");
                break;
            }
            break;
        case R_226:
            gMenuStart = 1;
            break;
        case R_227:
            if (pportc(buf) == -1){
                printf("Bad port syntax\r\n");
            }
            else{
                printf("Go Open Data Sock...\r\n ");
                ftpc.is_active = 0;
                ftpc.dsock_state = DATASOCK_READY;
            }
            break;
        case R_229:
            if (pportc_extend(buf) == -1){
                printf("Bad port syntax\r\n");
            }
            else{
                printf("Go Open Data Sock...\r\n ");
                ftpc.is_active = 0;
                ftpc.dsock_state = DATASOCK_READY;
            }
         break;
        default:
            printf("\r\nUnsupported Response = %s\r\n",buf);
            gMenuStart = 1;
            break;
        }
    return 1;
}
int pportc(char * arg)
{
    int i;
    char* tok=0;
    strtok(arg,"(");
    for (i = 0; i < 4; i++)
    {
        if(i==0) tok = strtok(NULL,",\r\n");
        else     tok = strtok(NULL,",");
        remote_ip[i] = (uint8_t)atoi(tok, 10);
        if (!tok){
            printf("bad pport : %s\r\n", arg);
            return -1;
        }
    }
    remote_port = 0;
    for (i = 0; i < 2; i++){
        tok = strtok(NULL,",\r\n");
        remote_port <<= 8;
        remote_port += atoi(tok, 10);
        if (!tok){
            printf("bad pport : %s\r\n", arg);
            return -1;
        }
    }
    printf("ip : %d.%d.%d.%d, port : %d\r\n", remote_ip[0], remote_ip[1], remote_ip[2], remote_ip[3], remote_port);
    return 0;
}

int pportc_extend(char * arg)
{
    char* tok=0;
    tok=strtok(arg,"(|||");
    tok=strtok(NULL, "|");
    remote_port=atoi(tok, 10);

    if (!tok){
        printf("bad pport : %s\r\n", arg);
        return -1;
    }

    printf("port : %d\r\n", remote_port);
    return 0;
}
uint8_t* User_Keyboard_MSG()
{
    uint8_t i=0;
    do{
        gMsgBuf[i] = ftp_getc();
        i++;
    }while(gMsgBuf[i-1]!=0x0d);
    gMsgBuf[i-1]=0;
    return gMsgBuf;
}
