using System.Numerics;
using System.Reflection;
using System.Runtime.InteropServices;
using GameScripts.CustomScripts;

namespace GameScripts
{
    [ComVisible(true)] 
    public class MyScript
    {
        private static List<ICustomScripts> _scripts = [];
        
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void CreateScriptDelegate([MarshalAs(UnmanagedType.LPStr)] string typeName);
        
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void SetPointerDelegate(
            [MarshalAs(UnmanagedType.LPStr)] string variableName,
            IntPtr valuePtr
        );
        
        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void SetVector3Delegate(
            [MarshalAs(UnmanagedType.LPStr)] string variableName,
            Vector3 valueVec
        );
 

        public static void CreateScript(string scriptName)
        {
            var t = Type.GetType("GameScripts.CustomScripts."+scriptName);
            if (t == null) return;

            if (Activator.CreateInstance(t) is ICustomScripts temp) _scripts.Add(temp);
        }

        public static void SetPointer(string variableName, IntPtr valuePtr)
        {
            var script = _scripts.Last();
            var type = script.GetType();
            var field = type.GetField(variableName, BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic);
            if (field == null) return;
            
            var fieldType = field.FieldType;

            var ctor = fieldType.GetConstructor([typeof(IntPtr)]);
            if (ctor == null) {
                return;
            }

            var instance = ctor.Invoke([valuePtr]);

            field.SetValue(script, instance);
        }
        
        public static void SetVector3(string variableName, Vector3 valueVec)
        {
            var script = _scripts.Last();
            var type = script.GetType();
            var field = type.GetField(variableName);
            if (field == null) return;
            
            field.SetValue(script, valueVec);
        }
        
        [UnmanagedCallersOnly (EntryPoint = "Update")]
        public static void Update()
        {
            foreach (var script in _scripts)
            {
                script.Update();
            }
        }
    }
}