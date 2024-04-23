
#include "board_handler.h"
#include "TinyTimber.h"
#include "application.h"
#include "canMsgs.h"

#define CLAIM_WAIT_TIME MSEC(1000)


void handle_node_timeout(BoardHandler* self, int index)
{
    if (RANK != index)
    {
        print("Node with id %i disconnected\n", index);
        if (self->node_states[index] != DISCONNECTED)
        {
            self->nodes_connected--;
        }
        self->node_states[index] = DISCONNECTED;

        if (self->node_states[RANK] == MUSICIAN)
        {
            ASYNC(self, check_stepup, 0);
        }
    }
}

void handle_node_alive(BoardHandler* self, int raw_heart_beat_msg_p)
{
    ASYNC(self, check_stepup, 0);
    HeartBeat* heart_beat_msg_p = (HeartBeat*)raw_heart_beat_msg_p;
    if (self->node_states[heart_beat_msg_p->id] == DISCONNECTED)
    {
        self->nodes_connected++;
    }
    
    self->node_states[heart_beat_msg_p->id] = heart_beat_msg_p->role;
}

int get_next_player(BoardHandler* self, int unused)
{
    static int current_player;
    for (int i = 0; i < MAX_BOARDS; i++)
    {
        current_player++;
        current_player %= MAX_BOARDS;
        if (self->node_states[current_player] != DISCONNECTED)
        {
            return current_player;
        }
    }
    return -1;
}


void request_conductorship(BoardHandler* self, int unused)
{
    ASYNC(self->m_app_p, send_claim_conductorship, unused);
}

void handle_conductorship_handout(BoardHandler* self, int index)
{
    if (index < 0 || index > MAX_BOARDS - 1)
    {
        return;
    }
    self->node_states[index] = CONDUCTOR;
    if (index == RANK)
    {
        print("I Am The New Conductor\n", 0);
        ASYNC(self->m_app_p, start_playing, 0);
    }
}

void init_claim(BoardHandler* self, int index)
{
    print("Init claim\n", 0);
    self->request_ongoing = 1;
    self->request_index = index;
    AFTER(CLAIM_WAIT_TIME, self, commit_claim_request, 0);
}

void perform_handout(BoardHandler* self, int unused)
{
    self->schedule_handout = 0;
    SYNC(self->m_app_p, send_handout_conductor, self->request_index);
    self->node_states[RANK] = MUSICIAN;
    AFTER(MSEC(500), self, set_request_ongoing, 0); //If the handout is not accepted within 500 ms we will see if we will try step up
    self->request_index = DEFAULT_REQUEST_INDEX;
}

void commit_claim_request(BoardHandler* self, int unused)
{
    self->schedule_handout = 1;
}

void print_status(BoardHandler* self, int unused)
{
    print("Status:\n", 0);
    for (int i = 0; i < MAX_BOARDS; i++)
    {
        print("%i ", self->node_states[i]);
    }
    print("\n", 0);
}

int handle_conductor_disconnect(BoardHandler* self, int unused)
{
    int boards_connected = 0;
    for (int i = 0; i < MAX_BOARDS; i++) //Count the musician boards
    {
        if (self->node_states[i] == MUSICIAN)
        {
            boards_connected++;
            self->node_states[i] = DISCONNECTED; //Mark musicians as disconnected to prevent us from stepping up
        }
        
    }
    if(boards_connected > 1) //If atleast two boards we should step down
    {
        self->node_states[RANK] = MUSICIAN;
        print("Conductorship Void Due To Failure\n", 0);
    }
    
}

void check_stepup(BoardHandler* self, int unused)
{
    if(self->request_ongoing)
    {
        return; //We should not step up if someone is trying to claim
    }
    int num_boards = 0;
    int lowest_idx = 10;
    for (int i = 0; i < MAX_BOARDS; i++)
    {
        if (self->node_states[i] == CONDUCTOR)
        {
            return; // We won't step up, since a conductor is already present
        }
        if (self->node_states[i] == MUSICIAN)
        {
            num_boards++;
            if (i < lowest_idx)
            {
                lowest_idx = i;
            }
        }
    }
    if (num_boards > 1 && lowest_idx == RANK && self->connection)
    { //If there is no conductor step up if connection has been fully established i.e all heartbeats should have been received
        self->node_states[RANK] = CONDUCTOR;
        ASYNC(self->m_app_p, start_playing, 0);
        print("I Am The New Conductor\n", 0);
    }
}


void lowest_request_index(BoardHandler* self, int index)
{
    if (index < self->request_index)
    {
        self->request_index = index;
    }
}

void set_request_ongoing(BoardHandler* self, int set)
{
    self->request_ongoing = set;
}

void join_choir(BoardHandler* self, int unused)
{
    self->connection = 1; //Connection is now fully established
    for(int i = 0; i < MAX_BOARDS; i++) //Check if conductor is already present
    {
        if(self->node_states[i] == CONDUCTOR && i != RANK)
        {
            print("I Now Join As A Musician\n", 0);
            return;
        }
    }
}

void reset_connection(BoardHandler* self, int unused)
{ //When CAN fails to send
    self->connection = 0; 
    self->nodes_connected = 1;
}

