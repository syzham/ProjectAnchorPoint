using System.Numerics;
using System.Runtime.InteropServices;

namespace GameScripts;

public enum Key {
    A = 0x41,
    B = 0x42,
    C = 0x43,
    D = 0x44,
    E = 0x45,
    F = 0x46,
    G = 0x47,
    H = 0x48,
    I = 0x49,
    J = 0x4A,
    K = 0x4B,
    L = 0x4C,
    M = 0x4D,
    N = 0x4E,
    O = 0x4F,
    P = 0x50,
    Q = 0x51,
    R = 0x52,
    S = 0x53,
    T = 0x54,
    U = 0x55,
    V = 0x56,
    W = 0x57,
    X = 0x58,
    Y = 0x59,
    Z = 0x5A,

    Left = 0x25,
    Up = 0x26,
    Right = 0x27,
    Down = 0x28,

    Space = 0x20,
    Esc = 0x1B,
    Enter = 0x0D,
    Shift = 0x10,
    Ctrl = 0x11,
    Alt = 0x12,
    Tab = 0x09
}


public class InputManager
{
    public static IntPtr inputManagerPtr;
    
    [DllImport("ScriptExport.dll")] 
    private static extern void getMouseMovement(IntPtr inputManager, out float dx, out float dy);

    [DllImport("ScriptExport.dll")]
    private static extern void isKeyDown(IntPtr inputManager, int keycode, out bool isDown);
    
    public static Vector2 GetMouseEvent()
    {
        getMouseMovement(inputManagerPtr, out var dx, out var dy);
        return new Vector2(dx, dy);
    }

    public static bool KeyDown(Key keycode)
    {
        isKeyDown(inputManagerPtr, (int)keycode, out var isDown);
        return isDown;
    }
}