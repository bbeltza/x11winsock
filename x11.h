#ifndef X11_H
#define X11_H

#include <WinSock2.h>

#define x11_PAD(x) ( ((x) + 0x3)&~0x3 )

typedef BYTE x11_BYTE;
typedef INT8 x11_INT8;
typedef INT16 x11_INT16;
typedef INT32 x11_INT32;
typedef UINT8 x11_CARD8;
typedef UINT16 x11_CARD16;
typedef UINT32 x11_CARD32;

typedef x11_CARD32 x11_WINDOW;
typedef x11_CARD32 x11_PIXMAP;
typedef x11_CARD32 x11_CURSOR;
typedef x11_CARD32 x11_FONT;
typedef x11_CARD32 x11_GCONTEXT;
typedef x11_CARD32 x11_COLORMAP;
typedef x11_CARD32 x11_DRAWABLE;
typedef x11_CARD32 x11_FONTABLE;
typedef x11_CARD32 x11_ATOM;
typedef x11_CARD32 x11_VISUALID;
typedef x11_CARD32 x11_VALUE;

typedef x11_CARD32 x11_TIMESTAMP;

    /**/

#define x11_None 0

#define x11_Forget    0
#define x11_Unmap     0
#define x11_NorthWest 1
#define x11_North     2
#define x11_NorthEast 3
#define x11_West      4
#define x11_Center    5
#define x11_East      6
#define x11_SouthWest 7
#define x11_South     8
#define x11_SouthEast 9
#define x11_Static    10

#define x11_False 0
#define x11_True  1

#define x11_MaskKeyPress             0x00000001
#define x11_MaskKeyRelease           0x00000002
#define x11_MaskButtonPress          0x00000004
#define x11_MaskButtonRelease        0x00000008
#define x11_MaskEnterWindow          0x00000010
#define x11_MaskLeaveWindow          0x00000020
#define x11_MaskPointerMotion        0x00000040
#define x11_MaskPointerMotionHint    0x00000080
#define x11_MaskButton1Motion        0x00000100
#define x11_MaskButton2Motion        0x00000200
#define x11_MaskButton3Motion        0x00000400
#define x11_MaskButton4Motion        0x00000800
#define x11_MaskButton5Motion        0x00001000
#define x11_MaskButtonMotion         0x00002000
#define x11_MaskKeymapState          0x00004000
#define x11_MaskExposure             0x00008000
#define x11_MaskVisibilityChange     0x00010000
#define x11_MaskStructureNotify      0x00020000
#define x11_MaskResizeRedirect       0x00040000
#define x11_MaskSubstructureNotify   0x00080000
#define x11_MaskSubstructureRedirect 0x00100000
#define x11_MaskFocusChange          0x00200000
#define x11_MaskPropertyChange       0x00400000
#define x11_MaskColormapChange       0x00800000
#define x11_MaskOwnerGrabButton      0x01000000

#define x11_MaskShift   0x0001
#define x11_MaskLocko   0x0002
#define x11_MaskControl 0x0004
#define x11_MaskMod1    0x0008
#define x11_MaskMod2    0x0010
#define x11_MaskMod3    0x0020
#define x11_MaskMod4    0x0040
#define x11_MaskMod5    0x0080
#define x11_MaskButton1 0x0100
#define x11_MaskButton2 0x0200
#define x11_MaskButton3 0x0400
#define x11_MaskButton4 0x0800
#define x11_MaskButton5 0x1000

/**/

#define x11_ErrorRequest        1
#define x11_ErrorValue          2
#define x11_ErrorWindow         3
#define x11_ErrorPixmap         4
#define x11_ErrorAtom           5
#define x11_ErrorCursor         6
#define x11_ErrorFont           7
#define x11_ErrorMatch          8
#define x11_ErrorDrawable       9
#define x11_ErrorAccess         10
#define x11_ErrorAlloc          11
#define x11_ErrorColormap       12
#define x11_ErrorGContext       13
#define x11_ErrorIDChoice       14
#define x11_ErrorName           15
#define x11_ErrorLength         16
#define x11_ErrorImplementation 17

/**/

