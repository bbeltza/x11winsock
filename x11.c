#include "x11.h"
#include "util.h"

x11_SCREEN* X11_GET_SCREEN_NEXT(x11_SCREEN* screen)
{
    x11_DEPTH* depths = X11_GET_SCREEN_DEPTHS(screen);
    for (int i = 0; i < screen->num_allowed_depths; i++) {
        depths = X11_GET_DEPTH_NEXT(depths);
    }

    return (x11_SCREEN*)depths;
}

/**/

x11_connection* x11_connect(SOCKET socket, const x11_connection_info* info)
{
    int status;
    x11_connection* connection;
    x11res_Connect resheader;

    {
        x11_CARD16 auth_protocol_name_len = info->auth_protocol_name_len;
        x11_CARD16 auth_protocol_data_len = info->auth_protocol_data_len;
        if (!auth_protocol_name_len && info->auth_protocol_name) {
            auth_protocol_name_len = (x11_CARD16)strlen(info->auth_protocol_name);
        } 

        x11_CARD16 auth_protocol_name_len_padded = x11_PAD(auth_protocol_name_len);
        x11_CARD16 auth_protocol_data_len_padded = x11_PAD(auth_protocol_data_len);
        size_t req_len = sizeof(x11req_Connect) + auth_protocol_data_len_padded + auth_protocol_name_len_padded;
        x11req_Connect* req = _alloca(req_len);
        req->byte_order = 0x6C; /* LSB first */
        req->protocol_major_version = 11;
        req->protocol_minor_version = 0;
        req->auth_protocol_name_len = auth_protocol_name_len;
        req->auth_protocol_data_len = auth_protocol_data_len;

        char* auth_name = (char*)(req + 1);
        char* auth_data = (char*)(auth_name + auth_protocol_name_len_padded);
        memcpy(auth_name, info->auth_protocol_name, req->auth_protocol_name_len);
        memcpy(auth_data, info->auth_protocol_data, req->auth_protocol_data_len);

        status = send(socket, (const char*)req, req_len, 0);
        if (status == SOCKET_ERROR) {
            return NULL;
        }

        status = recv(socket, (char*)&resheader, sizeof(resheader), 0);
        if (status == SOCKET_ERROR) {
            int lasterr = WSAGetLastError();
            CHAR buf[512] = "Unknown Error.\n";
            FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lasterr, 0, buf, sizeof(buf), NULL);
            printf("Code: %d, data recieve failed: %s", lasterr, buf);
            return NULL;
        }

        printlnf("{ %u ; %u ; %u.%u ; %u }", resheader.success, resheader.reason_len, resheader.protocol_major_version, resheader.protocol_minor_version, resheader.additional_data_len);
        if (resheader.success == 0) {
            char* reason = _alloca(resheader.reason_len);
            status = recv(socket, reason, resheader.reason_len, 0);
            printf("Failed connecting to the X server: %.*s", resheader.reason_len, reason);
            return NULL;
        }
        else if (resheader.success == 2) {
            printlnf("Authenticate connection status is not implemented! I do not know how it works!!!");
            abort();
        }

        HANDLE hheap = HeapCreate(HEAP_GENERATE_EXCEPTIONS, 0, 0);
        connection = HeapAlloc(hheap, HEAP_NO_SERIALIZE, sizeof(x11_connection) + resheader.additional_data_len*4);
        connection->hheap = hheap;
        connection->socket = socket;
        connection->id_freelist = NULL;
        connection->id_freelist_size = 0;
        connection->id_freelist_cap = 0;
    }

    x11res_ConnectSuccess* state = X11_GET_CONNECTION_STATE(connection);
    {  /* Apparently, reading the connection response only once can lead to a incomplete reads */
        char* stateptr = (char*)state;
        int bytes_remaining = resheader.additional_data_len*4;
        while (1) {
            status = recv(socket, stateptr, bytes_remaining, 0);
            if (status == SOCKET_ERROR) {
                x11_destroy_connection(connection);
                return NULL;
            }
            
            bytes_remaining -= status;
            if (!bytes_remaining)
                break;
            stateptr += status;
        }
    }

    connection->last_id = state->resource_id_base;
    connection->max_id = state->resource_id_base + state->resource_id_mask;
    connection->vendor = (char*)(state + 1);
    connection->pixmap_formats = (x11_FORMAT*)(connection->vendor + x11_PAD(state->vendor_len));
    connection->roots = (x11_SCREEN*)(connection->pixmap_formats + state->num_formats);
    printlnf("%.*s %u %u 0x%08x 0x%08x", state->vendor_len, connection->vendor, connection->pixmap_formats[1].bits_per_pixel, connection->roots->root, connection->last_id, connection->max_id);
    return connection;
}

void x11_destroy_connection(x11_connection* connection)
{
    if (!connection) {
        return;
    }

    HANDLE hheap = connection->hheap;
    HeapDestroy(hheap);
}

x11_CARD32 x11_generate_id(x11_connection* connection)
{
    if (connection->id_freelist_size) {
        connection->id_freelist_size--;
        return connection->id_freelist[connection->id_freelist_size];
    }

    if (connection->last_id <= connection->max_id)
        return connection->last_id++;

    printlnf("Out of resource IDs...");
    abort();
    return 0;
}

void x11_free_id(x11_connection* connection, x11_CARD32 id)
{
    if (!id) {
        return;
    }

    if (connection->last_id == id - 1) {
        connection->last_id--;
        return;
    }

    x11res_ConnectSuccess* state = X11_GET_CONNECTION_STATE(connection);
    if (state->resource_id_base < id) {
        printlnf("x11_free_id: Invalid ID...");
        abort();
    }

    if (connection->id_freelist_size >= connection->id_freelist_cap) {
        connection->id_freelist_cap += connection->id_freelist_size + 1;
        connection->id_freelist = HeapReAlloc(connection->hheap, 0, connection->id_freelist, sizeof(x11_CARD32)*connection->id_freelist_cap);
    }

}
