
namespace YouMe
{
    public static class TimeUtil
    {
        private static readonly System.DateTime Epoch = new System.DateTime(1970, 1, 1, 0, 0, 0, System.DateTimeKind.Utc);

        public static uint ConvertToTimestamp(System.DateTime value)
        {
            System.TimeSpan elapsedTime = value.ToUniversalTime() - Epoch;
            return System.Convert.ToUInt32(elapsedTime.TotalSeconds);
        }
    }
}