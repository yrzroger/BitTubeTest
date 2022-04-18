/**
 * Author RogerYu, Written in 2022/04/16
 */

#include <sys/types.h>          
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <private/gui/BitTube.h>

using namespace android;


struct Event {
    int id;
    int message;
};

int main()
{
    gui::BitTube* dataChannel = new gui::BitTube(gui::BitTube::DefaultSize);
    
    printf("\033[0mBitTube info: mReceiveFd=%d, mSendFd=%d\n", dataChannel->getFd(), dataChannel->getSendFd());

    if(fork()) {
        // 父进程发送数据
        Event events[] = { {0, 888}, {1, 999} };
        ssize_t size = gui::BitTube::sendObjects(dataChannel, events, 2);
        if(size < 0)
            printf("\033[32mprocess(%d) send failed, in parent process", getpid());
        else
            printf("\033[32mprocess(%d) send success, object size = %d\n", getpid(), size);

        sleep(1);

        // 父进程接收数据
        size = gui::BitTube::recvObjects(dataChannel, events, 2);
        if(size < 0) {
            printf("\033[32mprocess(%d) receive failed, in child process", getpid());
        }
        else {
            printf("\033[32mprocess(%d) receive success, object size = %d\n", getpid(), size);
            for(int i = 0; i < size; ++i) {
                printf("\033[32mprocess(%d): id=%d, message=%d\n", getpid(), events[i].id, events[i].message);
            }
        }

        sleep(1);
        
    } else {
        // 子进程接收数据
        Event events[2];
        ssize_t size = gui::BitTube::recvObjects(dataChannel, events, 2);
        if(size < 0) {
            printf("\033[31mprocess(%d) receive failed, in child process", getpid());
        }
        else {
            printf("\033[31mprocess(%d) receive success, object size = %d\n", getpid(), size);
            for(int i = 0; i < size; ++i) {
                printf("\033[31mprocess(%d): id=%d, message=%d\n", getpid(), events[i].id, events[i].message);
            }
        }
        
        // 子进程发送数据
        events[0].message+=1; events[1].message+=1;
        size = gui::BitTube::sendObjects(dataChannel, events, 2);
        if(size < 0)
            printf("\033[31mprocess(%d) send failed, in parent process", getpid());
        else
            printf("\033[31mprocess(%d) send success, object size = %d\n", getpid(), size);
        sleep(2);
    }


    delete dataChannel;

    return 0;
}
