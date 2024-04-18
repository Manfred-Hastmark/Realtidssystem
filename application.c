#include "application.h"
#include "TinyTimber.h"
#include "sciTinyTimber.h"
#include <stdio.h>
#include <stdlib.h>


Regulator regulator = initRegulator();
Transmitter transmitter = initTransmitter();
ReadBuffer readBuffer = initReadBuffer();
Timer timer = initTimer();

Can can0 = initCan(CAN_PORT0, &regulator, receive);
Serial sci0 = initSerial(SCI_PORT0, &regulator, reader);

void receive(Regulator* self, int unused)
{
    if(buffer_full(self, UNUSED)) //Discard if buffer is full
    {
        CANMsg msg;
        CAN_RECEIVE(&can0, &msg);
        if(self->print_out)
        {
            print("Buffer full, CAN message discarded\n", 0);
        }
        return;
    }
    //Buffer message
    CANMsg* msg = &self->buffer[self->msg_recv];
    CAN_RECEIVE(&can0, msg);
    self->msg_recv = (self->msg_recv + 1) % MAX_BUFFER;
    
    if(self->print_out)
    {
        print("CAN recv: %i\n", msg->msgId);
    }
    if(self->app_ready)
    {
        ASYNC(self, handle_msg, UNUSED);
    }
}

void set_ready(Regulator* self, int unused)
{
    self->app_ready = 1;
    //print("App is ready and next message will be handled\n", 0);
    if(self->msg_to_handle != self->msg_recv) //If there is new message handle it
    {
        ASYNC(self, handle_msg, UNUSED);
    }
}

void handle_msg(Regulator* self, int unused)
{
    //Do msg handling
    print("Message %i delivered at time: ", self->buffer[self->msg_to_handle].msgId);
    print("%i\n", SEC_OF(T_SAMPLE(&timer)));
    self->msg_to_handle = (self->msg_to_handle + 1) % MAX_BUFFER;
    self->app_ready = 0;
    AFTER(SEC(self->inter_arrival), self, set_ready, UNUSED);
}

int buffer_full(Regulator* self, int unused)
{
    int next = (self->msg_recv + 1) % MAX_BUFFER;
    return next == self->msg_to_handle;
}


void send(Transmitter* self, int unused)
{
    CANMsg msg;
    msg.msgId = self->msg_id;
    self->msg_id = (self->msg_id + 1) % 128;
    CAN_SEND(&can0, &msg);
    if(self->burst_mode)
    {
        AFTER(BURST_INTERVAL, self, send, UNUSED);
    }
}

void reader(Regulator* self, int c)
{
    //SCI_WRITE(&sci0, "Rcv: \'");
    SCI_WRITECHAR(&sci0, c);
    SCI_WRITECHAR(&sci0, '\n');

    // Call the keyhandler
    ASYNC(&regulator, keyHandler, c);
}


void keyHandler(Regulator* self, int c)
{
    switch (c)
    {
        case 'O':
        ASYNC(&transmitter, send, UNUSED);
        break;
        case 'B':
        transmitter.burst_mode = 1;
        ASYNC(&transmitter, send, UNUSED);
        break;
        case 'X':
        transmitter.burst_mode = 0;
        break;
        case 's':
        print("To handle: %i\n", self->msg_to_handle);
        print("Recv: %i\n", self->msg_recv);
        print("Appready: %i\n", self->app_ready);
        break;
        case '0' ... '9': // Add character to readbuffer
        case '-':
        ASYNC(&readBuffer, readBufferAdd, c);
        break;
        case 'a':
        self->inter_arrival = SYNC(&readBuffer, endBuffer, UNUSED);
        break;
        case 'p':
        self->print_out ^= 1;
        break;
        default:
        break;
    }
}

void readBufferAdd(ReadBuffer* self, int character)
{
    self->buffer[self->index++] = character;
}

int endBuffer(ReadBuffer* self, int unused)
{
    self->buffer[self->index] = '\0';
    self->index = 0;
    return atoi(self->buffer);
}



void print(char* string, int val)
{
    char output[50];
    sprintf(output, string, val);
    SCI_WRITE(&sci0, output);
}


void startapp(Regulator* self, int arg)
{
    CAN_INIT(&can0);
    SCI_INIT(&sci0);
    SCI_WRITE(&sci0, "Hello, hello...\n");
}

int main()
{
    INSTALL(&sci0, sci_interrupt, SCI_IRQ0);
    INSTALL(&can0, can_interrupt, CAN_IRQ0);
    TINYTIMBER(&regulator, startapp, 0);
    return 0;
}
