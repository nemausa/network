using System;
using UnityEngine;

public class CELLClientNode : CELLTCPClient
{
    private string IP = "192.168.1.102";
    private short PORT = 4567;
    // Use this for initialization
    void Start () {
        this.Create();
        this.Connect(IP, PORT);
        Debug.Log(IP);
        Debug.Log(PORT);
    }
	
	// Update is called once per frame
	void Update () {
        this.OnRun();

        ////CELLSendStream 由C#实现
        //CELLSendStream s = new CELLSendStream(256);
        ////CELLWriteStream 内部使用C++接口
        CELLWriteStream s = new CELLWriteStream(256);
        s.setNetCmd(NetCMD.LOGOUT);
        s.WriteInt8(1);
        s.WriteInt16(2);
        s.WriteInt32(3);
        s.WriteFloat(4.5f);
        s.WriteDouble(6.7);
        s.WriteString("哈哈哈client");
        s.WriteString("ahah嘿嘿嘿");
        int[] b = { 1, 2, 3, 4, 5 };
        s.WriteInt32s(b);
        s.finsh();
        this.SendData(s);
    }

    void OnDestroy()
    {
        this.Close();
    }

    public override void OnNetMsgBytes(IntPtr data, int len)
    {
        ////CELLRecvStream 由C#实现
        //CELLRecvStream r = new CELLRecvStream(data, len);
        ////CELLReadStream 内部使用C++接口
        CELLReadStream r = new CELLReadStream(data, len);
        //消息长度
        Debug.Log(r.ReadUInt16());
        //消息类型
        Debug.Log(r.ReadNetCmd());

        Debug.Log(r.ReadInt8());
        Debug.Log(r.ReadInt16());
        Debug.Log(r.ReadInt32());
        Debug.Log(r.ReadFloat());
        Debug.Log(r.ReadDouble());
        Debug.Log(r.ReadString());
        Debug.Log(r.ReadString());
        Int32[] arr = r.ReadInt32s();
        for (int n = 0; n < arr.Length; n++)
        {
            Debug.Log(arr[n]);
        }
    }
}
