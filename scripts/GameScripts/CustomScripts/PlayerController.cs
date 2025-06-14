using System.Numerics;

namespace GameScripts.CustomScripts;

public class PlayerController : ICustomScripts
{
    public Transform transform;
    public float Sensitivity = 0.0001f;

    public void Update()
    {
        var deltas = InputManager.GetMouseEvent();
        transform.AddRotation(deltas.X * Sensitivity, -1 * deltas.Y * Sensitivity, 0, true);
    }
}