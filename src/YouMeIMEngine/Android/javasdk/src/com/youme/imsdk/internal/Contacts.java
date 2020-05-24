package com.youme.imsdk.internal;

import com.google.gson.annotations.SerializedName;
import com.youme.imsdk.ContactsSessionInfo;

import java.util.ArrayList;

/**
 * Created by fishg on 2017/2/19.
 */
public class Contacts {
    @SerializedName("contacts")
    public ArrayList<ContactsSessionInfo> contacts;

}
