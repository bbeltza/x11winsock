#include <WinSock2.h>
#include <WS2tcpip.h>
#include <d3d9.h>

#include <stdio.h>

#include "x11.h"
#include "util.h"

/* Returns number from 0 to 15, or -1 on error (invalid character) */
static int parse_hexdigit(char chr)
{
    if (chr >= '0' && chr <= '9') {
        return chr - '0';
    }
    else if (chr >= 'a' && chr <= 'f') {
        return 10 + chr - 'a';
    }
    else if (chr >= 'A' && chr <= 'F') {
        return 10 + chr - 'A';
    }
    else {
        return -1;
    }
}

static const char* get_d3derr_string(HRESULT hr)
{
    switch (hr)
    {
        #define CASESTR(x) case x: return #x
        CASESTR(D3DERR_WRONGTEXTUREFORMAT);
        CASESTR(D3DERR_UNSUPPORTEDCOLOROPERATION);
        CASESTR(D3DERR_UNSUPPORTEDCOLORARG);
        CASESTR(D3DERR_UNSUPPORTEDALPHAOPERATION);
        CASESTR(D3DERR_UNSUPPORTEDALPHAARG);
        CASESTR(D3DERR_TOOMANYOPERATIONS);
        CASESTR(D3DERR_CONFLICTINGTEXTUREFILTER);
        CASESTR(D3DERR_UNSUPPORTEDFACTORVALUE);
        CASESTR(D3DERR_CONFLICTINGRENDERSTATE);
        CASESTR(D3DERR_UNSUPPORTEDTEXTUREFILTER);
        CASESTR(D3DERR_CONFLICTINGTEXTUREPALETTE);
        CASESTR(D3DERR_DRIVERINTERNALERROR);

        CASESTR(D3DERR_NOTFOUND);
        CASESTR(D3DERR_MOREDATA);
        CASESTR(D3DERR_DEVICELOST);
        CASESTR(D3DERR_DEVICENOTRESET);
        CASESTR(D3DERR_NOTAVAILABLE);
        CASESTR(D3DERR_OUTOFVIDEOMEMORY);
        CASESTR(D3DERR_INVALIDDEVICE);
        CASESTR(D3DERR_INVALIDCALL);
        CASESTR(D3DERR_DRIVERINVALIDCALL);
        CASESTR(D3DERR_WASSTILLDRAWING);
        CASESTR(D3DOK_NOAUTOGEN);
        default:
            return NULL;
    }
}

static int invalid_cookie(const char* magicookie) {
    printlnf("Invalid magic cookie: '%s'. Must be in hexadecimal format, without any prefix. Use `xauth list` in your host machine to lookup the magic cookie.", magicookie);
    return -1;
}

