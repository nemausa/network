using System;
using System.Text;
using System.Runtime.InteropServices;

public class CELLReadStream
{

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern IntPtr CELLReadStream_Create(IntPtr data, int len);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern sbyte CELLReadStream_ReadInt8(IntPtr cppStreamObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern Int16 CELLReadStream_ReadInt16(IntPtr cppStreamObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern Int32 CELLReadStream_ReadInt32(IntPtr cppStreamObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern Int64 CELLReadStream_ReadInt64(IntPtr cppStreamObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern byte CELLReadStream_ReadUInt8(IntPtr cppStreamObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern UInt16 CELLReadStream_ReadUInt16(IntPtr cppStreamObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern UInt32 CELLReadStream_ReadUInt32(IntPtr cppStreamObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern UInt64 CELLReadStream_ReadUInt64(IntPtr cppStreamObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern float CELLReadStream_ReadFloat(IntPtr cppStreamObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern double CELLReadStream_ReadDouble(IntPtr cppStreamObj);

#if (UNITY_IPHONE || UNITY_WEBGL) && !UNITY_EDITOR
	[DllImport("__Internal")]
#else
    [DllImport("CppNet100")]
#endif
    private static extern UInt32 CELLReadStream_OnlyReadUInt32(IntPtr cppStreamObj);

    //-------------------------------------------------------
    private IntPtr cppStreamObj = IntPtr.Zero;

    /// <summary>
    /// 
    /// </summary>
    /// <param name="data">C++消息回调传入的消息数据指针</param>
    /// <param name="len">数据字节长度</param>
    public CELLReadStream(IntPtr data, int len)
    {
        cppStreamObj = CELLReadStream_Create(data, len);
    }

    public NetCMD ReadNetCmd()
    {
        return (NetCMD)ReadUInt16();
    }

    public sbyte ReadInt8(sbyte n = 0)
    {
        return CELLReadStream_ReadInt8(cppStreamObj);
    }

    public Int16 ReadInt16(Int16 n = 0)
    {
        return CELLReadStream_ReadInt16(cppStreamObj);
    }

    public Int32 ReadInt32(Int32 n = 0)
    {
        return CELLReadStream_ReadInt32(cppStreamObj);
    }

    public Int64 ReadInt64(Int64 n = 0)
    {
        return CELLReadStream_ReadInt64(cppStreamObj);
    }

    public byte ReadUInt8(byte n = 0)
    {
        return CELLReadStream_ReadUInt8(cppStreamObj);
    }

    public UInt16 ReadUInt16(UInt16 n = 0)
    {
        return CELLReadStream_ReadUInt16(cppStreamObj);
    }

    public UInt32 ReadUInt32(UInt32 n = 0)
    {
        return CELLReadStream_ReadUInt32(cppStreamObj);
    }

    public UInt64 ReadUInt64(UInt64 n = 0)
    {
        return CELLReadStream_ReadUInt64(cppStreamObj);
    }

    public float ReadFloat(float n = 0.0f)
    {
        return CELLReadStream_ReadFloat(cppStreamObj);
    }

    public double ReadDouble(double n = 0)
    {
        return CELLReadStream_ReadDouble(cppStreamObj);
    }

    public UInt32 OnlyReadUInt32(UInt32 n = 0)
    {
        return CELLReadStream_OnlyReadUInt32(cppStreamObj);
    }

    public string ReadString()
    {
        Int32 len = (Int32)ReadUInt32();
        byte[] buffer = new byte[len];
        for (int n = 0; n < buffer.Length; n++)
        {
            buffer[n] = ReadUInt8();
        }
        return Encoding.UTF8.GetString(buffer, 0, len);
    }

    public Int32[] ReadInt32s()
    {
        int len = ReadInt32();
        Int32[] data = new Int32[len];
        for (int n = 0; n < len; n++)
        {
            data[n] = ReadInt32();
        }
        return data;
    }
}