using System.Runtime.InteropServices;

namespace GameScripts;

public class Game
{
    [DllImport("ScriptExport.dll")] 
    public static extern void QuitGame(); 
}