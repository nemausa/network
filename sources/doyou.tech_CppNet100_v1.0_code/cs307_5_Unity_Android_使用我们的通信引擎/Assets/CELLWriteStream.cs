using System;
using System.Runtime.InteropServices;
using System.Text;

public class CELLWriteStream
{
#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern IntPtr CELLWriteStream_Create(int nSize);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLWriteStream_WriteInt8(IntPtr cppStreamObj, sbyte n);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLWriteStream_WriteInt16(IntPtr cppStreamObj, Int16 n);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLWriteStream_WriteInt32(IntPtr cppStreamObj, Int32 n);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLWriteStream_WriteInt64(IntPtr cppStreamObj, Int64 n);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLWriteStream_WriteUInt8(IntPtr cppStreamObj, byte n);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLWriteStream_WriteUInt16(IntPtr cppStreamObj, UInt16 n);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLWriteStream_WriteUInt32(IntPtr cppStreamObj, UInt32 n);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLWriteStream_WriteUInt64(IntPtr cppStreamObj, UInt64 n);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLWriteStream_WriteFloat(IntPtr cppStreamObj, float n);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern bool CELLWriteStream_WriteDouble(IntPtr cppStreamObj, double n);

    //-------------------------------------------------------

    private IntPtr cppStreamObj = IntPtr.Zero;

    public CELLWriteStream(int nSzie = 128)
    {
        cppStreamObj = CELLWriteStream_Create(nSzie);
    }

    public IntPtr cppObj
    {
        get
        {
            return cppStreamObj;
        }
    }

    public void setNetCmd(NetCMD cmd)
    {
        WriteUInt16((UInt16)cmd);
    }

    public void finsh()
    {

    }

    public void WriteInt8(sbyte n)
    {
        CELLWriteStream_WriteInt8(cppStreamObj, n);
    }

    public void WriteInt16(Int16 n)
    {
        CELLWriteStream_WriteInt16(cppStreamObj, n);
    }

    public void WriteInt32(Int32 n)
    {
        CELLWriteStream_WriteInt32(cppStreamObj, n);
    }

    public void WriteInt64(Int64 n)
    {
        CELLWriteStream_WriteInt64(cppStreamObj, n);
    }

    public void WriteUInt8(byte n)
    {
        CELLWriteStream_WriteUInt8(cppStreamObj, n);
    }

    public void WriteUInt16(UInt16 n)
    {
        CELLWriteStream_WriteUInt16(cppStreamObj, n);
    }

    public void WriteUInt32(UInt32 n)
    {
        CELLWriteStream_WriteUInt32(cppStreamObj, n);
    }

    public void WriteUInt64(UInt64 n)
    {
        CELLWriteStream_WriteUInt64(cppStreamObj, n);
    }

    public void WriteFloat(float n)
    {
        CELLWriteStream_WriteFloat(cppStreamObj, n);
    }

    public void WriteDouble(double n)
    {
        CELLWriteStream_WriteDouble(cppStreamObj, n);
    }

    //UTF8
    public void WriteString(string s)
    {
        byte[] buffer = Encoding.UTF8.GetBytes(s);
        WriteUInt32((UInt32)buffer.Length + 1);
        for(int n = 0; n < buffer.Length; n++)
        {
            WriteUInt8(buffer[n]);
        }
        WriteUInt8(0);
    }

    public void WriteBytes(byte[] data)
    {
        WriteUInt32((UInt32)data.Length + 1);
        for (int n = 0; n < data.Length; n++)
        {
            WriteUInt8(data[n]);
        }
    }

    public void WriteInt32s(Int32[] data)
    {
        WriteUInt32((UInt32)data.Length);
        for (int n = 0; n < data.Length; n++)
        {
            WriteInt32(data[n]);
        }
    }
}
