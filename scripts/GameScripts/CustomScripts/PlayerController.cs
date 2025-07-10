namespace GameScripts.CustomScripts;

public class PlayerController : ICustomScripts
{
    public Transform transform;
    public float Sensitivity = 0.0001f;
    public float speed = 0.01f;

    public void Update()
    {
        var deltas = InputManager.GetMouseEvent();
        transform.AddRotation(deltas.X * Sensitivity, -1 * deltas.Y * Sensitivity, 0, true);
        
        if (InputManager.KeyDown(Key.Esc))
            Game.QuitGame();

        if (InputManager.KeyDown(Key.W))
            transform.MoveForwards(speed);
        
        if (InputManager.KeyDown(Key.S))
            transform.MoveForwards(-speed);
        
        if (InputManager.KeyDown(Key.A))
            transform.MoveRight(-speed);
        
        if (InputManager.KeyDown(Key.D))
            transform.MoveRight(speed);
    }
}