#define x11_KeyPress             2
#define x11_KeyRelease           3
#define x11_ButtonPress          4
#define x11_ButtonRelease        5
#define x11_MotionNotify         6
#define x11_EnterNotify          7
#define x11_LeaveNotify          8
#define x11_FocusIn              9
#define x11_FocusOut             10
#define x11_KeymapNotify         11
#define x11_Expose               12
#define x11_GraphicsExposure     13
#define x11_NoExposure           14
#define x11_VisibilityNotify     15
#define x11_CreateNotify         16
#define x11_DestroyNotify        17
#define x11_UnmapNotify          18
#define x11_MapNotify            19
#define x11_MapRequest           20
#define x11_ReparentNotify       21
#define x11_ConfigureNotify      22
#define x11_ConfigureRequest     23
#define x11_GravityNotify        24
#define x11_ResizeRequest        25
#define x11_CirculateNotify      26
#define x11_CirculateRequest     27
#define x11_PropertyNotify       28
#define x11_SelectionClear       29
#define x11_SelectionRequest     30
#define x11_SelectionNotify      31
#define x11_ColormapNotify       32
#define x11_ClientMessage        33
#define x11_MappingNotify        34

/**/

typedef x11_CARD8 x11_BITGRAVITY;
typedef x11_CARD8 x11_WINGRAVITY;
typedef x11_CARD8 x11_BOOL;
typedef x11_CARD8 x11_EVENT;
typedef x11_CARD8 x11_POINTEREVENT;
typedef x11_CARD8 x11_DEVICEEVENT;
typedef x11_CARD32 x11_KEYSYM;
typedef x11_CARD8 x11_KEYCODE;
typedef x11_CARD8 x11_BUTTON;

typedef x11_CARD32 x11_BITMASK;
typedef x11_BITMASK x11_KEYMASK;
typedef x11_BITMASK x11_BUTMASK;
typedef x11_BITMASK x11_KEYBUTMASK;

typedef struct x11_CHAR2B {
    x11_CARD8 byte1;
    x11_CARD8 byte2;
} x11_CHAR2B;

typedef struct x11_POINT {
    x11_INT16 x;
    x11_INT16 y;
} x11_POINT;

typedef struct x11_RECTANGLE {
    x11_INT16 x;
    x11_INT16 y;
    x11_CARD16 width;
    x11_CARD16 height;
} x11_RECTANGLE;

typedef struct x11_ARC {
    x11_INT16 x;
    x11_INT16 y;
    x11_CARD16 width;
    x11_CARD16 height;
    x11_INT16 angle1;
    x11_INT16 angle2;
} x11_ARC;

typedef struct x11_FORMAT {
    x11_CARD8 depth;
    x11_CARD8 bits_per_pixel;
    x11_CARD8 scanline_pad;
    x11_CARD8 _unused[5];
} x11_FORMAT;

typedef struct x11_SCREEN {
    x11_WINDOW root;
    x11_COLORMAP default_colormap;
    x11_CARD32 white_pixel;
    x11_CARD32 black_pixel;
    x11_BITMASK current_input_masks;
    x11_CARD16 width_in_pixels;
    x11_CARD16 height_in_pixels;
    x11_CARD16 width_in_millimeters;
    x11_CARD16 height_in_millimeters;
    x11_CARD16 min_installed_maps;
    x11_CARD16 max_installed_maps;
    x11_VISUALID root_visual;
    x11_CARD8 backing_stores;
    x11_BOOL save_unders;
    x11_CARD8 root_depth;
    x11_CARD8 num_allowed_depths;
} x11_SCREEN;

#define X11_GET_SCREEN_DEPTHS(p_screen) ( (x11_DEPTH*)((p_screen) + 1) )
x11_SCREEN* X11_GET_SCREEN_NEXT(x11_SCREEN* screen);

typedef struct x11_DEPTH {
    x11_CARD8 depth;
    x11_CARD8 _unused1;
    x11_CARD16 num_visuals;
    x11_CARD32 _unused;
} x11_DEPTH;

#define X11_GET_DEPTH_VISUALS(p_depth) ( (x11_VISUALTYPE*)((p_depth) + 1) )
#define X11_GET_DEPTH_NEXT(p_depth) ( (x11_DEPTH*)(((x11_CARD8*)((p_depth) + 1)) + sizeof(x11_VISUALTYPE*)*((p_depth)->num_visuals)) )

typedef struct x11_VISUALTYPE {
    x11_VISUALID visual_id;
    x11_CARD8 cls;
    x11_CARD8 bits_per_rgb_value;
    x11_CARD16 colormap_entries;
    x11_CARD32 red_mask;
    x11_CARD32 green_mask;
    x11_CARD32 blue_mask;
    x11_CARD32 _unused;
} x11_VISUALTYPE;

/**/

