{
   Copyright 2005 Sandy Barbour and Ben Supnik
   
   All rights reserved.  See license.txt for usage.
   
   X-Plane SDK Version: 1.0.2                                                  
}

UNIT XPLMDisplay;
INTERFACE
{
   XPLM Display APIs - THEORY OF OPERATION 
   
   This API provides the basic hooks to draw in X-Plane and create user 
   interface. All X-Plane drawing is done in OpenGL.  The X-Plane plug-in 
   manager takes care of properly setting up the OpenGL context and matrices.  
   You do not decide when in your code's  execution to draw; X-Plane tells you 
   when it is ready to have your plugin draw. 
   
   X-Plane's drawing strategy is straightforward: every "frame" the screen is 
   rendered by drawing the 3-d scene (dome, ground, objects, airplanes, etc.)  
   and then drawing the cockpit on top of it.  Alpha blending is used to 
   overlay the cockpit over the world (and the gauges over the panel, etc.). 
   
   There are two ways you can draw: directly and in a window. 
   
   Direct drawing involves drawing to the screen before or after X-Plane 
   finishes a phase of drawing.  When you draw directly, you can specify 
   whether x-plane is to complete this phase or not.  This allows you to do 
   three things: draw before x-plane does (under it), draw after x-plane does 
   (over it), or draw instead of  x-plane. 
   
   To draw directly, you register a callback and specify what phase you want 
   to intercept.  The plug-in manager will call you over and over to draw that 
   phase. 
   
   Direct drawing allows you to override scenery, panels, or anything.  Note 
   that  you cannot assume that you are the only plug-in drawing at this 
   phase. 
   
   Window drawing provides slightly higher level functionality.  With window 
   drawing you create a window that takes up a portion of the screen.  Window 
   drawing is always two dimensional.  Window drawing is front-to-back 
   controlled; you can  specify that you want your window to be brought on 
   top, and other plug-ins may put their window on top of you.  Window drawing 
   also allows you to sign up for  key presses and receive mouse clicks. 
   
   There are three ways to get keystrokes: 
   
   If you create a window, the window can take keyboard focus.  It will then 
   receive all keystrokes.  If no window has focus, X-Plane receives 
   keystrokes.  Use this to implement typing in dialog boxes, etc.  Only one 
   window may have focus at a time; your window will be notified if it loses 
   focus. 
   
   If you need to associate key strokes with commands/functions in your 
   plug-in, use a hot key.  A hoy is a key-specific callback.  Hotkeys are 
   sent based on virtual key strokes, so any key may be distinctly mapped with 
   any modifiers.  Hot keys  can be remapped by other plug-ins.  As a plug-in, 
   you don't have to worry about  what your hot key ends up mapped to; other 
   plug-ins may provide a UI for remapping keystrokes.  So hotkeys allow a 
   user to resolve conflicts and customize keystrokes. 
   
   If you need low level access to the keystroke stream, install a key 
   sniffer.  Key sniffers can be installed above everything or right in front 
   of the sim.                                                                 
}

USES   XPLMDefs;
   {$A4}
{$IFDEF MSWINDOWS}
   {$DEFINE DELPHI}
{$ENDIF}
{___________________________________________________________________________
 * DRAWING CALLBACKS
 ___________________________________________________________________________}
{
   Basic drawing callbacks, for low level intercepting of render loop. The 
   purpose of drawing callbacks is to provide targeted additions or  
   replacements to x-plane's graphics environment (for example, to add extra 
   custom objects, or replace drawing of the AI aircraft).  Do not assume that 
   the drawing callbacks will be called in the order implied by the 
   enumerations. Also do not assume that each drawing phase ends before 
   another begins; they may be nested.                                         
}

   {
    XPLMDrawingPhase
    
    This constant indicates which part of drawing we are in.  Drawing is done 
    from the back to the front.  We get a callback before or after each item. 
    Metaphases provide access to the beginning and end of the 3d (scene) and 2d 
    (cockpit) drawing in a manner that is independent of new phases added  via 
    x-plane implementation. 
    
    WARNING: As X-Plane's scenery evolves, some drawing phases may cease to  
    exist and new ones may be invented.  If you need a particularly specific 
    use of these codes, consult Austin and/or be prepared to revise your code 
    as X-Plane evolves.                                                         
   }
