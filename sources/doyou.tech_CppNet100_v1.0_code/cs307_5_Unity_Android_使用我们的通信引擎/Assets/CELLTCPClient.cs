using UnityEngine;
using System.Runtime.InteropServices;
using System;
using AOT;


public enum NetCMD
{
    LOGIN,
    LOGIN_RESULT,
    LOGOUT,
    LOGOUT_RESULT,
    NEW_USER_JOIN,
    C2S_HEART,
    S2C_HEART,
    ERROR
};

public class CELLTCPClient : MonoBehaviour {

    public delegate void OnNetMsgCallBack(IntPtr csObj, IntPtr data, int len);

    [MonoPInvokeCallback(typeof(OnNetMsgCallBack))]
    private static void OnNetMsgCallBack1(IntPtr csObj, IntPtr data, int len)
    {
        //Debug.Log("OnNetMsgCallBack1:"+len);
        //将将C++传入的对象指针还原为C#对象
        GCHandle h = GCHandle.FromIntPtr(csObj);
        CELLTCPClient obj = h.Target as CELLTCPClient;
        if(obj)
        {
            obj.OnNetMsgBytes(data, len);
        }
    }

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern IntPtr CELLClient_Create(IntPtr csObj, OnNetMsgCallBack cb, int sendSize, int recvSize);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLClient_Connect(IntPtr cppClientObj, string ip, short port);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLClient_OnRun(IntPtr cppClientObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern void CELLClient_Close(IntPtr cppClientObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern int CELLClient_SendData(IntPtr cppClientObj, byte[] data, int len);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern int CELLClient_SendWriteStream(IntPtr cppClientObj, IntPtr cppStreamObj);

    ////////////
    private GCHandle _handleThis;
    // this对象的指针 在C++消息回调中传回
    IntPtr _csThisObj = IntPtr.Zero;
    //C++ NativeTCPClient 对象的指针
    IntPtr _cppClientObj = IntPtr.Zero;
    //
    public void Create()
    {
        _handleThis = GCHandle.Alloc(this);
        _csThisObj = GCHandle.ToIntPtr(_handleThis);
        _cppClientObj = CELLClient_Create(_csThisObj, OnNetMsgCallBack1,10240, 10240);
    }

    public bool Connect(string ip, short port)
    {
        if (_cppClientObj == IntPtr.Zero)
            return false;
        return CELLClient_Connect(_cppClientObj, ip, port);
    }

    public bool OnRun()
    {
        if (_cppClientObj == IntPtr.Zero)
            return false;

        return CELLClient_OnRun(_cppClientObj);
    }

    public void Close()
    {
        if (_cppClientObj == IntPtr.Zero)
            return;

        CELLClient_Close(_cppClientObj);
        _cppClientObj = IntPtr.Zero;
        _handleThis.Free();
    }

    public int SendData(byte[] data)
    {
        if (_cppClientObj == IntPtr.Zero)
            return 0;

        return CELLClient_SendData(_cppClientObj, data, data.Length);
    }

    public int SendData(CELLSendStream ss)
    {
        return SendData(ss.Array);
    }

    public int SendData(CELLWriteStream ws)
    {
        if (_cppClientObj == IntPtr.Zero)
            return 0;

        return CELLClient_SendWriteStream(_cppClientObj, ws.cppObj);
    }


    public virtual void OnNetMsgBytes(IntPtr data, int len)
    {

    }
    ////////////

    // Use this for initialization
    void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
