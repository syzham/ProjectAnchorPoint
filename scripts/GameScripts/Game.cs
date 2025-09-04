using System.Runtime.InteropServices;

namespace GameScripts;

public class Game
{
    public static float DeltaTime;
    
    [DllImport("ScriptExport.dll")]
    public static extern void QuitGame();
}