TYPE
   XPLMDrawingPhase = (
     { This is the earliest point at which you can draw in 3-d.                    }
     xplm_Phase_FirstScene                    = 0,

     { Drawing of land and water.                                                  }
     xplm_Phase_Terrain                       = 5,

     { Drawing runways and other airport detail.                                   }
     xplm_Phase_Airports                      = 10,

     { Drawing roads, trails, trains, etc.                                         }
     xplm_Phase_Vectors                       = 15,

     { 3-d objects (houses, smokestacks, etc.                                      }
     xplm_Phase_Objects                       = 20,

     { External views of airplanes, both yours and the AI aircraft.                }
     xplm_Phase_Airplanes                     = 25,

     { This is the last point at which you can draw in 3-d.                        }
     xplm_Phase_LastScene                     = 30,

     { This is the first phase where you can draw in 2-d.                          }
     xplm_Phase_FirstCockpit                  = 35,

     { The non-moving parts of the aircraft panel.                                 }
     xplm_Phase_Panel                         = 40,

     { The moving parts of the aircraft panel.                                     }
     xplm_Phase_Gauges                        = 45,

     { Floating windows from plugins.                                              }
     xplm_Phase_Window                        = 50,

     { The last change to draw in 2d.                                              }
     xplm_Phase_LastCockpit                   = 55
   );
   PXPLMDrawingPhase = ^XPLMDrawingPhase;

   {
    XPLMDrawCallback_f
    
    This is the prototype for a low level drawing callback.  You are passed in 
    the phase and whether it is before or after.  If you are before the phase, 
    return 1 to let x-plane draw or 0 to suppress x-plane drawing.  If you are 
    after the phase the return value is ignored. 
    
    Refcon is a unique value that you specify when registering the callback, 
    allowing you to slip a pointer to your own data to the callback. 
    
    Upon entry the OpenGL context will be correctly set up for you and OpenGL 
    will be in 'local' coordinates for 3d drawing and panel coordinates for 2d 
    drawing.  The OpenGL state (texturing, etc.) will be unknown.               
   }
     XPLMDrawCallback_f = FUNCTION(
                                    inPhase             : XPLMDrawingPhase;    
                                    inIsBefore          : integer;    
                                    inRefcon            : pointer) : integer; cdecl;   

   {
    XPLMKeySniffer_f
    
    This is the prototype for a low level key-sniffing function.  Window-based 
    UI _should not use this_!  The windowing system provides high-level 
    mediated keyboard access.  By comparison, the key sniffer provides low 
    level keyboard access. 
    
    Key sniffers are provided to allow libraries to provide non-windowed user  
    interaction.  For example, the MUI library uses a key sniffer to do pop-up 
    text entry. 
    
    inKey is the character pressed, inRefCon is a value you supply during 
    registration. Return 1 to pass the key on to the next sniffer, the window 
    mgr, x-plane, or whomever is down stream.  Return 0 to consume the key.     
   }
     XPLMKeySniffer_f = FUNCTION(
                                    inChar              : char;    
                                    inFlags             : XPLMKeyFlags;    
                                    inVirtualKey        : char;    
                                    inRefcon            : pointer) : integer; cdecl;   

   {
    XPLMRegisterDrawCallback
    
    This routine registers a low level drawing callback.  Pass in the phase you 
    want to be called for and whether you want to be called before or after.  
    This routine returns 1 if the registration was successful, or 0 if the 
    phase does not exist in this version of x-plane.  You may register a 
    callback multiple times for the same or different phases as long as the 
    refcon is unique each time.                                                 
   }
   FUNCTION XPLMRegisterDrawCallback(
                                        inCallback          : XPLMDrawCallback_f;    
                                        inPhase             : XPLMDrawingPhase;    
                                        inWantsBefore       : integer;    
                                        inRefcon            : pointer) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMUnregisterDrawCallback
    
    This routine unregisters a draw callback.  You must unregister a callback 
    for each  time you register a callback if you have registered it multiple 
    times with different refcons.  The routine returns 1 if it can find the 
    callback to unregister, 0 otherwise.                                        
   }
   FUNCTION XPLMUnregisterDrawCallback(
                                        inCallback          : XPLMDrawCallback_f;    
                                        inPhase             : XPLMDrawingPhase;    
                                        inWantsBefore       : integer;    
                                        inRefcon            : pointer) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMRegisterKeySniffer
    
    This routine registers a key sniffing callback.  You specify whether you 
    want to sniff before the window system, or only sniff keys the window 
    system does not consume.  You should ALMOST ALWAYS sniff non-control keys 
    after the window system.  When the window system consumes a key, it is 
    because the user has "focused" a window.  Consuming the key or taking 
    action based on the key will produce very weird results.  Returns 1 if 
    successful.                                                                 
   }
   FUNCTION XPLMRegisterKeySniffer(
                                        inCallback          : XPLMKeySniffer_f;    
                                        inBeforeWindows     : integer;    
                                        inRefcon            : pointer) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMUnregisterKeySniffer
    
    This routine unregisters a key sniffer.  You must unregister a key sniffer 
    for every time you register one with the exact same signature.  Returns 1 
    if successful.                                                              
   }
   FUNCTION XPLMUnregisterKeySniffer(
                                        inCallback          : XPLMKeySniffer_f;    
                                        inBeforeWindows     : integer;    
                                        inRefcon            : pointer) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

