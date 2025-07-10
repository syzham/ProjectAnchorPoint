using System.Runtime.InteropServices;

namespace GameScripts;

public class Transform(IntPtr transformPtr)
{
    [DllImport("ScriptExport.dll")] 
    private static extern void SetPosition(IntPtr transform, float x, float y, float z);
    
    [DllImport("ScriptExport.dll")] 
    private static extern void SetRotation(IntPtr transform, float x, float y, float z);
    
    [DllImport("ScriptExport.dll")] 
    private static extern void SetScale(IntPtr transform, float x, float y, float z);
    
    [DllImport("ScriptExport.dll")] 
    private static extern void AddPosition(IntPtr transform, float x, float y, float z);
    
    [DllImport("ScriptExport.dll")] 
    private static extern void AddRotation(IntPtr transform, float x, float y, float z, bool yClamped);
    
    [DllImport("ScriptExport.dll")] 
    private static extern void AddScale(IntPtr transform, float x, float y, float z);
    
    [DllImport("ScriptExport.dll")] 
    private static extern void MoveForwards(IntPtr transform, float speed);
    
    [DllImport("ScriptExport.dll")] 
    private static extern void MoveRight(IntPtr transform, float speed);

    public void SetPosition(float x, float y, float z)
    {
        SetPosition(transformPtr, x, y, z);
    }

    public void SetRotation(float x, float y, float z)
    {
        SetRotation(transformPtr, x, y, z);
    }

    public void SetScale(float x, float y, float z)
    {
        SetScale(transformPtr, x, y, z);
    }

    public void AddPosition(float x, float y, float z)
    {
        AddPosition(transformPtr, x, y, z);
    }

    public void AddRotation(float x, float y, float z, bool yClamped = false)
    {
        AddRotation(transformPtr, x, y, z, yClamped);
    }

    public void AddScale(float x, float y, float z)
    {
        AddScale(transformPtr, x, y, z);
    }

    public void MoveForwards(float speed)
    {
        MoveForwards(transformPtr, speed);
    }

    public void MoveRight(float speed)
    {
        MoveRight(transformPtr, speed);
    }
}