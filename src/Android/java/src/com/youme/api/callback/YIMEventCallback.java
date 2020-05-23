package com.youme.api.callback;


import com.youme.api.YIMMessage;

/**
 * Created by winnie on 2018/7/30.
 */

public class YIMEventCallback {
    public interface ResultCallback<T> {

        public void onSuccess(T info);
        public void onFailed(int errorcode, T info);
    }

    public interface OperationCallback {
        public void onSuccess();
        public void onFailed(int errorcode);
    }

}

