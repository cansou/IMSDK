using System;

namespace YouMe{
    public class IMUser : IUser
    {
        string userID;
        // string token;

        public string UserID {
            get{
                return userID;
            }
        }
        
        // public string Token {
        //     get{
        //         return token;
        //     }
        // }

        public IMUser(string userID){
            this.userID = userID;
            // this.token = token;
        }
    }
}