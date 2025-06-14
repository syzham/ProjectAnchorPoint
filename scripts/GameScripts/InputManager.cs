using System.Numerics;
using System.Runtime.InteropServices;

namespace GameScripts;

public class InputManager
{
    public static IntPtr inputManagerPtr;
    
    [DllImport("ScriptExport.dll")] 
    private static extern void getMouseMovement(IntPtr inputManager, out float dx, out float dy); 
    
    public static Vector2 GetMouseEvent()
    {
        getMouseMovement(inputManagerPtr, out var dx, out var dy);
        return new Vector2(dx, dy);
    }
}