{___________________________________________________________________________
 * WINDOW API
 ___________________________________________________________________________}
{
   Window API, for higher level drawing with UI interaction. 
   
   Note: all 2-d (and thus all window drawing) is done in 'cockpit pixels'.  
   Even when the OpenGL window contains more than 1024x768 pixels, the cockpit 
   drawing is magnified so that only 1024x768 pixels are available.            
}

   {
    XPLMMouseStatus
    
    When the mouse is clicked, your mouse click routine is called repeatedly.  
    It is first called with the mouse down message.  It is then called zero or 
    more times with the mouse-drag message, and finally it is called once with 
    the mouse up message.  All of these messages will be directed to the same 
    window.                                                                     
   }
TYPE
   XPLMMouseStatus = (
     xplm_MouseDown                           = 1,

     xplm_MouseDrag                           = 2,

     xplm_MouseUp                             = 3
   );
   PXPLMMouseStatus = ^XPLMMouseStatus;

   {
    XPLMWindowID
    
    This is an opaque identifier for a window.  You use it to control your 
    window. When you create a window, you will specify callbacks to handle 
    drawing and mouse interaction, etc.                                         
   }
   XPLMWindowID = pointer;
   PXPLMWindowID = ^XPLMWindowID;

   {
    XPLMDrawWindow_f
    
    This function handles drawing.  You are passed in your window and its 
    refcon. Draw the window.  You can use XPLM functions to find the current 
    dimensions of your window, etc.  When this callback is called, the OpenGL 
    context will be set properly for cockpit drawing. NOTE: Because you are 
    drawing your window over a background, you can make a translucent window 
    easily by simply not filling in your entire window's bounds.                
   }
     XPLMDrawWindow_f = PROCEDURE(
                                    inWindowID          : XPLMWindowID;    
                                    inRefcon            : pointer); cdecl;   

   {
    XPLMHandleKey_f
    
    This function is called when a key is pressed or keyboard focus is taken 
    away from your window.  If losingFocus is 1, you are losign the keyboard 
    focus, otherwise a key was pressed and inKey contains its character.  You 
    are also passewd your window and a  refcon.                                 
   }
     XPLMHandleKey_f = PROCEDURE(
                                    inWindowID          : XPLMWindowID;    
                                    inKey               : char;    
                                    inFlags             : XPLMKeyFlags;    
                                    inVirtualKey        : char;    
                                    inRefcon            : pointer;    
                                    losingFocus         : integer); cdecl;   

   {
    XPLMHandleMouseClick_f
    
    You receive this call when the mouse button is pressed down or released.  
    Between then these two calls is a drag.  You receive the x and y of the 
    click, your window,  and a refcon.  Return 1 to consume the click, or 0 to 
    pass it through. 
    
    WARNING: passing clicks through windows (as of this writing) causes mouse 
    tracking problems in X-Plane; do not use this feature!                      
   }
     XPLMHandleMouseClick_f = FUNCTION(
                                    inWindowID          : XPLMWindowID;    
                                    x                   : integer;    
                                    y                   : integer;    
                                    inMouse             : XPLMMouseStatus;    
                                    inRefcon            : pointer) : integer; cdecl;   

   {
    XPLMGetScreenSize
    
    This routine returns the size of the size of the X-Plane OpenGL window in 
    pixels.  Please note that this is not the size of the screen when  doing 
    2-d drawing (the 2-d screen is currently always 1024x768, and  graphics are 
    scaled up by OpenGL when doing 2-d drawing for higher-res monitors).  This 
    number can be used to get a rough idea of the amount of detail the user 
    will be able to see when drawing in 3-d.                                    
   }
   PROCEDURE XPLMGetScreenSize(
                                        outWidth            : Pinteger;    { Can be nil }
                                        outHeight           : Pinteger);    { Can be nil }
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMGetMouseLocation
    
    This routine returns the current mouse location in cockpit pixels.  The 
    bottom left corner of the display is 0,0.  Pass NULL to not receive info 
    about either parameter.                                                     
   }
   PROCEDURE XPLMGetMouseLocation(
                                        outX                : Pinteger;    { Can be nil }
                                        outY                : Pinteger);    { Can be nil }
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMCreateWindow
    
    This routine creates a new window.  Pass in the dimensions and offsets to 
    the window's bottom left corner from the bottom left of the screen.  You 
    can specify whether the window is initially visible or not.  Also, you pass 
    in three callbacks to run the window and a refcon.  This function returns a 
    window ID you can use to refer to the new window. 
    
    NOTE: windows do not have "frames"; you are responsible for drawing the 
    background and frame of the window.  Higher level libraries have routines 
    which make this easy.                                                       
   }
   FUNCTION XPLMCreateWindow(
                                        inLeft              : integer;    
                                        inTop               : integer;    
                                        inRight             : integer;    
                                        inBottom            : integer;    
                                        inIsVisible         : integer;    
                                        inDrawCallback      : XPLMDrawWindow_f;    
                                        inKeyCallback       : XPLMHandleKey_f;    
                                        inMouseCallback     : XPLMHandleMouseClick_f;    
                                        inRefcon            : pointer) : XPLMWindowID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMDestroyWindow
    
    This routine destroys a window.  The callbacks are not called after this 
    call. Keyboard focus is removed from the window before destroying it.       
   }
   PROCEDURE XPLMDestroyWindow(
                                        inWindowID          : XPLMWindowID);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMGetWindowGeometry
    
    This routine returns the position and size of a window in cockpit pixels.  
    Pass NULL to not receive any paramter.                                      
   }
   PROCEDURE XPLMGetWindowGeometry(
                                        inWindowID          : XPLMWindowID;    
                                        outLeft             : Pinteger;    { Can be nil }
                                        outTop              : Pinteger;    { Can be nil }
                                        outRight            : Pinteger;    { Can be nil }
                                        outBottom           : Pinteger);    { Can be nil }
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMSetWindowGeometry
    
    This routine allows you to set the position or height aspects of a window.  
   }
   PROCEDURE XPLMSetWindowGeometry(
                                        inWindowID          : XPLMWindowID;    
                                        inLeft              : integer;    
                                        inTop               : integer;    
                                        inRight             : integer;    
                                        inBottom            : integer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMGetWindowIsVisible
    
    This routine returns whether a window is visible.                           
   }
   FUNCTION XPLMGetWindowIsVisible(
                                        inWindowID          : XPLMWindowID) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMSetWindowIsVisible
    
    This routine shows or hides a window.                                       
   }
   PROCEDURE XPLMSetWindowIsVisible(
                                        inWindowID          : XPLMWindowID;    
                                        inIsVisible         : integer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMGetWindowRefCon
    
    This routine returns a windows refcon, the unique value you can use for 
    your own purposes.                                                          
   }
   FUNCTION XPLMGetWindowRefCon(
                                        inWindowID          : XPLMWindowID) : pointer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMSetWindowRefCon
    
    This routine sets a window's reference constant.  Use this to pass data to 
    yourself in the callbacks.                                                  
   }
   PROCEDURE XPLMSetWindowRefCon(
                                        inWindowID          : XPLMWindowID;    
                                        inRefcon            : pointer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMTakeKeyboardFocus
    
    This routine gives a specific window keyboard focus.  Keystrokes will be 
    sent to  that window.  Pass a window ID of 0 to pass keyboard strokes 
    directly to X-Plane.                                                        
   }
   PROCEDURE XPLMTakeKeyboardFocus(
                                        inWindow            : XPLMWindowID);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMBringWindowToFront
    
    This routine brings the window to the front of the Z-order.  Windows are 
    brought to the front when they are created...beyond that you should make 
    sure you are front before handling mouse clicks.                            
   }
   PROCEDURE XPLMBringWindowToFront(
                                        inWindow            : XPLMWindowID);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMIsWindowInFront
    
    This routine returns true if you pass inthe ID of the frontmost visible 
    window.                                                                     
   }
   FUNCTION XPLMIsWindowInFront(
                                        inWindow            : XPLMWindowID) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

