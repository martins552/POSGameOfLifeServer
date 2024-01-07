#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>
#include "buffer.h"
#include "pos_sockets/char_buffer.h"
#include "pos_sockets/active_socket.h"
#include "pos_sockets/passive_socket.h"
#include <sys/stat.h>


//TODO zmeniť char_buffer length, aby sme zmestili doň naše dáta (svet aj názov súboru)

typedef struct world_state {
    char* file_name;
    char* world;
} WORLD_STATE_DATA;

//TODO
///UNFINISHED!!!
_Bool world_state_try_deserialize(struct world_state* worldState, struct char_buffer* buf) {
    char *pos = strchr(buf->data, ';');
    if(pos != NULL) {
        pos = strchr(pos+1, ';')+1;
        sscanf(buf->data, "%ld;%ld;");
        return true;
    }
    else {
        return false;
    }
}

_Bool send_world_state_to_client(struct char_buffer* buf, struct active_socket* client_sock) {
    char* delimiter_position = strchr(buf->data, '\n');
    if(delimiter_position != NULL)
    {
        int file_name_length = (int)(delimiter_position - buf->data[1]);
        if(file_name_length == 0) {
            printf("Klient poslal prázdny názov súboru\ndata: %s\n", buf->data);
            return false;
        }
        char file_name[file_name_length + 1];
        strncpy(file_name, &buf->data[1], file_name_length);
        file_name[file_name_length] = '\0';

        FILE* world_state_file = fopen(file_name, "r");
        if(world_state_file != NULL)
        {
            fseek(world_state_file, 0L, SEEK_END);
            int world_size = ftell(world_state_file);
            fseek(world_state_file, 0L, SEEK_SET);

            CHAR_BUFFER buffer;
            char_buffer_init_custom(&buffer, world_size);

            for (int i = 0; i < world_size; ++i) {
                char world_state_character = fgetc(world_state_file);
                char_buffer_append(&buffer, &world_state_character, 1);
            }
            active_socket_write_data(client_sock, &buffer);
            active_socket_write_end_message(client_sock);
            return true;
        }
    }
    return false;
}

_Bool determine_client_input(struct world_state* worldState, struct char_buffer* buf, struct active_socket* client_sock) {
    char determinant = buf->data[0];
    if(determinant == 'r')
    {
        return world_state_try_deserialize(worldState, buf);
    } else if(determinant == 'w')
    {
        return send_world_state_to_client(buf, client_sock);
    } else
    {
        printf("Klient poslal zlý determinant\ndata: %s\n", buf->data);
    }
    return false;
}

_Bool try_get_client_(struct active_socket* my_sock, struct pi_estimation* client_pi_estimaton) {
    _Bool result = false;
    CHAR_BUFFER r_buf;
    char_buffer_init(&r_buf);

    if(active_socket_try_get_read_data(my_sock, &r_buf)) {
        if(r_buf.size > 0) {
            if(active_socket_is_end_message(my_sock, &r_buf)) {
                active_socket_stop_reading(my_sock);
            } else if (pi_estimation_try_deserialize(client_pi_estimaton, &r_buf)) {
                result = true;
            } else {
                printf("Klient poslal spravu v zlom formate\ndata: %s\n", r_buf.data);
            }
        }
    }
    char_buffer_destroy(&r_buf);
    return result;
}

int main() {
    printf("Hello, World!\n");
    return 0;
}
