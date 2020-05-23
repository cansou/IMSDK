using UnityEngine;

namespace YouMe{
    public static class Log{
        public static void e(string log){
            Debug.LogError(log);
        }
    }
}