int main(int argc, char** argv)
{
    char* ipaddr = NULL;
    char* ipport = NULL;
    char* magicookie = NULL;

    for (int i = 1; i < argc; i++) {
        char* arg = argv[i];
        if (*arg == '-') {
        }
        else {
            if (!ipaddr) {
                ipaddr = arg;
            }
            else if (!magicookie) {
                magicookie = arg;
            }
        }
    }

    if (!ipaddr || !magicookie) {
        printf("No IP address and/or magic cookie specified\nUsage: %s [address]:[(default=6000) port] [magic cookie] [OPTIONS...]", argv[0]);
        return -1;
    }

        /* Find port inside `ipaddr` argument if there's a ':' separator inside of it. */
    for (char* p = ipaddr; *p; p++) {
        if (*p == ':') {
            *p = '\0';
            ipport = ++p; 
            break;
        }
    }

    if (!ipport) { /* Use default port 6000 if it hasn't been found... */
        ipport = "6000";
        printlnf("Using default port: %s", ipport);
    }

    int cookielen = (int)strlen(magicookie);
    if (!cookielen || cookielen & 0x1) {
        return invalid_cookie(magicookie);
    }
    int cookiebytelen = cookielen >> 1;
    x11_CARD8* cookiedata = _alloca(cookiebytelen);
    for (int i = 0, j = 0; i < cookielen; i += 2, j++) {
        char chr1 = magicookie[i + 0];
        char chr2 = magicookie[i + 1];
        
        int val1 = parse_hexdigit(chr1);
        int val2 = parse_hexdigit(chr2);
        if (val1 == -1 || val2 == -1) {
            cookiedata = NULL;
            break;
        }

        cookiedata[j] = (val1 << 4) | (val2 << 0);
    }

    if (!cookiedata) {
        return invalid_cookie(magicookie);
    }

    int status;
    WSADATA wsa_data;
    WORD wsa_version = MAKEWORD(2, 0);
    status = WSAStartup(wsa_version, &wsa_data);
    if (status != 0) {
        WSA_PRINTERRORLN(WSAStartup);
        return -1;
    }

    ADDRINFO* ipaddrinfo;
    status = GetAddrInfo(ipaddr, ipport, NULL, &ipaddrinfo);
    if (status != 0) {
        WSA_PRINTERRORLN(GetAddrInfo);
        goto END;
    }

    SOCKET socket;
    socket = WSASocket(ipaddrinfo->ai_family, ipaddrinfo->ai_socktype, IPPROTO_TCP, NULL, 0, 0);
    if (socket == INVALID_SOCKET) {
        status = -1;
        WSA_PRINTERRORLN(WSASocket);
        goto END;
    }

    printlnf("Connecting to address %s:%s...", ipaddr, ipport);
    status = WSAConnect(socket, ipaddrinfo->ai_addr, ipaddrinfo->ai_addrlen, NULL, NULL, NULL, NULL);
    if (status != 0) {
        CHAR buf[512] = "Unknown Error.\n";
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), 0, buf, sizeof(buf), NULL);
        printf("Connection failed: %s", buf);
        return NULL;
    }

    x11_connection_info connection_info = {
        "MIT-MAGIC-COOKIE-1",
        cookiedata,
        0,
        cookiebytelen
    };

    x11_connection* connection = x11_connect(socket, &connection_info);
    if (!connection) {
        status = -1;
        goto ENDSOCKET;
    }

    x11res_ConnectSuccess* state = X11_GET_CONNECTION_STATE(connection);

    /* D3D device creation happens here! */
    
    HRESULT hr;
    HWND dummy_hwnd;
    IDirect3D9* direct3d = NULL;
    IDirect3DDevice9* dxdevice = NULL;
    IDirect3DSurface9* dxreadback_surface = NULL;
    IDirect3DSurface9* dxrendertarget_surface = NULL;
    D3DPRESENT_PARAMETERS present_params;

    WNDCLASS wndclass;
    memset(&wndclass, 0x00, sizeof(wndclass));
    wndclass.lpfnWndProc = DefWindowProc;
    wndclass.lpszClassName = "X11-WINSOCK";
    RegisterClass(&wndclass); /* Handle error? */

    dummy_hwnd = CreateWindowExA(0, wndclass.lpszClassName, "", 0, 0, 0, 128, 128, NULL, NULL, NULL, NULL);
    if (!dummy_hwnd) {
        printlnf("Failed creating dummy window for the D3D9 device.");
        status = -1;
        goto ENDCONNECTION;
    }

    direct3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (!direct3d) {
        printlnf("Failed creating the D3D9 instance");
        status = -1;
        goto ENDCONNECTION;
    }

    present_params.BackBufferWidth = 0;
    present_params.BackBufferHeight = 0;
    present_params.BackBufferFormat = D3DFMT_UNKNOWN;
    present_params.BackBufferCount = 0;
    present_params.MultiSampleType = D3DMULTISAMPLE_NONE;
    present_params.MultiSampleQuality = 0;
    present_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    present_params.hDeviceWindow = NULL;
    present_params.Windowed = TRUE;
    present_params.EnableAutoDepthStencil = FALSE;
    present_params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
    present_params.Flags = 0;
    present_params.FullScreen_RefreshRateInHz = 0;
    present_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

    hr = direct3d->lpVtbl->CreateDevice(direct3d, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, dummy_hwnd, D3DCREATE_NOWINDOWCHANGES | D3DCREATE_HARDWARE_VERTEXPROCESSING, &present_params, &dxdevice);
    if (FAILED(hr)) {
        printlnf("Failed creating the D3D9 device: %s (code: 0x%08lx)", get_d3derr_string(hr), hr);
        status = -1;
        goto ENDD3D;
    }

    int w = 320;
    int h = 180;
    hr = dxdevice->lpVtbl->CreateRenderTarget(dxdevice, w, h, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, FALSE, &dxrendertarget_surface, NULL);
    if (FAILED(hr)) {
        printlnf("Failed creating render targets: %s (code: 0x%08lx)", get_d3derr_string(hr), hr);
        status = -1;
        goto ENDD3D;
    }
    hr = dxdevice->lpVtbl->CreateOffscreenPlainSurface(dxdevice, w, h, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &dxreadback_surface, NULL);
    if (FAILED(hr)) {
        printlnf("Failed creating render targets: %s (code: 0x%08lx)", get_d3derr_string(hr), hr);
        status = -1;
        goto ENDD3D;
    }
    
    struct VERTEX {
        float x, y, z;
        D3DCOLOR col;
    };
    static const struct VERTEX vertex_data[] = {
        { -0.5f, -0.5f, 0.0f, D3DCOLOR_XRGB(255, 0, 0) },
        {  0.0f,  0.5f, 0.0f, D3DCOLOR_XRGB(0, 255, 0) },
        {  0.5f, -0.5f, 0.0f, D3DCOLOR_XRGB(0, 0, 255) }
    };

    hr = dxdevice->lpVtbl->SetFVF(dxdevice, D3DFVF_XYZ | D3DFVF_DIFFUSE);
    hr = dxdevice->lpVtbl->SetRenderState(dxdevice, D3DRS_LIGHTING, FALSE);
    hr = dxdevice->lpVtbl->SetRenderTarget(dxdevice, 0, dxrendertarget_surface);
    hr = dxdevice->lpVtbl->BeginScene(dxdevice);
        hr = dxdevice->lpVtbl->Clear(dxdevice, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(180, 180, 255, 255), 0.0f, 0);
        hr = dxdevice->lpVtbl->DrawPrimitiveUP(dxdevice, D3DPT_TRIANGLELIST, 1, vertex_data, sizeof(struct VERTEX));
    hr = dxdevice->lpVtbl->EndScene(dxdevice);
    hr = dxdevice->lpVtbl->GetRenderTargetData(dxdevice, dxrendertarget_surface, dxreadback_surface);
    if (FAILED(hr)) {
        goto ENDD3D;
    }

    /**/

    printlnf("Connection successful!");

