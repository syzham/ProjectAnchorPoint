using System.Numerics;

namespace GameScripts.CustomScripts;

public class RotateCube : ICustomScripts
{
    public Transform transform;
    public Vector3 delta;
    
    public void Update()
    {
        transform.AddRotation(delta.X, delta.Y, delta.Z);
    }
}