typedef struct x11req_Connect {
    x11_CARD8 byte_order;
    x11_CARD8 _unused1;
    x11_CARD16 protocol_major_version;
    x11_CARD16 protocol_minor_version;
    x11_CARD16 auth_protocol_name_len;
    x11_CARD16 auth_protocol_data_len;
    x11_CARD16 _unused2;
} x11req_Connect;

typedef struct x11res_Connect {
    x11_CARD8 success;
    x11_CARD8 reason_len;
    x11_CARD16 protocol_major_version;
    x11_CARD16 protocol_minor_version;
    x11_CARD16 additional_data_len;
} x11res_Connect;

typedef struct x11res_ConnectSuccess {
    x11_CARD32 release_number;
    x11_CARD32 resource_id_base;
    x11_CARD32 resource_id_mask;
    x11_CARD32 motion_buffer_size;
    x11_CARD16 vendor_len;
    x11_CARD16 max_req_len;
    x11_CARD8 num_screens;
    x11_CARD8 num_formats;
    x11_CARD8 image_byte_order;
    x11_CARD8 bitmap_format_bit_order;
    x11_CARD8 bitmap_format_scanline_unit;
    x11_CARD8 bitmap_format_scanline_pad;
    x11_CARD8 min_keycode;
    x11_CARD8 max_keycode;
    x11_CARD32 _unused;
} x11res_ConnectSuccess;

typedef struct x11res_Reply {
    x11_CARD8 reply;
    x11_CARD8 req; /* Request specific */
    x11_CARD16 sequence_number;
    x11_CARD32 reply_length;
    x11_CARD8 data[24]; /* Request specific too */
} x11res_Reply;

typedef struct x11req_CreateWindow {
    x11_CARD8 opcode; /* 1 */
    x11_CARD8 depth;
    x11_CARD16 req_len; /* sizeof(x11req_CreateWindow)/4 + n */
    x11_WINDOW wid;
    x11_WINDOW parent;
    x11_INT16 x;
    x11_INT16 y;
    x11_CARD16 width;
    x11_CARD16 height;
    x11_CARD16 border_width;
    x11_CARD16 cls;
    x11_VISUALID visual;
    x11_BITMASK value_mask;
} x11req_CreateWindow;

typedef struct x11req_MapWindow {
    x11_CARD8 opcode; /* 8 */
    x11_CARD8 _unused;
    x11_CARD16 req_len; /* sizeof(x11req_MapWindow)/4 */
    x11_WINDOW window;
} x11req_MapWindow;

#define X11req_CreateWindow_values(p_req) ( (x11_VALUE*)((p_req) + 1) )

typedef struct x11req_CreateGC {
    x11_CARD8 opcode; /* 55 */
    x11_CARD8 _unused;
    x11_CARD16 req_len; /* sizeof(x11req_CreateGC)/4 + n */
    x11_GCONTEXT cid;
    x11_DRAWABLE drawable;
    x11_BITMASK value_mask;
} x11req_CreateGC;

typedef struct x11req_PutImage {
    x11_CARD8 opcode; /* 72 */
    x11_CARD8 format;
    x11_CARD16 req_len; /* (sizeof(x11req_PutImage)+x11_PAD(n))/4 */
    x11_DRAWABLE drawable;
    x11_GCONTEXT gc;
    x11_CARD16 width;
    x11_CARD16 height;
    x11_INT16 dst_x;
    x11_INT16 dst_y;
    x11_CARD8 left_pad;
    x11_CARD8 depth;
    x11_CARD16 _unused;
} x11req_PutImage;

#define x11req_PutImage_data(p_req) (x11_BYTE*)((p_req) + 1)

/**/

typedef struct x11_connection
{
    HANDLE hheap;
    SOCKET socket;
   
    x11_CARD32 last_id;
    x11_CARD32 max_id;
    x11_CARD32* id_freelist;
    UINT id_freelist_size;
    UINT id_freelist_cap;

    char* vendor; /* WARNING: `vendor` is NOT NULL-terminated */
    x11_FORMAT* pixmap_formats;
    x11_SCREEN* roots;
} x11_connection;

#define X11_GET_CONNECTION_STATE(connection) ( (x11res_ConnectSuccess*)((connection) + 1) )

typedef struct x11_connection_info {
    const char* auth_protocol_name;
    const void* auth_protocol_data;
    int auth_protocol_name_len;
    int auth_protocol_data_len;
} x11_connection_info;

x11_connection* x11_connect(SOCKET unconnected_socket, const x11_connection_info* info);
void x11_destroy_connection(x11_connection* connection);

x11_CARD32 x11_generate_id(x11_connection* connection);
void x11_free_id(x11_connection* connection, x11_CARD32 id);

#endif