#if 0
    {
        x11_SCREEN* scrn = connection->roots;
        for (int i = 0; i < state->num_screens; i++) {
            printlnf("Screen: %d %d", scrn->root_depth, scrn->root_visual);
            x11_DEPTH* depth = X11_GET_SCREEN_DEPTHS(scrn);
            for (int j = 0; j < scrn->num_allowed_depths; j++) {
                printlnf(" Depth: %d, %d visuals", depth->depth, depth->num_visuals);
                x11_VISUALTYPE* visual = X11_GET_DEPTH_VISUALS(depth);
                for (int k = 0; k < depth->num_visuals; k++) {
                    printlnf("   %d %d %d %d %x %x %x", visual->visual_id, visual->cls, visual->bits_per_rgb_value, visual->colormap_entries, visual->red_mask, visual->green_mask, visual->blue_mask);
                    visual++;
                }
                depth = (x11_DEPTH*)visual;
            }
            scrn = (x11_SCREEN*)depth;
        }
    }
#endif

    x11_WINDOW window = x11_generate_id(connection);
    x11_GCONTEXT gcontext = x11_generate_id(connection);
    {
        struct {
            x11req_CreateWindow createwindow;
            x11_CARD32 createwindow_values[1];
            x11req_CreateGC creategc;
            x11req_MapWindow mapwindow;
        } reqs;
        reqs.createwindow.opcode = 1;
        reqs.createwindow.depth = connection->roots->root_depth;
        reqs.createwindow.req_len = (sizeof(x11req_CreateWindow) + sizeof(reqs.createwindow_values))/4;
        reqs.createwindow.wid = window;
        reqs.createwindow.parent = connection->roots->root;
        reqs.createwindow.width = w;
        reqs.createwindow.height = h;
        reqs.createwindow.x = (connection->roots->width_in_pixels - w)/2;
        reqs.createwindow.y = (connection->roots->height_in_pixels - h)/2;
        reqs.createwindow.border_width = 0;
        reqs.createwindow.cls = 1; /* InputOutput */
        reqs.createwindow.visual = 0; /* CopyFromParent */
        reqs.createwindow.value_mask = 0x00000002;
        reqs.createwindow_values[0] = D3DCOLOR_RGBA(180, 180, 255, 255);

        reqs.creategc.opcode = 55;
        reqs.creategc.req_len = sizeof(x11req_CreateGC)/4;
        reqs.creategc.cid = gcontext;
        reqs.creategc.drawable = window;
        reqs.creategc.value_mask = 0x00000000;

        reqs.mapwindow.opcode = 8;
        reqs.mapwindow.req_len = sizeof(x11req_MapWindow)/4;
        reqs.mapwindow.window = window;
        status = send(socket, (const char*)&reqs, sizeof(reqs), 0);
    }

    {
        int row_pitch = (w*state->bitmap_format_scanline_unit)/8;
        int putimage_len = sizeof(x11req_PutImage) + row_pitch*h;
        x11req_PutImage* putimage = HeapAlloc(connection->hheap, 0, putimage_len);
        putimage->opcode = 72;
        putimage->format = 2; /* ZPixmap */
        putimage->req_len = x11_PAD(putimage_len)/4;
        putimage->drawable = window;
        putimage->gc = gcontext;
        putimage->width = w;
        putimage->height = h;
        putimage->dst_x = 0;
        putimage->dst_y = 0;
        putimage->left_pad = 0;
        putimage->depth = connection->roots->root_depth;

        D3DLOCKED_RECT locked;
        hr = dxreadback_surface->lpVtbl->LockRect(dxreadback_surface, &locked, NULL, D3DLOCK_READONLY);        
        if (FAILED(hr)) {
            printlnf("IDirect3DSurface9::LockRect failed: %s (code: 0x%08lx)", get_d3derr_string(hr), hr);
            goto ENDD3D;
        }

        x11_BYTE* pixel_data = x11req_PutImage_data(putimage);
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                x11_BYTE* p_srcpixel = &((x11_BYTE*)locked.pBits)[y * locked.Pitch + x*4];
                x11_BYTE* p_pixel = &pixel_data[y*row_pitch + x*4];
                p_pixel[0] = p_srcpixel[0];
                p_pixel[1] = p_srcpixel[1];
                p_pixel[2] = p_srcpixel[2];
            }
        }

        status = send(socket, (const char*)putimage, putimage_len, 0);
        HeapFree(connection->hheap, 0, putimage);
    }

    while (1) {
        x11res_Reply reply;
        status = recv(socket, (char*)&reply, sizeof(reply), 0);
        if (!status) {
            printlnf("Connection closed");
            break;
        }

        if (status == SOCKET_ERROR) {
            printlnf("IO error: Connection possibly lost?");
            break;
        }

        printlnf("Got reply: %u %u", reply.reply, reply.req);
        if (reply.reply == 0) {
            printlnf("Reply got error... %u (seq: %u)", reply.req, reply.sequence_number);
            break;
        }
    }

    x11_free_id(connection, gcontext);
    x11_free_id(connection, window);
ENDD3D:
    #define SAFERELEASE(p) if (p) (p)->lpVtbl->Release(p)
    SAFERELEASE(dxrendertarget_surface);
    SAFERELEASE(dxreadback_surface);
    SAFERELEASE(dxdevice);
    SAFERELEASE(direct3d);
ENDCONNECTION:
    x11_destroy_connection(connection);
ENDSOCKET:
    closesocket(socket);
END:
    WSACleanup();
    return status;
}