{___________________________________________________________________________
 * HOT KEYS
 ___________________________________________________________________________}
{
   Hot Keys - keystrokes that can be managed by others.                        
}

   {
    XPLMHotKey_f
    
    Your hot key callback simply takes a pointer of your choosing.              
   }
TYPE
     XPLMHotKey_f = PROCEDURE(
                                    inRefcon            : pointer); cdecl;   

   {
    XPLMHotKeyID
    
    Hot keys are identified by opaque IDs.                                      
   }
   XPLMHotKeyID = pointer;
   PXPLMHotKeyID = ^XPLMHotKeyID;

   {
    XPLMRegisterHotKey
    
    This routine registers a hot key.  You specify your preferred key stroke 
    virtual key/flag combination, a description of what your callback does (so 
    other plug-ins can describe the plug-in to the user for remapping) and a 
    callback function and opaque pointer to pass in).  A new hot key ID is 
    returned.  During execution, the actual key associated with your hot key 
    may change, but you are insulated from this.                                
   }
   FUNCTION XPLMRegisterHotKey(
                                        inVirtualKey        : char;    
                                        inFlags             : XPLMKeyFlags;    
                                        inDescription       : Pchar;    
                                        inCallback          : XPLMHotKey_f;    
                                        inRefcon            : pointer) : XPLMHotKeyID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMUnregisterHotKey
    
    This API unregisters a hot key.  You can only register your own hot keys.   
   }
   PROCEDURE XPLMUnregisterHotKey(
                                        inHotKey            : XPLMHotKeyID);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMCountHotKeys
    
    Returns the number of current hot keys.                                     
   }
   FUNCTION XPLMCountHotKeys: longint;
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMGetNthHotKey
    
    Returns a hot key by index, for iteration on all hot keys.                  
   }
   FUNCTION XPLMGetNthHotKey(
                                        inIndex             : longint) : XPLMHotKeyID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMGetHotKeyInfo
    
    Returns information about the hot key.  Return NULL for any  parameter you 
    don't want info about.  The description should be at least 512 chars long.  
   }
   PROCEDURE XPLMGetHotKeyInfo(
                                        inHotKey            : XPLMHotKeyID;    
                                        outVirtualKey       : Pchar;    { Can be nil }
                                        outFlags            : PXPLMKeyFlags;    { Can be nil }
                                        outDescription      : Pchar;    { Can be nil }
                                        outPlugin           : PXPLMPluginID);    { Can be nil }
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

   {
    XPLMSetHotKeyCombination
    
    XPLMSetHotKeyCombination remaps a hot keys keystrokes.  You may remap 
    another plugin's keystrokes.                                                
   }
   PROCEDURE XPLMSetHotKeyCombination(
                                        inHotKey            : XPLMHotKeyID;    
                                        inVirtualKey        : char;    
                                        inFlags             : XPLMKeyFlags);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external './Resources/plugins/XPLM.dll';
{$ENDIF}

IMPLEMENTATION
END.
